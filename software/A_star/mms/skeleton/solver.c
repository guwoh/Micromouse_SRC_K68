/*
 * =============================================================
 *   A* SOLVER CHO MMS MICROMOUSE SIMULATOR
 *
 *   Tinh nang:
 *   - Thuat toan A* tim duong ngan nhat
 *   - Tu dong kham pha me cung (vua di vua cap nhat tuong)
 *   - Hien thi chi phi g, h, f tren moi o
 *   - To mau cac o da kham pha, duong di, va dich
 * =============================================================
 */

#include "solver.h"
#include "API.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ===================== HANG SO ===================== */

#define MAX_CELLS (MAZE_SIZE * MAZE_SIZE) /* 256 o */
#define INF 9999

/* Huong tuong (index cho wall_map) */
#define DIR_N 0
#define DIR_E 1
#define DIR_S 2
#define DIR_W 3

/* Mau sac MMS */
/* G = Green, B = Blue, Y = Yellow, R = Red, c = cyan, o = orange */

/* ================== BIEN TOAN CUC ================== */

/* --- Ban do tuong ---
 * wall[x][y][dir] :
 *   0 = chua biet (gia su khong co tuong)
 *   1 = CO tuong
 *   2 = KHONG co tuong (da xac nhan)
 */
static int wall[MAZE_SIZE][MAZE_SIZE][4];

/* --- Trang thai chuot --- */
static int mouse_x = 0;
static int mouse_y = 0;
static Heading mouse_dir = NORTH;

/* --- A* data --- */
static int g_cost[MAZE_SIZE][MAZE_SIZE];
static int f_cost[MAZE_SIZE][MAZE_SIZE];
static int closed[MAZE_SIZE][MAZE_SIZE];
static int parent_x[MAZE_SIZE][MAZE_SIZE];
static int parent_y[MAZE_SIZE][MAZE_SIZE];

/* Open list (dung mang don gian) */
static int open_x[MAX_CELLS];
static int open_y[MAX_CELLS];
static int open_count = 0;

/* Duong di tim duoc */
static int path_x[MAX_CELLS];
static int path_y[MAX_CELLS];
static int path_len = 0;
static int path_index = 0; /* Buoc hien tai tren duong di */

/* Trang thai solver */
static int initialized = 0;
static int reached_goal = 0;

/* Dem so buoc va so lan chay A* */
static int total_steps = 0;
static int astar_runs = 0;

/* Che do nghiem ngat: chi di qua passage DA XAC NHAN mo (value=2)
 * 0 = binh thuong (unknown=khong co tuong, dung khi kham pha)
 * 1 = strict (unknown=co tuong, dung khi tinh duong toi uu cuoi) */
static int strict_mode = 0;

/* ================== HAM HO TRO ================== */

/* Manhattan distance den trung tam (chon o gan nhat trong 4 o trung tam) */
int heuristic(int x, int y) {
  /* 4 o trung tam cua me cung 16x16: (7,7), (7,8), (8,7), (8,8) */
  int centers[4][2] = {{7, 7}, {7, 8}, {8, 7}, {8, 8}};
  int min_dist = INF;
  int i;
  for (i = 0; i < 4; i++) {
    int d = abs(x - centers[i][0]) + abs(y - centers[i][1]);
    if (d < min_dist)
      min_dist = d;
  }
  return min_dist;
}

/* Kiem tra co phai o trung tam khong */
int is_goal(int x, int y) { return (x == 7 || x == 8) && (y == 7 || y == 8); }

/* ================== OPEN LIST ================== */

void open_add(int x, int y) {
  open_x[open_count] = x;
  open_y[open_count] = y;
  open_count++;
}

int open_get_best(void) {
  int best = 0;
  int best_f = f_cost[open_x[0]][open_y[0]];
  int i;
  for (i = 1; i < open_count; i++) {
    int cf = f_cost[open_x[i]][open_y[i]];
    if (cf < best_f) {
      best_f = cf;
      best = i;
    }
  }
  return best;
}

void open_remove(int idx) {
  open_x[idx] = open_x[open_count - 1];
  open_y[idx] = open_y[open_count - 1];
  open_count--;
}

int open_contains(int x, int y) {
  int i;
  for (i = 0; i < open_count; i++) {
    if (open_x[i] == x && open_y[i] == y)
      return 1;
  }
  return 0;
}

/* ================== TUONG ================== */

