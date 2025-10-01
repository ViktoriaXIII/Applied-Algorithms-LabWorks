#include <iostream>
#include <vector>
#include <list>
#include <utility>
#include <random>
#include <fstream>
using namespace std;

// Базовий клас
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

//Неорієнтований граф
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
};

// Орієнтований граф
class DirectedGraph : public Graph {
protected:
    void generate_ER(double p) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dist(0.0, 1.0);
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                if (i != j && dist(gen) <p) {
                    addEdge(i, j);
                }
            }
        }
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
};

// Базовий клас для Зважених
class WeightedGraph {
protected:
    int n;
    vector<list<pair<int, int>>> AdjList;
public:
    WeightedGraph(int Vertices) : n(Vertices), AdjList(Vertices + 1) {}
    virtual ~WeightedGraph() {}

    void addVertex() {
        AdjList.push_back(list<pair<int,int>>());
        n++;
    }
    void removeVertex(int v) {
        if (v < 1 || v > n) return;
        for (int i = 1; i <= n; i++) {
            if (i != v) {
                AdjList[i].remove_if([v](pair<int, int> p) {return p.first == v;});
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
            for (auto&p : AdjList[i]) {
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

// Неорієнтований Зважений граф
class WeightedUndirectedGraph : public WeightedGraph {
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
        AdjList[u].remove_if([v](pair<int, int> p) {return p.first == v;});
        AdjList[v].remove_if([u](pair<int, int> p) {return p.first == u; });
    }
    void printGraph() const override {
        cout << "graph:\n";
        for (int i = 1; i <= n; i++) {
            cout << i << " -> ";
            for (auto &p : AdjList[i])
                cout << "(" << p.first << ", w=" << p.second << ") ";
            cout << "\n";
        }
    }
    void exportToDOT(const string& filename) const {
        ofstream fout(filename);
        fout << "graph G {\n";
        for (int i = 0; i < n; i++) {
            fout << " " << i << ";\n";
            for (auto &neighbor : AdjList[i]) {
                fout << " " << i << "---" << neighbor.first << " [label=" << neighbor.second << "];\n";
            }
        }
        fout << "}\n";
        fout.close();
    }
};

// Орієнтований Зважений граф
class WeightedDirectedGraph : public WeightedGraph {
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
};

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

    UndirectedGraph UDG_5_ER(10, 0.5);
    UDG_5_ER.printGraph();
    UDG_5_ER.exportToDOT("UDG_5_ER.dot");

    DirectedGraph DG_6_ER(10, 0.5);
    DG_6_ER.printGraph();
    DG_6_ER.exportToDOT("DG_6_ER.dot");

    WeightedUndirectedGraph WUDG_7_ER(15, 0.9, 1, 15);
    WUDG_7_ER.printGraph();
    WUDG_7_ER.exportToDOT("WUDG_7_ER.dot");

    WeightedDirectedGraph WDG_8_ER(15, 0.9, 1, 15);
    WDG_8_ER.printGraph();
    WDG_8_ER.exportToDOT("WDG_8_ER.dot");
}