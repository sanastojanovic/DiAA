// .cu - file type for source codes written in C/C++ with CUDA concepts
// compilation: nvcc -std=c++20 -arch=sm_120 main_flattened.cu -o mf_cu

#include <iostream>
#include <limits>       // for std::numeric_limits<T>::max
#include <vector>       // for std::vector
#include <random>       // for std::random_device, std::mt19937
#include <bit>          // for std::bit_ceil
#include <format>       // for std::format
#include <chrono>       // for std::chrono::high_resolution_clock::now, std::chrono::duration

// segment tree over directed weighted graphs with nonnegative weights represnted with adjacency matrices
class SegmentTree{
  public:
    // constructor, O(n * v^3)
    SegmentTree(unsigned n, unsigned v):
      n(n), // number of periods
      v(v), // dimension of matrices
      s(std::bit_ceil(n)),  // the smallest number greater than n that is a power of 2, it will also represent the index of the first leaf
      v2(v * v), // cached dimension of matrices squared, used for 1D mapping
      data((s << 1) * v2, std::numeric_limits<int>::max() / 2),  // halving in order to avoid overflows
      paths(s * v2, -1)
    {
      
      static std::random_device rd; // obtain a random seed
      static std::mt19937 gen(rd());  // Mersenne Twister engine
      std::uniform_int_distribution<int> distrib(1, 5000); // uniform distribution, got to 5000 being a good example by trial and error
      
      int t;
      for(int k = s; k < s + n; ++k){ // i++ copies the current value of i, increments the original and returns the old value, ++i increments directly without making a copy
        for(int i = 0; i < v; ++i){   // compilers optimise in case of primitive data types, however this should be taken care of by programmer when using complex objects, iterators for example
          data[k * v2 + i * v + i] = 0;     // setting diagonal to zero, staying in place doesn't cost anything
          for(int j = 0; j < i; ++j){
            if((t = distrib(gen)) < 1000){  // if t is lesser than 1000, then the flightpath exists and it costs between 1 and 999 units
              data[k * v2 + i * v + j] = t;
              data[k * v2 + j * v + i] = (distrib(gen) - 1) * 998 / 4999 + 1; // scaling [1, 5000] down to [1, 999]
            }
          }
        }
      }
      // setting diagonals of neutrals-to-be to zero
      for(int k = s + n; k < (data.size() / v2); ++k){
        for(int i = 0; i < v; ++i){
          data[k * v2 + i * v + i] = 0;
        }
      }
      // calculating the tree
      for(int k = s - 1; k > 0; --k)
        matmul(2 * k, 2 * k + 1, k, k);
    }
    // copy constructor, default is good enough since there is no dynamic memory allocation
    SegmentTree(const SegmentTree& x) = default;
    // destructor, default is good enough since the only termination of the program is via return statement
    ~SegmentTree() = default;
    
    // leaf update, O(log(n) * v^3)
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
      
      // translating index
      t += s;
      
      // updating matrix
      for(int i = 0; i < v2; ++i){
        data[t * v2 + i] = x[i];
      }
      
      // recalculating part of the tree
      for(t >>= 1; t > 0; t >>= 1){
        for(int i = 0; i < v; ++i){
          for(int j = 0; j < v; ++j){
            if(i != j){
              data[t * v2 + i * v + j] = std::numeric_limits<int>::max() / 2;  // reseting target to neutral
              paths[t * v2 + i * v + j] = -1;
            }
          }
        }
        matmul(2 * t, 2 * t + 1, t, t);
      }
      
      return;
    }
    
    // update of the sequence of leaves, O(n * v^3), not O(n*log(n) * v^3) because of raw implementation instead of calling point_update
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
        
        // translating index
        t += s;
        
        // updating matrices
        for(int k = 0; k < num_of_matrices; ++k){
            for(int i = 0; i < v2; ++i){
                data[(t + k) * v2 + i] = x[k * v2 + i];
            }
        }
        
        // recalculating part of the tree
        int l = t >> 1;
        int r = (t + num_of_matrices - 1) >> 1;
        while(l > 0){
            for(int k = l; k <= r; ++k){
              for(int i = 0; i < v; ++i){
                for(int j = 0; j < v; ++j){
                  if(i != j){
                    data[k * v2 + i * v + j] = std::numeric_limits<int>::max() / 2;  // reseting target to neutral
                    paths[k * v2 + i * v + j] = -1;
                  }
                }
              }
              matmul(2 * k, 2 * k + 1, k, k);
            }
            l >>= 1;
            r >>= 1;
        }
        
        return;
    }
    
    // query for specific time, O(v^2)
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
    
    // query over a given period, returns a tree constructed over the given timeframe, O(n * v^3)
    // there are many possible optimizations to this method, however none of them seem to reduce worst-case complexity
    SegmentTree range_query(unsigned t1, unsigned t2){
        if(t1 >= t2 || t1 >= n || t2 >= n){
            std::cerr << "range_query; t1 >= t2 || t1 >= n || t2 >= n" << std::endl;
            throw std::exception();
        }
        // copy tree
        SegmentTree copy(*this);
        // set elements outside the given range to neutrals and update the tree
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
    // print the tree
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
    // print shortest path from point i to point j using helper function
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
        std::vector<int> data;
        std::vector<int> paths;
        
      // min-plus matrix multiplication
      /*
          a b c     j k l     min(a+j, b+m, c+p) min(a+m, b+n, c+o) min(a+p, b+q, c+r)
          d e f  x  m n o  =  min(d+j, e+m, f+p) min(d+m, e+n, f+o) min(d+p, e+q, f+r)
          g h i     p q r     min(g+j, h+m, i+p) min(g+m, h+n, i+o) min(g+p, h+q, i+r)
          
          associativity:
            (A x B)_ij = min_over_k(A_ik + B_kj)
            
            ((A x B) x C)_ij = min_over_l((A x B)_il + C_lj) = min_over_k,l(A_ik + B_kl + C_lj) = min_over_k(A_ik + (B x C)_kj) = A x (B x C)
          
          neutral:
              0  +inf +inf
            +inf   0  +inf
            +inf +inf   0 
      */
      void matmul(int x_idx, int y_idx, int z_idx, int p_idx){
        int x_base = x_idx * v2;
        int y_base = y_idx * v2;
        int z_base = z_idx * v2;
        int p_base = p_idx * v2;

        for(int i = 0; i < v; ++i){
          for(int j = 0; j < v; ++j){
            for(int k = 0; k < v; ++k){
              if(data[z_base + i * v + j] > data[x_base + i * v + k] + data[y_base + k * v + j]){
                data[z_base + i * v + j] = data[x_base + i * v + k] + data[y_base + k * v + j];
                paths[p_base + i * v + j] = k;
              }
            }
          }
        }
      }
      // print the shortest path from point i to point j found in node k, path reconstruction influenced by Floyd-Warshall
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