/* Huong doi dien */
int opposite_dir(int dir) {
  /* N<->S, E<->W */
  switch (dir) {
  case DIR_N:
    return DIR_S;
  case DIR_E:
    return DIR_W;
  case DIR_S:
    return DIR_N;
  case DIR_W:
    return DIR_E;
  }
  return -1;
}

/* Toa do lan can theo huong */
void neighbor_pos(int x, int y, int dir, int *nx, int *ny) {
  *nx = x;
  *ny = y;
  switch (dir) {
  case DIR_N:
    (*ny)++;
    break;
  case DIR_E:
    (*nx)++;
    break;
  case DIR_S:
    (*ny)--;
    break;
  case DIR_W:
    (*nx)--;
    break;
  }
}

/* Kiem tra co tuong chan khong (dung cho A*) */
int has_wall(int x, int y, int dir) {
  /* Tuong bien me cung */
  if (dir == DIR_N && y == MAZE_SIZE - 1)
    return 1;
  if (dir == DIR_S && y == 0)
    return 1;
  if (dir == DIR_E && x == MAZE_SIZE - 1)
    return 1;
  if (dir == DIR_W && x == 0)
    return 1;

  /* Tuong da biet */
  if (strict_mode) {
    /* Strict: chi cho di neu DA XAC NHAN khong co tuong (value=2) */
    return (wall[x][y][dir] != 2);
  }
  return (wall[x][y][dir] == 1);
}

/* Set tuong 2 phia (o hien tai va o ben kia) */
void set_wall(int x, int y, int dir, int value) {
  int nx, ny;
  wall[x][y][dir] = value;

  /* Cap nhat o ben kia tuong */
  neighbor_pos(x, y, dir, &nx, &ny);
  if (nx >= 0 && nx < MAZE_SIZE && ny >= 0 && ny < MAZE_SIZE) {
    wall[nx][ny][opposite_dir(dir)] = value;
  }

  /* Hien thi tuong tren simulator */
  if (value == 1) {
    char dir_char;
    switch (dir) {
    case DIR_N:
      dir_char = 'n';
      break;
    case DIR_E:
      dir_char = 'e';
      break;
    case DIR_S:
      dir_char = 's';
      break;
    case DIR_W:
      dir_char = 'w';
      break;
    default:
      dir_char = 'n';
      break;
    }
    API_setWall(x, y, dir_char);
  }
}

/* ================== SCAN SENSOR ================== */

/* Chuyen tu huong chuot (NORTH/EAST/SOUTH/WEST)
 * sang huong tuong (DIR_N/DIR_E/DIR_S/DIR_W)
 * cho cac cam bien front/right/left */

int front_dir(Heading h) {
  switch (h) {
  case NORTH:
    return DIR_N;
  case EAST:
    return DIR_E;
  case SOUTH:
    return DIR_S;
  case WEST:
    return DIR_W;
  }
  return DIR_N;
}

int right_dir(Heading h) {
  switch (h) {
  case NORTH:
    return DIR_E;
  case EAST:
    return DIR_S;
  case SOUTH:
    return DIR_W;
  case WEST:
    return DIR_N;
  }
  return DIR_E;
}

int left_dir(Heading h) {
  switch (h) {
  case NORTH:
    return DIR_W;
  case EAST:
    return DIR_N;
  case SOUTH:
    return DIR_E;
  case WEST:
    return DIR_S;
  }
  return DIR_W;
}

/* Doc cam bien va cap nhat ban do tuong */
void scan_walls(void) {
  int fd = front_dir(mouse_dir);
  int rd = right_dir(mouse_dir);
  int ld = left_dir(mouse_dir);

  if (API_wallFront()) {
    set_wall(mouse_x, mouse_y, fd, 1); /* Co tuong */
  } else {
    set_wall(mouse_x, mouse_y, fd, 2); /* Khong co tuong */
  }

  if (API_wallRight()) {
    set_wall(mouse_x, mouse_y, rd, 1);
  } else {
    set_wall(mouse_x, mouse_y, rd, 2);
  }

  if (API_wallLeft()) {
    set_wall(mouse_x, mouse_y, ld, 1);
  } else {
    set_wall(mouse_x, mouse_y, ld, 2);
  }
}

/* ================== HIEN THI ================== */

