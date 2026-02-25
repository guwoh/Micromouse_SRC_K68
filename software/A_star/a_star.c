/*
 * =============================================================
 *   THUAT TOAN A* - CHUYEN TU PYTHON SANG C
 *   Huong dan step-by-step cho Micromouse
 * =============================================================
 *
 *   TONG QUAN CHUYEN DOI PYTHON -> C:
 *
 *   Python                  C
 *   ------                  ---
 *   class Node         -->  struct Node
 *   list (dong)        -->  mang tinh [MAX_SIZE]
 *   float('inf')       -->  INT_MAX hoac 9999
 *   None               -->  NULL
 *   True/False          -->  1/0 (hoac bool trong stdbool.h)
 *   append/remove       -->  quan ly bang chi so (index)
 *   in / not in         -->  mang bool 2D
 * =============================================================
 */

#include <stdio.h>
#include <stdlib.h>  // abs()
#include <limits.h>  // INT_MAX

/* ========================== STEP 1 ==========================
 *   DINH NGHIA HANG SO
 *
 *   Python: rows, cols la bien dong
 *   C:      Phai dinh nghia TRUOC bang #define (hang so)
 *
 *   Trong Micromouse thuc te: ROWS=16, COLS=16
 *   O day dung 8x8 de demo
 * ============================================================ */

#define ROWS 8
#define COLS 8
#define MAX_OPEN (ROWS * COLS)  // Kich thuoc toi da cua Open List


/* ========================== STEP 2 ==========================
 *   KHAI BAO STRUCT NODE
 *
 *   Python:                        C:
 *   class Node:                    typedef struct { ... } Node;
 *       self.row = row             int row;
 *       self.g = float('inf')      int g;    // dung 9999 thay vo cuc
 *       self.parent = None         int parent_row, parent_col; (*)
 *
 *   (*) Trong C, thay vi luu con tro parent, ta luu TOA DO
 *       cua node cha. Don gian hon va khong can con tro.
 * ============================================================ */

typedef struct {
    int row;
    int col;
    int g;          // Chi phi thuc tu Start -> day
    int h;          // Heuristic: uoc luong tu day -> Goal
    int f;          // f = g + h
    int is_wall;    // 1 = tuong, 0 = di duoc
    int parent_row; // Toa do hang cua node cha (-1 = khong co)
    int parent_col; // Toa do cot cua node cha
} Node;


/* ========================== STEP 3 ==========================
 *   KHAI BAO BIEN TOAN CUC
 *
 *   Python: grid = [[Node(r,c) for c] for r]   (tao dong)
 *   C:      Node grid[ROWS][COLS];              (mang tinh 2D)
 *
 *   Python: closed_list = []  (list dong)
 *   C:      int closed[ROWS][COLS] = {0};  (mang bool 2D - nhanh hon!)
 *
 *   Python: open_list = []    (list dong)
 *   C:      Luu toa do cac node trong mang
 * ============================================================ */

Node grid[ROWS][COLS];          // Me cung: mang 2D cac Node

int closed[ROWS][COLS];         // 1 = da xet, 0 = chua xet
                                // Thay cho closed_list trong Python
                                // Tim kiem O(1) thay vi O(n)!

// Open List: luu toa do cac node can xet
int open_rows[MAX_OPEN];       // Mang luu hang cua cac node trong open list
int open_cols[MAX_OPEN];       // Mang luu cot cua cac node trong open list
int open_count = 0;            // So luong node trong open list
                               // Python: len(open_list)

// Ket qua: duong di
int path_rows[MAX_OPEN];       // Luu hang cua cac buoc tren duong di
int path_cols[MAX_OPEN];       // Luu cot cua cac buoc tren duong di
int path_length = 0;           // Chieu dai duong di


