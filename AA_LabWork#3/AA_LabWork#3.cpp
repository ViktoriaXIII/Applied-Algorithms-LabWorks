#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <random>
#include <fstream>
#include <chrono>
#include <functional>
using namespace std;


double measureTime(function<void()> func) {
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> duration = end - start;
    return duration.count();
}
class Graph {
protected:
    int n;
    vector<list<int>> AdjList;
    virtual void generate_ER(double p) = 0;
public:
    Graph(int Vertices) : n(Vertices), AdjList(Vertices + 1) {}
    Graph(int Vertices, double p) : n(Vertices), AdjList(Vertices + 1) {
        generate_ER(p);
    }
    virtual ~Graph() {}

    void addVertex() {
        AdjList.push_back(list<int>());
        n++;
    }
    void removeVertex(int v) {
        if (v < 1 || v > n) return;
        for (int i = 1; i <= n; i++) {
            if (i != v) AdjList[i].remove(v);
        }
        AdjList[v].clear();
    }
    virtual void addEdge(int u, int v) = 0;
    virtual void removeEdge(int u, int v) = 0;
    virtual void printGraph() const = 0;

    vector<vector<int>> toAdjMatrix() const {
        vector<vector<int>> M(n + 1, vector<int>(n + 1, 0));
        for (int i = 1; i <= n; i++) {
            for (int j : AdjList[i]) {
                M[i][j] = 1;
            }
        }
        return M;
    }
    void fromAdjMatrix(const vector<vector<int>>& M) {
        n = (int)M.size() - 1;
        AdjList.assign(n + 1, {});
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (M[i][j] != 0) {
                    AdjList[i].push_back(j);
                }
            }
        }
    }
    void printAdjMatrix() const {
        auto M = toAdjMatrix();
        cout << "Adjancy matrix:\n";
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                cout << M[i][j] << " ";
            }
            cout << "\n";
        }
    }
};

class UndirectedGraph : public Graph {
protected:
    void generate_ER(double p) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        for (int i = 1; i <= n; i++) {
            for (int j = i + 1; j <= n; j++) {
                if (dist(gen) < p) {
                    addEdge(i, j);
                }
            }
        }
    }
private:
    void dfs_util(int v, vector<bool>& visited) const {
        visited[v] = true;
        // Проходимо по всіх сусідах
        for (int neighbor : AdjList[v]) {
            if (!visited[neighbor]) {
                dfs_util(neighbor, visited);
            }
        }
    }
public:
    UndirectedGraph(int Vertices) : Graph(Vertices) {}
    UndirectedGraph(int Vertices, double p) : Graph(Vertices) {
        generate_ER(p);
    }

    void addEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].push_back(v);
        AdjList[v].push_back(u);
    }
    void removeEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].remove(v);
        AdjList[v].remove(u);
    }
    void printGraph() const override {
        cout << "Undirected graph:\n";
        for (int i = 1; i <= n; i++) {
            cout << i << " -> ";
            for (int neighbor : AdjList[i])
                cout << neighbor << " ";
            cout << "\n";
        }
    }
    void exportToDOT(const string& filename) const {
        ofstream fout(filename);
        fout << "graph G {\n";
        for (int i = 0; i < n; i++) {
            fout << " " << i << ";\n";
            for (int neighbor : AdjList[i]) {
                if (i < neighbor)
                    fout << " " << i << "---" << neighbor << "\n";
            }
        }
        fout << "}\n";
        fout.close();
    }
    bool isConnected() const {
        if (n <= 1) return true;
        vector<bool> visited(n + 1, false);
        int start_vertex = 1;
        while (start_vertex <= n && AdjList[start_vertex].empty()) {
            start_vertex++;
        }
        if (start_vertex > n) return true;
        dfs_util(start_vertex, visited);
        for (int i = 1; i <= n; i++) {
            if (!AdjList[i].empty() && !visited[i]) {
                return false;
            }
        }
        return true;
    }
    vector<vector<int>> getTransitiveClosureMatrix() const {
        vector<vector<int>> closure = toAdjMatrix();
        for (int i = 1; i <= n; i++) {
            closure[i][i] = 1;
        }
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]);
                }
            }
        }
        return closure;
    }
};

