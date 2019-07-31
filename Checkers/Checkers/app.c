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


void checkMemoryAllocation(void* ptr) { // This function checks if the memory allocation succeeded
	if (!ptr) { // We print an error and then exit this program
		printf("Memory allocation failed");
		exit(0);
	}
}

void checkFile(FILE* f) { // This function checks if the file was opened successfully
	if (!f) {
		printf("Couldn't open the file\n");
		exit(0);
	}
}

int colToInt(char col) { // This function receives a position and returns its col's int equivalent
	return col - '0' - 1;
}

int rowToInt(char row) { // This function receives a position and returns its row's int equivalent
	return row - 'A';
}

void freeTreeNode(SingleSourceMovesTreeNode* treeNode) {
	free(treeNode->pos);
	free(treeNode->next_move[0]);
	free(treeNode->next_move[1]);
	free(treeNode);
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

BOOL isMovePossible(int row, int col, char piece, char direction) {
	if ((row == 0 && piece == 'B') || (row == 7 && piece == 'T')) { // B or T reached the end
		return FALSE;
	}
	else if (direction == 'r') {
		if (col == 7)
			return FALSE;
		else
			return TRUE;
	}
	else if (direction == 'l') {
		if (col == 0)
			return FALSE;
		else
			return TRUE;
	}
}

BOOL isEnemy(checkersPos movePos, char piece, Board board) {
	int moveRow = rowToInt(movePos.row), moveCol = colToInt(movePos.col);
	// If an opponent game piece is in the box the game piece is moving to, mark a capture
	if (board[moveRow][moveCol] != piece && board[moveRow][moveCol] != ' ')
		return TRUE;
	else
		return FALSE;
}

BOOL hasNoMove(checkersPos* pos) {
	if (pos->col == NO_MOVE && pos->row == NO_MOVE)
		return TRUE;
	else
		return FALSE;
}

checkersPos findNextCell(checkersPos curPos, char piece, char direction) {
	checkersPos nextMove;
	if (direction == 'r') {
		if (piece == 'T') {
			nextMove.row = curPos.row + 1;
			nextMove.col = curPos.col + 1;
		}
		else if (piece == 'B') {
			nextMove.row = curPos.row - 1;
			nextMove.col = curPos.col + 1;
		}
	}
	else if (direction == 'l') {
		if (piece == 'T') {
			nextMove.row = curPos.row + 1;
			nextMove.col = curPos.col - 1;
		}
		else if (piece == 'B') {
			nextMove.row = curPos.row - 1;
			nextMove.col = curPos.col - 1;
		}
	}
	return nextMove;
}

checkersPos findCaptureCell(checkersPos capturePos, char piece, char direction) {
	int curRow = rowToInt(capturePos.row);
	int curCol = colToInt(capturePos.col);
	checkersPos newMove;
	newMove.row = NO_MOVE;
	newMove.col = NO_MOVE;
	if (isMovePossible(curRow, curCol, piece, direction)) {
		newMove = findNextCell(capturePos, piece, direction);
	}
	return newMove;
}

BOOL isEmpty(checkersPos movePos, char piece, Board board) {
	int moveRow = rowToInt(movePos.row), moveCol = colToInt(movePos.col);
	if (board[moveRow][moveCol] == ' ') {
		return TRUE;
	}
	else
		return FALSE;
}

void copyBoard(Board src_board, Board dest_board) {
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			dest_board[i][j] = src_board[i][j];
		}
	}
}

checkersPos findNextMove(SingleSourceMovesTreeNode* curNode, BOOL* canCapture, char piece, char direction) {
	checkersPos* curPos = curNode->pos;
	checkersPos newMove;
	int curRow = rowToInt(curPos->row);
	int curCol = colToInt(curPos->col);
	if (isMovePossible(curRow, curCol, piece, direction)) { //handle move
		newMove = findNextCell(*curPos, piece, direction);
		if (isEnemy(newMove, piece, curNode->board)) {
			newMove = findCaptureCell(newMove, piece, direction);
			if ((newMove.row != NO_MOVE) && (newMove.col != NO_MOVE))
				*canCapture = TRUE;
		}
		if (isEmpty(newMove, piece, curNode->board) == FALSE) {
			newMove.row = NO_MOVE;
			newMove.col = NO_MOVE;
			*canCapture = FALSE;
		}
	}
	else {
		newMove.row = NO_MOVE;
		newMove.col = NO_MOVE;
		*canCapture = FALSE;
	}
	return newMove;
}

