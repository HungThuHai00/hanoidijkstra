#include <iostream>
#include <algorithm>
#include <vector>
#include <chrono>
#include <fstream>
#include <stdexcept>

using namespace std;
using namespace std::chrono;

// TIMING TEMPLATE 
template <typename F>
double timeIt(F work, int repeats = 5) {
    double best = 1e18;
    for (int r = 0; r < repeats; ++r) {
        auto t0 = high_resolution_clock::now();
        work();
        auto t1 = high_resolution_clock::now();
        double ms = duration<double, milli>(t1 - t0).count();
        best = min(best, ms);
    }
    return best; 
}

// CUSTOM MIN-HEAP 
struct CustomPriorityQueue {
    vector<pair<long long, int>> heap;

    bool empty() const { return heap.empty(); }

    void push(pair<long long, int> val) {
        heap.push_back(val);
        int idx = heap.size() - 1;
        while (idx > 0) {
            int parent = (idx - 1) / 2;
            if (heap[idx].first < heap[parent].first) {
                swap(heap[idx], heap[parent]);
                idx = parent;
            } else break;
        }
    }

    pair<long long, int> top() const {
        return heap[0];
    }

    void pop() {
        if (heap.empty()) return;
        heap[0] = heap.back();
        heap.pop_back();
        int idx = 0;
        int size = heap.size();
        while (2 * idx + 1 < size) {
            int left = 2 * idx + 1;
            int right = 2 * idx + 2;
            int smallest = idx;

            if (left < size && heap[left].first < heap[smallest].first) smallest = left;
            if (right < size && heap[right].first < heap[smallest].first) smallest = right;

            if (smallest != idx) {
                swap(heap[idx], heap[smallest]);
                idx = smallest;
            } else break;
        }
    }
};

const int Max = 200005; // Fixed stack limit to handle n >= 100000
const long long int IFN = 1e18;

int n, m;
vector<pair<int, long long>> adj[Max];
long long dist[Max];

void addEdge(int u, int v, long long w) {
    // Edge case: Out of range check
    if (u < 1 || u > n || v < 1 || v > n) {
        throw out_of_range("Station index out of range!");
    }
    // Edge case: Duplicate keys 
    for (auto& edge : adj[u]) {
        if (edge.first == v) {
            edge.second = min(edge.second, w);
            return;
        }
    }
    adj[u].push_back({v, w});
    adj[v].push_back({u, w});
}

void dijkstra(int s){
    if (n <= 0) throw invalid_argument("Empty input graph!");
    if (s < 1 || s > n) throw out_of_range("Start station out of range!");

    for(int i=1; i<=n; i++)
        dist[i]=IFN;

    dist[s]=0;

    CustomPriorityQueue pq;
    pq.push({0, s});

    while(!pq.empty()){
        long long d=pq.top().first;
        int u=pq.top().second;
        pq.pop();

        if(d>dist[u]) continue;

        for(auto edge: adj[u]){
            int v=edge.first;
            long long w=edge.second;

            if(dist[u]+w<dist[v]){
                dist[v]=dist[u]+w;
                pq.push({dist[v], v});
            }
        }
    }
}

void resetGraph() {
    for (int i = 0; i < Max; ++i) adj[i].clear();
}

int main(){
    cout << "========================================================\n";
    cout << "  G6: HANOI METRO ROUTE OPTIMIZATION (DIJKSTRA DEMO)\n";
    cout << "========================================================\n\n";

    // ------------------------------------------------------------------------
    // DATA VERIFICATION
    // ------------------------------------------------------------------------
    cout << "[STEP 1] Data Verification:\n";
    ifstream fin("hanoidemo.txt");
    if (!fin.is_open()) {
        cout << "Cannot open hanoidemo.txt\n";
        return 1;
    }
    /*ifstream fin("hanoidemo2.txt");
    if (!fin.is_open()) {
        cout << "Cannot open hanoidemo2.txt\n";
        return 1;
    }*/
    fin >> n >> m;
    int target_n = n; // Save n read from file
    cout << "  - Input dataset loaded: n = " << n << " stations, m = " << m << " lines.\n\n";

    // ------------------------------------------------------------------------
    // EDGE CASE EXECUTION
    // ------------------------------------------------------------------------
    cout << "[STEP 2] Edge Case Execution:\n";

    // Edge Case 1: Out of range access
    try {
        cout << "  - Out-of-range station query... ";
        int temp_n = n;
        n = 10;
        dijkstra(999);
        n = temp_n;
    } catch (const exception& e) {
        cout << "PASSED -> " << e.what() << "\n";
    }

    // Edge Case 2: Duplicate key handling
    try {
        cout << "  - Duplicate key/edge insertion... ";
        int old_n = n;
        n = 50;
        resetGraph();
        addEdge(1, 2, 100);
        addEdge(1, 2, 40); 
        dijkstra(1);
        if (dist[2] == 40) cout << "PASSED -> Updated weight to minimum (40).\n";
        resetGraph();
        n = old_n;
    } catch (const exception& e) {
        cout << "FAILED -> " << e.what() << "\n";
    }
    cout << "\n";

    // ------------------------------------------------------------------------
    // CORE ALGORITHM EXECUTION (SMALL SUBSET n = 50)
    // ------------------------------------------------------------------------
    cout << "[STEP 3] Core Algorithm Verification (Small subset n = 50):\n";
    int old_n = n;
    n = 50;
    resetGraph();
    for(int i = 1; i < 50; ++i) addEdge(i, i + 1, 120);
    dijkstra(1);

    cout << "  - Shortest path distance from Station 1 to 50: " << dist[50] << "\n";
    resetGraph();
    n = old_n;
    cout << "\n";

    // ------------------------------------------------------------------------
    // STRESS TEST & PROFILING (n >= 100,000)
    // ------------------------------------------------------------------------
    fin.close();
    fin.open("hanoidemo.txt");
    fin >> n >> m; 
    resetGraph();

    cout << "[STEP 4] Stress Test & Profiling (n = " << n << "):\n";
    
    for(int i = 0; i < m; i++){
        int u, v;
        long long w;
        if (fin >> u >> v >> w) {
            addEdge(u, v, w);
        }
    }
    fin.close();

    double ms = timeIt([&]() {
        dijkstra(1);
    }, 5);

    cout << "Shortest path length:\n";
    for(int i = 1; i <= min(n, 10); i++){
        if(dist[i] == IFN) cout << "-1 ";
        else cout << dist[i] << " ";
    }
    cout << "\n";

    printf("BestCase: %.4f ms\n", ms);

    return 0;
}