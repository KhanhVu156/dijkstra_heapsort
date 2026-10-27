import networkx as nx
import matplotlib.pyplot as plt
import subprocess
import platform
import re
import sys

test_cases = [
    ("A01", 3, 1, 2, [(1, 2, 4), (1, 3, 1), (3, 2, 2)]),
    ("A02", 3, 1, 3, [(1, 2, 5)]),
    ("A03", 5, 2, 5, [(1, 2, 10), (1, 3, 5), (1, 4, 8), (1, 5, 2)]),
    ("A04", 4, 1, 4, [(1, 2, 2), (2, 3, 2), (3, 4, 2), (4, 1, 10)]),
    ("A05", 3, 2, 2, [(1, 2, 5), (2, 3, 6)]),
    ("A06", 3, 1, 3, [(1, 2, 0), (2, 3, 0), (1, 3, 5)]),
    ("A07", 1, 1, 1, []),
    ("A08", 3, 1, 3, [(1, 2, -5), (2, 3, 4)]),
    ("KT09", 2, 1, 2, [(1, 2, 10)]),
    ("KT10", 4, 1, 4, [(1, 2, 1), (2, 3, 1), (3, 4, 1)]),
    ("KT11", 3, 1, 3, [(1, 2, 100), (2, 3, 1), (1, 3, 50)]),
    ("KT12", 4, 1, 4, [(1, 2, 1), (1, 3, 5), (2, 3, 1), (2, 4, 5), (3, 4, 1)])
]

def run_cpp_exe(n, s, t, edges):
    exe_name = '../build/dijkstra.exe' if platform.system() == 'Windows' else '../build/dijkstra'
    input_str = f"2\n{n} {len(edges)}\n"
    for u, v, w in edges:
        input_str += f"{u} {v} {w}\n"
    input_str += f"{s} {t}\n"

    try:
        process = subprocess.run([exe_name], input=input_str, text=True, capture_output=True, encoding='utf-8')
        return process.stdout
    except FileNotFoundError:
        print(f"FATAL: Không tìm thấy '{exe_name}'.")
        sys.exit(1)

def draw_graph(n, edges, path, total_weight, case_id, is_error=False, error_msg=""):
    G = nx.Graph()
    G.add_nodes_from(range(1, n + 1)) 
    for u, v, w in edges:
        G.add_edge(u, v, weight=w)

    fig, ax = plt.subplots(figsize=(8, 6))
    pos = nx.spring_layout(G, seed=42, k=1.5, scale=1.2)
    nx.draw(G, pos, ax=ax, with_labels=True, node_color='lightblue', node_size=800, font_size=12, font_weight='bold')
    
    if edges:
        edge_labels = nx.get_edge_attributes(G, 'weight')
        nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels, ax=ax, font_size=10)

    if path and len(path) > 1:
        path_edges = list(zip(path, path[1:]))
        nx.draw_networkx_edges(G, pos, edgelist=path_edges, edge_color='red', width=3, ax=ax)
        nx.draw_networkx_nodes(G, pos, nodelist=path, node_color='lightgreen', node_size=800, ax=ax)

    title_color = 'red' if is_error else 'blue'
    fig.suptitle(f"Test Case: {case_id}", fontsize=16, fontweight='bold', color=title_color)
    if is_error:
        ax.set_title(f"Lỗi: {error_msg}", fontsize=12, color='red')
    elif total_weight == -1:
        ax.set_title("Không có đường đi", fontsize=12, color='red')
    else:
        ax.set_title(f"Tổng trọng số: {total_weight}", fontsize=12, color='blue')
    
    fig.tight_layout()
    fig.subplots_adjust(top=0.85)
    plt.show()

if __name__ == "__main__":
    print("=== PIPELINE: TÍNH TOÁN & TRỰC QUAN HÓA ĐỒ THỊ ===")
    for case_id, n, s, t, edges in test_cases:
        print(f"Đang xử lý: {case_id} ...")
        output = run_cpp_exe(n, s, t, edges)
        
        if "LOI" in output:
            error_msg = "Cạnh âm không hợp lệ" if "trong so am" in output else "Nguồn/đích không hợp lệ"
            draw_graph(n, edges, [], "", case_id, is_error=True, error_msg=error_msg)
            continue

        path_match = re.search(r'duong=\[([^\]]+)\]', output)
        total_match = re.search(r'tong=(\d+)', output)

        if path_match and total_match:
            path = [int(x) for x in path_match.group(1).split(',')]
            total = int(total_match.group(1))
            draw_graph(n, edges, path, total, case_id)
        else:
            draw_graph(n, edges, [], "KHÔNG CÓ ĐƯỜNG", case_id, is_error=True, error_msg="Không có đường đi")

    print("\n=== HOÀN TẤT! ===")