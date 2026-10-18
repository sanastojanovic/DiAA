#include "input_parser.hpp"
#include "Point.hpp"
#include "point_generator.hpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>

void parse_input(int argc, char *argv[], int &N, int &W, int &H, std::vector <Point> &points, bool &visualization, bool &naive, bool &kruskal, std::string &visualization_filename){
    if(argc < 2){
        std::cout << "Requires at least one argument\n";
        exit(1);
    }

    std::vector <std::string> args;
    for(int i=1; i<argc; i++){
        args.push_back(argv[i]);
    }

    if(args[0] == "random"){
        N = std::stoi(args[1]);
        W = std::stoi(args[2]);
        H = std::stoi(args[3]);

        int disp = 0;
        for(int i=0; i+1<args.size(); i++){
            if(args[i] == "--dispersion"){
                disp = std::stoi(args[i+1]);
            }
        }

        int seed = -1;
        for(int i=0; i+1<args.size(); i++){
            if(args[i] == "--seed"){
                seed = std::stoi(args[i+1]);
            }
        }

        if(seed == -1){
            std::random_device rd;
            seed = rd();
        }

        std::cout << "seed " << seed << std::endl;
        points = generate_random_points(N, W, H, disp, seed);
    }

    else if(args[0] == "file"){
        if(args.size() < 2){
            std::cerr << "No file provided\n";
            exit(1);
        }
        std::ifstream fin(args[1]);
        if(!fin.is_open()){
            std::cerr << "File opening error\n";
            exit(1);
        }
        fin >> N >> W >> H;
        for(int i=1; i<=N; i++){
            int x, y;
            fin >> x >> y;
            points.push_back(Point(x, y));
        }
    }

    else if(args[0] == "stdin"){
        std::cout << "Input N: ";
        std::cin >> N;
        std::cout << "Input W: ";
        std::cin >> W;
        std::cout << "Input H: ";
        std::cin >> H;
        std::cout << "Input point coordinates:\n";
        for(int i=1; i<=N; i++){
            int x, y;
            std::cin >> x >> y;
            points.push_back(Point(x, y));
        }
    }

    else{
        std::cout << "Invalid input type\n";
        exit(1);
    }

    for(int i=0; i+1<args.size(); i++){
        if(args[i] == "--visualize"){
            if(args[i+1] == "true"){
                visualization = true;
            }
        }
        if(args[i] == "--visualization_filename"){
            visualization_filename = args[i+1];
        }
        if(args[i] == "--method"){
            if(args[i+1] == "naive"){
                naive = true;
            }
            if(args[i+1] == "kruskal"){
                kruskal = true;
            }
        }
    }
}