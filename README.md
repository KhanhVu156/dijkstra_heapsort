# 🎯 Dijkstra & Heapsort Benchmark Suite (Technical Deep Dive)

Ứng dụng tìm đường đi ngắn nhất trên đồ thị vô hướng và thuật toán sắp xếp tại chỗ, được thiết kế với trọng tâm là **tối ưu hóa hiệu năng (Performance Optimization)** và **tích hợp cấu trúc dữ liệu cấp thấp**.

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Python](https://img.shields.io/badge/Python-3.10%2B-yellow.svg)](https://www.python.org/)
[![CMake](https://img.shields.io/badge/CMake-3.10%2B-brightgreen.svg)](https://cmake.org/)
[![MSYS2](https://img.shields.io/badge/Environment-MSYS2%20UCRT64-orange.svg)](https://www.msys2.org/)

---

## 📖 Tổng quan

Dự án là sự kết hợp chéo (Cross-integration) giữa **Đồ án 1 (Tìm đường đi ngắn nhất)** và **Đồ án 2 (Sắp xếp vun đống)**. Thay vì sử dụng các thư viện có sẵn, dự án tự xây dựng các cấu trúc dữ liệu và kỹ thuật tối ưu từ con số 0 để đáp ứng tiêu chuẩn của các kỳ thi lập trình (Competitive Programming) và môi trường Production.

---

## 🔥 TRỌNG TÂM KỸ THUẬT (TECHNICAL HIGHLIGHTS)

### 1. Tích hợp chéo Cấu trúc Dữ liệu (Cross-Integration)

* **Tự chế Custom Min-Heap cho Dijkstra:** Thay vì dùng `std::priority_queue`, dự án tự cài đặt `MinHeap` (dựa trên tư duy `sift_up`, `sift_down` của Heapsort) để đưa vào thuật toán tìm đường. Điều này giúp kiểm soát hoàn toàn bộ nhớ và cho phép can thiệp sâu vào các phép so sánh.
* **Ứng dụng thuật toán vào thực tế:** Hàm `heapsort` được ứng dụng để sắp xếp các mảng kết quả (như danh sách khoảng cách) với độ phức tạp không gian \(O(1)\) (In-place).

### 2. Tối ưu hóa Vi xử lý và Bộ nhớ (Memory & CPU Optimization)

* **Kỹ thuật "Hole Technique" (Tạo hố):** Áp dụng trong hàm `sift_down` của Heapsort. Thay vì dùng hàm `swap` (tốn 3 phép gán), kỹ thuật này "nhấc" phần tử ra, kéo con lên lấp hố và nhét lại vào vị trí cuối, giúp **giảm 2/3 số lần ghi vào RAM**.
* **Cấu trúc CSR (Compressed Sparse Row):** Đồ thị được nén vào 3 mảng 1 chiều liên tục thay vì mảng 2 chiều. Điều này giúp CPU đọc dữ liệu tuần tự, giảm thiểu hiện tượng **Cache Miss** (tối ưu hóa L1/L2 Cache).
* **Bitwise Shift:** Sử dụng phép dịch bit `(i << 1)` thay vì phép nhân `(2 * i)` trong Heapsort để tiết kiệm chu kỳ CPU.

### 3. Kỹ thuật I/O Siêu tốc (Fast I/O)

* **FastScanner:** Sử dụng `fread` để đọc toàn bộ luồng dữ liệu thô vào bộ đệm (Buffer) thay vì dùng `cin/scanf`. Kỹ thuật này giúp tốc độ đọc nhanh gấp 3-5 lần, loại bỏ chi phí đồng bộ I/O.

### 4. Tối ưu hóa Trình biên dịch (Compiler-level Optimization)

* Tự động kích hoạt các cờ tối ưu đỉnh cao: `-O3` (tối ưu tốc độ), `-flto` (Link Time Optimization), và `-march=native` (tận dụng tối đa tập lệnh phần cứng của CPU hiện tại như AVX, SSE).

---

## 🚀 CÁC THUẬT TOÁN ĐƯỢC CÀI ĐẶT

### Đồ án 1: Tìm đường đi ngắn nhất

1. **Dijkstra (CSR + Custom Min-Heap):** Thuật toán chuẩn mực \(O((V + E) \log V)\).
2. **Dial's Algorithm (Bucket Queue):** Tối ưu vượt trội cho đồ thị có trọng số nguyên nhỏ, giảm độ phức tạp xuống \(O(E + V \cdot C)\).
3. **Bidirectional Dijkstra:** Chạy 2 luồng từ nguồn và đích, thu hẹp không gian tìm kiếm một nửa.
4. **A\* Algorithm (A-Star):** Tìm kiếm có định hướng bằng hàm Heuristic, dừng sớm ngay khi chạm đích.

### Đồ án 2: Sắp xếp vun đống

1. **Heapsort (In-place Max-Heap):** Sắp xếp tại chỗ với độ phức tạp không gian \(O(1)\), tích hợp kỹ thuật "Hole Technique".

---

## 🧪 HỆ THỐNG KIỂM THỬ & ĐO ĐẠC

* **Kiểm thử tĩnh:** 12 ca test bao phủ từ đồ thị cơ bản, đồ thị thưa đến các ca biên (đỉnh cô lập, không có đường đi, cạnh âm).
* **Fuzzing Test:** Tự động sinh 100 đồ thị ngẫu nhiên đối chiếu với thư viện chuẩn `NetworkX`, đảm bảo tính đúng đắn 100%.
* **Stress Test:** Ép hệ thống chạy với đồ thị dày đặc (50 đỉnh, 1000 cạnh) để đo giới hạn chịu tải.
* **Benchmark Suite:** So sánh trực tiếp thời gian thực thi (micro-giây) của cả 5 thuật toán.
* **Visualizer:** Trực quan hóa đồ thị và highlight đường đi ngắn nhất bằng `NetworkX` và `Matplotlib`.

---

## 🛠️ Công nghệ sử dụng

| Thành phần | Công nghệ |
| ------------ | ----------- |
| **Ngôn ngữ lõi** | C++ 17 |
| **Tương tác Python/C++** | `subprocess` (Gọi CLI) |
| **Build System** | CMake |
| **Testing & Tool** | Python 3.10+, NetworkX, Matplotlib |
| **Môi trường (Khuyên dùng)** | MSYS2 UCRT64 (Windows) |

---

## 📂 Cấu trúc thư mục

```text
DijkstraProject/
├── src/
│   ├── dijkstra.h              # Cấu trúc dữ liệu, FastScanner, Custom MinHeap, khai báo hàm
│   ├── dijkstra.cpp            # Cài đặt 4 thuật toán đồ thị
│   ├── heapsort.cpp            # Cài đặt thuật toán sắp xếp vun đống (Hole Technique)
│   └── main.cpp                # Chương trình CLI đa chế độ (Mode 1-4)
├── scripts/
│   ├── test_suite.py           # Script chạy 12 ca test + Fuzzing + Stress test
│   ├── benchmark.py            # Script tự động đo đạc và so sánh 5 thuật toán
│   └── visualize.py            # Script vẽ đồ thị trực quan bằng NetworkX
├── CMakeLists.txt              # Cấu hình build & Compiler Flags
└── README.md                   # Tài liệu dự án
```

---

## ⚙️ Hướng dẫn cài đặt & Chạy (MSYS2 UCRT64)

> ⚠️ **Lưu ý**: Dự án được tối ưu để build trên Windows sử dụng **MSYS2 UCRT64**.

### Bước 1: Cài đặt môi trường

Mở terminal **MSYS2 UCRT64** và cài đặt Toolchain, CMake, Python:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-python
pacman -S mingw-w64-ucrt-x86_64-python-pip
```

### Bước 2: Cài đặt thư viện Python

```bash
pip install networkx matplotlib
```

### Bước 3: Build dự án với CMake

```bash
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
cmake --build . -j
```

### Bước 4: Chạy chương trình

#### 🔹 Chạy CLI (Console)

```bash
./dijkstra.exe
```

Chương trình hỗ trợ 4 chế độ (Mode):
* **Mode 1**: Heapsort (nhập `1`, sau đó nhập `n` và mảng).
* **Mode 2**: Dijkstra (nhập `2`, sau đó nhập đồ thị).
* **Mode 3**: Benchmark Đồ thị (dùng nội bộ cho script).
* **Mode 4**: Benchmark Heapsort (dùng nội bộ cho script).

#### 🔹 Chạy Kiểm thử tự động

Di chuyển vào thư mục `scripts` và chạy:

```bash
cd ../scripts
python test_suite.py
```

#### 🔹 Chạy Benchmark so sánh thuật toán

```bash
python benchmark.py
```

#### 🔹 Chạy Visualize đồ thị

```bash
python visualize.py
```

---

## ⚠️ Giới hạn & Lưu ý kỹ thuật

- **Số đỉnh `n` tối đa = 50**: Giới hạn ở `main.cpp` để đảm bảo thời gian in kết quả không bị quá tải.
* **Không hỗ trợ trọng số âm**: Thuật toán Dijkstra có bộ lọc `hasNegativeWeight` và chặn ngay từ đầu.
* **Dial's Algorithm & Trọng số lớn**: Dial sử dụng mảng Bucket có kích thước `V * max_weight`. Nếu trọng số quá lớn, mảng sẽ phình to và thuật toán chạy chậm hơn cả Dijkstra CSR. Dial chỉ tỏa sáng khi trọng số nguyên nhỏ.
