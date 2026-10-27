#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <tuple>
#include <vector>

#include "dijkstra.h"

using namespace std;
using namespace std::chrono;

// Hàm phụ trợ chuyển đổi danh sách cạnh sang cấu trúc CSR
CSRGraph build_csr(int n, const vector<tuple<int, int, int>>& edges) {
    CSRGraph g;
    g.offset.assign(n + 2, 0);
    for (const auto& edge : edges) {
        g.offset[get<0>(edge) + 1]++;
        g.offset[get<1>(edge) + 1]++;
    }
    for (int i = 1; i <= n + 1; ++i) g.offset[i] += g.offset[i - 1];
    g.to.resize(g.offset[n + 1]);
    g.weight.resize(g.offset[n + 1]);
    vector<int> curr = g.offset;
    for (const auto& edge : edges) {
        int u = get<0>(edge), v = get<1>(edge), w = get<2>(edge);
        g.to[curr[u]] = v;
        g.weight[curr[u]++] = w;
        g.to[curr[v]] = u;
        g.weight[curr[v]++] = w;
    }
    return g;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int mode;
    if (!(cin >> mode)) return 1;

    if (mode == 1) {
        // ==========================================
        // CHẠY ĐỒ ÁN 2: HEAPSORT (CÓ LOG)
        // ==========================================
        int n;
        cin >> n;
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];
        heapsort(a);
        cout << "ket_qua=[";
        for (int i = 1; i <= n; ++i) cout << a[i] << (i == n ? "" : ",");
        cout << "];\n";
    } else if (mode == 2) {
        // ==========================================
        // CHẠY ĐỒ ÁN 1: DIJKSTRA (CÓ LOG ĐỂ TEST)
        // ==========================================
        int n, m;
        cin >> n >> m;
        vector<tuple<int, int, int>> edges;
        int max_w = 0;
        bool hasNegativeWeight = false;
        for (int i = 0; i < m; ++i) {
            int u, v, w;
            cin >> u >> v >> w;
            if (w < 0) hasNegativeWeight = true;
            edges.push_back({u, v, w});
            max_w = max(max_w, w);
        }
        int s, t;
        cin >> s >> t;
        if (hasNegativeWeight) {
            cout << "LOI: Do thi chua canh co trong so am. Dijkstra khong the "
                    "chay.\n";
            return 1;
        }

        CSRGraph adj = build_csr(n, edges);
        Counters ctr;
        vector<int> dist, parent;
        dijkstra_csr(n, s, adj, dist, parent, ctr);

        cout << "d=[";
        for (int i = 1; i <= n; ++i) {
            if (dist[i] == INF)
                cout << "INF";
            else
                cout << dist[i];
            cout << (i == n ? "" : ", ");
        }
        cout << "];\n";

        vector<int> path = truy_vet(s, t, dist, parent);
        if (path.empty())
            cout << "KHONG CO DUONG\n";
        else {
            cout << "duong=[";
            for (size_t i = 0; i < path.size(); ++i)
                cout << path[i] << (i == path.size() - 1 ? "" : ", ");
            cout << "]; tong=" << dist[t] << "\n";
        }
    } else if (mode == 3) {
        // ==========================================
        // BENCHMARK TẤT CẢ THUẬT TOÁN ĐỒ THỊ
        // ==========================================
        int n, m;
        cin >> n >> m;
        vector<tuple<int, int, int>> edges;
        int max_w = 0;
        for (int i = 0; i < m; ++i) {
            int u, v, w;
            cin >> u >> v >> w;
            edges.push_back({u, v, w});
            max_w = max(max_w, w);
        }
        int s, t;
        cin >> s >> t;
        CSRGraph adj = build_csr(n, edges);
        Counters ctr;
        vector<int> dist, parent;

        // 1. Dijkstra CSR
        auto st1 = high_resolution_clock::now();
        dijkstra_csr(n, s, adj, dist, parent, ctr);
        auto en1 = high_resolution_clock::now();

        // 2. Dial
        auto st2 = high_resolution_clock::now();
        dijkstra_dial(n, s, adj, max_w, dist, parent, ctr);
        auto en2 = high_resolution_clock::now();

        // 3. Bidirectional
        auto st3 = high_resolution_clock::now();
        int b_total;
        bidirectional_dijkstra(n, s, t, adj, b_total);
        auto en3 = high_resolution_clock::now();

        // 4. A*
        auto st4 = high_resolution_clock::now();
        int a_total;
        a_star(n, s, t, adj, a_total);
        auto en4 = high_resolution_clock::now();

        cout << "Dijkstra (CSR)       : "
             << duration_cast<microseconds>(en1 - st1).count() << " us\n";
        cout << "Dial's Algorithm     : "
             << duration_cast<microseconds>(en2 - st2).count() << " us\n";
        cout << "Bidirectional (2-way): "
             << duration_cast<microseconds>(en3 - st3).count() << " us\n";
        cout << "A* (Heuristic=0)     : "
             << duration_cast<microseconds>(en4 - st4).count() << " us\n";
    } else if (mode == 4) {
        // ==========================================
        // BENCHMARK HEAPSORT
        // ==========================================
        int n;
        cin >> n;
        vector<int> a(n + 1);
        for (int i = 1; i <= n; ++i) cin >> a[i];

        auto st = high_resolution_clock::now();
        heapsort_no_log(a);
        auto en = high_resolution_clock::now();

        cout << "Heapsort             : "
             << duration_cast<microseconds>(en - st).count() << " us\n";
    }
    return 0;
}