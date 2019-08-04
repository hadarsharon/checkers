#ifndef __STRUCTS_H
#define __STRUCTS_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define Tmask 1
#define Bmask 2
#define EMPTY_BOX ' '
#define NO_MOVE '\0'
#define LEFT 'l'
#define RIGHT 'r'
#define GAME_PIECES_PER_PLAYER 12

typedef int BOOL;
#define TRUE 1
#define FALSE 0

typedef struct _checkersPos {
	char row, col;
} checkersPos;

typedef unsigned char Board[BOARD_SIZE][BOARD_SIZE];

typedef unsigned char Player;

typedef struct _SingleSourceMovesTreeNode {
	Board board;
	checkersPos *pos;
	unsigned short total_captures_so_far;
	struct _SingleSourceMovesTreeNode *next_move[2];
} SingleSourceMovesTreeNode;

typedef struct _SingleSourceMovesTree {
	SingleSourceMovesTreeNode *source;
} SingleSourceMovesTree;

typedef struct _SingleSourceMovesListCell {
	checkersPos *position;
	unsigned short captures;
	struct _SingleSourceMovesListCell *next;
} SingleSourceMovesListCell;

typedef struct _SingleSourceMovesList {
	SingleSourceMovesListCell *head;
	SingleSourceMovesListCell *tail;
} SingleSourceMovesList;

typedef struct _multipleSourceMovesListCell {
	SingleSourceMovesList *single_source_moves_list;
	struct _multipleSourceMovesListCell *next;
} MultipleSourceMovesListCell;

typedef struct _MultipleSourceMovesList {
	MultipleSourceMovesListCell* head;
	MultipleSourceMovesListCell* tail;
} MultipleSourceMovesList;


#endif __STRUCT_H