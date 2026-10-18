// .cu - file type for source codes written in C/C++ with CUDA concepts
// compilation: nvcc -std=c++20 -arch=sm_120 main_unified.cu -o mu

#include <iostream>
#include <limits>       // for std::numeric_limits<T>::max
#include <vector>       // for std::vector
#include <random>       // for std::random_device, std::mt19937
#include <bit>          // for std::bit_ceil
#include <format>       // for std::format
#include <chrono>       // for std::chrono::high_resolution_clock::now, std::chrono::duration

// allocator that maps std::vector directly to CUDA Unified Memory
template <class T>
struct UnifiedAllocator {
    using value_type = T;
    
    UnifiedAllocator() = default;
    
    template <class U> 
    constexpr UnifiedAllocator(const UnifiedAllocator<U>&) noexcept {}
    
    T* allocate(std::size_t n) {
        T* ptr = nullptr;
        cudaError_t err = cudaMallocManaged(&ptr, n * sizeof(T)); // allocate
        if(err != cudaSuccess) {  // check error
            throw std::bad_alloc();
        }
        return ptr;
    }
    
    void deallocate(T* p, std::size_t) noexcept { 
        cudaFree(p);  // free
    }
};

template <class T, class U>
bool operator==(const UnifiedAllocator<T>&, const UnifiedAllocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const UnifiedAllocator<T>&, const UnifiedAllocator<U>&) { return false; }

// defines the size of tiles, 32x32 = 1024 threads per CUDA block, this is recommended value for rtx5090
#define TILE_SIZE 32

__global__ void min_plus_matmul_kernel( // __global__ is part of CUDA kernel definition syntax, means this function will be executed on the GPU
    const int* __restrict__ X,  // __restrict__ tells the compiler that these variable are truly different
    const int* __restrict__ Y,
    int* __restrict__ Z,
    int* __restrict__ P,
    int v) 
{
    // __shared__ means that this memory will be accessible by all threads in the given block
    __shared__ int tile_X[TILE_SIZE][TILE_SIZE];
    __shared__ int tile_Y[TILE_SIZE][TILE_SIZE];

    // position of block
    int bx = blockIdx.x;
    int by = blockIdx.y;
    // position of thread within a block
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    // calculate which row and collumn we are calculating using previous information
    int row = by * TILE_SIZE + ty;
    int col = bx * TILE_SIZE + tx;

    constexpr int INF = 1073741823; // equals to std::numeric_limits<int>::max() / 2, to prevent recalculation of the same thing, also this max cannot be called from within kernel
    int min_val = INF;  // instead of rewriting directly into tree's data, we will completely calculate the shortest path here and then copy it
    int best_k = -1;  // same as above, but for path

    // using some algebra so that we correctly calculate number of tiles even when the number of edges per node is not a power of two
    int num_tiles = (v + TILE_SIZE - 1) / TILE_SIZE;

    for (int m = 0; m < num_tiles; ++m) {
        // load a row into shared memory
        if (row < v && m * TILE_SIZE + tx < v) {  // bound check
            tile_X[ty][tx] = X[row * v + m * TILE_SIZE + tx]; // if in bounds load data from the provided matrix
        } else {
            tile_X[ty][tx] = INF; // if out of bounds use neutral
        }

        // load a collumn into shared memory
        if (m * TILE_SIZE + ty < v && col < v) {
            tile_Y[ty][tx] = Y[(m * TILE_SIZE + ty) * v + col];
        } else {
            tile_Y[ty][tx] = INF;
        }

        // wait for all threads to finish, so that the tile is fully loaded
        __syncthreads();

        // compute min-plus matrix multiplication for the current tile
        #pragma unroll  // unroll the loop, this can be used only when the number of iterations is known to the compiler
        for (int k = 0; k < TILE_SIZE; ++k) {
                int val = tile_X[ty][k] + tile_Y[k][tx];
                if (val < min_val) {
                    min_val = val;
                    best_k = m * TILE_SIZE + k;
                }
        }
        
        // wait for all threads to finish
        __syncthreads();
    }

    // write results to global memory
    if (row < v && col < v) {
            Z[row * v + col] = min_val;
            P[row * v + col] = best_k;
    }
}