/* Hien thi chi phi g tren moi o */
void display_costs(void) {
  int x, y;
  char buf[16];

  API_clearAllColor();
  API_clearAllText();

  for (x = 0; x < MAZE_SIZE; x++) {
    for (y = 0; y < MAZE_SIZE; y++) {
      if (g_cost[x][y] < INF && g_cost[x][y] >= 0) {
        /* Hien thi g cost */
        sprintf(buf, "%d", g_cost[x][y]);
        API_setText(x, y, buf);

        /* --- TO MAU THEO CHI PHI --- */
        if (is_goal(x, y)) {
          /* O trung tam: mau VANG */
          API_setColor(x, y, 'Y');
        } else if (closed[x][y]) {
          /* O da xet (closed): mau XANH DUONG nhat */
          API_setColor(x, y, 'B');
        }
      }
    }
  }
}

/* To mau duong di */
void display_path(void) {
  int i;
  char buf[16];

  for (i = 0; i < path_len; i++) {
    int px = path_x[i];
    int py = path_y[i];

    if (is_goal(px, py)) {
      API_setColor(px, py, 'Y'); /* Dich: VANG */
    } else if (i == 0) {
      API_setColor(px, py, 'G'); /* Start: XANH LA */
    } else {
      API_setColor(px, py, 'R'); /* Duong di: CYAN */
    }

    /* Hien thi thu tu buoc */
    sprintf(buf, "%d", i);
    API_setText(px, py, buf);
  }

  /* Hien thi vi tri chuot */
  API_setColor(mouse_x, mouse_y, 'o');
}

/* Hien thi thong tin debug */
void display_info(void) {
  char buf[64];
  sprintf(buf, "Steps:%d AStar:%d Path:%d", total_steps, astar_runs, path_len);
  debug_log(buf);
}

/* ================== THUAT TOAN A* ================== */

int run_astar(int sx, int sy) {
  int x, y, i;

  /* Reset */
  for (x = 0; x < MAZE_SIZE; x++) {
    for (y = 0; y < MAZE_SIZE; y++) {
      g_cost[x][y] = INF;
      f_cost[x][y] = INF;
      closed[x][y] = 0;
      parent_x[x][y] = -1;
      parent_y[x][y] = -1;
    }
  }
  open_count = 0;
  path_len = 0;

  /* Khoi tao start */
  g_cost[sx][sy] = 0;
  f_cost[sx][sy] = heuristic(sx, sy);
  open_add(sx, sy);

  /* 4 huong */
  int dirs[] = {DIR_N, DIR_E, DIR_S, DIR_W};

  /* Vong lap chinh */
  while (open_count > 0) {
    int best = open_get_best();
    int cx = open_x[best];
    int cy = open_y[best];

    /* Da den dich? */
    if (is_goal(cx, cy)) {
      /* Truy nguoc duong di */
      int tx = cx, ty = cy;
      int tmp_x[MAX_CELLS], tmp_y[MAX_CELLS];
      int tmp_len = 0;

      while (tx != -1 && ty != -1) {
        tmp_x[tmp_len] = tx;
        tmp_y[tmp_len] = ty;
        tmp_len++;
        int px = parent_x[tx][ty];
        int py = parent_y[tx][ty];
        tx = px;
        ty = py;
      }

      /* Dao nguoc */
      path_len = tmp_len;
      for (i = 0; i < tmp_len; i++) {
        path_x[i] = tmp_x[tmp_len - 1 - i];
        path_y[i] = tmp_y[tmp_len - 1 - i];
      }

      /* Hien thi ket qua */
      display_costs();
      display_path();

      {
        char buf[64];
        sprintf(buf, "A* OK! Cost=%d Path=%d steps", g_cost[cx][cy], path_len);
        debug_log(buf);
      }

      astar_runs++;
      return 1; /* Tim thay */
    }

    open_remove(best);
    closed[cx][cy] = 1;

    /* Xet 4 huong */
    for (i = 0; i < 4; i++) {
      int dir = dirs[i];

      /* Kiem tra tuong */
      if (has_wall(cx, cy, dir))
        continue;

      int nx, ny;
      neighbor_pos(cx, cy, dir, &nx, &ny);

      /* Kiem tra bien */
      if (nx < 0 || nx >= MAZE_SIZE || ny < 0 || ny >= MAZE_SIZE)
        continue;

      /* Da xet roi */
      if (closed[nx][ny])
        continue;

      int tent_g = g_cost[cx][cy] + 1;

      if (tent_g < g_cost[nx][ny]) {
        g_cost[nx][ny] = tent_g;
        f_cost[nx][ny] = tent_g + heuristic(nx, ny);
        parent_x[nx][ny] = cx;
        parent_y[nx][ny] = cy;

        if (!open_contains(nx, ny)) {
          open_add(nx, ny);
        }
      }
    }
  }

  debug_log("A* FAIL: Khong tim thay duong!");
  astar_runs++;
  return 0; /* Khong tim thay */
}

