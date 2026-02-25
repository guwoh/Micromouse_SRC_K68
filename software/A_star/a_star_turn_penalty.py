"""
  SO SANH A* THONG THUONG vs A* CO PHAT RE
  Micromouse thuc te: re mat nhieu thoi gian hon di thang
"""

class Node:
    def __init__(self, row, col):
        self.row = row
        self.col = col
        self.g = float('inf')
        self.h = 0
        self.f = float('inf')
        self.parent = None
        self.is_wall = False

    def __eq__(self, other):
        return self.row == other.row and self.col == other.col


def heuristic(node, goal):
    return abs(node.row - goal.row) + abs(node.col - goal.col)


# ===================== XAC DINH HUONG DI =====================
#
#   Khi robot di tu node A sang node B, ta biet HUONG di chuyen:
#
#   A -> B (sang phai)  : direction = (0, +1)   = PHAI
#   A -> B (sang trai)  : direction = (0, -1)   = TRAI
#   A -> B (di len)     : direction = (-1, 0)   = TREN
#   A -> B (di xuong)   : direction = (+1, 0)   = DUOI
#
#   Neu huong tu (parent -> current) KHAC huong tu (current -> neighbor)
#   => Robot phai RE => Them chi phi phat!
#
# ==============================================================

def get_direction(from_node, to_node):
    """Tra ve huong di chuyen tu from_node sang to_node"""
    if from_node is None:
        return None  # Node dau tien, chua co huong
    dr = to_node.row - from_node.row   # delta row
    dc = to_node.col - from_node.col   # delta col
    return (dr, dc)


def is_turn(parent, current, neighbor):
    """
    Kiem tra: di tu current sang neighbor co phai RE khong?

    Cach xac dinh:
      1. Tim huong cu:  parent -> current     (truoc do di huong nao?)
      2. Tim huong moi: current -> neighbor   (bay gio muon di huong nao?)
      3. Neu khac nhau => PHAI RE!

    Vi du:
      parent=(0,0) -> current=(0,1) -> neighbor=(0,2)  : DI THANG (phai->phai)
      parent=(0,0) -> current=(0,1) -> neighbor=(1,1)  : RE!      (phai->xuong)
    """
    if parent is None:
        return False  # Buoc dau tien, khong co re

    old_dir = get_direction(parent, current)    # Huong cu
    new_dir = get_direction(current, neighbor)  # Huong moi

    return old_dir != new_dir   # Khac huong = RE!


# ===================== THUAT TOAN A* VOI PHAT RE =====================

def a_star(grid, start_pos, goal_pos, rows, cols, turn_cost=0):
    """
    A* co chi phi phat re (turn penalty).

    turn_cost = 0 : A* thuong (khong phat re)
    turn_cost = 2 : Re phai tra them 2 (tong = 1 + 2 = 3 cho moi lan re)
    turn_cost = 5 : Re phai tra them 5 (robot THAT SU khong muon re)
    """
    # Reset
    for r in range(rows):
        for c in range(cols):
            grid[r][c].g = float('inf')
            grid[r][c].f = float('inf')
            grid[r][c].parent = None

    start = grid[start_pos[0]][start_pos[1]]
    goal = grid[goal_pos[0]][goal_pos[1]]

    open_list = []
    closed_set = set()
    nodes_explored = 0

    start.g = 0
    start.h = heuristic(start, goal)
    start.f = start.g + start.h
    open_list.append(start)

    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    while len(open_list) > 0:
        # Tim node co f nho nhat
        best = open_list[0]
        for node in open_list:
            if node.f < best.f:
                best = node
        current = best

        if current == goal:
            path = []
            node = current
            while node is not None:
                path.append((node.row, node.col))
                node = node.parent
            path.reverse()

            # Dem so lan re
            turn_count = 0
            for i in range(2, len(path)):
                old_dr = path[i-1][0] - path[i-2][0]
                old_dc = path[i-1][1] - path[i-2][1]
                new_dr = path[i][0] - path[i-1][0]
                new_dc = path[i][1] - path[i-1][1]
                if (old_dr, old_dc) != (new_dr, new_dc):
                    turn_count += 1

            return path, current.g, nodes_explored, turn_count

        open_list.remove(current)
        closed_set.add((current.row, current.col))
        nodes_explored += 1

        for dr, dc in directions:
            nr, nc = current.row + dr, current.col + dc
            if 0 <= nr < rows and 0 <= nc < cols:
                neighbor = grid[nr][nc]
                if neighbor.is_wall or (nr, nc) in closed_set:
                    continue

                # ====== PHAN QUAN TRONG: TINH CHI PHI ======
                #
                #   Chi phi = 1 (di chuyen) + them phat neu RE
                #
                move_cost = 1

                if is_turn(current.parent, current, neighbor):
                    move_cost += turn_cost   # PHAT RE!

                tentative_g = current.g + move_cost
                # ============================================

                if tentative_g < neighbor.g:
                    neighbor.g = tentative_g
                    neighbor.h = heuristic(neighbor, goal)
                    neighbor.f = neighbor.g + neighbor.h
                    neighbor.parent = current
                    if neighbor not in open_list:
                        open_list.append(neighbor)

    return None, -1, 0, 0


