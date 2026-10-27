#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <stdexcept>

#include "dijkstra.h"

namespace py = pybind11;

// Hàm phụ trợ: Chuyển đổi mảng cạnh sang cấu trúc CSR siêu tốc
CSRGraph build_csr(int n, const std::vector<std::tuple<int, int, int>>& edges) {
    CSRGraph g;
    g.offset.assign(n + 2, 0);

    // Bước 1: Đếm bậc (degree) của từng đỉnh
    for (const auto& edge : edges) {
        int u = std::get<0>(edge), v = std::get<1>(edge);
        if (std::get<2>(edge) < 0) throw std::invalid_argument("Trong so am!");
        g.offset[u + 1]++;
        g.offset[v + 1]++;
    }

    // Bước 2: Tính mảng offset (cộng dồn)
    for (int i = 1; i <= n + 1; ++i) {
        g.offset[i] += g.offset[i - 1];
    }

    // Bước 3: Cấp phát mảng to và weight, điền dữ liệu
    g.to.resize(g.offset[n + 1]);
    g.weight.resize(g.offset[n + 1]);
    std::vector<int> curr = g.offset;

    for (const auto& edge : edges) {
        int u = std::get<0>(edge), v = std::get<1>(edge), w = std::get<2>(edge);
        g.to[curr[u]] = v;
        g.weight[curr[u]++] = w;
        g.to[curr[v]] = u;
        g.weight[curr[v]++] = w;
    }
    return g;
}

py::dict run_dijkstra_py(int n, int s, int t,
                         const std::vector<std::tuple<int, int, int>>& edges) {
    CSRGraph adj = build_csr(n, edges);

    std::vector<int> dist, parent;
    Counters ctr;
    dijkstra_csr(n, s, adj, dist, parent, ctr);  // Gọi hàm siêu tốc

    std::vector<int> path = truy_vet(s, t, dist, parent);

    py::dict result;
    result["dist"] = dist;
    result["path"] = path;
    result["total"] = (dist[t] == INF) ? -1 : dist[t];
    return result;
}

PYBIND11_MODULE(dijkstra_cpp, m) {
    m.def("run", &run_dijkstra_py, "Chạy Dijkstra (CSR)", py::arg("n"),
          py::arg("s"), py::arg("t"), py::arg("edges"));
}