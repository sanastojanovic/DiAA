// compilation: g++ -std=c++20 main_single_threaded.cpp -o mst_cpp

/*
average runtimes:
  n = 150, v = 150
  mst_cpp, mst_cu: 4.5s
    mf_cpp, mf_cu: 2.8s
               mu: 0.185s
             main: 0.055s
  n = 400, v = 400
               mu: 2.85s
             main: 0.86s
*/

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
      data(s << 1, std::vector<std::vector<int>>(v, std::vector<int>(v, std::numeric_limits<int>::max() / 2))),  // halving in order to avoid overflows
      paths(s, std::vector<std::vector<int>>(v, std::vector<int>(v, -1)))
    {
      
      static std::random_device rd; // obtain a random seed
      static std::mt19937 gen(rd());  // Mersenne Twister engine
      std::uniform_int_distribution<int> distrib(1, 5000); // uniform distribution, got to 5000 being a good example by trial and error
      
      int t;
      for(int k = s; k < s + n; ++k){ // i++ copies the current value of i, increments the original and returns the old value, ++i increments directly without making a copy
        for(int i = 0; i < v; ++i){   // compilers optimise in case of primitive data types, however this should be taken care of by programmer when using complex objects, iterators for example
          data[k][i][i] = 0;    // setting diagonal to zero, staying in place doesn't cost anything
          for(int j = 0; j < i; ++j){
            if((t = distrib(gen)) < 1000){  // if t is lesser than 1000, then the flightpath exists and it costs between 1 and 999 units
              data[k][i][j] = t;
              data[k][j][i] = (distrib(gen) - 1) * 998 / 4999 + 1;  // scaling [1, 5000] down to [1, 999]
            }
          }
        }
      }
      // setting diagonals of neutrals-to-be to zero
      for(int k = s + n; k < data.size(); ++k){
        for(int i = 0; i < v; ++i){
          data[k][i][i] = 0;
        }
      }
      // calculating the tree
      for(int k = s - 1; k > 0; --k)
        matmul(data[2 * k], data[2 * k + 1], data[k], paths[k]);
    }
    // copy constructor, default is good enough since there is no dynamic memory allocation
    SegmentTree(const SegmentTree& x) = default;
    // destructor, default is good enough since the only termination of the program is via return statement
    ~SegmentTree() = default;
    
    // leaf update, O(log(n) * v^3)
    void point_update(const std::vector<std::vector<int>>& x, unsigned t){
      if(t >= n || x.size() != v){
        std::cerr << "point_update; t >= n || x.size() != v" << std::endl;
        throw std::exception();
      }
      for(int i = 0; i < v; ++i){
        if(x[i].size() != v || x[i][i] != 0){
            std::cerr << "point_update; x[i].size() != v || x[i][i] != 0" << std::endl;
            throw std::exception();
        }
        for(int j = 0; j < v; ++j){
            if(i != j && (x[i][j] < 1 || (x[i][j] > 999 && (x[i][j] != (std::numeric_limits<int>::max() / 2))))){
                std::cerr << "point_update; i != j && (x[i][j] < 1 || (x[i][j] > 999 && (x[i][j] != (std::numeric_limits<int>::max() / 2))))" << std::endl;
                throw std::exception();
            }
        }
      }
      
      // translating index
      t += s;
      
      // updating matrix
      data[t] = x;
      
      // recalculating part of the tree
      for(t >>= 1; t > 0; t >>= 1){
        for(int i = 0; i < v; ++i){
          for(int j = 0; j < v; ++j){
            if(i != j){
              data[t][i][j] = std::numeric_limits<int>::max() / 2;  // reseting target to neutral
              paths[t][i][j] = -1;
            }
          }
        }
        matmul(data[2 * t], data[2 * t + 1], data[t], paths[t]);
      }
      
      return;
    }
    
    // update of the sequence of leaves, O(n * v^3), not O(n*log(n) * v^3) because of raw implementation instead of calling point_update
    void range_update(const std::vector<std::vector<std::vector<int>>>& x, unsigned t){
        if(t >= n || x.size() > (n - t)){
          std::cerr << "range_update; t >= n || x.size() > (n - t)" << std::endl;
            throw std::exception();
        }
        for(int k = 0; k < x.size(); ++k){
            if(x[k].size() != v){
                std::cerr << "range_update; x[k].size() != v" << std::endl;
                throw std::exception();
            }
            for(int i = 0; i < v; ++i){
                if(x[k][i].size() != v || x[k][i][i] != 0){
                    std::cerr << "range_update; x[k][i].size() != v || x[k][i][i] != 0" << std::endl;
                    throw std::exception();
                }
                for(int j = 0; j < v; ++j){
                    if(i != j && (x[k][i][j] < 1 || (x[k][i][j] > 999 && (x[k][i][j] != (std::numeric_limits<int>::max() / 2))))){
                        std::cerr << "range_update; i != j && (x[k][i][j] < 1 || (x[k][i][j] > 999 && (x[k][i][j] != (std::numeric_limits<int>::max() / 2))))" << std::endl;
                        throw std::exception();
                    }
                }
            }
        }
        
        // translating index
        t += s;
        
        // updating matrices
        for(int k = 0; k < x.size(); ++k){
            data[t + k] = x[k];
        }
        
        // recalculating part of the tree
        int l = t >> 1;
        int r = (t + x.size() - 1) >> 1;
        while(l > 0){
            for(int k = l; k <= r; ++k){
              for(int i = 0; i < v; ++i){
                for(int j = 0; j < v; ++j){
                  if(i != j){
                    data[k][i][j] = std::numeric_limits<int>::max() / 2;  // reseting target to neutral
                    paths[k][i][j] = -1;
                  }
                }
              }
              matmul(data[2 * k], data[2 * k + 1], data[k], paths[k]);
            }
            l >>= 1;
            r >>= 1;
        }
        
        return;
    }
    
    // query for specific time, O(v^2)
    std::vector<std::vector<int>> point_query(unsigned t){
        if(t >= n){
            std::cerr << "point_query; t > n" << std::endl;
            throw std::exception();
        }
        
        return data[data.size() / 2 + t];
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
          std::vector<std::vector<std::vector<int>>> x(t1, std::vector<std::vector<int>>(v, std::vector<int>(v, std::numeric_limits<int>::max() / 2)));
          for(int k = 0; k < t1; ++k){
            for(int i = 0; i < v; ++i){
              x[k][i][i] = 0;
            }
          }
          copy.range_update(x, 0);
        }
        if(t2 < n - 1){
          std::vector<std::vector<std::vector<int>>> y(n - 1 - t2, std::vector<std::vector<int>>(v, std::vector<int>(v, std::numeric_limits<int>::max() / 2)));
          for(int k = 0; k < n - 1 - t2; ++k){
            for(int i = 0; i < v; ++i){
              y[k][i][i] = 0;
            }
          }
          copy.range_update(y, t2 + 1);
        }
        
        return copy;
    }
    // print the tree
    void print_tree(){
      for(int k = 1; k < data.size(); k *= 2){
        for(int i = 0; i < v; ++i){
          for(int l = 0; l < k; ++l){
            for(int j = 0; j < v; ++j){
              if(data[k + l][i][j] == std::numeric_limits<int>::max() / 2){
                std::cout << "+inf" << " ";
              } else {
                std::cout << std::format("{:04}", data[k + l][i][j]) << " ";
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
      if(-1 == paths[1][i][j]){
        std::cout << "path does not exist" << std::endl;
        return;
      }
      
      std::cout << "weight = " << data[1][i][j] << std::endl;
      
      std::cout << "path: " << std::endl;
      print_path_help(1, i, j);
    }
    
    private:
        unsigned n;
        unsigned v;
        unsigned s;
        std::vector<std::vector<std::vector<int>>> data;
        std::vector<std::vector<std::vector<int>>> paths;
        
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
      void matmul(const std::vector<std::vector<int>> &x, const std::vector<std::vector<int>> &y, std::vector<std::vector<int>> &z, std::vector<std::vector<int>> &p){
        for(int i = 0; i < v; ++i){
          for(int j = 0; j < v; ++j){
            for(int k = 0; k < v; ++k){
              if(z[i][j] > x[i][k] + y[k][j]){
                z[i][j] = x[i][k] + y[k][j];
                p[i][j] = k;
              }
            }
          }
        }
      }
      // print the shortest path from point i to point j found in node k, path reconstruction influenced by Floyd-Warshall
      void print_path_help(int k, int i, int j){
        if(k < s){
          if(-1 == paths[k][i][j]){
            std::cout << "nonexistant" << std::endl;
            return;
          }
          
          print_path_help(2 * k, i, paths[k][i][j]);
          print_path_help(2 * k + 1, paths[k][i][j], j);
        } else {
          if(i != j){
            std::cout << "     " << i << " -> " << j << ", time: " << k - s << ", cost: ";
            if(data[k][i][j] == std::numeric_limits<int>::max() / 2){
              std::cout << "+inf" << std::endl;
            } else {
              std::cout << data[k][i][j] << std::endl;
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
      std::vector<std::vector<int>> x(v, std::vector<int>(v, std::numeric_limits<int>::max() / 2));
      x[0][0] = (x[1][1] = (x[2][2] = (x[3][3] = 0)));
      x[0][3] = 123;
      x[3][0] = 800;
      tree.point_update(x, 4);
      tree.print_tree();
      
      std::cout << std::endl << "range_update(y, 0)" << std::endl << std::endl;
      std::vector<std::vector<std::vector<int>>> y(3, std::vector<std::vector<int>>(v, std::vector<int>(v, std::numeric_limits<int>::max() / 2)));
      y[0][0][0] = (y[0][1][1] = (y[0][2][2] = (y[0][3][3] = 0)));
      y[1][0][0] = (y[1][1][1] = (y[1][2][2] = (y[1][3][3] = 0)));
      y[2][0][0] = (y[2][1][1] = (y[2][2][2] = (y[2][3][3] = 0)));
      y[0][0][3] = 123;
      y[0][3][0] = 800;
      y[1][0][3] = 123;
      y[1][3][0] = 800;
      y[2][0][3] = 123;
      y[2][3][0] = 800;
      tree.range_update(y, 0);
      tree.print_tree();
      
      std::cout << std::endl << "point_query()" << std::endl << std::endl;
      std::vector<std::vector<int>> z = tree.point_query(1);
      for(int i = 0; i < v; ++i){
        for(int j = 0; j < v; ++j){
          if(z[i][j] == std::numeric_limits<int>::max() / 2){
            std::cout << "+inf" << " ";
          } else {
            std::cout << std::format("{:04}", z[i][j]) << " ";
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