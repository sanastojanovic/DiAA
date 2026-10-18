#include <algorithm>
#include <cmath>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Point{
    vector<double> x;
    string id;
};

struct MstEdge{
    int u, v;
    double w;
};

class DSU {
    private: 
        vector<int> parent, rnk, sz;
    public:
        DSU() = default;

        DSU(int n){
            parent.resize(n);

            for(int i = 0 ; i < n ; i++){
                parent[i] = i;
            }
            rnk.assign(n,  0);
            sz.assign(n, 1);
        }

        int find_parent(int a){
            if(parent[a] == a) return a;
            parent[a] = parent[parent[a]];
            return find_parent(parent[a]);
        }

        void unite(int a, int b){
            a = find_parent(a);
            b = find_parent(b);

            if(a == b) return;
            
            if(rnk[a] < rnk[b]) swap(a, b);

            parent[b] = a;
            sz[a] += sz[b];

            if(rnk[a] == rnk[b]) rnk[a]++;
        }

        int get_size(int a){
            return sz[find_parent(a)];
        }
};

string trim(const string &s){
    int l = 0;
    int r = s.size() - 1;

    while(l <= r && isspace(s[l])) l++;
    while(l <= r && isspace(s[r])) r--;

    if(l > r) return "";

    return s.substr(l, r - l + 1);
}

vector<string> commaSplit(const string &s){
    vector<string> parts;
    string cur;

    for(int i = 0 ; i < s.length(); i++){   
        if(s[i] == ','){
            parts.push_back(trim(cur));
            cur.clear();
        } else{
            cur.push_back(s[i]);
        }
    }

    parts.push_back(trim(cur));
    return parts;
}

bool tryDouble(const string& s, double &val){
    try{
        // ovako jer stod() moze da pretovri i 12.3abc u double 
        string t = trim(s);
        size_t pos = 0;
        val = stod(t, &pos);
        if(pos == t.size()){
            return true;
        } else{
            return false;
        }
    } catch(...){
        return false;
    }
}

unordered_map<string, int> buildColumnMap(const vector<string> &header){
    unordered_map<string, int> col_map;
    for(int i = 0 ; i < header.size() ; i++){
        col_map[trim(header[i])] = i;
    }

    return col_map;
}

vector<int> resolveColumns(const unordered_map<string, int> &col_map, const vector<string> &names){
    vector<int> ids;
    ids.reserve(names.size());
    for(const string& name : names){
        auto it = col_map.find(name);
        if(it == col_map.end()){
            throw runtime_error("Kolona " + name + " nije pronadjena.");
        }
        ids.push_back(it->second);
    }

    return ids;
}

vector<Point> readCSV(const string& filename, const vector<string> &features, const string& idName = "-", int maxRows = -1){
    ifstream fin(filename);

    if(!fin){
        throw runtime_error("Fajl " + filename + " ne moze da se otvori.");
    }
    if(features.empty()){
        throw runtime_error("Lista kolona je prazna.");
    }
    string line;
    if(!getline(fin, line)){
        throw runtime_error("CSV je prazan.");
    }

    vector<string> header = commaSplit(line);
    unordered_map<string, int> col_map = buildColumnMap(header);

    vector<int> featureInd = resolveColumns(col_map, features);

    int id_col = -1;
    if(idName != "-" && !idName.empty()){
        auto it = col_map.find(idName);
        if(it == col_map.end()){
            throw runtime_error("ID kolona " + idName + " ne postoji.");
        }
        id_col = it->second;
    }

    vector<Point> points;

    while(getline(fin, line)){
        if(maxRows != -1 && points.size() >= maxRows) break;

        line = trim(line);
        if(line.empty()) continue;
        vector<string> cells = commaSplit(line);

        Point p;
        p.x.reserve(featureInd.size());

        bool ok = true;

        for(int ind : featureInd){
            if(ind >= cells.size()){
                ok = false;
                break;
            }

            double val;
            if(!tryDouble(cells[ind], val)){
                ok = false;
                break;
            }

            p.x.push_back(val);

        }

        if(!ok) continue;

        if(id_col != -1 && id_col < cells.size()){
            p.id = trim(cells[id_col]);
        }

        if(p.id.empty()){
            p.id = "row_" + to_string((int)points.size());
        }

        points.push_back(p);
    }

    if(points.empty()){
        throw runtime_error("Nema validnih redova.");
    }

    return points;

}

void normalize(vector<Point> &points){
    if(points.empty()) return;

    int n = points.size();
    int d = points[0].x.size();

    vector<double> mean(d, 0.0);
    vector<double> stdev(d, 0.0);

    for(Point &p : points){
        for(int i = 0 ; i < d ; i++){
            mean[i] += p.x[i];
        }
    }

    for(int i = 0 ; i < d ; i++){
        mean[i] /= n;
    }

    for(Point &p : points){
        for(int i = 0 ; i < d ; i++){
            double dif = p.x[i] - mean[i];
            stdev[i] += dif * dif;
        }
    }

    for(int i = 0 ; i < d ; i++){
        stdev[i] = sqrt(stdev[i] / n);

        if(stdev[i] == 0.0){
            stdev[i] = 1.0;
        }
    }

    for(Point &p : points){
        for(int i = 0 ; i < d ; i++){
            p.x[i] = (p.x[i] - mean[i]) / stdev[i];
        }
    }
}

