#include "solver.hpp"
#include "visualizer.hpp"
#include "Point.hpp"
#include "MST.hpp"
#include "point_generator.hpp"
#include "input_parser.hpp"
#include "naive_solver.hpp"
#include <chrono>
#include <string>
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]){
    std::cout << "Program start" << std::endl;

    std::vector <Point> points;
    int n, W, H;
    bool visualization = false;
    bool naive = false;
    bool kruskal = false;
    std::string visualization_filename = "visualization";
    parse_input(argc, argv, n, W, H, points, visualization, naive, kruskal, visualization_filename);

    std::cout << "Input successfully parsed" << std::endl;

    std::vector <std::pair <int, int>> mst_edges;

    if(naive){
        auto time1 = std::chrono::steady_clock::now();

        std::cout << "Naively finding MST" << std::endl;

        mst_edges = find_MST_naively(points);

        auto time2 = std::chrono::steady_clock::now();
        auto elapsed_time = time2 - time1;

        std::cout << "MST naively found in " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time).count() << "ms" << std::endl;
        
        std::ofstream fout("logs/output_log_buffer.txt", std::ios::app);
        fout << n << " naive " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time).count() << std::endl;
    
        fout.close();
    }

    else{
        auto time0 = std::chrono::steady_clock::now();

        std::vector <std::array <int, 3>> filtered_edges = create_edges(points);

        auto time1 = std::chrono::steady_clock::now();
        auto elapsed_time_1 = time1 - time0;

        std::cout << "Edges filtered in " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_1).count() << "ms" << std::endl;

        std::cout << "Edge count: " << filtered_edges.size() << std::endl;

        time1 = std::chrono::steady_clock::now();

        if(!kruskal){
            std::cout << "Using prim" << std::endl;
            mst_edges = solve_MST_prim(n, filtered_edges);
        }
        else{
            std::cout << "Using kruskal" << std::endl;
            mst_edges = solve_MST_kruskal(n, filtered_edges);
        }

        auto time2 = std::chrono::steady_clock::now();
        auto elapsed_time_2 = time2 - time1;
        auto elapsed_time_total = time2 - time0;

        std::cout << "MST found in " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_2).count() << "ms" << std::endl;
        std::cout << "Total: " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_total).count() << "ms" << std::endl;
        
        std::ofstream fout("logs/output_log_buffer.txt", std::ios::app);
        fout << n << " " << (kruskal ? "kruskal" : "prim") << " " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_total).count() << std::endl;
        std::ofstream fout_mst("logs/output_log_mst_buffer.txt", std::ios::app);
        fout_mst << n << " " << (kruskal ? "kruskal" : "prim") << " " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_2).count() << std::endl;
        std::ofstream fout_solver("logs/output_log_solver_buffer.txt", std::ios::app);
        fout_solver << n << " " << (kruskal ? "kruskal" : "prim") << " " << std::chrono::duration_cast <std::chrono::milliseconds>(elapsed_time_1).count() << std::endl;
        
        fout.close();
        fout_mst.close();
        fout_solver.close();

    }

    if(visualization){
        std::cout << "---------------------------------" << std::endl;
        std::cout << "Starting visualization of " << visualization_filename + ".png" << std::endl;
        visualize(points, mst_edges, W, H, visualization_filename);
        std::cout << "Drawing successful" << std::endl;
    }

    return 0;
}