#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <vector>
#include <fstream>
using namespace std;
using namespace chrono;

class Set {
private:
    struct Node {
        double value;
        Node* prev;
        Node* next;
        Node(double v) : value(v), prev(nullptr), next(nullptr) {}
    };

    Node* head;  // початок списку
    Node* tail;  // кінець списку
    size_t count;
    string name;

public:
    Set(const string& setName = "Set") : head(nullptr), tail(nullptr), count(0), name(setName) {}
    ~Set() { Clear(); }

    // Заборона копіювання (щоб не було помилок у роботі з вузлами)
    Set(const Set&) = delete;
    Set& operator=(const Set&) = delete;

    // Дозволити переміщення
    // Конструктор переміщення
    Set(Set&& other) noexcept
        : head(other.head), tail(other.tail), count(other.count), name(move(other.name)) {
        other.head = nullptr;
        other.tail = nullptr;
        other.count = 0;
    }
    // Оератор присвоєння переміщення
    Set& operator=(Set&& other) noexcept {
        if (this != &other) {
            Clear();
            head = other.head;
            tail = other.tail;
            count = other.count;
            name = move(other.name);

            other.head = nullptr;
            other.tail = nullptr;
            other.count = 0;
        }
        return *this;
    }

    // Перевірка, чи містить множина елемент
    bool Search(double x) const {
        Node* curr = head;
        while (curr) {
            if (curr->value == x) return true;
            if (curr->value > x) return false; // бо впорядковано
            curr = curr->next;
        }
        return false;
    }

    // Вставка у відсортоване місце
    bool Insert(double x) {
        if (Search(x)) return false; // дубль не вставляємо

        if (!head) { // якщо список порожній
            head = tail = new Node(x);
            ++count;
            return true;
        }

        Node* curr = head;
        while (curr && curr->value < x) {
            curr = curr->next;
        }

        if (!curr) {
            // вставка у кінець
            Node* node = new Node(x);
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
        else {
            Node* node = new Node(x);
            node->next = curr;
            node->prev = curr->prev;
            if (curr->prev) curr->prev->next = node;
            else head = node;
            curr->prev = node;
        }
        ++count;
        return true;
    }

    // Видалення елемента
    bool Delete(double x) {
        if (!Search(x)) return false; // якщо елемента немає - нічого не змінюємо

        Node* curr = head;
        while (curr) {
            if (curr->value == x) {
                if (curr->prev) curr->prev->next = curr->next;
                else head = curr->next;

                if (curr->next) curr->next->prev = curr->prev;
                else tail = curr->prev;

                delete curr;
                --count;
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    size_t size() const { return count; }
    bool empty() const { return count == 0; }

    // Друк множини
    void print() const {
        cout << name << " = { ";
        Node* curr = head;
        while (curr) {
            cout << curr->value;
            if (curr->next) cout << ", ";
            curr = curr->next;
        }
        cout << " }";
    }

    // Очистити список
    void Clear() {
        Node* curr = head;
        while (curr) {
            Node* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
        head = tail = nullptr;
        count = 0;
    }

    // Об'єднання множин
    static Set Union(const Set& A, const Set& B, const string& name = "Union") {
        Set result(name);

        // Додаємо всі елементи з А
        Node* currA = A.head;
        while (currA) {
            result.Insert(currA->value);
            currA = currA->next;
        }

        // Додаємо елементи з В, яких ще немає у А
        Node* currB = B.head;
        while (currB) {
            if (!result.Search(currB->value)) {
                result.Insert(currB->value);
            }
            currB = currB->next;
        }
        return move(result);
    }

    // Перетин множин
    static Set Intersection(const Set& A, const Set& B, const string& name = "Intersection") {
        Set result(name);

        Node* currA = A.head;
        while (currA) {
            if (B.Search(currA->value)) {
                result.Insert(currA->value);
            }
            currA = currA->next;
        }
        return move(result);
    }

    // Різниця множин
    static Set SetDifference(const Set& A, const Set& B, const string& name = "Difference") {
        Set result(name);

        Node* currA = A.head;
        while (currA) {
            if (!B.Search(currA->value)) {
                result.Insert(currA->value);
            }
            currA = currA->next;
        }
        return move(result);
    }

    // Симетрична різниця множин
    static Set SymDifference(const Set& A, const Set& B, const string& name = "SymDifference") {
        Set diff1 = Set::SetDifference(A, B, "diff1"); // A\B
        Set diff2 = Set::SetDifference(B, A, "diff2"); //B\A
        Set result = Set::Union(diff1, diff2, name);
        return move(result);
    }

    // Чи є одна множина підмножиною іншої?
    static bool IsSubset(const Set& A, const Set& B) {
        Node* currA = A.head;
        while (currA) {
            if (!B.Search(currA->value)) {
                return false;
            }
            currA = currA->next;
        }
        return true;
    }
};

// ---------------- Приклад використання ----------------
int main() {
    Set A ("A");
    A.Insert(3.14);
    A.Insert(2.71);
    A.Insert(1.41);
    A.Insert(3.14); // дубль не вставиться
    cout << "Set: ";
    A.print();
    cout << "\n";
    cout << "Is there 2.71? " << (A.Search(2.71) ? "Yes" : "No") << "\n";
    cout << "Is there 5? " << (A.Search(5) ? "Yes" : "No") << "\n";
    A.Delete(2.71);
    cout << "After deleting 2.71: ";
    A.print();
    cout << "\n";

    Set B("B");
    B.Insert(2.71);
    B.Insert(3.14);
    B.print();
    cout << "\n";

    Set C = Set::Union(A, B, "C");
    C.print();
    cout << "\n";


    Set D = Set::Intersection(A, B, "D");
    D.print();
    cout << "\n";

    Set E = Set::SetDifference(A, B, "E");
    E.print();
    cout << "\n";

    Set F = Set::SymDifference(A, B, "F");
    F.print();
    cout << "\n";

    cout << "Is A subset of B? " << (Set::IsSubset(A, B) ? "Yes" : "No") << "\n";
    cout << "Is B subset of A? " << (Set::IsSubset(B, A) ? "Yes" : "No") << "\n";
    cout << "Is A subset of A? " << (Set::IsSubset(A, A) ? "Yes" : "No") << "\n";

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dist(0.0, 10000.0);

    ofstream fout("results.csv");
    fout << "N;Search_hit(ns);Search_miss(ns);SetDifference(ms)\n";

    for (int N : {100, 500, 1000, 2000, 5000}) {
        Set A, B;
        vector<double> valuesA, valuesB;
        for (int i = 0; i < N; i++) {
            double valA = dist(gen);
            double valB = dist(gen);
            A.Insert(valA);
            B.Insert(valB);
            valuesA.push_back(valA);
            valuesB.push_back(valB);
        }

        // ~~~Search_hit~~~
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            A.Search(valuesA[i % valuesA.size()]);
        }
        auto end = high_resolution_clock::now();
        double avg_search_hit = duration_cast<nanoseconds>(end - start).count() / 1000.0;

        // ~~~Search_miss~~~
        start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            A.Search(1e9 + i);
        }
        end = high_resolution_clock::now();
        double avg_search_miss = duration_cast<nanoseconds>(end - start).count() / 1000.0;

        // ~~~SetDifference~~~
        start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            Set C = Set::SetDifference(A, B);
        }
        end = high_resolution_clock::now();
        double avg_diff = duration_cast<milliseconds>(end - start).count() / 1000.0;

        fout << N << ";" << avg_search_hit << ";" << avg_search_miss << ";" << avg_diff << "\n";
        cout << "N=" << N << "done\n";
    }

    fout.close();
    return 0;
}
