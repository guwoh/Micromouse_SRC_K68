"""
=============================================================
  THUAT TOAN A* - DEMO PYTHON CHO MICROMOUSE
  Muc dich: Hieu ro logic truoc khi chuyen sang C/C++
=============================================================
"""

# ===================== CAU TRUC DU LIEU =====================

class Node:
    """
    Moi o trong me cung la mot Node.
    Trong C/C++, day se la mot struct.
    """
    def __init__(self, row, col):
        self.row = row          # Toa do hang
        self.col = col          # Toa do cot
        self.g = float('inf')   # Chi phi thuc tu Start -> Node nay (ban dau = vo cuc)
        self.h = 0              # Chi phi uoc luong tu Node nay -> Goal (heuristic)
        self.f = float('inf')   # f = g + h (tong chi phi)
        self.parent = None      # Node cha (de truy nguoc duong di)
        self.is_wall = False    # True neu o nay la tuong

    def __eq__(self, other):
        """So sanh 2 node bang toa do"""
        return self.row == other.row and self.col == other.col

    def __repr__(self):
        return f"({self.row}, {self.col})"


# ===================== HAM HEURISTIC =====================

def heuristic_manhattan(node, goal):
    """
    Manhattan Distance: |x1 - x2| + |y1 - y2|
    Phu hop cho Micromouse vi robot chi di 4 huong (tren/duoi/trai/phai)
    
    Trong C/C++: abs(node.row - goal.row) + abs(node.col - goal.col)
    """
    return abs(node.row - goal.row) + abs(node.col - goal.col)


# ===================== TIM NODE CO f NHO NHAT =====================

def get_lowest_f_node(open_list):
    """
    Tim node co f(n) nho nhat trong Open List.
    
    LUU Y: Trong C/C++, nen dung Priority Queue (Min-Heap) 
    de toi uu tu O(n) xuong O(log n).
    O day dung cach don gian de de hieu.
    """
    best = open_list[0]
    for node in open_list:
        if node.f < best.f:
            best = node
    return best


# ===================== LAY CAC O LAN CAN =====================

def get_neighbors(grid, node, rows, cols):
    """
    Tra ve danh sach cac o lan can (4 huong) hop le.
    Trong Micromouse: can kiem tra them co TUONG chan giua 2 o khong.
    
    4 huong: Tren, Duoi, Trai, Phai
    """
    neighbors = []
    # (delta_row, delta_col) cho 4 huong
    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]
    #              Tren     Duoi    Trai     Phai

    for dr, dc in directions:
        new_row = node.row + dr
        new_col = node.col + dc

        # Kiem tra bien
        if 0 <= new_row < rows and 0 <= new_col < cols:
            neighbor = grid[new_row][new_col]
            # Kiem tra khong phai tuong
            if not neighbor.is_wall:
                neighbors.append(neighbor)

    return neighbors


# ===================== TRUY NGUOC DUONG DI =====================

def reconstruct_path(node):
    """
    Truy nguoc tu Goal ve Start thong qua parent.
    Trong C/C++: dung mang hoac linked list.
    """
    path = []
    current = node
    while current is not None:
        path.append((current.row, current.col))
        current = current.parent
    path.reverse()  # Dao nguoc: Start -> Goal
    return path


# ===================== THUAT TOAN A* CHINH =====================

def a_star(grid, start, goal, rows, cols):
    """
    Thuat toan A* tim duong di ngan nhat tu start den goal.
    
    Tham so:
        grid  : Mang 2D cac Node (me cung)
        start : Node bat dau
        goal  : Node dich
        rows  : So hang
        cols  : So cot
    
    Tra ve:
        path  : Danh sach toa do [(r,c), ...] neu tim thay
        None  : Neu khong co duong di
    """

    # === BUOC 1: Khoi tao ===
    open_list = []      # Danh sach cac node CAN XET (chua mo rong)
    closed_list = []    # Danh sach cac node DA XET (da mo rong)
    # Trong C/C++: dung mang bool closed[ROWS][COLS] se hieu qua hon

    # Thiet lap node bat dau
    start.g = 0                                 # Chi phi tu Start den Start = 0
    start.h = heuristic_manhattan(start, goal)  # Uoc luong den Goal
    start.f = start.g + start.h                 # Tong chi phi
    start.parent = None

    open_list.append(start)  # Dua Start vao Open List

    # === BUOC 2: Vong lap chinh ===
    while len(open_list) > 0:

        # 2a. Chon node co f nho nhat
        current = get_lowest_f_node(open_list)

        # 2b. Kiem tra da den dich chua?
        if current == goal:
            print(f"[OK] Tim thay duong di! Chi phi = {current.g}")
            return reconstruct_path(current)

        # 2c. Chuyen current tu Open -> Closed
        open_list.remove(current)
        closed_list.append(current)

        # 2d. Xet tat ca cac o lan can
        for neighbor in get_neighbors(grid, current, rows, cols):

            # Bo qua neu da xet roi
            if neighbor in closed_list:
                continue

            # Tinh chi phi moi den neighbor qua current
            # (chi phi moi buoc = 1 trong me cung don gian)
            tentative_g = current.g + 1

            # Neu tim thay duong di TOT HON den neighbor
            if tentative_g < neighbor.g:
                # Cap nhat thong tin
                neighbor.g = tentative_g
                neighbor.h = heuristic_manhattan(neighbor, goal)
                neighbor.f = neighbor.g + neighbor.h
                neighbor.parent = current  # Ghi nho duong di

                # Them vao Open List neu chua co
                if neighbor not in open_list:
                    open_list.append(neighbor)

    # === BUOC 3: Khong tim thay duong di ===
    print("[FAIL] Khong co duong di!")
    return None