class SegmentTree{
  public:
    SegmentTree(unsigned n, unsigned v):
      n(n),
      v(v),
      s(std::bit_ceil(n)),
      v2(v * v), // cached dimension of matrices squared, used for 1D mapping
      data((s << 1) * v2, std::numeric_limits<int>::max() / 2),
      paths(s * v2, -1)
    {
      
      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_int_distribution<int> distrib(1, 5000);
      
      int t;
      for(int k = s; k < s + n; ++k){
        for(int i = 0; i < v; ++i){
          data[k * v2 + i * v + i] = 0;
          for(int j = 0; j < i; ++j){
            if((t = distrib(gen)) < 1000){
              data[k * v2 + i * v + j] = t;
              data[k * v2 + j * v + i] = (distrib(gen) - 1) * 998 / 4999 + 1;
            }
          }
        }
      }

      for(int k = s + n; k < (data.size() / v2); ++k){
        for(int i = 0; i < v; ++i){
          data[k * v2 + i * v + i] = 0;
        }
      }
      // calculating the tree layer by layer
      if (s > 1) {
          std::vector<cudaStream_t> streams(s >> 1);

          for (auto& stream : streams) {
              cudaStreamCreate(&stream);  // create streams
          }

          for (unsigned first = s >> 1; first > 0; first >>= 1) {
              unsigned count = first; // number of multiplications to be done in this layer
              for (unsigned offset = 0; offset < count; ++offset) {
                unsigned k = first + offset;  // caluclate index
                matmul_streamed(2 * k, 2 * k + 1, k, k, streams[offset]); // start matrix multiplication
              }

              // wait for the entire layer to be calculated
              cudaDeviceSynchronize();
          }

          for (auto& stream : streams) {
              cudaStreamDestroy(stream);  // destroy streams
          }
      }
    }
    SegmentTree(const SegmentTree& x) = default;
    ~SegmentTree() = default;
    
    void point_update(const std::vector<int>& x, unsigned t){
      if(t >= n || x.size() != v2){
        std::cerr << "point_update; t >= n || x.size() != v2" << std::endl;
        throw std::exception();
      }
      for(int i = 0; i < v; ++i){
        if(x[i * v + i] != 0){
            std::cerr << "point_update; x[i][i] != 0" << std::endl;
            throw std::exception();
        }
        for(int j = 0; j < v; ++j){
            if(i != j && (x[i * v + j] < 1 || (x[i * v + j] > 999 && (x[i * v + j] != (std::numeric_limits<int>::max() / 2))))){
                std::cerr << "point_update; i != j && (x[i][j] < 1 || (x[i][j] > 999 && (x[i][j] != (std::numeric_limits<int>::max() / 2))))" << std::endl;
                throw std::exception();
            }
        }
      }
      
      t += s;
      
      for(int i = 0; i < v2; ++i){
        data[t * v2 + i] = x[i];
      }
      
      for(t >>= 1; t > 0; t >>= 1)
        matmul(2 * t, 2 * t + 1, t, t);
      
      return;
    }
    
    void range_update(const std::vector<int>& x, unsigned t){
        unsigned num_of_matrices = x.size() / v2;
        if(t >= n || num_of_matrices > (n - t) || x.size() % v2 != 0){
            std::cerr << "range_update; t >= n || num_of_matrices > (n - t) || x.size() % v2 != 0" << std::endl;
            throw std::exception();
        }
        for(int k = 0; k < num_of_matrices; ++k){
            for(int i = 0; i < v; ++i){
                if(x[k * v2 + i * v + i] != 0){
                    std::cerr << "range_update; x[k * v2 + i * v + i] != 0" << std::endl;
                    throw std::exception();
                }
                for(int j = 0; j < v; ++j){
                    if(i != j && (x[k * v2 + i * v + j] < 1 || (x[k * v2 + i * v + j] > 999 && (x[k * v2 + i * v + j] != (std::numeric_limits<int>::max() / 2))))){
                        std::cerr << "range_update; i != j && (x[k * v2 + i * v + j] < 1 || (x[k * v2 + i * v + j] > 999 && (x[k * v2 + i * v + j] != (std::numeric_limits<int>::max() / 2))))" << std::endl;
                        throw std::exception();
                    }
                }
            }
        }
        
        t += s;
        
        for(int k = 0; k < num_of_matrices; ++k){
            for(int i = 0; i < v2; ++i){
                data[(t + k) * v2 + i] = x[k * v2 + i];
            }
        }
        
        int l = t >> 1;
        int r = (t + num_of_matrices - 1) >> 1;
        while(l > 0){
            for(int k = l; k <= r; ++k){
              matmul(2 * k, 2 * k + 1, k, k);
            }
            l >>= 1;
            r >>= 1;
        }
        
        return;
    }
    
