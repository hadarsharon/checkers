#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define Tmask 00000001
#define Bmask 00000010

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
	unsigned char curChar;
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		curChar = 0;
		if (i % 2 == 0) {
			for (j = 1; j < BOARD_SIZE; j += 2) {
				curChar << 4;
				if (board[i][j] == 'T')
					curChar |= Tmask;
				else if (board[i][j] == 'B')
					curChar |= Bmask;
				if (j == 3 || j == 7) {
					//TODO: write curChar to file
					curChar = 0;
				}
			}
		}
		else {
			for (j = 0; j < BOARD_SIZE; j += 2) {
				curChar << 4;
				if (board[i][j] == 'T')
					curChar |= Tmask;
				else if (board[i][j] == 'B')
					curChar |= Bmask;
				if (j == 2 || j == 6) {
					//TODO: write curChar to file
					curChar = 0;
				}
			}
		}
	}
}

//Q6:
voidLoadBoard(char *filename, Board board) {

}

//Q7:
void PlayGame(Board board, Player starting_player) {

}

void resetBoard(Board board) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (i == 3 || i == 4) //these lines are empty
				board[i][j] = ' ';
			else if (i % 2 == 0) {
				if (j % 2 == 0)
					board[i][j] = ' ';
				else if (i == 0 || i == 2)
					board[i][j] = 'T';
				else
					board[i][j] = 'B';
			}
			else {
				if (j % 2 != 0)
					board[i][j] = ' ';
				else if (i == 1)
					board[i][j] = 'T';
				else
					board[i][j] = 'B';
			}
		}
	
	}
}