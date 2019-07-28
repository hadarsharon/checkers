#define _CRT_SECURE_NO_WARNINGS
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

typedef struct _multipleSourceMovesListCell {
	SingleSourceMovesList *single_source_moves_list;
	struct _multipleSourceMovesListCell *next;
} MultipleSourceMovesList;

int rowToInt(checkersPos pos) { //This function receives a position and returns its row's int equivalent
	return pos.row - 'A';
}

void checkMemoryAllocation(void* ptr) { //This function checks if the memory allocation succeeded
	if (!ptr) { //We print an error and then exit this program
		printf("Memory allocation failed");
		exit(0);
	}
}

void checkFile(FILE* f) { //this function checks if the file was opened successfully
	if (!f) {
		printf("Couldn't open the file\n");
		exit(0);
	}
}

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
	FILE *bin_file;
	bin_file = fopen(filename, "wb");
	checkFile(bin_file);
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		curChar = 0;
		if (i % 2 == 0) {
			for (j = 1; j < BOARD_SIZE; j += 2) {
				curChar <<= 4;
				if (board[i][j] == 'T')
					curChar |= Tmask;
				else if (board[i][j] == 'B')
					curChar |= Bmask;
				if (j == 3 || j == 7) {
					fwrite(&curChar, sizeof(unsigned char), 1, bin_file);
					curChar = 0;
				}
			}
		}
		else {
			for (j = 0; j < BOARD_SIZE; j += 2) {
				curChar <<= 4;
				if (board[i][j] == 'T')
					curChar |= Tmask;
				else if (board[i][j] == 'B')
					curChar |= Bmask;
				if (j == 2 || j == 6) {
					fwrite(&curChar, sizeof(unsigned char), 1, bin_file);
					curChar = 0;
				}
			}
		}
	}
	fclose(bin_file);
}

//Q6:
void LoadBoard(char *filename, Board board) {
	FILE* bin_file;
	bin_file = fopen(filename, "rb");
	checkFile(bin_file);
	unsigned char firstByte, secondByte, curCell;
	int i, j;
	for (i = 0; i < BOARD_SIZE || feof(bin_file); i++) {
		fread(&firstByte, sizeof(unsigned char), 1, bin_file);
		fread(&secondByte, sizeof(unsigned char), 1, bin_file); //each line is represented by 2 bytes
		for (j = 0; j < BOARD_SIZE; j++) {
			if (j < BOARD_SIZE / 2) { //TODO: move to a function
				curCell = firstByte >> 6;
				if (curCell == 0)
					board[i][j] = ' ';
				else if (curCell == Tmask)
					board[i][j] = 'T';
				else
					board[i][j] = 'B';
				firstByte <<= 2;
			}
			else { //TODO: move to a function
				curCell = secondByte >> 6;
				if (curCell == 0)
					board[i][j] = ' ';
				else if (curCell == Tmask)
					board[i][j] = 'T';
				else
					board[i][j] = 'B';
				secondByte <<= 2;
			}
		}
	}
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

int main() {
	Board testBoard, newTestBoard;
	printf("Resetting...");
	resetBoard(testBoard);
	printf("Storing...");
	StoreBoard(testBoard, "testfile.bin");
	printf("Loading...");
	LoadBoard("testfile.bin", newTestBoard);
	printf("Done!");
	return 0;
}