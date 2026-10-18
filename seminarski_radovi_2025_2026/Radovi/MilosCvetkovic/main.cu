// .cu - file type for source codes written in C/C++ with CUDA concepts
// compilation: nvcc -std=c++20 -arch=sm_120 main.cu -o main

#include <iostream>
#include <limits>         // for std::numeric_limits<T>::max
#include <vector>         // for std::vector
#include <random>         // for std::random_device, std::mt19937
#include <bit>            // for std::bit_ceil
#include <format>         // for std::format
#include <chrono>         // for std::chrono::high_resolution_clock::now, std::chrono::duration
#include <cuda_runtime.h> // for cudaMalloc(), cudaMemcpy(), cudaDeviceSynchronize(), cudaFree()

// defines the size of tiles, 32x32 = 1024 threads per CUDA block, this is recommended value for rtx5090
#define TILE_SIZE 32

__global__ void min_plus_matmul_kernel( // __global__ is part of CUDA kernel definition syntax, means this function will be executed on the gpu
  const int* __restrict__ X,  // __restrict__ tells the compiler that these variable are truly different
  const int* __restrict__ Y,
  int* __restrict__ Z,
  int* __restrict__ P,
  unsigned v){
    // position of thread within a block
    int tx = threadIdx.x;
    int ty = threadIdx.y;
    // calculate which row and collumn we are calculating using previous information
    int row = blockIdx.y * TILE_SIZE + ty;  // BlockIdx - position of block
    int col = blockIdx.x * TILE_SIZE + tx;
    // __shared__ means that this memory will be accessible by all threads in the given block
    __shared__ int As[TILE_SIZE][TILE_SIZE];
    __shared__ int Bs[TILE_SIZE][TILE_SIZE];

    // instead of rewriting directly into tree's data, we will completely calculate the shortest path here and then copy it
    int min_val = 1073741823; // 1073741823 equals to std::numeric_limits<int>::max() / 2, to prevent recalculation of the same thing, also this max cannot be called from within kernel
    // same as above, but for path
    int min_k = -1;

    // using some algebra so that we correctly calculate number of tiles even when the number of edges per node is not a power of two
    for (int m = 0; m < (v + TILE_SIZE - 1) / TILE_SIZE; ++m) {
        // load a row into shared memory
        if (row < v && m * TILE_SIZE + tx < v) {  // bound check
            As[ty][tx] = X[row * v + (m * TILE_SIZE + tx)]; // if in bounds load data from the provided matrix
        } else {
            As[ty][tx] = 1073741823; // if out of bounds use neutral
        }
        // load a collumn into shared memory
        if (col < v && m * TILE_SIZE + ty < v) {
            Bs[ty][tx] = Y[(m * TILE_SIZE + ty) * v + col];
        } else {
            Bs[ty][tx] = 1073741823;
        }
        // wait for all threads to finish, so that the tile is fully loaded
        __syncthreads();
        // compute min-plus matrix multiplication for the current tile
        #pragma unroll  // unroll the loop, this can be used only when the number of iterations is known to the compiler
        for (int k = 0; k < TILE_SIZE; ++k) {
            int val = As[ty][k] + Bs[k][tx];
            if (val < min_val) {
                min_val = val;
                min_k = m * TILE_SIZE + k;
            }
        }
        // wait for all threads to finish
        __syncthreads();
    }
    // write results to global memory
    if (row < v && col < v) {
        Z[row * v + col] = min_val;
        if (P != nullptr) {
            P[row * v + col] = min_k;
        }
    }
}

