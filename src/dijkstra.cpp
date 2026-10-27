#include "dijkstra.h"

#include <algorithm>
#include <queue>

using namespace std;

// ===========================================================================
// PHÉP NỚI LỎNG (RELAXATION)
// ===========================================================================
/*
 * Tư duy: Cập nhật khoảng cách tới v nếu tìm được đường đi tốt hơn thông qua u.
 * Nếu dist[v] được cải thiện, ghi nhận u là cha của v để phục vụ truy vết.
 */
bool noi_long(int u, int v, int w, vector<int>& dist, vector<int>& parent,
              Counters& ctr) {
    ctr.relaxations++;
    if (dist[u] != INF && dist[u] + w < dist[v]) {
        dist[v] = dist[u] + w;
        parent[v] = u;
        ctr.updates++;
        return true;
    }
    return false;
}

// ===========================================================================
// DIJKSTRA CSR (SỬ DỤNG CUSTOM MIN-HEAP)
// ===========================================================================
/*
 * Tư duy: Kết hợp cấu trúc MinHeap tự chế và mảng CSR.
 * - Duyệt cạnh bằng mảng CSR giúp tối ưu Cache.
 * - Dùng Lazy Deletion (bỏ qua nếu d > dist[u]) thay vì Decrease-Key.
 */
void dijkstra_csr(int n, int s, const CSRGraph& adj, vector<int>& dist,
                  vector<int>& parent, Counters& ctr) {
    dist.assign(n + 1, INF);
    parent.assign(n + 1, -1);
    dist[s] = 0;

    MinHeap pq(n * n);
    pq.push(0, s);

    while (!pq.empty()) {
        auto top = pq.pop();
        int d = top.first;
        int u = top.second;

        ctr.pops++;
        if (d > dist[u]) continue;  // Lazy Deletion

        for (int i = adj.offset[u]; i < adj.offset[u + 1]; ++i) {
            int v = adj.to[i];
            int w = adj.weight[i];
            if (noi_long(u, v, w, dist, parent, ctr)) {
                pq.push(dist[v], v);
            }
        }
    }
}

// ===========================================================================
// DIAL'S ALGORITHM (BUCKET QUEUE)
// ===========================================================================
/*
 * Tư duy: Thay thế Min-Heap bằng mảng các "xô" (buckets).
 * Vì trọng số là số nguyên nhỏ, khoảng cách tối đa bị chặn trên (max_dist).
 * Độ phức tạp giảm từ O((V+E)logV) xuống O(E + V*C).
 */
void dijkstra_dial(int n, int s, const CSRGraph& adj, int max_weight,
                   vector<int>& dist, vector<int>& parent, Counters& ctr) {
    dist.assign(n + 1, INF);
    parent.assign(n + 1, -1);
    dist[s] = 0;

    int max_dist = n * max_weight + 1;
    vector<vector<int>> buckets(max_dist + 1);
    buckets[0].push_back(s);

    for (int d = 0; d <= max_dist; ++d) {
        while (!buckets[d].empty()) {
            int u = buckets[d].back();
            buckets[d].pop_back();
            ctr.pops++;
            if (d > dist[u]) continue;

            for (int i = adj.offset[u]; i < adj.offset[u + 1]; ++i) {
                int v = adj.to[i], w = adj.weight[i];
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    parent[v] = u;
                    ctr.updates++;
                    buckets[dist[v]].push_back(v);
                }
            }
        }
    }
}

// ===========================================================================
// BIDIRECTIONAL DIJKSTRA (TÌM KIẾM 2 CHIỀU)
// ===========================================================================
/*
 * Tư duy: Chạy 2 luồng Dijkstra song song từ s và t.
 * Khi 2 vùng tìm kiếm gặp nhau tại đỉnh meet_node, ta có đường đi ngắn nhất.
 * Thu hẹp không gian tìm kiếm (2 hình tròn bán kính R/2 thay vì 1 hình tròn R).
 */