    std::vector<int> point_query(unsigned t){
        if(t >= n){
            std::cerr << "point_query; t > n" << std::endl;
            throw std::exception();
        }
        
        std::vector<int> res(v2);
        int base_idx = ((data.size() / v2) / 2 + t) * v2;
        for(int i = 0; i < v2; ++i){
            res[i] = data[base_idx + i];
        }
        
        return res;
    }
    
    SegmentTree range_query(unsigned t1, unsigned t2){
        if(t1 >= t2 || t1 >= n || t2 >= n){
            std::cerr << "range_query; t1 >= t2 || t1 >= n || t2 >= n" << std::endl;
            throw std::exception();
        }

        SegmentTree copy(*this);

        if(t1 > 0){
          std::vector<int> x(t1 * v2, std::numeric_limits<int>::max() / 2);
          for(int k = 0; k < t1; ++k){
            for(int i = 0; i < v; ++i){
              x[k * v2 + i * v + i] = 0;
            }
          }
          copy.range_update(x, 0);
        }
        if(t2 < n - 1){
          std::vector<int> y((n - 1 - t2) * v2, std::numeric_limits<int>::max() / 2);
          for(int k = 0; k < n - 1 - t2; ++k){
            for(int i = 0; i < v; ++i){
              y[k * v2 + i * v + i] = 0;
            }
          }
          copy.range_update(y, t2 + 1);
        }
        
        return copy;
    }

    void print_tree(){
      for(int k = 1; k < (data.size() / v2); k *= 2){
        for(int i = 0; i < v; ++i){
          for(int l = 0; l < k; ++l){
            for(int j = 0; j < v; ++j){
              if(data[(k + l) * v2 + i * v + j] == std::numeric_limits<int>::max() / 2){
                std::cout << "+inf" << " ";
              } else {
                std::cout << std::format("{:04}", data[(k + l) * v2 + i * v + j]) << " ";
              }
            }
            std::cout << "  ";
          }
          std::cout << std::endl;
        }
        std::cout << std::endl << std::endl;
      }
    }

    void print_path(int i, int j){
      if(-1 == paths[1 * v2 + i * v +j]){
        std::cout << "path does not exist" << std::endl;
        return;
      }

      std::cout << "weight = " << data[1 * v2 + i * v + j] << std::endl;
      
      std::cout << "path: " << std::endl;
      print_path_help(1, i, j);
    }
    
    private:
        unsigned n;
        unsigned v;
        unsigned s;
        unsigned v2;
        // allocated in unified vram
        std::vector<int, UnifiedAllocator<int>> data;
        std::vector<int, UnifiedAllocator<int>> paths;
     
      // method that calls kernel
      void matmul(int x_idx, int y_idx, int z_idx, int p_idx) {
        int x_base = x_idx * v2;
        int y_base = y_idx * v2;
        int z_base = z_idx * v2;
        int p_base = p_idx * v2;

        // dim3 = struct { unsigned x, y, z; }
        dim3 blockDim(TILE_SIZE, TILE_SIZE);  // defines the size of block
        dim3 gridDim((v + blockDim.x - 1) / blockDim.x, (v + blockDim.y - 1) / blockDim.y); // defines the size of grid consisting of blocks

        min_plus_matmul_kernel<<<gridDim, blockDim>>>(  // CUDA kernel execution syntax, means this function will be executed on the GPU
            data.data() + x_base,
            data.data() + y_base,
            data.data() + z_base,
            paths.data() + p_base,
            v
        );

        // wait for GPU to finish before continuing (without this CPU would just move on)
        cudaDeviceSynchronize();
      }
      
      // method that allows concurrency within layers
      void matmul_streamed(int x_idx, int y_idx, int z_idx, int p_idx, cudaStream_t stream) {
        int x_base = x_idx * v2;
        int y_base = y_idx * v2;
        int z_base = z_idx * v2;
        int p_base = p_idx * v2;

        // dim3 = struct { unsigned x, y, z; }
        dim3 blockDim(TILE_SIZE, TILE_SIZE);  // defines the size of block
        dim3 gridDim((v + blockDim.x - 1) / blockDim.x, (v + blockDim.y - 1) / blockDim.y); // defines the size of grid consisting of blocks

        // Pass the underlying pointers directly to the kernel
        min_plus_matmul_kernel<<<gridDim, blockDim, 0, stream>>>(  // CUDA kernel execution syntax, means this function will be executed on the GPU
            data.data() + x_base,
            data.data() + y_base,
            data.data() + z_base,
            paths.data() + p_base,
            v
        );
      }
      