class DirectedGraph : public Graph {
protected:
    void generate_ER(double p) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i != j && dist(gen) < p) {
                    addEdge(i, j);
                }
            }
        }
    }
private:
    void dfs_util(int v, vector<bool>& visited, const vector<list<int>>& currentAdjList) const {
        visited[v] = true;
        for (int neighbor : currentAdjList[v]) {
            if (!visited[neighbor]) {
                dfs_util(neighbor, visited, currentAdjList);
            }
        }
    }
    vector<list<int>> getTransposeAdjList() const {
        vector<list<int>> TransposeAdjList(n + 1);
        for (int u = 1; u <= n; ++u) {
            for (int v : AdjList[u]) {
                TransposeAdjList[v].push_back(u);
            }
        }
        return TransposeAdjList;
    }
public:
    DirectedGraph(int Vertices) : Graph(Vertices) {}
    DirectedGraph(int Vertices, double p) : Graph(Vertices) {
        generate_ER(p);
    }

    void addEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].push_back(v);
    }
    void removeEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].remove(v);
    }
    void printGraph() const override {
        cout << "Directed graph:\n";
        for (int i = 1; i <= n; i++) {
            cout << i << " -> ";
            for (int neighbor : AdjList[i])
                cout << neighbor << " ";
            cout << "\n";
        }
    }
    void exportToDOT(const string& filename) const {
        ofstream fout(filename);
        fout << "digraph G {\n";
        for (int i = 0; i < n; i++) {
            fout << " " << i << ";\n";
            for (int neighbor : AdjList[i]) {
                if (i < neighbor)
                    fout << " " << i << "->" << neighbor << "\n";
            }
        }
        fout << "}\n";
        fout.close();
    }
    bool isStronglyConnected() const {
        if (n <= 1) return true;
        auto TransposeAdjList = getTransposeAdjList();
        int start_vertex = 1;
        vector<int> relevant_vertices;

        for (int i = 1; i <= n; i++) {
            if (!AdjList[i].empty() || !TransposeAdjList[i].empty()) {
                relevant_vertices.push_back(i);
            }
        }
        if (relevant_vertices.empty()) return true;

        start_vertex = relevant_vertices[0];
        vector<bool> visited1(n + 1, false);
        dfs_util(start_vertex, visited1, AdjList);
        for (int v : relevant_vertices) {
            if (!visited1[v]) {
                return false;
            }
        }
        vector<bool> visited2(n + 1, false);
        dfs_util(start_vertex, visited2, TransposeAdjList);
        for (int v : relevant_vertices) {
            if (!visited2[v]) {
                return false;
            }
        }
        return true;
    }
    vector<vector<int>> getTransitiveClosureMatrix() const {
        vector<vector<int>> closure = toAdjMatrix();
        for (int i = 1; i <= n; i++) {
            closure[i][i] = 1;
        }
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]);
                }
            }
        }
        return closure;
    }
};

class WeightedGraph {
protected:
    int n;
    vector<list<pair<int, int>>> AdjList;
public:
    WeightedGraph(int Vertices) : n(Vertices), AdjList(Vertices + 1) {}
    virtual ~WeightedGraph() {}

    void addVertex() {
        AdjList.push_back(list<pair<int, int>>());
        n++;
    }
    void removeVertex(int v) {
        if (v < 1 || v > n) return;
        for (int i = 1; i <= n; i++) {
            if (i != v) {
                AdjList[i].remove_if([v](pair<int, int> p) {return p.first == v; });
            }
        }
        AdjList[v].clear();
    }
    virtual void addEdge(int u, int v, int w) = 0;
    virtual void removeEdge(int u, int v) = 0;
    virtual void printGraph() const = 0;

    vector<vector<int>> toAdjMatrix() const {
        vector<vector<int>> M(n + 1, vector<int>(n + 1, 0));
        for (int i = 1; i <= n; i++) {
            for (auto& p : AdjList[i]) {
                int j = p.first, w = p.second;
                M[i][j] = w;
            }
        }
        return M;
    }
    void fromAdjMatrix(const vector<vector<int>>& M) {
        n = (int)M.size() - 1;
        AdjList.assign(n + 1, {});
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (M[i][j] != 0) {
                    AdjList[i].push_back({ j, M[i][j] });
                }
            }
        }
    }
    void printAdjMatrix() const {
        auto M = toAdjMatrix();
        cout << "Weighted adjancy matrix:\n";
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                cout << M[i][j] << " ";
            }
            cout << "\n";
        }
    }
};

