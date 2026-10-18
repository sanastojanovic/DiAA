#include <iostream>
#include <vector>
#include <climits>
#include <queue>
#include <algorithm>
#include <array>
#include "Point.hpp"
#include "solver.hpp"

class SegmentTree{
public:

    SegmentTree(int n) : m_n(n) {
        min_value.resize(4*m_n + 1);
        min_value_index.resize(4*m_n + 1);
        clearTree();
    }

    void clearTree(){
        _clear(1, 1, m_n);
    }

    void addValue(int position, int value, int value_index){
        _add(1, 1, m_n, position, value, value_index);
    }

    std::pair <int, int> getMin(int l, int r){
        return _find_minimum(1, 1, m_n, l, r);
    }

private:
    void _clear(int node, int l, int r){
        min_value[node] = INT_MAX;
        min_value_index[node] = -1;
        if(l == r) return;
        int mid = (l+r)/2;
        _clear(node*2, l, mid);
        _clear(node*2+1, mid+1, r);
    }

    void _add(int node, int l, int r, int pos, int val, int idx){
        if(l == r){
            if(min_value[node] > val){
                min_value[node] = val;
                min_value_index[node] = idx;
            }
            return;
        }
        int mid = (l+r)/2;
        if(pos <= mid){
            _add(node*2, l, mid, pos, val, idx);
        }
        else{
            _add(node*2+1, mid+1, r, pos, val, idx);
        }

        if(min_value[node*2] <= min_value[node*2+1]){
            min_value[node] = min_value[node*2];
            min_value_index[node] = min_value_index[node*2];
        }
        else{
            min_value[node] = min_value[node*2+1];
            min_value_index[node] = min_value_index[node*2+1];
        }
    }

    std::pair <int, int> _find_minimum(int node, int l, int r, int tl, int tr){
        if(tl > r || l > tr) return {INT_MAX, -1};
        if(tl <= l && r <= tr){
            return {min_value[node], min_value_index[node]};
        }
        int mid = (l+r)/2;
        std::pair <int, int> lval = _find_minimum(node*2, l, mid, tl, tr);
        std::pair <int, int> rval = _find_minimum(node*2+1, mid+1, r, tl, tr);
        if(lval.first <= rval.first) return lval;
        return rval;
    }

    int m_n;
    std::vector <int> min_value;
    std::vector <int> min_value_index;
};


void connectUpperRight(std::vector <Point> &points, std::vector <std::array <int, 3>> &candidate_edges){
    // connects each point with its nearest northeastern neighbour (including points with equal y-coordinate, not including points with equal x-coordinate)

    int n = points.size();


    std::vector <std::pair <int, int>> y_coords;
    for(int i=0; i<n; i++){
        y_coords.push_back({points[i].y(), i});
    }
    std::sort(y_coords.begin(), y_coords.end());
    int no_of_distinct_y = 0;
    std::vector <int> compressed_y(n);
    for(int i=0; i<n; i++){
        if(i == 0 || y_coords[i].first != y_coords[i-1].first){
            no_of_distinct_y++;
        }
        compressed_y[y_coords[i].second] = no_of_distinct_y;
    }

    std::vector <int> sorted_indices;
    for(int i=0; i<n; i++) sorted_indices.push_back(i);
    std::sort(sorted_indices.begin(), sorted_indices.end(), [&](int i, int j){
        return points[i].x() > points[j].x();
    });

    SegmentTree SegTree(no_of_distinct_y);
    int j = 0;
    for(int i=0; i<n; i++){
        // first add all points with greater x
        while(j < n && points[sorted_indices[j]].x() > points[sorted_indices[i]].x()){
            SegTree.addValue(compressed_y[sorted_indices[j]], points[sorted_indices[j]].x() + points[sorted_indices[j]].y(), sorted_indices[j]);
            j++;
        }

        // query
        std::pair <int, int> p = SegTree.getMin(compressed_y[sorted_indices[i]], no_of_distinct_y);
        if(p.second != -1){
            candidate_edges.push_back({sorted_indices[i], p.second, points[sorted_indices[i]].distance(points[p.second])});
        }
    }
}

void processQuadrant(std::vector <Point> &points, std::vector <std::array <int, 3>> &candidate_edges){
    // processes the two octants belonging to the northeastern quadrant

    // transform the lower octant into the northeastern quadrant
    std::vector <Point> transformed_points = points;
    for(auto &c : transformed_points){
        c = Point(c.x() - c.y(), 2*c.y());
    }
    connectUpperRight(transformed_points, candidate_edges);

    // transform the upper octant into the northeastern quadrant
    transformed_points = points;
    for(auto &c : transformed_points){
        c = Point(2*c.x(), -c.x() + c.y());
    }
    connectUpperRight(transformed_points, candidate_edges);
}

std::vector <std::array <int, 3>> create_edges(std::vector <Point> points){
    int n = points.size();
    std::vector <std::array <int, 3>> candidate_edges;

    for(int t=0; t<=3; t++){
        processQuadrant(points, candidate_edges);
        for(auto &c : points){
            c = c.rotate90();
        }
    }
    return candidate_edges;
}