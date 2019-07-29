#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8
#define Tmask 00000001
#define Bmask 00000010

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

int colToInt(char col) { // This function receives a position and returns its col's int equivalent
	return col - '0' - 1;
}

int rowToInt(char row) { // This function receives a position and returns its row's int equivalent
	return row - 'A';
}

void checkMemoryAllocation(void* ptr) { // This function checks if the memory allocation succeeded
	if (!ptr) { // We print an error and then exit this program
		printf("Memory allocation failed");
		exit(0);
	}
}

BOOL isGameOver(int curRow, char piece, char* winner) {
	if (curRow == 0 && piece == 'B') { // B or T reached the end
		*winner = 'B';
		return TRUE;
	}
	else if (curRow == 7 && piece == 'T') {
		*winner = 'T';
		return TRUE;
	}
	else
		return FALSE;
}

BOOL isMovePossible(checkersPos* src, checkersPos* movePos, Board board) {
	int srcRow = rowToInt(src->row), srcCol = colToInt(src->col);
	int moveRow = rowToInt(movePos->row), moveCol = colToInt(movePos->col);
	char piece = board[srcRow][srcCol];
	// Move is not possible if a friendly game piece is already there
	if (board[moveRow][moveCol] == piece)
		return FALSE;
	else
		return TRUE;
}

BOOL checkCapture(checkersPos* movePos, char piece, Board board) {
	int moveRow = rowToInt(movePos->row), moveCol = colToInt(movePos->col);
	// If an opponent game piece is in the box the game piece is moving to, mark a capture
	if (board[moveRow][moveCol] != piece && board[moveRow][moveCol] != ' ')
		return TRUE;
	else
		return FALSE;
}

void checkFile(FILE* f) { // This function checks if the file was opened successfully
	if (!f) {
		printf("Couldn't open the file\n");
		exit(0);
	}
}

void copyBoard(Board src_board, Board dest_board) {
	int i;
	for (i = 0; i < BOARD_SIZE; i++) {
		strcpy(dest_board[i], src_board[i]);
	}
}

void findNextCells(SingleSourceMovesTreeNode* curNode, checkersPos* nextLeft, checkersPos* nextRight) {
	checkersPos* curPos = curNode->pos;
	int curRow = rowToInt(curPos->row);
	int curCol = colToInt(curPos->col);
	char curPiece = curNode->board[curRow][curCol];
	if ((curRow == 0 && curPiece == 'B') || (curRow == 7 && curPiece == 'T')) { // B or T reached the end
		nextLeft = NULL;
		nextRight = NULL;
	}
	else if (curCol == 7) {
		if (curPiece == 'B') {
			nextRight = NULL;
			nextLeft->row = curPos->row - 1;
			nextLeft->col = curPos->col - 1;
		}
		else if (curPiece == 'T') {
			nextRight = NULL;
			nextLeft->row = curPos->row + 1;
			nextLeft->col = curPos->col - 1;
		}
	}
	else if (curCol == 0) {
		if (curPiece == 'B') {
			nextLeft = NULL;
			nextRight->row = curPos->row - 1;
			nextRight->col = curPos->col + 1;
		}
		else if (curPiece == 'T') {
			nextLeft = NULL;
			nextRight->row = curPos->row + 1;
			nextRight->col = curPos->col + 1;
		}
	}
	else { // Row != 0,7 && Col != 0,7
		if (curPiece == 'B') {
			nextLeft->row = curPos->row - 1;
			nextLeft->col = curPos->col - 1;
			nextRight->row = curPos->row - 1;
			nextRight->col = curPos->col + 1;
		}
		else if (curPiece == 'T') {
			nextLeft->row = curPos->row + 1;
			nextLeft->col = curPos->col - 1;
			nextRight->row = curPos->row + 1;
			nextRight->col = curPos->col + 1;
		}
	}
}

SingleSourceMovesTreeNode* FindSingleSourceMovesRec(SingleSourceMovesTreeNode* tempNode, char piece) {
	checkersPos* nextLeftPos, * nextRightPos;
	findNextCells(tempNode, nextLeftPos, nextRightPos);
	tempNode->next_move[0] = NULL;
	tempNode->next_move[1] = NULL;
	if (isMovePossible(tempNode->pos, nextLeftPos, tempNode->board)) {
		SingleSourceMovesTreeNode* newLeft = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		copyBoard(tempNode->board, newLeft->board);
		newLeft->pos = nextLeftPos;
		newLeft->total_captures_so_far = tempNode->total_captures_so_far;
		if (checkCapture(nextRightPos, piece, tempNode->board) == TRUE)
			newLeft->total_captures_so_far++;
		tempNode->next_move[0] = FindSingleSourceMovesRec(newLeft, piece);
	}
	if (isMovePossible(tempNode->pos, nextRightPos, tempNode->board)) {
		SingleSourceMovesTreeNode* newRight = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		copyBoard(tempNode->board, newRight->board);
		newRight->pos = nextRightPos;
		newRight->total_captures_so_far = tempNode->total_captures_so_far;
		if (checkCapture(nextRightPos, piece, tempNode->board) == TRUE)
			newRight->total_captures_so_far ++;
		tempNode->next_move[1] = FindSingleSourceMovesRec(newRight, piece);
	}
	return tempNode;
}

char findPiece(checkersPos* src, Board board) {
	int row = rowToInt(src->row), col = colToInt(src->col);
	return board[row][col];
}

//Q1:
SingleSourceMovesTree *FindSingleSourceMoves(Board board, checkersPos *src) {
	// Check if the box contains a game piece, else return NULL
	if (board[rowToInt(src->row)][colToInt(src->col)] == ' ') {
		return NULL;
	}
	else {
		char piece = findPiece(src, board);
		SingleSourceMovesTree* tree = (SingleSourceMovesTree*)calloc(1, sizeof(SingleSourceMovesTree));
		SingleSourceMovesTreeNode* root = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		copyBoard(board, root->board);
		root->pos = src;
		root->total_captures_so_far = 0;
		FindSingleSourceMovesRec(root, piece);
		tree->source = root;
		return tree;
	}
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