SingleSourceMovesTreeNode* FindSingleSourceMovesRec(SingleSourceMovesTreeNode* tempNode, char piece) {
	// Create left and right children
	SingleSourceMovesTreeNode* newLeft = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
	checkMemoryAllocation(newLeft);
	SingleSourceMovesTreeNode* newRight = (SingleSourceMovesTreeNode*)calloc(1, sizeof(SingleSourceMovesTreeNode));
	checkMemoryAllocation(newRight);
	// Set these children to be the next moves of original node
	tempNode->next_move[0] = newLeft;
	tempNode->next_move[1] = newRight;
	// Set children captures to their father's
	newLeft->total_captures_so_far = tempNode->total_captures_so_far;
	newRight->total_captures_so_far = tempNode->total_captures_so_far;
	// Update children boards using original node's board
	copyBoard(tempNode->board, newLeft->board);
	copyBoard(tempNode->board, newRight->board);
	// Find left move position
	checkersPos* nextLeftPos = (checkersPos*)calloc(1, sizeof(checkersPos));
	checkMemoryAllocation(nextLeftPos);
	BOOL leftCapture = FALSE;
	*nextLeftPos = findNextMove(tempNode, &leftCapture, piece, 'l');
	newLeft->pos = nextLeftPos;
	// Check if a capture is possible and update total_captures_so_far accordingly
	if (leftCapture == TRUE) {
		newLeft->total_captures_so_far++;
		newLeft = FindSingleSourceMovesRec(newLeft, piece);
	}
	// If no capture, check if move is possible - if not, set move to NULL
	// If move is possible to an empty box but capture was already made - set to NULL
	else if ( (hasNoMove(newLeft->pos)) || (tempNode->total_captures_so_far != 0) ) {
		freeTreeNode(newLeft);
		tempNode->next_move[0] = NULL;
	}
	// Find right move position
	checkersPos* nextRightPos = (checkersPos*)calloc(1, sizeof(checkersPos));
	checkMemoryAllocation(nextRightPos);
	BOOL rightCapture = FALSE;
	*nextRightPos = findNextMove(tempNode, &rightCapture, piece, 'r');
	newRight->pos = nextRightPos;
	// Check if a capture is possible and update total_captures_so_far accordingly
	if (rightCapture == TRUE) {
		newRight->total_captures_so_far++;
		newRight = FindSingleSourceMovesRec(newRight, piece);
	}
	// If no capture, check if move is possible - if not, set move to NULL
	// If move is possible to an empty box but capture was already made - set to NULL
	else if ( (hasNoMove(newRight->pos)) ||	(tempNode->total_captures_so_far != 0) ) {
		freeTreeNode(newRight);
		tempNode->next_move[1] = NULL;
	}
	// Finally, return original node after his children have been updated
	return tempNode;
}

char findPiece(checkersPos* src, Board board) {
	int row = rowToInt(src->row), col = colToInt(src->col);
	return board[row][col];
}

SingleSourceMovesList makeEmptyList() {
	SingleSourceMovesList lst;
	lst.head = lst.tail = NULL;
	return lst;
}

int isListEmpty(const SingleSourceMovesList* lst) {
	return lst->head == NULL;
}

void insertNodeToTail(SingleSourceMovesList* lst, SingleSourceMovesTreeNode* newNode) {
	if (isListEmpty(lst))
		lst->head = lst->tail = newNode;
	else {
		lst->tail->next = newNode;
		lst->tail = newNode;
	}
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

void FindSingleSourceOptimalMoveRec(SingleSourceMovesTreeNode* treeNode, SingleSourceMovesList* optimalMovesList, unsigned short* leftCaptures, unsigned short* rightCaptures) {
	SingleSourceMovesTreeNode* leftMove = treeNode->next_move[0];
	SingleSourceMovesTreeNode* rightMove = treeNode->next_move[1];
	if (leftMove == NULL && rightMove == NULL) {
		return;
	}
	else if (leftMove == NULL && rightMove != NULL) {
		insertNodeToTail(optimalMovesList, rightMove);
		*rightCaptures = rightMove->total_captures_so_far;
		if (*rightCaptures != 0) {
			FindSingleSourceOptimalMoveRec(rightMove, optimalMovesList, leftCaptures, rightCaptures);
		}
	}
	else if (leftMove != NULL && rightMove == NULL) {
		insertNodeToTail(optimalMovesList, leftMove);
		*leftCaptures = leftMove->total_captures_so_far;
		if (*leftCaptures != 0) {
			FindSingleSourceOptimalMoveRec(leftMove, optimalMovesList, leftCaptures, rightCaptures);
		}
	}
	else { // Both moves aren't NULL
		*leftCaptures = leftMove->total_captures_so_far;
		*rightCaptures = rightMove->total_captures_so_far;
		if (*leftCaptures == 0 && *rightCaptures == 0) { // No captures
			insertNodeToTail(optimalMovesList, leftMove); // Randomly add left
		}
		else if (*leftCaptures > *rightCaptures) {
			insertNodeToTail(optimalMovesList, leftMove);
			FindSingleSourceOptimalMoveRec(leftMove, optimalMovesList, leftCaptures, rightCaptures);
		}
		else if (*leftCaptures < *rightCaptures) {
			insertNodeToTail(optimalMovesList, rightMove);
			FindSingleSourceOptimalMoveRec(rightMove, optimalMovesList, leftCaptures, rightCaptures);
		}
		else { // leftCaptures == rightCaptures != 0

		}
	}
}

//Q2:
SingleSourceMovesList *FindSingleSourceOptimalMove(SingleSourceMovesTree *moves_tree) {
	SingleSourceMovesList optimalMoves = makeEmptyList();
	SingleSourceMovesTreeNode* root = moves_tree->source;
	insertNodeToTail(&optimalMoves, root);
	unsigned short leftCaptures;
	unsigned short rightCaptures;
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
	testPos.row = 'C';
	testPos.col = '8';
	printf("Resetting...");
	resetBoard(testBoard);
	printf("Storing...");
	StoreBoard(testBoard, "testfile.bin");
	printf("Loading...");
	LoadBoard("testfile.bin", newTestBoard);
	newTestBoard[5][6] = ' ';
	newTestBoard[3][6] = 'B';
	newTestBoard[6][3] = ' ';
	printBoard(newTestBoard);
	testTree = FindSingleSourceMoves(newTestBoard, &testPos);
	printf("Done!");
	return 0;
}