/* ========================== STEP 4 ==========================
 *   HAM HEURISTIC (Manhattan Distance)
 *
 *   Python:
 *       def heuristic_manhattan(node, goal):
 *           return abs(node.row - goal.row) + abs(node.col - goal.col)
 *
 *   C: Giong het, chi khac cu phap
 * ============================================================ */

int heuristic(int row1, int col1, int row2, int col2)
{
    return abs(row1 - row2) + abs(col1 - col2);
}


/* ========================== STEP 5 ==========================
 *   THEM NODE VAO OPEN LIST
 *
 *   Python: open_list.append(node)
 *   C:      Luu toa do vao mang, tang open_count
 * ============================================================ */

void open_list_add(int row, int col)
{
    open_rows[open_count] = row;
    open_cols[open_count] = col;
    open_count++;
}


/* ========================== STEP 6 ==========================
 *   TIM NODE CO f NHO NHAT TRONG OPEN LIST
 *
 *   Python:
 *       def get_lowest_f_node(open_list):
 *           best = open_list[0]
 *           for node in open_list:
 *               if node.f < best.f:
 *                   best = node
 *           return best
 *
 *   C: Duyet mang, tra ve CHI SO (index) cua node tot nhat
 *      (Trong du an thuc te, dung Min-Heap se nhanh hon)
 * ============================================================ */

int open_list_get_best(void)
{
    int best_index = 0;
    int best_f = grid[open_rows[0]][open_cols[0]].f;

    int i;
    for (i = 1; i < open_count; i++) {
        int current_f = grid[open_rows[i]][open_cols[i]].f;
        if (current_f < best_f) {
            best_f = current_f;
            best_index = i;
        }
    }

    return best_index;
}


/* ========================== STEP 7 ==========================
 *   XOA NODE KHOI OPEN LIST
 *
 *   Python: open_list.remove(current)
 *   C:      Chuyen phan tu cuoi vao vi tri can xoa, giam open_count
 *           (Thay vi dich ca mang - tiet kiem thoi gian!)
 * ============================================================ */

void open_list_remove(int index)
{
    // Thay phan tu can xoa bang phan tu cuoi cung
    open_rows[index] = open_rows[open_count - 1];
    open_cols[index] = open_cols[open_count - 1];
    open_count--;
}


/* ========================== STEP 8 ==========================
 *   KIEM TRA NODE CO TRONG OPEN LIST KHONG
 *
 *   Python: if neighbor in open_list:
 *   C:      Duyet mang kiem tra toa do
 * ============================================================ */

int is_in_open_list(int row, int col)
{
    int i;
    for (i = 0; i < open_count; i++) {
        if (open_rows[i] == row && open_cols[i] == col) {
            return 1;  // Tim thay (True)
        }
    }
    return 0;  // Khong tim thay (False)
}


/* ========================== STEP 9 ==========================
 *   TRUY NGUOC DUONG DI (tu Goal ve Start)
 *
 *   Python:
 *       def reconstruct_path(node):
 *           path = []
 *           current = node
 *           while current is not None:
 *               path.append((current.row, current.col))
 *               current = current.parent
 *           path.reverse()
 *
 *   C: Dung mang, sau do dao nguoc
 * ============================================================ */

void reconstruct_path(int goal_row, int goal_col)
{
    int r, c;
    int temp_rows[MAX_OPEN];
    int temp_cols[MAX_OPEN];
    int temp_len = 0;
    int i;

    // Truy nguoc tu Goal ve Start
    r = goal_row;
    c = goal_col;

    while (r != -1 && c != -1) {  // -1 = khong co parent (la Start)
        temp_rows[temp_len] = r;
        temp_cols[temp_len] = c;
        temp_len++;

        // Lay parent
        int pr = grid[r][c].parent_row;
        int pc = grid[r][c].parent_col;
        r = pr;
        c = pc;
    }

    // Dao nguoc: temp dang luu Goal->Start, can Start->Goal
    path_length = temp_len;
    for (i = 0; i < temp_len; i++) {
        path_rows[i] = temp_rows[temp_len - 1 - i];
        path_cols[i] = temp_cols[temp_len - 1 - i];
    }
}


