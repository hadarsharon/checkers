#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define Tmask 1
#define Bmask 2
#define NO_MOVE '\0'

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

BOOL isMovePossible(checkersPos* src, checkersPos* movePos, Board board, char piece) {
	int srcRow = rowToInt(src->row), srcCol = colToInt(src->col);
	int moveRow = rowToInt(movePos->row), moveCol = colToInt(movePos->col);
	// Move is not possible if a friendly game piece is already there
	if (board[moveRow][moveCol] == piece || movePos->col == NO_MOVE)
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
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			dest_board[i][j] = src_board[i][j];
		}
	}
}

void findNextCells(SingleSourceMovesTreeNode* curNode, checkersPos* nextLeft, checkersPos* nextRight, char curPiece) {
	checkersPos* curPos = curNode->pos;
	int curRow = rowToInt(curPos->row);
	int curCol = colToInt(curPos->col);
	if ((curRow == 0 && curPiece == 'B') || (curRow == 7 && curPiece == 'T')) { // B or T reached the end
		nextLeft->row = NO_MOVE;
		nextLeft->col = NO_MOVE;
		nextRight->row = NO_MOVE;
		nextRight->col = NO_MOVE;
	}
	else if (curCol == 7) {
		if (curPiece == 'B') {
			nextRight->row = NO_MOVE;
			nextRight->col = NO_MOVE;
			nextLeft->row = curPos->row - 1;
			nextLeft->col = curPos->col - 1;
		}
		else if (curPiece == 'T') {
			nextRight->row = NO_MOVE;
			nextRight->col = NO_MOVE;
			nextLeft->row = curPos->row + 1;
			nextLeft->col = curPos->col - 1;
		}
	}
	else if (curCol == 0) {
		if (curPiece == 'B') {
			nextLeft->row = NO_MOVE;
			nextLeft->col = NO_MOVE;
			nextRight->row = curPos->row - 1;
			nextRight->col = curPos->col + 1;
		}
		else if (curPiece == 'T') {
			nextLeft->row = NO_MOVE;
			nextLeft->col = NO_MOVE;
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
	checkersPos* nextLeftPos = (checkersPos*)calloc(1, sizeof(checkersPos));
	checkersPos* nextRightPos = (checkersPos*)calloc(1, sizeof(checkersPos));
	findNextCells(tempNode, nextLeftPos, nextRightPos, piece);
	tempNode->next_move[0] = NULL;
	tempNode->next_move[1] = NULL;
	if (isMovePossible(tempNode->pos, nextLeftPos, tempNode->board, piece)) {
		SingleSourceMovesTreeNode* newLeft = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		copyBoard(tempNode->board, newLeft->board);
		newLeft->pos = nextLeftPos;
		newLeft->total_captures_so_far = tempNode->total_captures_so_far;
		if (checkCapture(nextRightPos, piece, tempNode->board) == TRUE) {
			newLeft->total_captures_so_far++;
			tempNode->next_move[0] = FindSingleSourceMovesRec(newLeft, piece);
		}
		else {
			tempNode->next_move[0] = newLeft;
		}
	}
	if (isMovePossible(tempNode->pos, nextRightPos, tempNode->board, piece)) {
		SingleSourceMovesTreeNode* newRight = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		copyBoard(tempNode->board, newRight->board);
		newRight->pos = nextRightPos;
		newRight->total_captures_so_far = tempNode->total_captures_so_far;
		if (checkCapture(nextRightPos, piece, tempNode->board) == TRUE) {
			newRight->total_captures_so_far++;
			tempNode->next_move[1] = FindSingleSourceMovesRec(newRight, piece);
		}
		else {
			tempNode->next_move[1] = newRight;
		}
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
		checkMemoryAllocation(tree);
		SingleSourceMovesTreeNode* root = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
		checkMemoryAllocation(root);
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
		for (j = 0; j < BOARD_SIZE; j ++) {
			if (board[i][j] == 'T')
				curChar |= Tmask;
			else if (board[i][j] == 'B')
				curChar |= Bmask;
			if (j == 3 || j == 7) {
				fwrite(&curChar, sizeof(unsigned char), 1, bin_file);
				curChar = 0;
			}
			curChar <<= 2;
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

void printBoard(Board board) {
	int i, j;
	char row;
	printf("\n");
	for (i = 0; i < BOARD_SIZE; i++) {
		row = i + 'A';
		for (j = 0; j < BOARD_SIZE; j++) {
			printf("|%c|", board[i][j]);
		}
		printf("\n");
	}
}

int main() {
	Board testBoard, newTestBoard;
	checkersPos testPos;
	SingleSourceMovesTree* testTree;
	testPos.row = 'F';
	testPos.col = '1';
	printf("Resetting...");
	resetBoard(testBoard);
	printf("Storing...");
	StoreBoard(testBoard, "testfile.bin");
	printf("Loading...");
	LoadBoard("testfile.bin", newTestBoard);
	printBoard(newTestBoard);
	testTree = FindSingleSourceMoves(newTestBoard, &testPos);
	printf("Done!");
	return 0;
}