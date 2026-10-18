#include<iostream>
#include<vector>
#include<queue>
#include<climits>

using namespace std;


class Graph{

public:
    Graph(int n){
        this->neighbours.resize(n);
        number_of_nodes=n;
    }

    void addEdge(int u,int v,int weight){
        neighbours[u].push_back(std::make_pair(v,weight));
        neighbours[v].push_back(std::make_pair(u,weight));
    }

    int Bidirectional_Dijikstra(int start_point,int end_point,int& number_of_edges){
        vector<bool> visited_from_start_point(number_of_nodes,false);
        vector<bool> visited_from_end_point(number_of_nodes,false);

        vector<int> distance_from_start_point(number_of_nodes,INT_MAX);
        vector<int> distance_from_end_point(number_of_nodes,INT_MAX);

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> start_point_queue;
        start_point_queue.emplace(0,start_point);
        distance_from_start_point[start_point]=0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> end_point_queue;
        end_point_queue.emplace(0,end_point);
        distance_from_end_point[end_point]=0;

        int dist_start,dist_end,node_start,node_end;
        int current_minimal_distance=INT_MAX;

        while(!start_point_queue.empty() || !end_point_queue.empty()){
            while(visited_from_start_point[start_point_queue.top().second]){
                start_point_queue.pop();
            }

            auto [dist_start,node_start]=start_point_queue.top();
            start_point_queue.pop();

            visited_from_start_point[node_start]=true;

            while(visited_from_end_point[end_point_queue.top().second]){
                end_point_queue.pop();
            }
            auto [dist_end,node_end]=end_point_queue.top();
            end_point_queue.pop();

            visited_from_end_point[node_end]=true;

            if(current_minimal_distance<=dist_start+dist_end){
                break;
            }

            for(auto [neighbour,weight]:neighbours[node_start]){
                number_of_edges++;
                if(!visited_from_start_point[neighbour]){

                    if(visited_from_end_point[neighbour]){
                        if(dist_start+weight+distance_from_end_point[neighbour]<current_minimal_distance){
                            current_minimal_distance=dist_start+weight+distance_from_end_point[neighbour];
                        }
                    }

                    if(dist_start+weight<distance_from_start_point[neighbour]){
                        distance_from_start_point[neighbour]=dist_start+weight;
                        start_point_queue.emplace(distance_from_start_point[neighbour],neighbour);
                    }
                }
            }

            for(auto [neighbour,weight]:neighbours[node_end]){
                number_of_edges++;
                if(!visited_from_end_point[neighbour]){

                    if(visited_from_start_point[neighbour]){
                        if(dist_end+weight+distance_from_start_point[neighbour]<current_minimal_distance){
                            current_minimal_distance=dist_end+weight+distance_from_start_point[neighbour];
                        }
                    }
                    if(dist_end+weight<distance_from_end_point[neighbour]){
                        distance_from_end_point[neighbour]=dist_end+weight;
                        end_point_queue.emplace(distance_from_end_point[neighbour],neighbour);
                    }
                }
            }


        }

        return current_minimal_distance;
    }



int Bidirectional_Dijikstra2(int start_point,int end_point,int& number_of_edges){
        vector<bool> visited_from_start_point(number_of_nodes,false);
        vector<bool> visited_from_end_point(number_of_nodes,false);

        vector<int> distance_from_start_point(number_of_nodes,INT_MAX);
        vector<int> distance_from_end_point(number_of_nodes,INT_MAX);

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> start_point_queue;
        //start_point_queue.emplace(0,start_point);
        //distance_from_start_point[start_point]=0;

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> end_point_queue;
        //end_point_queue.emplace(0,end_point);
        //distance_from_end_point[end_point]=0;

        int dist_start,dist_end,node_start,node_end;
        int current_minimal_distance=INT_MAX;
        

        int dist_forward,dist_backward,neighbour_forward,neighbour_backward;
        int weight_forward,weight_backward;

        int node_forward,node_backward;

        int number_of_visited_forward=1;
        int number_of_visited_backward=1;

        node_forward=start_point;
        node_backward=end_point;

        visited_from_start_point[start_point]=true;
        visited_from_end_point[end_point]=true;

        distance_from_start_point[start_point]=0;
        distance_from_end_point[end_point]=0;

        auto forward_iterator=neighbours[start_point].begin();
        auto backward_iterator=neighbours[end_point].begin();

        while(number_of_visited_forward<number_of_nodes && number_of_visited_backward<number_of_nodes){


            auto [neighbour_forward,weight_forward]=*forward_iterator;
            number_of_edges++;

            if(!visited_from_start_point[neighbour_forward]){
                if(distance_from_start_point[neighbour_forward]>distance_from_start_point[node_forward]+weight_forward){
                    distance_from_start_point[neighbour_forward]=distance_from_start_point[node_forward]+weight_forward; 
                    start_point_queue.emplace(distance_from_start_point[neighbour_forward],neighbour_forward);
                }
            }

            if(visited_from_end_point[neighbour_forward]){
                if(current_minimal_distance>distance_from_start_point[node_forward]+weight_forward+distance_from_end_point[neighbour_forward]){
                    current_minimal_distance=distance_from_start_point[node_forward]+weight_forward+distance_from_end_point[neighbour_forward];

                    if(current_minimal_distance<=distance_from_start_point[node_forward]+distance_from_end_point[node_backward]){
                        return current_minimal_distance;
                    }
                 }
            }



            auto [neighbour_backward,weight_backward]=*backward_iterator;
            number_of_edges++;

            if(!visited_from_end_point[neighbour_backward]){
                if(distance_from_end_point[neighbour_backward]>distance_from_end_point[node_backward]+weight_backward){
                    distance_from_end_point[neighbour_backward]=distance_from_end_point[node_backward]+weight_backward;
                    end_point_queue.emplace(distance_from_end_point[neighbour_backward],neighbour_backward);
                }
            }

            if(visited_from_start_point[neighbour_backward]){
                if(current_minimal_distance>distance_from_end_point[node_backward]+weight_backward+distance_from_start_point[neighbour_backward]){
                    current_minimal_distance=distance_from_end_point[node_backward]+weight_backward+distance_from_start_point[neighbour_backward];

                    if(current_minimal_distance<=distance_from_start_point[node_forward]+distance_from_end_point[node_backward]){
                        return current_minimal_distance;
                    }
                }
            }

            forward_iterator++;
            if(forward_iterator==neighbours[node_forward].end()){

                do{

                    if(start_point_queue.empty()){
                        return current_minimal_distance;
                    }

                    while(visited_from_start_point[start_point_queue.top().second]){
                        start_point_queue.pop();
                    }

                    node_forward=start_point_queue.top().second;
                    start_point_queue.pop();

                    visited_from_start_point[node_forward]=true;
                    number_of_visited_forward++;

                    if(current_minimal_distance<=distance_from_start_point[node_forward]+distance_from_end_point[node_backward]){
                        return current_minimal_distance;
                    }

                    forward_iterator=neighbours[node_forward].begin();

                }while(forward_iterator==neighbours[node_forward].end());

            }

            backward_iterator++;
            if(backward_iterator==neighbours[node_backward].end()){

                do{

                    if(end_point_queue.empty()){
                        return current_minimal_distance;
                    }

                    while(visited_from_end_point[end_point_queue.top().second]){
                        end_point_queue.pop();
                    }

                    node_backward=end_point_queue.top().second;
                    end_point_queue.pop();

                    visited_from_end_point[node_backward]=true;
                    number_of_visited_backward++;

                    if(current_minimal_distance<=distance_from_start_point[node_forward]+distance_from_end_point[node_backward]){
                        return current_minimal_distance;
                    }

                    backward_iterator=neighbours[node_backward].begin();

                }while(backward_iterator==neighbours[node_backward].end());

            }
        }

        return current_minimal_distance;

}

   

