import subprocess
import platform
import sys
import random
import re
import time
import networkx as nx

test_cases = [
    ("TC01_Basic", 3, 1, 2, [(1, 2, 4), (1, 3, 1), (3, 2, 2)], [1, 3, 2], 3),
    ("TC02_NoPath", 3, 1, 3, [(1, 2, 5)], [], -1),
    ("TC03_SingleNode", 1, 1, 1, [], [1], 0),
    ("TC04_ZeroWeight", 3, 1, 3, [(1, 2, 0), (2, 3, 0), (1, 3, 5)], [1, 2, 3], 0),
    ("TC05_Detour", 3, 1, 3, [(1, 3, 100), (1, 2, 10), (2, 3, 10)], [1, 2, 3], 20),
    ("TC06_TieBreak", 4, 1, 4, [(1, 2, 1), (2, 4, 1), (1, 3, 1), (3, 4, 1)], [1, 2, 4], 2),
    ("TC07_Line", 5, 1, 5, [(1, 2, 10), (2, 3, 10), (3, 4, 10), (4, 5, 10)], [1, 2, 3, 4, 5], 40),
    ("TC08_Disconnected", 5, 1, 5, [(1, 2, 5), (3, 4, 5)], [], -1),
    ("TC09_Complete", 4, 1, 4, [(1, 2, 1), (1, 3, 2), (1, 4, 5), (2, 3, 1), (2, 4, 3), (3, 4, 1)], [1, 3, 4], 3),
    ("TC10_SameNode", 5, 3, 3, [(1, 2, 10), (2, 3, 10), (3, 4, 10)], [3], 0),
    ("TC11_LargeWeight", 2, 1, 2, [(1, 2, 1000000)], [1, 2], 1000000),
    ("TC12_Star", 5, 1, 5, [(1, 2, 1), (1, 3, 1), (1, 4, 1), (1, 5, 1)], [1, 5], 1),
]

def run_cpp_exe(n, s, t, edges):
    # Gọi mode 2 (Dijkstra)
    exe_name = '../build/dijkstra.exe' if platform.system() == 'Windows' else '../build/dijkstra'
    input_str = f"2\n{n} {len(edges)}\n"
    for u, v, w in edges:
        input_str += f"{u} {v} {w}\n"
    input_str += f"{s} {t}\n"

    try:
        process = subprocess.run([exe_name], input=input_str, text=True, capture_output=True, encoding='utf-8')
        output = process.stdout
    except FileNotFoundError:
        print(f"FATAL: Không tìm thấy file '{exe_name}'.")
        sys.exit(1)
    
    if "LOI" in output:
        return "LOI", -1
    if "KHONG CO DUONG" in output:
        return [], -1
        
    path_match = re.search(r'duong=\[([^\]]+)\]', output)
    total_match = re.search(r'tong=(\d+)', output)
    
    if not path_match or not total_match:
        print(f"  -> LỖI PARSE OUTPUT: \n{output}")
        return [], -1

    path = [int(x) for x in path_match.group(1).split(',')]
    total = int(total_match.group(1))
    return path, total

def test_static():
    print("--- KIỂM THỬ TĨNH (12 CA CỐ ĐỊNH) ---")
    pass_count = 0
    for case_id, n, s, t, edges, exp_path, exp_total in test_cases:
        path, total = run_cpp_exe(n, s, t, edges)
        
        is_pass = (path == exp_path and total == exp_total)
        
        if is_pass:
            print(f"[{case_id}] PASS")
            pass_count += 1
        else:
            print(f"[{case_id}] FAIL")
            
        print(f"   Input   : n={n}, s={s}, t={t}, edges={edges}")
        print(f"   Expected: path={exp_path}, total={exp_total}")
        print(f"   Actual  : path={path}, total={total}")
        print("-" * 70)
            
    return pass_count == len(test_cases)

def test_fuzzing(num_tests=100):
    print(f"\n--- FUZZING TEST (ĐỐI CHIẾU VỚI NETWORKX) ---")
    for i in range(num_tests):
        n = random.randint(2, 20)
        m = random.randint(1, min(50, n * (n - 1) // 2))
        G = nx.Graph()
        G.add_nodes_from(range(1, n + 1))
        edges = []
        for _ in range(m):
            u, v = random.randint(1, n), random.randint(1, n)
            if u != v and not G.has_edge(u, v):
                w = random.randint(0, 100)
                G.add_edge(u, v, weight=w)
                edges.append((u, v, w))
        s, t = random.randint(1, n), random.randint(1, n)
        path, total = run_cpp_exe(n, s, t, edges)
        try:
            nx_path = nx.dijkstra_path(G, s, t, weight='weight')
            nx_total = nx.dijkstra_path_length(G, s, t, weight='weight')
        except nx.NetworkXNoPath:
            nx_path, nx_total = [], -1

        if path != nx_path or total != nx_total:
            print(f"FAIL ở ca {i}: n={n}, s={s}, t={t}, edges={edges}")
            return False
    print(f"-> PASS TOÀN BỘ {num_tests} CA FUZZING!")
    return True

def test_stress():
    print("\n--- STRESS TEST (n=50, m=1000) ---")
    n = 50
    edges = [(random.randint(1, n), random.randint(1, n), random.randint(0, 100)) for _ in range(1000)]
    start = time.perf_counter()
    path, total = run_cpp_exe(n, 1, n, edges)
    end = time.perf_counter()
    print(f"-> Chạy xong đồ thị 50 đỉnh, 1000 cạnh trong: {(end - start) * 1000:.4f} ms")
    return True

def test_error_handling():
    print("\n--- KIỂM THỬ XỬ LÝ NGOẠI LỆ (CẠNH ÂM) ---")
    exe_name = '../build/dijkstra.exe' if platform.system() == 'Windows' else '../build/dijkstra'
    input_str = "2\n3 2\n1 2 -5\n2 3 4\n1 3\n"
    process = subprocess.run([exe_name], input=input_str, text=True, capture_output=True, encoding='utf-8')
    if "LOI: Do thi chua canh co trong so am" in process.stdout:
        print("[PASS] Đã chặn cạnh âm thành công.")
        return True
    else:
        print("[FAIL] Không chặn được cạnh âm. Output nhận được:")
        print(process.stdout)
        return False

if __name__ == "__main__":
    if test_static() and test_fuzzing(100) and test_stress() and test_error_handling():
        print("\n=> TRẠNG THÁI: ĐẠT TOÀN BỘ KIỂM THỬ")
        sys.exit(0)
    else:
        print("\n=> TRẠNG THÁI: CÓ LỖI CẦN SỬA")
        sys.exit(1)