/* ========================== STEP 10 ==========================
 *   THUAT TOAN A* CHINH
 *
 *   Day la phan CORE - logic giong het Python,
 *   chi khac cu phap va cach quan ly du lieu.
 *
 *   Python:                          C:
 *   open_list.append(start)     -->  open_list_add(sr, sc)
 *   current = get_lowest()      -->  idx = open_list_get_best()
 *   if current == goal          -->  if (cr == gr && cc == gc)
 *   if neighbor in closed       -->  if (closed[nr][nc])
 *   neighbor.g = tentative_g    -->  grid[nr][nc].g = tentative_g
 * ============================================================ */

int a_star(int start_row, int start_col, int goal_row, int goal_col)
{
    int r, c;

    /* --- Khoi tao --- */

    // Reset tat ca node
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            grid[r][c].g = 9999;          // float('inf') -> 9999
            grid[r][c].h = 0;
            grid[r][c].f = 9999;
            grid[r][c].parent_row = -1;   // None -> -1
            grid[r][c].parent_col = -1;
            closed[r][c] = 0;             // Chua xet
        }
    }

    // Khoi tao Open List
    open_count = 0;

    // Thiet lap node bat dau
    grid[start_row][start_col].g = 0;
    grid[start_row][start_col].h = heuristic(start_row, start_col, goal_row, goal_col);
    grid[start_row][start_col].f = grid[start_row][start_col].g
                                 + grid[start_row][start_col].h;

    open_list_add(start_row, start_col);

    /* --- 4 huong di chuyen: Tren, Duoi, Trai, Phai --- */
    int dr[] = {-1, 1, 0, 0};
    int dc[] = {0, 0, -1, 1};

    /* --- Vong lap chinh --- */
    while (open_count > 0) {

        // Buoc 1: Chon node co f nho nhat
        int best_idx = open_list_get_best();
        int cr = open_rows[best_idx];  // current row
        int cc = open_cols[best_idx];  // current col

        // Buoc 2: Da den dich chua?
        if (cr == goal_row && cc == goal_col) {
            printf("[OK] Tim thay duong di! Chi phi = %d\n", grid[cr][cc].g);
            reconstruct_path(goal_row, goal_col);
            return 1;  // Thanh cong
        }

        // Buoc 3: Chuyen current tu Open -> Closed
        open_list_remove(best_idx);
        closed[cr][cc] = 1;

        // Buoc 4: Xet 4 o lan can
        int i;
        for (i = 0; i < 4; i++) {
            int nr = cr + dr[i];  // neighbor row
            int nc = cc + dc[i];  // neighbor col

            // Kiem tra bien
            if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS)
                continue;

            // Kiem tra tuong
            if (grid[nr][nc].is_wall)
                continue;

            // Kiem tra da xet (closed)
            if (closed[nr][nc])
                continue;

            // Tinh chi phi moi
            int tentative_g = grid[cr][cc].g + 1;

            // Neu duong di moi TOT HON
            if (tentative_g < grid[nr][nc].g) {
                // Cap nhat node
                grid[nr][nc].g = tentative_g;
                grid[nr][nc].h = heuristic(nr, nc, goal_row, goal_col);
                grid[nr][nc].f = grid[nr][nc].g + grid[nr][nc].h;
                grid[nr][nc].parent_row = cr;  // Ghi nho node cha
                grid[nr][nc].parent_col = cc;

                // Them vao Open List neu chua co
                if (!is_in_open_list(nr, nc)) {
                    open_list_add(nr, nc);
                }
            }
        }
    }

    // Khong tim thay duong di
    printf("[FAIL] Khong co duong di!\n");
    return 0;
}


