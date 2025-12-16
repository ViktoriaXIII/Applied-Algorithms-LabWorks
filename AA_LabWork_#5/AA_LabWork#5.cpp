#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <iomanip>

using namespace std;

class Vector {
public:
    vector<double> data;
    int size;
    Vector(int n) : size(n), data(n, 0.0) {}
    Vector(initializer_list<double> list) : data(list), size(list.size()) {}
    double& operator[](int i) { return data[i]; }
    const double& operator[](int i) const { return data[i]; }
    void print() const {
        for (double x_val : data) {
            if (abs(x_val) < 1e-12) x_val = 0.0;
            cout << setw(10) << fixed << setprecision(4) << x_val << " ";
        }
        cout << endl;
    }
};

class Matrix {
public:
    int n;
    vector<vector<double>> data;
    Matrix(int size) : n(size), data(size, vector<double>(size, 0.0)) {}
    static Matrix identity(int n) {
        Matrix res(n);
        for (int i = 0; i < n; i++) res[i][i] = 1.0;
        return res;
    }
    vector<double>& operator[](int i) { return data[i]; }
    const vector<double>& operator[](int i) const { return data[i]; }
    Matrix operator+(const Matrix& other) const {
        Matrix res(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++) res[i][j] = data[i][j] + other[i][j];
        return res;
    }
    Matrix operator*(const Matrix& other) const {
        Matrix res(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                for (int k = 0; k < n; k++)
                    res[i][j] += data[i][k] * other[k][j];
        return res;
    }
    Vector operator*(const Vector& v) const {
        Vector res(n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                res[i] += data[i][j] * v[j];
        return res;
    }
    void print() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) cout << setw(10) << data[i][j] << " ";
            cout << endl;
        }
    }
};

class LUPMethod {
public:
    Matrix L, U;
    vector<int> P;
    int n;
    LUPMethod(const Matrix& A) : L(A.n), U(A.n), n(A.n), P(A.n) {
        decompose(A);
    }

private:
    void decompose(const Matrix& A) {
        U = A;
        L = Matrix::identity(n);
        for (int i = 0; i < n; i++) P[i] = i;
        for (int i = 0; i < n; i++) {
            double max_val = 0;
            int pivot_row = i;
            for (int k = i; k < n; k++) {
                if (abs(U[k][i]) > max_val) {
                    max_val = abs(U[k][i]);
                    pivot_row = k;
                }
            }
            if (max_val < 1e-9) throw runtime_error("Matrix is degenerate!");
            swap(U.data[i], U.data[pivot_row]);
            swap(P[i], P[pivot_row]);
            for (int k = 0; k < i; k++) swap(L[i][k], L[pivot_row][k]);
            for (int j = i + 1; j < n; j++) {
                L[j][i] = U[j][i] / U[i][i];
                for (int k = i; k < n; k++) {
                    U[j][k] -= L[j][i] * U[i][k];
                }
            }
        }
    }

public:
    Vector solve(const Vector& b) {
        Vector b_p(n);
        for (int i = 0; i < n; i++) b_p[i] = b[P[i]];
        Vector y(n);
        for (int i = 0; i < n; i++) {
            double sum = 0;
            for (int j = 0; j < i; j++) sum += L[i][j] * y[j];
            y[i] = b_p[i] - sum;
        }
        Vector x(n);
        for (int i = n - 1; i >= 0; i--) {
            double sum = 0;
            for (int j = i + 1; j < n; j++) sum += U[i][j] * x[j];
            x[i] = (y[i] - sum) / U[i][i];
        }
        return x;
    }
};

int main() {
    try {
        int size = 10;
        Matrix A(size);
        A.data = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
            {1, 1, 2, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 2, 1, 1, 1, 1, 1, 1},
            {2, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 2, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 2, 1, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 2, 1, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 2}
        };
        Vector b = {55, 385, 58, 59, 56, 57, 61, 62, 63, 65};
        cout << "Matrix A:" << endl; A.print();
        cout << "Vector b:" << endl; b.print();
        LUPMethod equation_1(A);
        Vector x = equation_1.solve(b);
        cout << "\nSolution x:" << endl;
        x.print();
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}