#ifndef SOLVER_H
#define SOLVER_H

/*
 * =============================================================
 *   SOLVER.H - Dinh nghia cho A* Micromouse Solver
 *   Dung voi MMS (Micromouse Simulator)
 * =============================================================
 */

#define MAZE_SIZE 16

/* Huong di cua chuot */
typedef enum Heading { NORTH, EAST, SOUTH, WEST } Heading;

/* Hanh dong tra ve cho main loop */
typedef enum Action { LEFT, FORWARD, RIGHT, IDLE } Action;

/* Cac ham solver */
Action solver();
Action leftWallFollower();
Action floodFill();
Action aStarSolver();

#endif