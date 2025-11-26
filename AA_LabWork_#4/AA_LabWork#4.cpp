#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <tuple>
#include <fstream>

using namespace std;

class UnionFind {
private:
    vector<int> parent;
    vector<int> rank;
public:
    UnionFind(int n) {
        parent.resize(n);
        rank.resize(n, 0);
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }
    int find(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = find(parent[i]);
    }
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);

        if (root_i != root_j) {
            if (rank[root_i] < rank[root_j]) {
                parent[root_i] = root_j;
            }
            else if (rank[root_i] > rank[root_j]) {
                parent[root_j] = root_i;
            }
            else {
                parent[root_j] = root_i;
                rank[root_i]++;
            }
            return true;
        }
        return false;
    }
};

struct Edge {
    int u, v;
    int weight;
    // < - сортування за вагою
    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

pair<vector<Edge>, long long> kruskalMWST(int n, vector<Edge>& edges) {
    sort(edges.begin(), edges.end());
    UnionFind uf(n);
    vector<Edge> mwst_edges;
    long long mwst_weight = 0;
    int edges_count = 0;
    for (const auto& edge : edges) {
        if (uf.unite(edge.u, edge.v)) {
            mwst_edges.push_back(edge);
            mwst_weight += edge.weight;
            edges_count++;
            if (edges_count == n - 1)
                break;
        }
    }
    return { mwst_edges, mwst_weight };
}

vector<Edge> random_graph(int n, int num_edges) {
    vector<Edge> edges;
    for (int i = 0; i < num_edges; ++i) {
        int u = rand() % n;
        int v = rand() % n;
        if (u == v) {
            --i;
            continue;
        }
        int weight = rand() % 1000 + 1;
        edges.push_back({ u, v, weight });
    }
    return edges;
}

double kruskal_time(int n, int e_factor) {
    int num_edges = n * e_factor;
    vector<Edge> edges = random_graph(n, num_edges);
    sort(edges.begin(), edges.end());
    auto start = chrono::high_resolution_clock::now();
    UnionFind uf(n);
    vector<Edge> mst_edges;
    int edges_count = 0;
    for (const auto& edge : edges) {
        if (uf.unite(edge.u, edge.v)) {
            mst_edges.push_back(edge);
            edges_count++;

            if (edges_count == n - 1)
                break;
        }
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}

int main() {
    srand(time(0));
    int N_VERTICES = 5;
    vector<Edge> graph_edges = {
        {0, 1, 10},
        {0, 2, 6},
        {0, 3, 5},
        {1, 3, 15},
        {2, 3, 4},
        {3, 4, 2},
        {0, 4, 13}
    };
    auto [mst_result, total_weight] = kruskalMWST(N_VERTICES, graph_edges);
    cout << "### Kruskal's algorithm ###" << endl;
    cout << "Amount of vertices (N): " << N_VERTICES << endl;
    cout << "Edges of Minimal-Weight Spanning Tree:" << endl;
    for (const auto& edge : mst_result) {
        cout << "  " << edge.u << " -- " << edge.v << " (weight: " << edge.weight << ")" << endl;
    }
    cout << "General weight of Minimal-Weight Spanning Tree: " << total_weight << endl;
    cout << "-----------------------------------------------" << endl;
    ofstream csv_file("kruskal_complexity.csv");
    if (!csv_file.is_open()) {
        cerr << "Error: kruskal_complexity.csv cannot be open." << endl;
        return 1;
    }
    csv_file << "Experiment_Type,N_Vertices,E_Edges,Time_ms,T(N;E) (thousands of steps)" << endl;
    cout << "-----------------------------------------------" << endl;
    cout << "### Runtime (N) ###" << endl;
    cout << "N_Vertices\tE_Edges\t\tTime (ms)\tT(N,E)" << endl;
    vector<int> N_sizes = { 1000, 2000, 4000, 8000, 16000 };
    int E_FACTOR = 4;
    for (int N : N_sizes) {
        int E = N * E_FACTOR;
        double time_ms = 0;
        int num_runs = 5;
        for (int i = 0; i < num_runs; ++i) {
            time_ms += kruskal_time(N, E_FACTOR);
        }
        time_ms /= num_runs;
        double theoretical_metric = (double)E;
        cout << fixed << setprecision(0)
            << N << "\t\t" << E << "\t\t";
        cout << fixed << setprecision(4)
            << time_ms << "\t\t"
            << theoretical_metric / 1000.0 << " (thousands of steps)" << endl;
        csv_file << "UnionFind_Only," << N << "," << E << "," << time_ms << "," << theoretical_metric / 1000.0 << endl;
    }
    csv_file.close();
    cout << "-----------------------------------------------" << endl;
    cout << "Experimental data has been uploaded to the kruskal_complexity.csv." << endl;
    return 0;
}