vector<int> bidirectional_dijkstra(int n, int s, int t, const CSRGraph& adj,
                                   int& total_weight) {
    if (s == t) {
        total_weight = 0;
        return {s};
    }
    vector<int> dist_s(n + 1, INF), dist_t(n + 1, INF);
    vector<int> parent_s(n + 1, -1), parent_t(n + 1, -1);

    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq_s, pq_t;
    dist_s[s] = 0;
    pq_s.push({0, s});
    dist_t[t] = 0;
    pq_t.push({0, t});
    int best_dist = INF, meet_node = -1;

    while (!pq_s.empty() && !pq_t.empty()) {
        if (pq_s.top().first + pq_t.top().first >= best_dist) break;

        if (pq_s.top().first <= pq_t.top().first) {
            int d = pq_s.top().first, u = pq_s.top().second;
            pq_s.pop();
            if (d > dist_s[u]) continue;
            for (int i = adj.offset[u]; i < adj.offset[u + 1]; ++i) {
                int v = adj.to[i], w = adj.weight[i];
                if (dist_s[u] + w < dist_s[v]) {
                    dist_s[v] = dist_s[u] + w;
                    parent_s[v] = u;
                    pq_s.push({dist_s[v], v});
                    if (dist_s[v] + dist_t[v] < best_dist) {
                        best_dist = dist_s[v] + dist_t[v];
                        meet_node = v;
                    }
                }
            }
        } else {
            int d = pq_t.top().first, u = pq_t.top().second;
            pq_t.pop();
            if (d > dist_t[u]) continue;
            for (int i = adj.offset[u]; i < adj.offset[u + 1]; ++i) {
                int v = adj.to[i], w = adj.weight[i];
                if (dist_t[u] + w < dist_t[v]) {
                    dist_t[v] = dist_t[u] + w;
                    parent_t[v] = u;
                    pq_t.push({dist_t[v], v});
                    if (dist_s[v] + dist_t[v] < best_dist) {
                        best_dist = dist_s[v] + dist_t[v];
                        meet_node = v;
                    }
                }
            }
        }
    }
    if (meet_node == -1) {
        total_weight = -1;
        return {};
    }
    total_weight = best_dist;

    vector<int> path;
    int curr = meet_node;
    while (curr != -1) {
        path.push_back(curr);
        if (curr == s) break;
        curr = parent_s[curr];
    }
    reverse(path.begin(), path.end());
    curr = parent_t[meet_node];
    while (curr != -1) {
        path.push_back(curr);
        if (curr == t) break;
        curr = parent_t[curr];
    }
    return path;
}

// ===========================================================================
// A* ALGORITHM (TÌM KIẾM CÓ ĐỊNH HƯỚNG)
// ===========================================================================
/*
 * Tư duy: Sử dụng hàm heuristic h(u) để ước lượng khoảng cách từ u đến đích t.
 * Hàng đợi ưu tiên sắp xếp theo f(u) = g(u) + h(u). Dừng sớm khi gặp t.
 */
vector<int> a_star(int n, int s, int t, const CSRGraph& adj,
                   int& total_weight) {
    vector<int> dist(n + 1, INF), parent(n + 1, -1);
    dist[s] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    pq.push({0, s});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (u == t) break;

        for (int i = adj.offset[u]; i < adj.offset[u + 1]; ++i) {
            int v = adj.to[i], w = adj.weight[i];
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                int h = 0;  // Heuristic = 0
                pq.push({dist[v] + h, v});
            }
        }
    }
    total_weight = (dist[t] == INF) ? -1 : dist[t];
    return truy_vet(s, t, dist, parent);
}

// ===========================================================================
// TRUY VẾT ĐƯỜNG ĐI
// ===========================================================================
/*
 * Tư duy: parent[v] là con trỏ ngược. Bắt đầu từ đích t, nhảy ngược theo
 * parent cho tới khi gặp nguồn s, sau đó đảo ngược mảng để có thứ tự xuôi.
 */
vector<int> truy_vet(int s, int t, const vector<int>& dist,
                     const vector<int>& parent) {
    if (dist[t] == INF) return {};
    vector<int> path;
    int curr = t;
    while (curr != -1) {
        path.push_back(curr);
        if (curr == s) break;
        curr = parent[curr];
    }
    reverse(path.begin(), path.end());
    return path;
}