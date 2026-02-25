"""
  SO SANH A* THONG THUONG vs WEIGHTED A*
  Me cung 16x16 - co nhieu duong di de thay su khac biet
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


def heuristic_manhattan(node, goal):
    return abs(node.row - goal.row) + abs(node.col - goal.col)


def a_star(grid, start_pos, goal_pos, rows, cols, weight=1):
    """
    A* voi trong so w cho heuristic.
      w = 1  : A* chuan (tim duong NGAN NHAT, cham hon)
      w > 1  : Weighted A* (tim duong NHANH HON, co the khong toi uu)
      w = 0  : Dijkstra (khong dung heuristic, cham nhat)
    """
    # Reset tat ca node
    for r in range(rows):
        for c in range(cols):
            grid[r][c].g = float('inf')
            grid[r][c].h = 0
            grid[r][c].f = float('inf')
            grid[r][c].parent = None

    start = grid[start_pos[0]][start_pos[1]]
    goal = grid[goal_pos[0]][goal_pos[1]]

    open_list = []
    closed_set = set()   # Dung set de tim kiem nhanh O(1)
    nodes_explored = 0   # Dem so node da xet (do hieu qua)

    start.g = 0
    start.h = heuristic_manhattan(start, goal)
    start.f = start.g + weight * start.h   # <<< TRONG SO DUNG O DAY
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
            # Truy nguoc duong di
            path = []
            node = current
            while node is not None:
                path.append((node.row, node.col))
                node = node.parent
            path.reverse()
            return path, current.g, nodes_explored

        open_list.remove(current)
        closed_set.add((current.row, current.col))
        nodes_explored += 1

        for dr, dc in directions:
            nr, nc = current.row + dr, current.col + dc
            if 0 <= nr < rows and 0 <= nc < cols:
                neighbor = grid[nr][nc]
                if neighbor.is_wall or (nr, nc) in closed_set:
                    continue

                tentative_g = current.g + 1
                if tentative_g < neighbor.g:
                    neighbor.g = tentative_g
                    neighbor.h = heuristic_manhattan(neighbor, goal)
                    neighbor.f = neighbor.g + weight * neighbor.h  # <<< TRONG SO O DAY
                    neighbor.parent = current
                    if neighbor not in open_list:
                        open_list.append(neighbor)

    return None, -1, nodes_explored


def create_grid(rows, cols, walls):
    grid = []
    for r in range(rows):
        row = []
        for c in range(cols):
            node = Node(r, c)
            row.append(node)
        grid.append(row)
    for wr, wc in walls:
        grid[wr][wc].is_wall = True
    return grid


def print_result(grid, path, rows, cols):
    path_set = set(path) if path else set()
    for r in range(rows):
        for c in range(cols):
            if path and (r, c) == path[0]:
                print(" S", end="")
            elif path and (r, c) == path[-1]:
                print(" G", end="")
            elif (r, c) in path_set:
                print(" *", end="")
            elif grid[r][c].is_wall:
                print(" #", end="")
            else:
                print(" .", end="")
        print()


# ===================== ME CUNG 16x16 =====================
# Co NHIEU duong di de thay su khac biet giua w=1 va w>1

ROWS, COLS = 16, 16

# Tao tuong tao nhieu ngo re
walls = []
# Tuong doc
for r in range(1, 10):
    walls.append((r, 5))
for r in range(4, 14):
    walls.append((r, 10))
# Tuong ngang
for c in range(0, 5):
    walls.append((7, c))
for c in range(8, 13):
    walls.append((3, c))
for c in range(2, 8):
    walls.append((12, c))
# Them mot so tuong nho
for r in range(9, 13):
    walls.append((r, 2))
for c in range(11, 15):
    walls.append((7, c))

start_pos = (0, 0)
goal_pos = (15, 15)


# ===================== CHAY SO SANH =====================

if __name__ == "__main__":
    print("=" * 50)
    print("  SO SANH A* VOI CAC TRONG SO KHAC NHAU")
    print("=" * 50)
    print()

    test_weights = [1, 2, 5, 10]

    for w in test_weights:
        grid = create_grid(ROWS, COLS, walls)
        path, cost, explored = a_star(grid, start_pos, goal_pos, ROWS, COLS, weight=w)

        if w == 1:
            label = "A* chuan (toi uu)"
        else:
            label = f"Weighted A* (w={w})"

        print(f"--- {label} ---")
        if path:
            print(f"  Chi phi (g):      {cost}")
            print(f"  So buoc:          {len(path)}")
            print(f"  Node da xet:      {explored}")
            print()
            print_result(grid, path, ROWS, COLS)
        else:
            print("  Khong tim thay duong di!")
        print()