# ===================== TAO ME CUNG VA CHAY THU =====================

def create_maze():
    """
    Tao me cung 8x8 (giong Micromouse 8x8 don gian)
    
    Ky hieu:
        0 = Duong di duoc
        1 = Tuong
        S = Start (goc tren-trai)
        G = Goal  (trung tam)
    """
    maze_data = [
    #    0  1  2  3  4  5  6  7
        [0, 0, 0, 1, 0, 0, 0, 0],  # Hang 0
        [1, 1, 0, 1, 0, 1, 1, 0],  # Hang 1
        [0, 0, 0, 0, 0, 0, 1, 0],  # Hang 2
        [0, 1, 1, 1, 1, 0, 0, 0],  # Hang 3
        [0, 0, 0, 0, 1, 0, 1, 1],  # Hang 4
        [1, 1, 0, 0, 0, 0, 0, 0],  # Hang 5
        [0, 0, 0, 1, 1, 1, 1, 0],  # Hang 6
        [0, 1, 0, 0, 0, 0, 0, 0],  # Hang 7
    ]

    rows = len(maze_data)
    cols = len(maze_data[0])

    # Tao grid chua cac Node
    grid = []
    for r in range(rows):
        row_nodes = []
        for c in range(cols):
            node = Node(r, c)
            if maze_data[r][c] == 1:
                node.is_wall = True
            row_nodes.append(node)
        grid.append(row_nodes)

    return grid, rows, cols


def print_maze_with_path(grid, path, rows, cols):
    """In me cung ra console, danh dau duong di bang *"""
    path_set = set(path) if path else set()

    print()
    print("=" * 35)
    print("  ME CUNG VA DUONG DI A*")
    print("=" * 35)

    # Header cot
    print("    ", end="")
    for c in range(cols):
        print(f" {c} ", end="")
    print()

    for r in range(rows):
        print(f" {r} |", end="")
        for c in range(cols):
            if path and (r, c) == path[0]:
                print(" S ", end="")        # Start
            elif path and (r, c) == path[-1]:
                print(" G ", end="")        # Goal
            elif (r, c) in path_set:
                print(" * ", end="")        # Duong di
            elif grid[r][c].is_wall:
                print(" # ", end="")        # Tuong
            else:
                print(" . ", end="")        # Duong trong
        print(f"| {r}")

    print()

    # In chi tiet duong di
    if path:
        print(f"Duong di ({len(path)} buoc):")
        for i, (r, c) in enumerate(path):
            if i == 0:
                label = "(Start)"
            elif i == len(path) - 1:
                label = "(Goal)"
            else:
                label = ""
            print(f"   Buoc {i}: ({r}, {c}) {label}")


# ===================== CHAY CHUONG TRINH =====================

if __name__ == "__main__":
    print("THUAT TOAN A* - MICROMOUSE DEMO")
    print("=" * 40)

    # Tao me cung
    grid, rows, cols = create_maze()

    # Dinh nghia Start va Goal
    start = grid[0][0]          # Goc tren-trai (giong Micromouse)
    goal  = grid[3][5]          # Vi tri trung tam (tuy chinh)

    print(f"Start: ({start.row}, {start.col})")
    print(f"Goal:  ({goal.row}, {goal.col})")

    # Chay A*
    path = a_star(grid, start, goal, rows, cols)

    # Hien thi ket qua
    print_maze_with_path(grid, path, rows, cols)

    # === GIAI THICH DE CHUYEN SANG C/C++ ===
    print()
    print("=" * 50)
    print("GHI CHU CHUYEN SANG C/C++:")
    print("=" * 50)
    print("""
    1. class Node    -> struct Node { int row, col, g, h, f; bool is_wall; Node* parent; };
    2. open_list     -> Dung mang + Priority Queue (Min-Heap) theo f
    3. closed_list   -> Dung mang bool closed[ROWS][COLS] (tiet kiem bo nho)
    4. grid          -> Mang 2D: Node grid[ROWS][COLS]
    5. float('inf')  -> Dung INT_MAX hoac 9999
    6. path (list)   -> Mang tinh hoac stack de luu duong di
    7. heuristic     -> Ham don gian: abs(a.row - b.row) + abs(a.col - b.col)
    """)
