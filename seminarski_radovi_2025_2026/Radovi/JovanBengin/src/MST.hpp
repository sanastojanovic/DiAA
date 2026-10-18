#pragma once

#include <array>
#include <vector>

std::vector <std::pair <int, int>> solve_MST_prim(int n, std::vector <std::array <int, 3>> edges);

std::vector <std::pair <int, int>> solve_MST_kruskal(int n, std::vector <std::array <int, 3>> edges);