class WeightedUndirectedGraph : public WeightedGraph {
private:
    void dfs_util(int v, vector<bool>& visited) const {
        visited[v] = true;
        for (const auto& p : AdjList[v]) {
            int neighbor = p.first;
            if (!visited[neighbor]) {
                dfs_util(neighbor, visited);
            }
        }
    }
public:
    WeightedUndirectedGraph(int Vertices) : WeightedGraph(Vertices) {}
    WeightedUndirectedGraph(int Vertices, double p, int w_min, int w_max) : WeightedGraph(Vertices) {
        generate_ER(p, w_min, w_max);
    }
    void generate_ER(double p, int w_min, int w_max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> prob(0.0, 1.0);
        uniform_real_distribution<> weight(w_min, w_max);
        for (int i = 1; i <= n; i++) {
            for (int j = i + 1; j <= n; j++) {
                if (prob(gen) <= p) {
                    int w = weight(gen);
                    addEdge(i, j, w);
                }
            }
        }
    }

    void addEdge(int u, int v, int w) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].push_back({ v, w });
        AdjList[v].push_back({ u, w });
    }
    void removeEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].remove_if([v](pair<int, int> p) {return p.first == v; });
        AdjList[v].remove_if([u](pair<int, int> p) {return p.first == u; });
    }
    void printGraph() const override {
        cout << "Weighted undirected graph:\n";
        for (int i = 1; i <= n; i++) {
            cout << i << " -> ";
            for (auto& p : AdjList[i])
                cout << "(" << p.first << ", w=" << p.second << ") ";
            cout << "\n";
        }
    }
    void exportToDOT(const string& filename) const {
        ofstream fout(filename);
        fout << "Weighted undirected graph G {\n";
        for (int i = 0; i < n; i++) {
            fout << " " << i << ";\n";
            for (auto& neighbor : AdjList[i]) {
                fout << " " << i << "---" << neighbor.first << " [label=" << neighbor.second << "];\n";
            }
        }
        fout << "}\n";
        fout.close();
    }
    bool isConnected() const {
        if (n <= 1) return true;
        vector<bool> visited(n + 1, false);
        int start_vertex = 1;
        while (start_vertex <= n && AdjList[start_vertex].empty()) {
            start_vertex++;
        }
        if (start_vertex > n) return true;
        dfs_util(start_vertex, visited);
        for (int i = 1; i <= n; i++) {
            if (!AdjList[i].empty() && !visited[i]) {
                return false;
            }
        }
        return true;
    }
    vector<vector<int>> getTransitiveClosureMatrix() const {
        vector<vector<int>> closure = toAdjMatrix();
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                closure[i][j] = (closure[i][j] > 0) ? 1 : 0;
            }
        }
        for (int i = 1; i <= n; i++) {
            closure[i][i] = 1;
        }
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]);
                }
            }
        }
        return closure;
    }
};

class WeightedDirectedGraph : public WeightedGraph {
private:
    void dfs_util(int v, vector<bool>& visited, const vector<list<pair<int, int>>>& currentAdjList) const {
        visited[v] = true;
        for (const auto& p : currentAdjList[v]) {
            int neighbor = p.first;
            if (!visited[neighbor]) {
                dfs_util(neighbor, visited, currentAdjList);
            }
        }
    }
    vector<list<pair<int, int>>> getTransposeAdjList() const {
        vector<list<pair<int, int>>> TransposeAdjList(n + 1);
        for (int u = 1; u <= n; ++u) {
            for (const auto& p : AdjList[u]) {
                int v = p.first;
                int weight = p.second;
                TransposeAdjList[v].push_back({ u, weight });
            }
        }
        return TransposeAdjList;
    }
public:
    WeightedDirectedGraph(int Vertices) : WeightedGraph(Vertices) {}
    WeightedDirectedGraph(int Vertices, double p, int w_min, int w_max) : WeightedGraph(Vertices) {
        generate_ER(p, w_min, w_max);
    }
    void generate_ER(double p, int w_min, int w_max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> prob(0.0, 1.0);
        uniform_real_distribution<> weight(w_min, w_max);
        for (int i = 1; i <= n; i++) {
            for (int j = i + 1; j <= n; j++) {
                if (prob(gen) <= p) {
                    int w = weight(gen);
                    addEdge(i, j, w);
                }
            }
        }
    }