/* ================== CHUYEN DOI HANH DONG ================== */

/* Tu huong can di (direction) va huong hien tai (heading)
 * -> tra ve Action can thuc hien */
Action get_action_for_direction(int target_dir) {
  /* Tinh goc quay can thiet */
  int diff = (target_dir - (int)mouse_dir + 4) % 4;

  switch (diff) {
  case 0:
    return FORWARD; /* Cung huong -> di thang */
  case 1:
    return RIGHT; /* Lech phai 90 */
  case 3:
    return LEFT; /* Lech trai 90 */
  case 2:
    return RIGHT; /* Quay 180 -> quay phai (lan 1) */
  }
  return IDLE;
}

/* Tim huong tu (x1,y1) sang (x2,y2) */
int direction_to(int x1, int y1, int x2, int y2) {
  int dx_val = x2 - x1;
  int dy_val = y2 - y1;

  if (dy_val == 1)
    return DIR_N;
  if (dy_val == -1)
    return DIR_S;
  if (dx_val == 1)
    return DIR_E;
  if (dx_val == -1)
    return DIR_W;

  return -1;
}

/* Cap nhat vi tri chuot sau khi di chuyen */
void move_mouse_forward(void) {
  switch (mouse_dir) {
  case NORTH:
    mouse_y++;
    break;
  case SOUTH:
    mouse_y--;
    break;
  case EAST:
    mouse_x++;
    break;
  case WEST:
    mouse_x--;
    break;
  }
  total_steps++;
}

void turn_mouse_right(void) { mouse_dir = (Heading)((mouse_dir + 1) % 4); }

void turn_mouse_left(void) { mouse_dir = (Heading)((mouse_dir + 3) % 4); }

/* ================== SOLVER CHINH ================== */

/* Trang thai may trang thai cua solver */
typedef enum {
  STATE_SCAN,        /* Doc sensor, cap nhat tuong */
  STATE_COMPUTE,     /* Chay A*, tim duong */
  STATE_FOLLOW_PATH, /* Di theo duong da tim */
  STATE_DONE         /* Da den dich */
} SolverState;

static SolverState state = STATE_SCAN;
static int need_turn = 0;        /* Can quay them 1 lan (cho quay 180) */
static int turn_target_dir = -1; /* Huong can quay den */