/* ========================== STEP 11 ==========================
 *   TAO ME CUNG VA IN KET QUA
 *
 *   Python: maze_data = [[0,0,1,...], ...]
 *   C:      Mang 2D int maze_data[ROWS][COLS]
 * ============================================================ */

void create_maze(void)
{
    int maze_data[ROWS][COLS] = {
    /*       0  1  2  3  4  5  6  7  */
    /* 0 */ {0, 0, 0, 1, 0, 0, 0, 0},
    /* 1 */ {1, 1, 0, 1, 0, 1, 1, 0},
    /* 2 */ {0, 0, 0, 0, 0, 0, 1, 0},
    /* 3 */ {0, 1, 1, 1, 1, 0, 0, 0},
    /* 4 */ {0, 0, 0, 0, 1, 0, 1, 1},
    /* 5 */ {1, 1, 0, 0, 0, 0, 0, 0},
    /* 6 */ {0, 0, 0, 1, 1, 1, 1, 0},
    /* 7 */ {0, 1, 0, 0, 0, 0, 0, 0},
    };

    int r, c;
    for (r = 0; r < ROWS; r++) {
        for (c = 0; c < COLS; c++) {
            grid[r][c].row = r;
            grid[r][c].col = c;
            grid[r][c].is_wall = maze_data[r][c];
        }
    }
}


void print_maze(void)
{
    int r, c, i;

    // Tao mang danh dau duong di (de in nhanh)
    int on_path[ROWS][COLS] = {{0}};
    for (i = 0; i < path_length; i++) {
        on_path[path_rows[i]][path_cols[i]] = 1;
    }

    printf("\n===================================\n");
    printf("  ME CUNG VA DUONG DI A*\n");
    printf("===================================\n");

    // Header cot
    printf("    ");
    for (c = 0; c < COLS; c++) {
        printf(" %d ", c);
    }
    printf("\n");

    // In tung hang
    for (r = 0; r < ROWS; r++) {
        printf(" %d |", r);
        for (c = 0; c < COLS; c++) {
            if (i > 0 && r == path_rows[0] && c == path_cols[0]) {
                printf(" S ");              // Start
            } else if (i > 0 && r == path_rows[path_length-1]
                              && c == path_cols[path_length-1]) {
                printf(" G ");              // Goal
            } else if (on_path[r][c]) {
                printf(" * ");              // Duong di
            } else if (grid[r][c].is_wall) {
                printf(" # ");              // Tuong
            } else {
                printf(" . ");              // Trong
            }
        }
        printf("| %d\n", r);
    }

    // In chi tiet duong di
    if (path_length > 0) {
        printf("\nDuong di (%d buoc):\n", path_length);
        for (i = 0; i < path_length; i++) {
            if (i == 0)
                printf("   Buoc %d: (%d, %d) (Start)\n", i, path_rows[i], path_cols[i]);
            else if (i == path_length - 1)
                printf("   Buoc %d: (%d, %d) (Goal)\n", i, path_rows[i], path_cols[i]);
            else
                printf("   Buoc %d: (%d, %d)\n", i, path_rows[i], path_cols[i]);
        }
    }
}


/* ========================== STEP 12 ==========================
 *   HAM MAIN
 *
 *   Python: if __name__ == "__main__":
 *   C:      int main(void)
 * ============================================================ */

int main(void)
{
    printf("THUAT TOAN A* - MICROMOUSE (C VERSION)\n");
    printf("========================================\n");

    // Tao me cung
    create_maze();

    // Dinh nghia Start va Goal
    int start_row = 0, start_col = 0;   // Goc tren-trai
    int goal_row = 3, goal_col = 5;     // Trung tam

    printf("Start: (%d, %d)\n", start_row, start_col);
    printf("Goal:  (%d, %d)\n", goal_row, goal_col);

    // Chay A*
    int found = a_star(start_row, start_col, goal_row, goal_col);

    // In ket qua
    if (found) {
        print_maze();
    }

    return 0;
}
