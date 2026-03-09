/*
 * Algorithm.c
 *
 *  Created on: Nov 22, 2025
 *      Author: kinhz
 */

#include <algorithm.h>

uint8_t maze[MAZE_SIZE][MAZE_SIZE] = { 0 };
int16_t distances[MAZE_SIZE][MAZE_SIZE];
struct Coordinate position;
Heading heading;
int destination_X = 6;
int destination_Y = 6;

int reached_center = 0;

void initialize() {
    for (int i = 1; i < MAZE_SIZE - 1; ++i) {
        maze[0][i] = _0001;
        maze[i][0] = _0010;
        maze[i][MAZE_SIZE - 1] = _1000;
        maze[MAZE_SIZE - 1][i] = _0100;
    }
    maze[0][0] = _0011;
    maze[0][MAZE_SIZE - 1] = _1001;
    maze[MAZE_SIZE - 1][0] = _0110;
    maze[MAZE_SIZE - 1][MAZE_SIZE - 1] = _1100;

    resetDistances();

    position.x = 0;
    position.y = 0;
    heading = NORTH;
}


void updateMaze() {
    int x = position.x;
    int y = position.y;

    uint8_t walls = _0000;
    switch (heading) {
        case NORTH:
            if (wall_LF_Check && wall_RF_Check) {
                walls |= _1000;

                if (y + 1 != MAZE_SIZE)
                    maze[x][y + 1] |= _0010;
            }
            if (wall_L_Check) {
                walls |= _0001;
                if (x - 1 >= 0)
                    maze[x - 1][y] |= _0100;
            }
            if (wall_R_Check) {
                walls |= _0100;
                if (x + 1 != MAZE_SIZE)
                    maze[x + 1][y] |= _0001;
            }
            break;
        case EAST:
            if (wall_LF_Check && wall_RF_Check) {
                walls |= _0100;
                if (x + 1 != MAZE_SIZE)
                    maze[x + 1][y] |= _0001;
            }
            if (wall_L_Check) {
                walls |= _1000;
                if (y + 1 != MAZE_SIZE)
                    maze[x][y + 1] |= _0010;
            }
            if (wall_R_Check) {
                walls |= _0010;
                if (y - 1 >= 0)
                    maze[x][y - 1] |= _1000;
            }
            break;
        case SOUTH:
            if (wall_LF_Check && wall_RF_Check) {
                walls |= _0010;
                if (y - 1 >= 0)
                    maze[x][y - 1] |= _1000;
            }
            if (wall_L_Check) {
                walls |= _0100;
                if (x + 1 != MAZE_SIZE)
                    maze[x + 1][y] |= _0001;
            }
            if (wall_R_Check) {
                walls |= _0001;
                if (x - 1 >= 0)
                    maze[x - 1][y] |= _0100;
            }
            break;
        case WEST:
            if (wall_LF_Check && wall_RF_Check) {
                walls |= _0001;
                if (x - 1 >= 0)
                    maze[x - 1][y] |= _0100;
            }
            if (wall_L_Check) {
                walls |= _0010;
                if (y - 1 >= 0)
                    maze[x][y - 1] |= _1000;
            }
            if (wall_R_Check) {
                walls |= _1000;
                if (y + 1 != MAZE_SIZE)
                    maze[x][y + 1] |= _0010;
            }
            break;
    }

    maze[x][y] |= walls;

}

int xyToSquare(int x, int y) {
    return x + MAZE_SIZE * y;
}

struct Coordinate squareToCoord(int square) {
    struct Coordinate coord;
    coord.x = square % MAZE_SIZE;
    coord.y = square / MAZE_SIZE;
    return coord;
}

void resetDistances() {
    for (int x = 0; x < MAZE_SIZE; ++x) {
        for (int y = 0; y < MAZE_SIZE; ++y) {
            distances[x][y] = -1;
        }
    }

    if (!reached_center) {
        // if (MAZE_SIZE % 2 == 0) {
        //     distances[MAZE_SIZE/2][MAZE_SIZE/2] = 0;
        //     distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2] = 0;
        //     distances[MAZE_SIZE/2][MAZE_SIZE/2 - 1] = 0;
        //     distances[MAZE_SIZE/2 - 1][MAZE_SIZE/2 - 1] = 0;
        // }
        if (MAZE_SIZE % 2 == 0) {
            distances[destination_X][destination_Y] = 0;
            distances[destination_X-1][destination_Y] = 0;
            distances[destination_X][destination_Y-1] = 0;
            distances[destination_X-1][destination_Y-1] = 0;
        }
        else {
            distances[MAZE_SIZE/2][MAZE_SIZE/2] = 0;
        }
    }
    else {
        distances[0][0] = 0;
    }
}

int isWallInDirection(int x, int y, Heading direction) {
    switch (direction) {
        case NORTH:
            if (maze[x][y] >= 8)
                return 1;
            break;
        case EAST:
            if (maze[x][y] % 8 >= 4)
                return 1;
            break;
        case SOUTH:
            if (maze[x][y] % 4 >= 2)
                return 1;
            break;
        case WEST:
            if (maze[x][y] % 2 == 1)
                return 1;
            break;
    }
    return 0;
}