# ===================== TAO ME CUNG =====================

def create_grid(rows, cols, walls):
    grid = []
    for r in range(rows):
        row = []
        for c in range(cols):
            row.append(Node(r, c))
        grid.append(row)
    for wr, wc in walls:
        grid[wr][wc].is_wall = True
    return grid


def print_result(grid, path, rows, cols):
    path_set = set(path) if path else set()

    # Tim cac diem re
    turns = set()
    if path:
        for i in range(2, len(path)):
            old_dr = path[i-1][0] - path[i-2][0]
            old_dc = path[i-1][1] - path[i-2][1]
            new_dr = path[i][0] - path[i-1][0]
            new_dc = path[i][1] - path[i-1][1]
            if (old_dr, old_dc) != (new_dr, new_dc):
                turns.add(path[i-1])   # Diem re

    for r in range(rows):
        for c in range(cols):
            if path and (r, c) == path[0]:
                print(" S", end="")
            elif path and (r, c) == path[-1]:
                print(" G", end="")
            elif (r, c) in turns:
                print(" +", end="")     # Diem re = dau +
            elif (r, c) in path_set:
                print(" *", end="")     # Di thang = dau *
            elif grid[r][c].is_wall:
                print(" #", end="")
            else:
                print(" .", end="")
        print()


# ===================== ME CUNG 16x16 =====================

ROWS, COLS = 16, 16

walls = []
for r in range(1, 10):
    walls.append((r, 5))
for r in range(4, 14):
    walls.append((r, 10))
for c in range(0, 5):
    walls.append((7, c))
for c in range(8, 13):
    walls.append((3, c))
for c in range(2, 8):
    walls.append((12, c))
for r in range(9, 13):
    walls.append((r, 2))
for c in range(11, 15):
    walls.append((7, c))

start_pos = (0, 0)
goal_pos = (15, 15)


# ===================== CHAY SO SANH =====================

if __name__ == "__main__":
    print("=" * 55)
    print("  SO SANH A* THUONG vs A* CO PHAT RE")
    print("  Ky hieu: * = di thang, + = diem re")
    print("=" * 55)
    print()

    test_cases = [
        (0, "A* thuong (khong phat re)"),
        (2, "A* phat re nhe (turn_cost=2)"),
        (5, "A* phat re manh (turn_cost=5)"),
    ]

    for tc, label in test_cases:
        grid = create_grid(ROWS, COLS, walls)
        path, cost, explored, turns = a_star(
            grid, start_pos, goal_pos, ROWS, COLS, turn_cost=tc
        )

        print(f"--- {label} ---")
        if path:
            print(f"  Chi phi tong (g):   {cost}")
            print(f"  So o di qua:        {len(path)}")
            print(f"  So lan RE:          {turns}")
            print(f"  Node da xet:        {explored}")
            print()
            print_result(grid, path, ROWS, COLS)
        else:
            print("  Khong tim thay duong di!")
        print()
