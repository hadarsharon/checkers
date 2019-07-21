#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 8

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
	unsigned short capturesr;
	struct _SingleSourceMovesListCell *next;
} SingleSourceMovesListCell;

typedef struct _SingleSourceMovesList {
	SingleSourceMovesListCell *head;
	SingleSourceMovesListCell *tail;
} SingleSourceMovesList;

typedef struct _multipeSourceMovesListCell {
	SingleSourceMovesList *single_source_mpves_list;
	struct _multipeSourceMovesListCell *next;
} MultipleSourceMovesList;

//Q1:
SingleSourceMovesTree *FindSingleSourceMoves(Board board, checkersPos *src) {

}

//Q2:
SingleSourceMovesList *FindSingleSourceOptimalMove(SingleSourceMovesTree *moves_tree) {

}

//Q3:
MultipleSourceMovesList *FindAllPossiblePlayerMoves(Board board, Player player) {

}

//Q4:
void Turn(Board board, Player player) {

}

//Q5:
void StoreBoard(Board board, char *filename) {

}

//Q6:
voidLoadBoard(char *filename, Board board) {

}

//Q7:
void PlayGame(Board board, Player starting_player) {

}