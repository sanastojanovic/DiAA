#include "naive_solver.hpp"
#include "Point.hpp"

#include <algorithm>
#include <vector>
#include <climits>

std::vector <std::pair <int, int>> find_MST_naively(std::vector <Point> points){

	int n = points.size();

	std::vector <bool> visited(n, 0);
    std::vector <int> dist(n, INT_MAX);
    std::vector <int> parent(n, -1);

    dist[0] = 0;
    parent[0] = 0;
    visited[0] = 1;

    for(int i=1; i<n; i++){
		dist[i] = points[i].distance(points[0]);
		parent[i] = 0;
	}

    for(int turns=1; turns<n; turns++){
    	int min_dist = INT_MAX;
    	int best_candidate = -1;
    	for(int i=1; i<n; i++){
    		if(!visited[i] && min_dist > dist[i]){
    			min_dist = dist[i];
    			best_candidate = i;
    		}
    	}

    	visited[best_candidate] = 1;
    	for(int i=1; i<n; i++){
    		if(visited[i]) continue;
    		int ndist = points[i].distance(points[best_candidate]);
    		if(dist[i] > ndist){
    			dist[i] = ndist;
    			parent[i] = best_candidate;
    		}
    	}
    }
	
	std::vector <std::pair <int, int>> res_edges;
    for(int i=1; i<n; i++){
        res_edges.push_back({i, parent[i]});
    }

    return res_edges;
}