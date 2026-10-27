cmake_minimum_required(VERSION 3.10)

# Tên project tổng; gồm 2 target: module Python (pybind11) và exe console.
project(AlgorithmSuiteProject)

# Sử dụng C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tìm package Python và pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 CONFIG REQUIRED)

# Đảm bảo trình biên dịch tìm thấy các header nội bộ
include_directories(src)

# ---------------------------------------------------------------------------
# Target 1: module Python "dijkstra_cpp".
# ---------------------------------------------------------------------------
pybind11_add_module(dijkstra_cpp bindings/pybind_wrapper.cpp src/dijkstra.cpp)

# ---------------------------------------------------------------------------
# Target 2: chương trình console "dijkstra".
# Bao gồm cả logic Dijkstra và Heapsort
# ---------------------------------------------------------------------------
add_executable(dijkstra src/main.cpp src/dijkstra.cpp src/heapsort.cpp)

target_link_options(dijkstra_cpp PRIVATE -static-libgcc -static-libstdc++)

# ---------------------------------------------------------------------------
# Cờ tối ưu hóa trình biên dịch
# -O3: Tối ưu hóa mã nguồn ở mức cao nhất.
# -flto: Link Time Optimization, tối ưu hóa xuyên suốt các file object.
# -march=native: Tận dụng tối đa tập lệnh phần cứng của CPU hiện tại.
# ---------------------------------------------------------------------------
target_compile_options(dijkstra_cpp PRIVATE -O3 -flto -march=native)
target_compile_options(dijkstra PRIVATE -O3 -flto -march=native)
target_link_options(dijkstra PRIVATE -flto)