    void addEdge(int u, int v, int w) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].push_back({ v, w });
    }
    void removeEdge(int u, int v) override {
        if (u < 1 || u > n || v < 1 || v > n) return;
        AdjList[u].remove_if([v](pair<int, int> p) {return p.first == v; });
    }
    void printGraph() const override {
        cout << "Weighted directed graph:\n";
        for (int i = 1; i <= n; i++) {
            cout << i << " -> ";
            for (auto& p : AdjList[i])
                cout << "(" << p.first << ", w=" << p.second << ")";
            cout << "\n";
        }
    }
    void exportToDOT(const string& filename) const {
        ofstream fout(filename);
        fout << "digraph G {\n";
        for (int i = 0; i < n; i++) {
            fout << " " << i << ";\n";
            for (auto& neighbor : AdjList[i]) {
                fout << " " << i << "->" << neighbor.first << " [label=" << neighbor.second << "];\n";
            }
        }
        fout << "}\n";
        fout.close();
    }
    bool isStronglyConnected() const {
        if (n <= 1) return true;
        auto TransposeAdjList = getTransposeAdjList();
        int start_vertex = 1;
        vector<int> relevant_vertices;
        for (int i = 1; i <= n; i++) {
            if (!AdjList[i].empty() || !TransposeAdjList[i].empty()) {
                relevant_vertices.push_back(i);
            }
        }
        if (relevant_vertices.empty()) return true;
        start_vertex = relevant_vertices[0];
        vector<bool> visited1(n + 1, false);
        dfs_util(start_vertex, visited1, AdjList);
        for (int v : relevant_vertices) {
            if (!visited1[v]) {
                return false;
            }
        }
        vector<bool> visited2(n + 1, false);
        dfs_util(start_vertex, visited2, TransposeAdjList);

        for (int v : relevant_vertices) {
            if (!visited2[v]) {
                return false;
            }
        }
        return true;
    }
    vector<vector<int>> getTransitiveClosureMatrix() const {
        vector<vector<int>> closure = toAdjMatrix();
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                closure[i][j] = (closure[i][j] > 0) ? 1 : 0;
            }
        }
        for (int i = 1; i <= n; i++) {
            closure[i][i] = 1;
        }
        for (int k = 1; k <= n; k++) {
            for (int i = 1; i <= n; i++) {
                for (int j = 1; j <= n; j++) {
                    closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]);
                }
            }
        }
        return closure;
    }
};

void printTransitiveClosureCheck(const vector<vector<int>>& closure, const string& graphName, bool isDirected) {
    int n = closure.size() - 1;
    bool isConnected = true;
    bool foundRelevant = false;
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            if (closure[i][j] == 1 && i != j) {
                foundRelevant = true;
                break;
            }
        }
    }
    if (!foundRelevant) {
        cout << graphName << " (Warshall): Graph is empty. Connected: YES (trivial).\n";
        return;
    }
    if (isDirected) {
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i != j && closure[i][i] == 1 && closure[j][j] == 1) {
                    if (closure[i][j] == 0) {
                        isConnected = false;
                        break;
                    }
                }
            }
            if (!isConnected) break;
        }
        cout << graphName << " (Warshall): Graph is strongly connected? " << (isConnected ? "YES" : "NO") << "\n";
    }
    else {
        int start_vertex = 0;
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i != j && closure[i][j] == 1) {
                    start_vertex = i;
                    break;
                }
            }
            if (start_vertex != 0) break;
        }
        if (start_vertex == 0) {
            cout << graphName << " (Warshall): Graph is connected? YES (only isolated vertices stayed).\n";
            return;
        }
        for (int i = 1; i <= n; i++) {
            if (closure[i][i] == 1 && closure[start_vertex][i] == 0) {
                isConnected = false;
                break;
            }
        }
        cout << graphName << " (Warshall): Graph is connected? " << (isConnected ? "YES" : "NO") << "\n";
    }
}