double euclideanDistance(const Point &a, const Point &b){
    double sum = 0.0;
    int d = a.x.size();

    for(int i = 0 ; i < d ; i++){
        double dif = a.x[i] - b.x[i];
        sum += dif * dif;
    }

    return sqrt(sum);
}

vector<MstEdge> primMST(const vector<Point> &points){
    int n = points.size();
    if(n == 0) return {};

    const double INF = numeric_limits<double>::infinity();

    vector<int> inTree(n, 0);
    vector<int> parent(n, -1);
    vector<double> best(n, INF);

    vector<MstEdge> mst;
    mst.reserve(n - 1);

    int start = 0;
    inTree[start] = 1;

    for(int v = 1; v < n ; v++){
        best[v] = euclideanDistance(points[start], points[v]);
        parent[v] = start;
    }

    for(int i = 1 ; i < n ; i++){
        double mn = INF;
        int next = -1;

        for(int v = 0 ; v < n ; v++){
            if(!inTree[v] && best[v] < mn){
                mn = best[v];
                next = v;
            }
        }

        mst.push_back({parent[next], next, best[next]});
        inTree[next] = 1;

        for(int v = 0 ; v < n ; v++){
            if(inTree[v]) continue;

            double w = euclideanDistance(points[next], points[v]);
            if(w < best[v]){
                best[v] = w;
                parent[v] = next;
            }
        }
    }

    return mst;

}

vector<int> getClusters(vector<MstEdge> &mst, int k){
    int n = mst.size() + 1;

    if(k < 1 || k > n){
        throw runtime_error("Broj klastera je neispravan.");
    }

    sort(mst.begin(), mst.end(), [](const MstEdge &a, const MstEdge &b){
        return a.w > b.w;
    });

    DSU dsu(n);

    for(int i = k - 1 ; i < mst.size(); i++){
        dsu.unite(mst[i].u , mst[i].v);
    }

    unordered_map<int, int> rootToCluster;
    vector<int> clusterId(n);

    int nextId = 0;

    for(int i = 0 ; i < n ; i++){
        int root = dsu.find_parent(i);

        if(rootToCluster.find(root) == rootToCluster.end()){
            rootToCluster[root] = nextId;
            nextId++;
        }

        clusterId[i] = rootToCluster[root];
    }

    return clusterId;
}

void printClusterSummary(const vector<int> &clusterId, const vector<Point> &points){
    unordered_map<int, vector<int>> groups;

    for(int i = 0 ; i < clusterId.size(); i++){
        groups[clusterId[i]].push_back(i);
    }

    vector<pair<int,int>> order;
    for(auto &cluster : groups){
        int cid = cluster.first;
        int sz = cluster.second.size();
        order.push_back({sz, cid});
    }

    sort(order.rbegin(), order.rend());

    cout << "\n --- Klasteri --- \n";

    for(auto &cluster : order){
        int cid = cluster.second;

        cout << "Klaster " << cid << ", velicine " << cluster.first << "\n";
        cout << "Prvih 5 tacaka: ";

        for(int i = 0 ; i < cluster.first && i < 5 ; i ++){
            cout << points[groups[cid][i]].id; 
            
            if(i + 1 < (cluster.first) && i < 4){
                cout << ", ";
            }
        }
        cout << "\n";
    }
}

void saveCSV(const string &filename, const vector<Point> &points, const vector<int> &clusterId, const vector<string> &features){
    ofstream fout(filename);
    if(!fout){
        throw runtime_error("File nije mogao da se otvori.");
    }

    fout << "id,cluster";
    for(const string &feature : features){
        fout << "," << feature;
    }
    fout << "\n";

    for(int i = 0 ; i < points.size() ; i++){
        fout << points[i].id << "," << clusterId[i];
        
        for(double val : points[i].x){
            fout << "," << val;
        }

        fout << "\n";
    }
}

int main(int argc, char ** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc < 4 || argc > 6){
        cerr << "Upotreba: \n";
        cerr << " " << argv[0] << " data.csv num_clusters feature1, ... [id_col |-] [max_rows]\n";

        return 1;
    }

    

    try{
        string filename = argv[1];
        int k = stoi(argv[2]);
        vector<string> features = commaSplit(argv[3]);
        string idName = "-";
        if(argc >= 5){
            idName = argv[4];
        }

        int maxRows = -1;
        if(argc >= 6){
            maxRows = stoi(argv[5]);
        }
        vector<Point> points = readCSV(filename, features, idName, maxRows);

        normalize(points);

        vector<MstEdge> mst = primMST(points);
        vector<int> clusterId = getClusters(mst, k);

        cout << "Ucitan broj tacaka: " << points.size() << "\n";
        cout << "Broj feature-a: " << features.size() << "\n";
        cout << "Trazeni broj klastera: " << k << "\n";

        printClusterSummary(clusterId, points);

        saveCSV("output.csv", points , clusterId, features);
    } catch(exception &e){
        cerr << "Greska: " << e.what() << "\n";
        return 1;
    }

    return 0;

}