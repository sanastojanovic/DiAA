#include "point_generator.hpp"
#include <iostream>
#include <random>
#include "Point.hpp"

using namespace std;

Point generate_valid_point(int W, int H, vector <Point> &points, int dispersion,
                            std::mt19937 &rng,
    std::uniform_int_distribution <int> &distrib_W,
    std::uniform_int_distribution <int> &distrib_H){
    
    int x, y;
    for(;;){
        x = distrib_W(rng);
        y = distrib_H(rng);
        if(dispersion == 0) break;
        bool too_close = false;
        for(auto p : points){
            if(abs(x - p.x()) + abs(y - p.y()) < dispersion){
                too_close = true;
                break;
            }
        }
        if(!too_close) break;
    }
    return Point(x, y);
}

vector <Point> generate_random_points(int n, int W, int H, int dispersion, int seed){

    std::mt19937 rng(seed);
    std::uniform_int_distribution <int> distrib_W(50, W - 50);
    std::uniform_int_distribution <int> distrib_H(50, H - 50);

    vector <Point> points;
    for(int i=1; i<=n; i++){
        points.push_back(generate_valid_point(W, H, points, dispersion, rng, distrib_W, distrib_H));
    }
    return points;
}