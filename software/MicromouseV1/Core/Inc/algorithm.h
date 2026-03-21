/*
 * Algorithm.h
 *
 *  Created on: Nov 22, 2025
 *      Author: kinhz
 */

#ifndef INC_ALGORITHM_H_
#define INC_ALGORITHM_H_

#include "main.h"

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE, BACK} Action;


#define MAZE_SIZE 16

#define _0000 0
#define _0001 1
#define _0010 2
#define _0011 3
#define _0100 4
#define _0101 5
#define _0110 6
#define _0111 7
#define _1000 8
#define _1001 9
#define _1010 10
#define _1011 11
#define _1100 12
#define _1101 13
#define _1110 14
#define _1111 15


#define SEARCH 0
#define SPEED  1

extern int mode;

extern uint8_t known[MAZE_SIZE][MAZE_SIZE];
extern uint8_t maze[MAZE_SIZE][MAZE_SIZE];
extern int16_t distances[MAZE_SIZE][MAZE_SIZE];
extern int readyFlag;
extern int destination_X;
extern int destination_Y;
extern uint16_t pathLength;
extern uint16_t path[1000];

extern Heading heading;

struct Coordinate {
    int x;
    int y;
};
extern struct Coordinate position;

void initialize();
void updateMaze();
void updateDistances();
void resetDistances();
int xyToSquare(int x, int y);
struct Coordinate squareToCoord(int square);
int isWallInDirection(int x, int y, Heading direction);
void updateHeading(Action nextAction);
void updatePosition(Action nextAction);
Action solver();
Action leftWallFollower();
Action floodFill();
Action peekNextAction();
void buildPath();

#endif /* INC_ALGORITHM_H_ */