class SegmentTree {
  public:
    SegmentTree(unsigned n, unsigned v) :
      n(n),
      v(v),
      s(std::bit_ceil(n)),
      v2(v * v), // cached dimension of matrices squared, used for 1D mapping
      h_data((s << 1) * v2, std::numeric_limits<int>::max() / 2),
      h_paths(s * v2, -1),
      d_data(nullptr),
      d_paths(nullptr)
    {
      static std::random_device rd; 
      static std::mt19937 gen(rd());  
      std::uniform_int_distribution<int> distrib(1, 5000); 
      
      int t;
      for(int k = s; k < s + n; ++k){ 
        for(int i = 0; i < v; ++i){   
          h_data[k * v2 + i * v + i] = 0;
          for(int j = 0; j < i; ++j){
            if((t = distrib(gen)) < 1000){  
              h_data[k * v2 + i * v + j] = t;
              h_data[k * v2 + j * v + i] = (distrib(gen) - 1) * 998 / 4999 + 1;  
            }
          }
        }
      }
      
      for(int k = s + n; k < (h_data.size() / v2); ++k){
        for(int i = 0; i < v; ++i){
          h_data[k * v2 + i * v + i] = 0;
        }
      }
      
      // allocate vram
      cudaMalloc(&d_data, h_data.size() * sizeof(int));
      cudaMalloc(&d_paths, h_paths.size() * sizeof(int));
      
      // push initial host state to the gpu
      cudaMemcpy(d_data, h_data.data(), h_data.size() * sizeof(int), cudaMemcpyHostToDevice);
      cudaMemcpy(d_paths, h_paths.data(), h_paths.size() * sizeof(int), cudaMemcpyHostToDevice);
      
      // calculating the tree layer by layer
      int k = s - 1;
      t = s >> 1;
      while(t > 0){
        while(k >= t){
          matmul(
            d_data + (2 * k) * v * v, 
            d_data + (2 * k + 1) * v * v, 
            d_data + k * v * v, 
            d_paths + k * v * v
          );
          --k;
        }
        t >>= 1;
        // wait for the entire layer to be calculated
        cudaDeviceSynchronize();
      }
      
      // pull calculated tree matrix back to host
      cudaMemcpy(h_data.data(), d_data, h_data.size() * sizeof(int), cudaMemcpyDeviceToHost);
      cudaMemcpy(h_paths.data(), d_paths, h_paths.size() * sizeof(int), cudaMemcpyDeviceToHost);
    }
    // deep copy constructor
    SegmentTree(const SegmentTree& x):
      n(x.n),
      v(x.v),
      s(x.s),
      v2(x.v2),
      h_data(x.h_data),
      h_paths(x.h_paths),
      d_data(nullptr),
      d_paths(nullptr)
    {
      cudaMalloc(&d_data, h_data.size() * sizeof(int));
      cudaMalloc(&d_paths, h_paths.size() * sizeof(int));
      cudaMemcpy(d_data, x.d_data, h_data.size() * sizeof(int), cudaMemcpyDeviceToDevice);
      cudaMemcpy(d_paths, x.d_paths, h_paths.size() * sizeof(int), cudaMemcpyDeviceToDevice);
    }
    ~SegmentTree(){
        // free vram
        if (d_data) cudaFree(d_data);
        if (d_paths) cudaFree(d_paths);
    }
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
        h_data[t * v2 + i] = x[i];
      }
      
      // push the updated leaf to the gpu before recomputing its ancestors
      cudaMemcpy(d_data + t * v2, h_data.data() + t * v2, v2 * sizeof(int), cudaMemcpyHostToDevice);
      
      for(t >>= 1; t > 0; t >>= 1){
        matmul(d_data + 2 * t * v2, d_data + (2 * t + 1) * v2, d_data + t * v2, d_paths + t * v2);
      }
      
      // wait for the path to the root to finish, then pull results back to host
      cudaDeviceSynchronize();
      cudaMemcpy(h_data.data(), d_data, h_data.size() * sizeof(int), cudaMemcpyDeviceToHost);
      cudaMemcpy(h_paths.data(), d_paths, h_paths.size() * sizeof(int), cudaMemcpyDeviceToHost);
      
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
                h_data[(t + k) * v2 + i] = x[k * v2 + i];
            }
        }
        
        // push the updated leaves to the gpu before recomputing their ancestors
        cudaMemcpy(d_data + t * v2, h_data.data() + t * v2, num_of_matrices * v2 * sizeof(int), cudaMemcpyHostToDevice);
        
        int l = t >> 1;
        int r = (t + num_of_matrices - 1) >> 1;
        while(l > 0){
            for(int k = l; k <= r; ++k){
              matmul(d_data + 2 * k * v2, d_data + (2 * k + 1) * v2, d_data + k * v2, d_paths + k * v2);
            }
            l >>= 1;
            r >>= 1;
        }
        
        // wait for every level to finish, then pull the whole tree back to host
        cudaDeviceSynchronize();
        cudaMemcpy(h_data.data(), d_data, h_data.size() * sizeof(int), cudaMemcpyDeviceToHost);
        cudaMemcpy(h_paths.data(), d_paths, h_paths.size() * sizeof(int), cudaMemcpyDeviceToHost);
        
        return;
    }
    
    std::vector<int> point_query(unsigned t){
        if(t >= n){
            std::cerr << "point_query; t > n" << std::endl;
            throw std::exception();
        }
        
        std::vector<int> res(v2);
        int base_idx = ((h_data.size() / v2) / 2 + t) * v2;
        for(int i = 0; i < v2; ++i){
            res[i] = h_data[base_idx + i];
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
      for(int k = 1; k < (h_data.size() / v2); k *= 2){
        for(int i = 0; i < v; ++i){
          for(int l = 0; l < k; ++l){
            for(int j = 0; j < v; ++j){
              if(h_data[(k + l) * v2 + i * v + j] == std::numeric_limits<int>::max() / 2){
                std::cout << "+inf" << " ";
              } else {
                std::cout << std::format("{:04}", h_data[(k + l) * v2 + i * v + j]) << " ";
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
      if(-1 == h_paths[1 * v2 + i * v +j]){
        std::cout << "path does not exist" << std::endl;
        return;
      }
      
      std::cout << "weight = " << h_data[1 * v2 + i * v + j] << std::endl;
      
      std::cout << "path: " << std::endl;
      print_path_help(1, i, j);
    }
  private:
    unsigned n;
    unsigned v;
    unsigned s;
    unsigned v2;
    // allocated in vram
    std::vector<int> h_data;
    std::vector<int> h_paths;
    int* d_data;
    int* d_paths;
    
    // method that calls kernel
    void matmul(int* d_x, int* d_y, int* d_z, int* d_p) {
        // dim3 = struct { unsigned x, y, z; }
        dim3 threadsPerBlock(TILE_SIZE, TILE_SIZE); // defines the size of block
        dim3 numBlocks((v - 1) / TILE_SIZE + 1, (v - 1) / TILE_SIZE + 1); // defines the size of grid consisting of blocks
        min_plus_matmul_kernel<<<numBlocks, threadsPerBlock>>>(d_x, d_y, d_z, d_p, v);  // CUDA kernel execution syntax, means this function will be executed on the gpu
    }
    void print_path_help(int k, int i, int j){
        if(k < s){
          if(-1 == h_paths[k * v2 + i * v + j]){
            std::cout << "nonexistant" << std::endl;
            return;
          }
          
          print_path_help(2 * k, i, h_paths[k * v2 + i * v + j]);
          print_path_help(2 * k + 1, h_paths[k * v2 + i * v + j], j);
        } else {
          if(i != j){
            std::cout << "     " << i << " -> " << j << ", time: " << k - s << ", cost: ";
            if(h_data[k * v2 + i * v + j] == std::numeric_limits<int>::max() / 2){
              std::cout << "+inf" << std::endl;
            } else {
              std::cout << h_data[k * v2 + i * v + j] << std::endl;
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