Action aStarSolver() {
  /* Khoi tao lan dau */
  if (!initialized) {
    int x, y, d;
    for (x = 0; x < MAZE_SIZE; x++)
      for (y = 0; y < MAZE_SIZE; y++)
        for (d = 0; d < 4; d++)
          wall[x][y][d] = 0; /* Chua biet tuong nao */

    mouse_x = 0;
    mouse_y = 0;
    mouse_dir = NORTH;
    initialized = 1;
    state = STATE_SCAN;

    debug_log("=== A* SOLVER STARTED ===");
    debug_log("Goal: center (7,7)-(8,8)");

    /* Dat tuong bien me cung */
    for (x = 0; x < MAZE_SIZE; x++) {
      set_wall(x, 0, DIR_S, 1);             /* Bien duoi */
      set_wall(x, MAZE_SIZE - 1, DIR_N, 1); /* Bien tren */
    }
    for (y = 0; y < MAZE_SIZE; y++) {
      set_wall(0, y, DIR_W, 1);             /* Bien trai */
      set_wall(MAZE_SIZE - 1, y, DIR_E, 1); /* Bien phai */
    }
  }

  /* Kiem tra da den dich */
  if (is_goal(mouse_x, mouse_y) && !reached_goal) {
    reached_goal = 1;
    state = STATE_DONE;

    char buf[64];
    sprintf(buf, "=== REACHED GOAL! Total steps: %d ===", total_steps);
    debug_log(buf);

    /* Chay lai A* tu (0,0) tren ban do DA BIET de tim duong NGAN NHAT */
    /* Bat strict mode: chi di qua duong DA XAC NHAN khong co tuong */
    debug_log("Computing optimal path from (0,0)...");
    strict_mode = 1;
    run_astar(0, 0);
    strict_mode = 0;

    /* Xoa het mau cu, ve lai duong di toi uu noi bat */
    API_clearAllColor();
    API_clearAllText();

    {
      int i;
      for (i = 0; i < path_len; i++) {
        int px = path_x[i];
        int py = path_y[i];

        /* Mau sac */
        if (is_goal(px, py)) {
          API_setColor(px, py, 'Y'); /* Dich: VANG */
        } else if (i == 0) {
          API_setColor(px, py, 'G'); /* Start: XANH LA */
        } else {
          API_setColor(px, py, 'c'); /* Duong di: CYAN */
        }

        /* Hien thi so buoc */
        sprintf(buf, "%d", i);
        API_setText(px, py, buf);
      }
    }

    /* Hien thi chi phi tren debug */
    sprintf(buf, "SHORTEST PATH: %d steps (from start to goal)", path_len - 1);
    debug_log(buf);

    return IDLE;
  }

  if (state == STATE_DONE) {
    return IDLE;
  }

  /* Xu ly quay 180 do (can 2 lan quay phai) */
  if (need_turn) {
    need_turn = 0;
    turn_mouse_right();
    /* Sau khi quay xong, di toi */
    state = STATE_FOLLOW_PATH;
    return RIGHT;
  }

  /* ---- STATE MACHINE ---- */

  switch (state) {

  case STATE_SCAN: {
    /* Doc sensor tai vi tri hien tai */
    scan_walls();

    /* To mau o da tham */
    API_setColor(mouse_x, mouse_y, 'o'); /* Orange: da tham */

    state = STATE_COMPUTE;
    return IDLE; /* Khong di chuyen, chi doc sensor */
  }

  case STATE_COMPUTE: {
    /* Chay A* tu vi tri hien tai */
    int found = run_astar(mouse_x, mouse_y);

    if (!found) {
      debug_log("ERROR: No path found!");
      return IDLE;
    }

    /* Tim vi tri hien tai tren duong di */
    path_index = 0;
    {
      int i;
      for (i = 0; i < path_len; i++) {
        if (path_x[i] == mouse_x && path_y[i] == mouse_y) {
          path_index = i + 1; /* Buoc tiep theo */
          break;
        }
      }
    }

    display_info();
    state = STATE_FOLLOW_PATH;
    return IDLE;
  }

  case STATE_FOLLOW_PATH: {
    /* Da het duong di? -> scan lai */
    if (path_index >= path_len) {
      state = STATE_SCAN;
      return IDLE;
    }

    /* Lay buoc tiep theo */
    int next_x = path_x[path_index];
    int next_y = path_y[path_index];

    /* Tim huong can di */
    int target = direction_to(mouse_x, mouse_y, next_x, next_y);

    if (target == -1) {
      /* Loi: buoc tiep theo khong lien ke */
      debug_log("ERROR: next step not adjacent!");
      state = STATE_SCAN;
      return IDLE;
    }

    /* Tinh so lan quay */
    int diff = (target - (int)mouse_dir + 4) % 4;

    if (diff == 0) {
      /* Di thang */
      /* Kiem tra tuong truoc khi di (co the moi phat hien) */
      if (API_wallFront()) {
        /* Tường mới! Cap nhat va tinh lai */
        set_wall(mouse_x, mouse_y, front_dir(mouse_dir), 1);
        state = STATE_SCAN;
        return IDLE;
      }

      move_mouse_forward();
      API_setColor(mouse_x, mouse_y, 'w'); /* Xanh la: dang di */

      /* Hien thi so buoc tren o */
      {
        char buf[16];
        sprintf(buf, "%d", total_steps);
        API_setText(mouse_x, mouse_y, buf);
      }

      path_index++;

      /* Scan lai sau moi buoc di */
      state = STATE_SCAN;
      return FORWARD;
    } else if (diff == 1) {
      /* Quay phai */
      turn_mouse_right();
      /* Sau khi quay, can scan lai vi co the thay tuong moi */
      state = STATE_SCAN;
      return RIGHT;
    } else if (diff == 3) {
      /* Quay trai */
      turn_mouse_left();
      state = STATE_SCAN;
      return LEFT;
    } else if (diff == 2) {
      /* Quay 180: quay phai 2 lan */
      turn_mouse_right();
      need_turn = 1; /* Lan sau quay tiep */
      return RIGHT;
    }

    return IDLE;
  }

  case STATE_DONE:
    return IDLE;
  }

  return IDLE;
}

/* ================== SOLVER ENTRY POINT ================== */

Action solver() {
  /* Dung A* solver */
  return aStarSolver();
}

/* ================== THUAT TOAN CU (GIU LAI) ================== */

Action leftWallFollower() {
  if (API_wallFront()) {
    if (API_wallLeft()) {
      return RIGHT;
    }
    return LEFT;
  }
  return FORWARD;
}

Action floodFill() { return IDLE; }