void updateDistances() {
    resetDistances();
    queue squares = queue_create();

    for (int x = 0; x < MAZE_SIZE; ++x) {
        for (int y = 0; y < MAZE_SIZE; ++y) {
            if (distances[x][y] == 0)
                queue_push(squares, xyToSquare(x, y));
        }
    }

    while (!queue_is_empty(squares)) {
        struct Coordinate square = squareToCoord(queue_pop(squares));
        int x = square.x;
        int y = square.y;


        if (isWallInDirection(x, y, NORTH) == 0 && distances[x][y + 1] == -1) {
            distances[x][y + 1] = distances[x][y] + 1;
            queue_push(squares, xyToSquare(x, y + 1));
        }

        if (isWallInDirection(x, y, EAST) == 0 && distances[x + 1][y] == -1) {
            distances[x + 1][y] = distances[x][y] + 1;
            queue_push(squares, xyToSquare(x + 1, y));
        }

        if (isWallInDirection(x, y, SOUTH) == 0 && distances[x][y - 1] == -1) {
            distances[x][y - 1] = distances[x][y] + 1;
            queue_push(squares, xyToSquare(x, y - 1));
        }

        if (isWallInDirection(x, y, WEST) == 0 && distances[x - 1][y] == -1) {
            distances[x - 1][y] = distances[x][y] + 1;
            queue_push(squares, xyToSquare(x - 1, y));
        }
    }
}

void updateHeading(Action nextAction) {
	if (nextAction == FORWARD || nextAction == IDLE) {
		return;
	}
	else if (nextAction == LEFT) {
		switch (heading) {
			case NORTH:
				heading = WEST;
                break;
            case EAST:
                heading = NORTH;
                break;
            case SOUTH:
                heading = EAST;
                break;
            case WEST:
                heading = SOUTH;
                break;
            default:
                break;
        }
    }
    else if (nextAction == RIGHT) {
        switch (heading) {
            case NORTH:
                heading = EAST;
                break;
            case EAST:
                heading = SOUTH;
                break;
            case SOUTH:
                heading = WEST;
                break;
            case WEST:
                heading = NORTH;
                break;
            default:
                break;
        }
    }
}

void updatePosition(Action nextAction) {
    if (nextAction != FORWARD) {
        return;
    }

    switch (heading) {
        case NORTH:
            position.y += 1;
            break;
        case SOUTH:
            position.y -= 1;
            break;
        case EAST:
            position.x += 1;
            break;
        case WEST:
            position.x -= 1;
            break;
        default:
            break;
    }
}

Action solver() {

    if (!reached_center && distances[position.x][position.y] == 0) {
        reached_center = 1;
    }

    else if (reached_center && distances[position.x][position.y] == 0) {
        reached_center = 0;
    }

    updateMaze();
    updateDistances();

    Action action = floodFill();

    updateHeading(action);
    updatePosition(action);
    return action;
}

Action floodFill() {
    uint16_t least_distance = 300;
    Action optimal_move = IDLE;

    if (heading == NORTH) {
        if (!isWallInDirection(position.x, position.y, NORTH) && distances[position.x][position.y + 1] < least_distance) {
            least_distance = distances[position.x][position.y + 1];
            optimal_move = FORWARD;
        }
        if (!isWallInDirection(position.x, position.y, EAST) && distances[position.x + 1][position.y] < least_distance) {
            least_distance = distances[position.x + 1][position.y];
            optimal_move = RIGHT;
        }
        if (!isWallInDirection(position.x, position.y, WEST) && distances[position.x - 1][position.y] < least_distance) {
            least_distance = distances[position.x - 1][position.y];
            optimal_move = LEFT;
        }
    }
    else if (heading == EAST) {
        if (!isWallInDirection(position.x, position.y, EAST) && distances[position.x + 1][position.y] < least_distance) {
            least_distance = distances[position.x + 1][position.y];
            optimal_move = FORWARD;
        }
        if (!isWallInDirection(position.x, position.y, SOUTH) && distances[position.x][position.y - 1] < least_distance) {
            least_distance = distances[position.x][position.y - 1];
            optimal_move = RIGHT;
        }
        if (!isWallInDirection(position.x, position.y, NORTH) && distances[position.x][position.y + 1] < least_distance) {
            least_distance = distances[position.x][position.y + 1];
            optimal_move = LEFT;
        }
    }
    else if (heading == SOUTH) {
        if (!isWallInDirection(position.x, position.y, SOUTH) && distances[position.x][position.y - 1] < least_distance) {
            least_distance = distances[position.x][position.y - 1];
            optimal_move = FORWARD;
        }
        if (!isWallInDirection(position.x, position.y, WEST) && distances[position.x - 1][position.y] < least_distance) {
            least_distance = distances[position.x - 1][position.y];
            optimal_move = RIGHT;
        }
        if (!isWallInDirection(position.x, position.y, EAST) && distances[position.x + 1][position.y] < least_distance) {
            least_distance = distances[position.x + 1][position.y];
            optimal_move = LEFT;
        }
    }
    else if (heading == WEST) {
        if (!isWallInDirection(position.x, position.y, WEST) && distances[position.x - 1][position.y] < least_distance) {
            least_distance = distances[position.x - 1][position.y];
            optimal_move = FORWARD;
        }
        if (!isWallInDirection(position.x, position.y, NORTH) && distances[position.x][position.y + 1] < least_distance) {
            least_distance = distances[position.x][position.y + 1];
            optimal_move = RIGHT;
        }
        if (!isWallInDirection(position.x, position.y, SOUTH) && distances[position.x][position.y - 1] < least_distance) {
            least_distance = distances[position.x][position.y - 1];
            optimal_move = LEFT;
        }
    }


    if (least_distance == 300)
        optimal_move = RIGHT;

    return optimal_move;
}


Action leftWallFollower() {
    if(wall_RF_Check || wall_LF_Check) {
        if(wall_L_Check){
            return RIGHT;
        }
        return LEFT;
    }
    return FORWARD;
}
