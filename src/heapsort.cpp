#include <iostream>

#include "dijkstra.h"

using namespace std;

// ===========================================================================
// ĐỒ ÁN 2: SẮP XẾP VUN ĐỐNG (HEAPSORT) BẰNG MAX-HEAP (1-BASED)
// ===========================================================================

/*
 * Tư duy tối ưu "Hole Technique" (Kỹ thuật tạo hố):
 * Thay vì dùng hàm swap (tốn 3 phép gán bộ nhớ), ta nhấc phần tử ra (tạo hố),
 * kéo các con lớn hơn lên lấp hố, và cuối cùng nhét phần tử gốc vào hố cuối.
 * Kỹ thuật này giúp giảm 2/3 số lần ghi vào RAM, tăng tốc độ đáng kể.
 * Phép dịch bit (i << 1) thay cho (2 * i) giúp tối ưu chu kỳ CPU.
 */
void day_xuong(vector<int>& a, int i, int heap_size) {
    int val = a[i];  // Nhấc phần tử ra tạo "hố"
    int cha = i;

    while ((cha << 1) <= heap_size) {
        int con = cha << 1;  // con = 2 * cha
        // Chọn đứa con lớn hơn (nếu có con phải)
        if (con + 1 <= heap_size && a[con] < a[con + 1]) {
            con++;
        }

        // Nếu val đã lớn hơn hoặc bằng con lớn nhất -> Đã đúng vị trí
        if (val >= a[con]) break;

        // Kéo con lên lấp hố
        a[cha] = a[con];
        cha = con;
    }
    a[cha] = val;  // Nhét val vào hố cuối cùng
}

/*
 * Tư duy: Xây dựng Max-Heap.
 * Duyệt từ nút cha cuối cùng (n/2) ngược về gốc (1), gọi day_xuong.
 * Độ phức tạp O(n) - tối ưu hơn so với việc chèn từng phần tử O(n log n).
 */
void tao_heap(vector<int>& a) {
    int n = a.size() - 1;
    for (int i = n / 2; i >= 1; i--) {
        day_xuong(a, i, n);
    }
}

/*
 * Tư duy: Sắp xếp tại chỗ (In-place).
 * 1. Biến mảng thành Max-Heap.
 * 2. Liên tục lấy phần tử lớn nhất (gốc a[1]) đổi chỗ với phần tử cuối mảng.
 * 3. Giảm heap_size để cô lập phần tử đã sắp xếp ở cuối.
 * 4. Vun đống lại phần còn lại.
 */
void heapsort(vector<int>& a) {
    tao_heap(a);

    // In trạng thái mảng sau khi tạo heap (theo yêu cầu đề bài)
    cout << "heap=[";
    for (size_t i = 1; i < a.size(); ++i)
        cout << a[i] << (i == a.size() - 1 ? "" : ",");
    cout << "];\n";

    int heap_size = a.size() - 1;
    while (heap_size > 1) {
        swap(a[1], a[heap_size]);
        heap_size--;
        day_xuong(a, 1, heap_size);
    }
}

/*
 * Tư duy: Hàm tương tự heapsort nhưng không in log, dùng để đo hiệu năng.
 */
void heapsort_no_log(vector<int>& a) {
    tao_heap(a);
    int heap_size = a.size() - 1;
    while (heap_size > 1) {
        swap(a[1], a[heap_size]);
        heap_size--;
        day_xuong(a, 1, heap_size);
    }
}