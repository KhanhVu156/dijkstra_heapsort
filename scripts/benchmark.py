import subprocess
import platform
import random
import re
import sys

def generate_random_graph(n, m, max_w):
    edges = set()
    while len(edges) < m:
        u, v = random.randint(1, n), random.randint(1, n)
        if u != v:
            edge = (min(u, v), max(u, v))
            if edge not in edges: edges.add(edge)
    return [(u, v, random.randint(1, max_w)) for u, v in edges]

def run_dijkstra_benchmark(case_name, n, m, max_w):
    edges = generate_random_graph(n, m, max_w)
    input_str = f"3\n{n} {m}\n" # Mode 3
    for u, v, w in edges: input_str += f"{u} {v} {w}\n"
    input_str += f"1 {n}\n"

    exe_name = '../build/dijkstra.exe' if platform.system() == 'Windows' else '../build/dijkstra'
    try:
        process = subprocess.run([exe_name], input=input_str, text=True, capture_output=True, encoding='utf-8')
        output = process.stdout
        print(f"\n[ĐỒ THỊ - {case_name}] n={n}, m={m}, max_w={max_w}")
        print(output.strip())
    except FileNotFoundError:
        print(f"FATAL: Không tìm thấy file '{exe_name}'.")
        sys.exit(1)

def run_heapsort_benchmark(case_name, n):
    # Sinh mảng ngẫu nhiên
    arr = [random.randint(-1000, 1000) for _ in range(n)]
    input_str = f"4\n{n}\n" + " ".join(map(str, arr)) + "\n"

    exe_name = '../build/dijkstra.exe' if platform.system() == 'Windows' else '../build/dijkstra'
    try:
        process = subprocess.run([exe_name], input=input_str, text=True, capture_output=True, encoding='utf-8')
        output = process.stdout
        print(f"[MẢNG - {case_name}] n={n}")
        print(output.strip())
    except FileNotFoundError:
        print(f"FATAL: Không tìm thấy file '{exe_name}'.")
        sys.exit(1)

if __name__ == "__main__":
    print("=====================================================================")
    print("    BENCHMARK TOÀN DIỆN: 4 THUẬT TOÁN ĐỒ THỊ VÀ 1 THUẬT TOÁN SẮP XẾP   ")
    print("=====================================================================")
    
    print("\n--- PHẦN 1: ĐO HIỆU NĂNG TÌM ĐƯỜNG ĐI (ĐỒ ÁN 1) ---")
    run_dijkstra_benchmark("Trọng số nhỏ", n=50, m=100, max_w=10)
    run_dijkstra_benchmark("Trọng số lớn", n=50, m=100, max_w=10000)
    run_dijkstra_benchmark("Đồ thị dày đặc", n=50, m=1200, max_w=100)
    
    print("\n--- PHẦN 2: ĐO HIỆU NĂNG SẮP XẾP (ĐỒ ÁN 2) ---")
    run_heapsort_benchmark("Mảng nhỏ", n=1000)
    run_heapsort_benchmark("Mảng vừa", n=10000)
    run_heapsort_benchmark("Mảng lớn", n=100000)
    
    print("\n=====================================================================")