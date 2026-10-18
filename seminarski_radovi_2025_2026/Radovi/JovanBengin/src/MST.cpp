#include "MST.hpp"
#include <vector>
#include <queue>
#include <climits>
#include <vector>
#include <array>
#include <algorithm>
#include <iostream>

class DSU{
public:
    DSU (int n) : m_n(n){
        m_parent.resize(m_n);
        m_sz.resize(m_n);

        for(int i=0; i<m_n; i++){
            m_parent[i] = i;
            m_sz[i] = 1;
        }
    }

    int find_root(int u){
        return (m_parent[u] == u ? u : m_parent[u] = find_root(m_parent[u]));
    }

    bool unite(int u, int v){
        int u1 = find_root(u);
        int v1 = find_root(v);
        if(u1 == v1) return false;
        if(m_sz[u1] < m_sz[v1]){
            std::swap(u1, v1);
        }
        m_sz[u1] += m_sz[v1];
        m_parent[v1] = u1;
        return true;
    }

private:
    int m_n;
    std::vector <int> m_parent;
    std::vector <int> m_sz;
};

class Graph{
public:
    Graph (int n) : m_n(n) {
        m_edges.resize(m_n);
    }

    void addEdge(int u, int v, int w){
        m_edges[u].push_back({v, w});
        m_edges[v].push_back({u, w});
    }

    std::vector <int> MST_prim(){
        std::vector <bool> visited(m_n, 0);
        std::vector <int> dist(m_n, INT_MAX);
        std::vector <int> parent(m_n, -1);

        std::priority_queue <std::pair <int, int>, std::vector <std::pair <int, int>>, std::greater <std::pair <int, int>>> pq;
        pq.push({0, 0});
        dist[0] = 0;
        parent[0] = 0;

        while(!pq.empty()){
            int u = pq.top().second;
            pq.pop();
            if(visited[u]) continue;
            visited[u] = 1;

            for(auto f : m_edges[u]){
                int v = f.first, w = f.second;
                if(!visited[v] && dist[v] > w){
                    dist[v] = w;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        return parent;
    }

    std::vector <std::pair <int, int>> MST_kruskal(){
        std::vector <std::pair <int, int>> mst_edges;
        std::vector <std::array <int, 3>> all_edges;

        for(int i=0; i<m_n; i++){
            for(auto c : m_edges[i]){
                if(c.first > i){
                    all_edges.push_back({c.second, i, c.first});
                }
            }
        }

        std::sort(all_edges.begin(), all_edges.end());

        DSU dsu(m_n);

        for(auto edg : all_edges){
            if(dsu.unite(edg[1], edg[2])){
                mst_edges.push_back({edg[1], edg[2]});
            }
        }

        return mst_edges;
    }

private:
    int m_n;
    std::vector <std::vector <std::pair <int, int>>> m_edges;
};


std::vector <std::pair <int, int>> solve_MST_prim(int n, std::vector <std::array <int, 3>> edges){
    Graph graf(n);

    for(auto c : edges){
        graf.addEdge(c[0], c[1], c[2]);
    }

    std::vector <int> parents = graf.MST_prim();
    std::vector <std::pair <int, int>> res_edges;
    for(int i=1; i<n; i++){
        res_edges.push_back({i, parents[i]});
    }
    return res_edges;
}

std::vector <std::pair <int, int>> solve_MST_kruskal(int n, std::vector <std::array <int, 3>> edges){
    // no need for explicit graph construction

    std::vector <std::pair <int, int>> mst_edges;

    std::sort(edges.begin(), edges.end(), [](std::array <int, 3> a, std::array <int, 3> b){
        if(a[2] == b[2] && a[0] == b[0]) return a[1] < b[1];
        if(a[2] == b[2]) return a[0] < b[0];
        return a[2] < b[2];
    });

    DSU dsu(n);

    
    for(auto &edg : edges){
        if(dsu.unite(edg[0], edg[1])){
            mst_edges.push_back({edg[0], edg[1]});
        }
    }
    return mst_edges;
}