    int Dijikstra(int start_point,int end_point,int& number_of_edges){

        vector<bool> visited(number_of_nodes,false);

        vector<int> distance(number_of_nodes,INT_MAX);

        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> queue;
        queue.emplace(0,start_point);
        distance[start_point]=0;



        while(!queue.empty()){

            auto [distance_to_current_node,node]=queue.top();
            queue.pop();

            

            if(visited[node])
                continue;

            visited[node]=true;
            if(node==end_point){
                return distance[end_point];
            }

            for(auto [neighbour,weight]:neighbours[node]){
                number_of_edges++;
                if(!visited[neighbour]){
                    if(distance_to_current_node+weight<distance[neighbour]){
                        distance[neighbour]=distance_to_current_node+weight;
                        queue.emplace(distance[neighbour],neighbour);
                    }
                }
            }
        }

        return INT_MAX;

    }



private:
   std::vector<std::vector<std::pair<int,int>>> neighbours;
   int number_of_nodes;

   

};

int main(){
    int n,m;

    std::cin>>n>>m;

    Graph g(n+1);

    int u,v,w;

    for(int i=0;i<m;i++){
        std::cin>>u>>v>>w;
        g.addEdge(u,v,w);
    }

    

    int start,finish;

    std::cin>>start>>finish;

    int number_of_edges_onedirectional=0;
    int number_of_edges_bidirectional=0;
    int number_of_edges_bidirectional_instance_optimal=0;

    int cost_onedirectional,cost_bidirectional,cost_bidirectional_instance_optimal;

    cost_onedirectional=g.Dijikstra(start,finish,number_of_edges_onedirectional);
    cost_bidirectional=g.Bidirectional_Dijikstra(start,finish,number_of_edges_bidirectional);
    cost_bidirectional_instance_optimal=g.Bidirectional_Dijikstra2(start,finish,number_of_edges_bidirectional_instance_optimal);

    std::cout<<"Onedirectional Dijikstra: Minimal cost is: "<< cost_onedirectional<<
    "\n Number of edges examined is: "<<number_of_edges_onedirectional<<"\n\n";

    std::cout<<"Bidirectional Dijikstra: Minimal cost is: "<< cost_bidirectional<<
    "\n Number of edges examined is: "<<number_of_edges_bidirectional<<"\n\n";

    std::cout<<"Bidirectional Dijikstra instance-optimal: Minimal cost is: "<< cost_bidirectional<<
    "\n Number of edges examined is: "<<number_of_edges_bidirectional_instance_optimal<<"\n\n";

    return 0;

}