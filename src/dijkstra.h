#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

const int INF = 1e9;

/**
 * @brief Một cung trong danh sách kề truyền thống.
 */
struct Edge {
    int to;
    int weight;
};

/**
 * @brief Cấu trúc dữ liệu CSR (Compressed Sparse Row) cho đồ thị.
 * Tư duy: Nén đồ thị vào 3 mảng 1 chiều liên tục. Việc này giúp CPU đọc dữ
 * liệu tuần tự, tối ưu hóa bộ nhớ đệm (Cache-friendly), giảm thiểu Cache Miss.
 */
struct CSRGraph {
    std::vector<int> offset;
    std::vector<int> to;
    std::vector<int> weight;
};

/**
 * @brief Bộ đếm thống kê phục vụ phân tích độ phức tạp thực nghiệm.
 */
struct Counters {
    int pops = 0;
    int relaxations = 0;
    int updates = 0;
};

// ===========================================================================
// CẤU TRÚC MIN-HEAP TỰ CHẾ (KẾT HỢP TƯ DUY HEAPSORT VÀO ĐỒ THỊ)
// Tư duy: Thay vì dùng std::priority_queue (khó can thiệp sâu), ta tự viết
// Min-Heap 1-based để tối ưu bộ nhớ và kiểm soát các phép toán.
// ===========================================================================
class MinHeap {
   private:
    std::vector<std::pair<int, int>> heap;  // Lưu cặp {dist, node}
    int heap_size;

    // Tư duy: Vun đống từ trên xuống. Đảm bảo nút cha nhỏ hơn 2 nút con.
    void sift_down(int i) {
        int cha = i, con_trai = 2 * i, con_phai = 2 * i + 1, nho_nhat = cha;
        if (con_trai <= heap_size &&
            heap[con_trai].first < heap[nho_nhat].first)
            nho_nhat = con_trai;
        if (con_phai <= heap_size &&
            heap[con_phai].first < heap[nho_nhat].first)
            nho_nhat = con_phai;
        if (nho_nhat != cha) {
            std::swap(heap[cha], heap[nho_nhat]);
            sift_down(nho_nhat);
        }
    }

    // Tư duy: Vun đống từ dưới lên. Dùng khi chèn phần tử mới.
    void sift_up(int i) {
        while (i > 1 && heap[i].first < heap[i / 2].first) {
            std::swap(heap[i], heap[i / 2]);
            i = i / 2;
        }
    }

   public:
    MinHeap(int max_size) {
        heap.resize(max_size + 1);
        heap_size = 0;
    }
    bool empty() const { return heap_size == 0; }
    void push(int dist, int node) {
        heap_size++;
        heap[heap_size] = {dist, node};
        sift_up(heap_size);
    }
    std::pair<int, int> pop() {
        std::pair<int, int> top = heap[1];
        heap[1] = heap[heap_size];
        heap_size--;
        if (heap_size > 0) sift_down(1);
        return top;
    }
};

// ===========================================================================
// FAST I/O - KỸ THUẬT ĐỌC DỮ LIỆU SIÊU TỐC TRONG THI ĐẤU
// Tư duy: Sử dụng fread thay vì cin/scanf để giảm chi phí đồng bộ I/O.
// ===========================================================================
struct FastScanner {
    static const int BUFSIZE = 1 << 20;
    int idx, size;
    char buf[BUFSIZE];
    FastScanner() : idx(0), size(0) {}
    inline char read() {
        if (idx >= size) {
            size = (int)fread(buf, 1, BUFSIZE, stdin);
            idx = 0;
            if (size == 0) return 0;
        }
        return buf[idx++];
    }
    template <typename T>
    bool nextInt(T& out) {
        char c;
        T sign = 1;
        out = 0;
        c = read();
        if (!c) return false;
        while (c != '-' && (c < '0' || c > '9')) {
            c = read();
            if (!c) return false;
        }
        if (c == '-') {
            sign = -1;
            c = read();
        }
        for (; c >= '0' && c <= '9'; c = read()) out = out * 10 + c - '0';
        out *= sign;
        return true;
    }
};

bool noi_long(int u, int v, int w, std::vector<int>& dist,
              std::vector<int>& parent, Counters& ctr);
void dijkstra_csr(int n, int s, const CSRGraph& adj, std::vector<int>& dist,
                  std::vector<int>& parent, Counters& ctr);
void dijkstra_dial(int n, int s, const CSRGraph& adj, int max_weight,
                   std::vector<int>& dist, std::vector<int>& parent,
                   Counters& ctr);
std::vector<int> bidirectional_dijkstra(int n, int s, int t,
                                        const CSRGraph& adj, int& total_weight);
std::vector<int> a_star(int n, int s, int t, const CSRGraph& adj,
                        int& total_weight);
std::vector<int> truy_vet(int s, int t, const std::vector<int>& dist,
                          const std::vector<int>& parent);

// --- KHAI BÁO HÀM HEAPSORT (Đồ án 2) ---
void day_xuong(std::vector<int>& a, int i, int heap_size);
void tao_heap(std::vector<int>& a);
void heapsort(std::vector<int>& a);         // Có in log
void heapsort_no_log(std::vector<int>& a);  // Không in log (dùng cho benchmark)

#endif  // DIJKSTRA_H