int main()
{
    UndirectedGraph UDG_1(6);
    UDG_1.addEdge(1, 2);
    UDG_1.addEdge(3, 4);
    UDG_1.addEdge(5, 6);
    UDG_1.printGraph();
    UDG_1.addVertex();
    UDG_1.printGraph();
    UDG_1.addEdge(7, 3);
    UDG_1.printGraph();
    UDG_1.printAdjMatrix();
    UDG_1.exportToDOT("UDG_1.dot");
    cout << "--- DFS for UDG_1 ---\n";
    cout << "Is UDG_1 connected? " << (UDG_1.isConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for UDG_1 ---\n";
    printTransitiveClosureCheck(UDG_1.getTransitiveClosureMatrix(), "UDG_1", false);

    DirectedGraph DG_2(7);
    DG_2.addEdge(1, 2);
    DG_2.addEdge(4, 3);
    DG_2.addEdge(5, 6);
    DG_2.printGraph();
    DG_2.addVertex();
    DG_2.printGraph();
    DG_2.addEdge(3, 4);
    DG_2.addEdge(7, 8);
    DG_2.printGraph();
    DG_2.exportToDOT("DG_2.dot");
    cout << "--- DFS for DG_2 ---\n";
    cout << "Is DG_2 strongly connected? " << (DG_2.isStronglyConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for DG_2 ---\n";
    printTransitiveClosureCheck(DG_2.getTransitiveClosureMatrix(), "DG_2", true);

    WeightedUndirectedGraph WUDG_3(8);
    WUDG_3.addEdge(2, 1, 11);
    WUDG_3.addEdge(7, 8, 2);
    WUDG_3.addEdge(7, 2, 7);
    WUDG_3.printGraph();
    WUDG_3.addVertex();
    WUDG_3.printGraph();
    WUDG_3.addEdge(9, 3, 5);
    WUDG_3.printGraph();
    WUDG_3.exportToDOT("WUDG_3.dot");
    cout << "--- DFS for WUDG_3 ---\n";
    cout << "Is WUDG_3 connected? " << (WUDG_3.isConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for WUDG_3 ---\n";
    printTransitiveClosureCheck(WUDG_3.getTransitiveClosureMatrix(), "WUDG_3", false);

    WeightedDirectedGraph WDG_4(9);
    WDG_4.printGraph();
    WDG_4.addEdge(3, 6, 1);
    WDG_4.addEdge(7, 9, 8);
    WDG_4.addEdge(4, 2, 6);
    WDG_4.printGraph();
    WDG_4.addVertex();
    WDG_4.printGraph();
    WDG_4.addEdge(9, 10, 0);
    WDG_4.printGraph();
    WDG_4.printAdjMatrix();
    WDG_4.exportToDOT("WDG_4.dot");
    cout << "--- DFS for WDG_4 ---\n";
    cout << "Is WDG_4 strongly connected? " << (WDG_4.isStronglyConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for WDG_4 ---\n";
    printTransitiveClosureCheck(WDG_4.getTransitiveClosureMatrix(), "WDG_4", true);

    UndirectedGraph UDG_5_ER(10, 0.5);
    UDG_5_ER.printGraph();
    UDG_5_ER.exportToDOT("UDG_5_ER.dot");
    cout << "--- DFS for UDG_5 ---\n";
    cout << "Is UDG_5_ER connected? " << (UDG_5_ER.isConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for UDG_5_ER ---\n";
    printTransitiveClosureCheck(UDG_5_ER.getTransitiveClosureMatrix(), "UDG_5", false);

    DirectedGraph DG_6_ER(10, 0.5);
    DG_6_ER.printGraph();
    DG_6_ER.exportToDOT("DG_6_ER.dot");
    cout << "--- DFS for DG_6_ER ---\n";
    cout << "Is DG_6_ER strongly connected? " << (DG_6_ER.isStronglyConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for DG_6_ER ---\n";
    printTransitiveClosureCheck(DG_6_ER.getTransitiveClosureMatrix(), "DG_6_ER", true);

    WeightedUndirectedGraph WUDG_7_ER(15, 0.9, 1, 15);
    WUDG_7_ER.printGraph();
    WUDG_7_ER.exportToDOT("WUDG_7_ER.dot");
    cout << "--- DFS for WUDG_7_ER ---\n";
    cout << "Is WUDG_7_ER connected? " << (WUDG_7_ER.isConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for WUDG_7_ER ---\n";
    printTransitiveClosureCheck(WUDG_7_ER.getTransitiveClosureMatrix(), "WUDG_7_ER", false);

    WeightedDirectedGraph WDG_8_ER(15, 0.9, 1, 15);
    WDG_8_ER.printGraph();
    WDG_8_ER.exportToDOT("WDG_8_ER.dot");
    cout << "--- DFS for WDG_8_ER ---\n";
    cout << "Is WDG_8_ER strongly connected? " << (WDG_8_ER.isStronglyConnected() ? "YES" : "NO") << "\n";
    cout << "--- Warshall's for WDG_8_ER ---\n";
    printTransitiveClosureCheck(WDG_8_ER.getTransitiveClosureMatrix(), "WDG_8_ER", true);

    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout << "Comparison of algorithms execution time (DFS vs WARSHALL) \n";
    cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

    int V_large = 200;
    double P_density = 0.1;
    int W_min = 1;
    int W_max = 100;

    UndirectedGraph UDG_Benchmark(V_large, P_density);
    cout << "UndirectedGraph with V=" << V_large << " and P=" << P_density << " was created\n";
    double time_dfs_udg = measureTime([&UDG_Benchmark]() {
        UDG_Benchmark.isConnected();
        });
    cout << "Time of DFS (isConnected): " << time_dfs_udg << " ms\n";
    double time_warshall_udg = measureTime([&UDG_Benchmark]() {
        UDG_Benchmark.getTransitiveClosureMatrix();
        });
    cout << "Time of Warshall (Transitive Closure): " << time_warshall_udg << " ms\n";

    DirectedGraph DG_Benchmark(V_large, P_density);
    cout << "DirectedGraph with V=" << V_large << " and P=" << P_density << " was created\n";
    double time_dfs_dg = measureTime([&DG_Benchmark]() {
        DG_Benchmark.isStronglyConnected();
        });
    cout << "Time of DFS (isStronglyConnected): " << time_dfs_dg << " ms\n";
    double time_warshall_dg = measureTime([&DG_Benchmark]() {
        DG_Benchmark.getTransitiveClosureMatrix();
        });
    cout << "Time of Warshall (Transitive Closure): " << time_warshall_dg << " ms\n";

    WeightedUndirectedGraph WUDG_Benchmark(V_large, P_density, W_min, W_max);
    cout << "WeightedUndirectedGraph with V=" << V_large << ", P=" << P_density << ", W_min=" << W_min << " and W_max=" << W_max << " was created\n";
    double time_dfs_wudg = measureTime([&WUDG_Benchmark]() {
        WUDG_Benchmark.isConnected();
        });
    cout << "Time of DFS (isConnected): " << time_dfs_wudg << " ms\n";
    double time_warshall_wudg = measureTime([&WUDG_Benchmark]() {
        WUDG_Benchmark.getTransitiveClosureMatrix();
        });
    cout << "Time of Warshall (Transitive Closure): " << time_warshall_wudg << " ms\n";

    WeightedDirectedGraph WDG_Benchmark(V_large, P_density, W_min, W_max);
    cout << "WeightedDirectedGraph with V=" << V_large << ", P=" << P_density << ", W_min=" << W_min << " and W_max=" << W_max << " was created\n";
    double time_dfs_wdg = measureTime([&WDG_Benchmark]() {
        WDG_Benchmark.isStronglyConnected();
        });
    cout << "Time of DFS (isStronglyConnected): " << time_dfs_wdg << " ms\n";
    double time_warshall_wdg = measureTime([&WDG_Benchmark]() {
        WDG_Benchmark.getTransitiveClosureMatrix();
        });
    cout << "Time of Warshall (Transitive Closure): " << time_warshall_wdg << " ms\n";
}