      void print_path_help(int k, int i, int j){
        if(k < s){
          if(-1 == paths[k * v2 + i * v + j]){
            std::cout << "nonexistant" << std::endl;
            return;
          }
          
          print_path_help(2 * k, i, paths[k * v2 + i * v + j]);
          print_path_help(2 * k + 1, paths[k * v2 + i * v + j], j);
        } else {
          if(i != j){
            std::cout << "     " << i << " -> " << j << ", time: " << k - s << ", cost: ";
            if(data[k * v2 + i * v + j] == std::numeric_limits<int>::max() / 2){
              std::cout << "+inf" << std::endl;
            } else {
              std::cout << data[k * v2 + i * v + j] << std::endl;
            }
          }
        }
      }
};

int main(int argc, char** argv){
  try {
    unsigned n, v, a, b;
    
    if(1 == argc){
      n = 7;
      v = 4;
      a = 0;
      b = 2;
    } else if(5 == argc){
      n = atoi(argv[1]);
      v = atoi(argv[2]);
      a = atoi(argv[3]);
      b = atoi(argv[4]);
   } else {
      std::cerr << "Wrong arguments" << std::endl;
      return 1;
    }
    
    cudaFree(0);  // driver initialization before timing
    
    auto start = std::chrono::high_resolution_clock::now();
    
    SegmentTree tree(n, v);
    
    auto end = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double> duration = end - start;
    
    std::cout << "Time: " << duration.count() << " seconds" << std::endl;
    
    if(1 == argc){
      std::cout << std::endl << "print_tree()" << std::endl << std::endl;
      tree.print_tree();
      
      std::cout << std::endl << "point_update(x, 4)" << std::endl << std::endl;
      std::vector<int> x(v * v, std::numeric_limits<int>::max() / 2);
      x[0 * v + 0] = (x[1 * v + 1] = (x[2 * v + 2] = (x[3 * v + 3] = 0)));
      x[0 * v + 3] = 123;
      x[3 * v + 0] = 800;
      tree.point_update(x, 4);
      tree.print_tree();
      
      std::cout << std::endl << "range_update(y, 0)" << std::endl << std::endl;
      std::vector<int> y(3 * v * v, std::numeric_limits<int>::max() / 2);
      y[0 * v * v + 0 * v + 0] = (y[0 * v * v + 1 * v + 1] = (y[0 * v * v + 2 * v + 2] = (y[0 * v * v + 3 * v + 3] = 0)));
      y[1 * v * v + 0 * v + 0] = (y[1 * v * v + 1 * v + 1] = (y[1 * v * v + 2 * v + 2] = (y[1 * v * v + 3 * v + 3] = 0)));
      y[2 * v * v + 0 * v + 0] = (y[2 * v * v + 1 * v + 1] = (y[2 * v * v + 2 * v + 2] = (y[2 * v * v + 3 * v + 3] = 0)));
      y[0 * v * v + 0 * v + 3] = 123;
      y[0 * v * v + 3 * v + 0] = 800;
      y[1 * v * v + 0 * v + 3] = 123;
      y[1 * v * v + 3 * v + 0] = 800;
      y[2 * v * v + 0 * v + 3] = 123;
      y[2 * v * v + 3 * v + 0] = 800;
      tree.range_update(y, 0);
      tree.print_tree();
      
      std::cout << std::endl << "point_query()" << std::endl << std::endl;
      std::vector<int> z = tree.point_query(1);
      for(int i = 0; i < v; ++i){
        for(int j = 0; j < v; ++j){
          if(z[i * v + j] == std::numeric_limits<int>::max() / 2){
            std::cout << "+inf" << " ";
          } else {
            std::cout << std::format("{:04}", z[i * v + j]) << " ";
          }
        }
        std::cout << std::endl;
      }
      
      std::cout << std::endl << "range_query()" << std::endl << std::endl;
      SegmentTree w = tree.range_query(1, n-2);
      w.print_tree();
      w.print_path(a, b);
      
      std::cout << std::endl << "print_path()" << std::endl << std::endl;
      tree.print_path(a, b);
    }
  } catch (const std::exception &e){
    return 1;
  }
  
  return 0;
}