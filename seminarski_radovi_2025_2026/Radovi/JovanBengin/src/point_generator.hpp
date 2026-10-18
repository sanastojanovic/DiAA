#pragma once

#include <vector>
#include "Point.hpp"

std::vector <Point> generate_random_points(int n, int W, int H, int dispersion, int seed);

std::vector <Point> load_points(int n);