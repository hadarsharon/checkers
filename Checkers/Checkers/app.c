#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#define BOARD_SIZE 8
#define Tmask 1
#define Bmask 2
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

char colToChar(int row) { // This function receives an int and returns its col's char equivalent
	return row + '0' + 1;
}

char rowToChar(int row) { // This function receives an int and returns its row's char equivalent
	return row + 'A';
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
	else if (direction == RIGHT) {
		if (col == 7)
			return FALSE;
		else
			return TRUE;
	}
	else if (direction == LEFT) {
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
	if (direction == RIGHT) {
		if (piece == 'T') {
			nextMove.row = curPos.row + 1;
			nextMove.col = curPos.col + 1;
		}
		else if (piece == 'B') {
			nextMove.row = curPos.row - 1;
			nextMove.col = curPos.col + 1;
		}
	}
	else if (direction == LEFT) {
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
	*nextLeftPos = findNextMove(tempNode, &leftCapture, piece, LEFT);
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
	*nextRightPos = findNextMove(tempNode, &rightCapture, piece, RIGHT);
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

void makeEmptyList(SingleSourceMovesList* lst) {
	lst->head = lst->tail = NULL;
}

int isListEmptySingle(const SingleSourceMovesList* lst) {
	return lst->head == NULL;
}

int isListEmptyMultiple(const MultipleSourceMovesList* lst) {
	return lst->head == NULL;
}

SingleSourceMovesListCell* createNewListCellSingle(SingleSourceMovesTreeNode* node) {
	SingleSourceMovesListCell* newListCell = (SingleSourceMovesListCell*)calloc(1, sizeof(SingleSourceMovesListCell));
	checkMemoryAllocation(newListCell);
	newListCell->next = NULL;
	newListCell->captures = node->total_captures_so_far;
	newListCell->position = node->pos;
	return newListCell;
}

MultipleSourceMovesListCell* createNewListCellMultiple(SingleSourceMovesList* lst) {
	MultipleSourceMovesListCell* newListCell = (MultipleSourceMovesListCell*)calloc(1, sizeof(MultipleSourceMovesListCell));
	checkMemoryAllocation(newListCell);
	newListCell->next = NULL;
	newListCell->single_source_moves_list = lst;
	return newListCell;
}

void insertNodeToTailSingleSource(SingleSourceMovesList* lst, SingleSourceMovesListCell* newNode) {
	if (isListEmptySingle(lst))
		lst->head = lst->tail = newNode;
	else {
		lst->tail->next = newNode;
		lst->tail = newNode;
	}
}
void insertNodeToTailMultipleSource(MultipleSourceMovesList* lst, MultipleSourceMovesListCell* newNode) {
	if (isListEmptyMultiple(lst))
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

unsigned short countTotalCaptures(SingleSourceMovesTreeNode* treeNode) {
	SingleSourceMovesTreeNode* leftMove = treeNode->next_move[0];
	SingleSourceMovesTreeNode* rightMove = treeNode->next_move[1];
	if (leftMove == NULL && rightMove == NULL) {
		return 0;
	}
	else if (leftMove == NULL && rightMove != NULL) {
		if (rightMove->total_captures_so_far != 0)
			return 1 + countTotalCaptures(rightMove);
		else
			return 0;
	}
	else if (leftMove != NULL && rightMove == NULL) {
		if (leftMove->total_captures_so_far != 0)
			return 1 + countTotalCaptures(leftMove);
		else
			return 0;
	}
	else { // Both moves aren't NULL
		if (leftMove->total_captures_so_far == rightMove->total_captures_so_far == 0)
			return 0;
		else if (leftMove->total_captures_so_far > rightMove->total_captures_so_far)
			return 1 + countTotalCaptures(leftMove);
		else if (leftMove->total_captures_so_far < rightMove->total_captures_so_far)
			return 1 + countTotalCaptures(rightMove);
		else {
			// Keep running on both recursively until one reaches NULL, then choose the one with more captures
			int rightCaptures = 1 + countTotalCaptures(rightMove);
			int leftCaptures = 1 + countTotalCaptures(leftMove);
			if (leftCaptures >= rightCaptures) // Includes the possibility of equality, then randomly chooses left
				return leftCaptures;
			else
				return rightCaptures;
		}
	}
}

void FindSingleSourceOptimalMoveRec(SingleSourceMovesTreeNode* treeNode, SingleSourceMovesList* optimalMovesList) {
	// TODO: Maybe we need to add a conditional that ensures captures != 0
	insertNodeToTailSingleSource(optimalMovesList, createNewListCellSingle(treeNode));
	SingleSourceMovesTreeNode* leftMove = treeNode->next_move[0];
	SingleSourceMovesTreeNode* rightMove = treeNode->next_move[1];
	if (leftMove == NULL && rightMove == NULL)
		return;
	else if (leftMove == NULL && rightMove != NULL)
		FindSingleSourceOptimalMoveRec(rightMove, optimalMovesList);
	else if (leftMove != NULL && rightMove == NULL)
		FindSingleSourceOptimalMoveRec(leftMove, optimalMovesList);
	else {
		// Check which route has more captures
		unsigned short leftCaptures = countTotalCaptures(leftMove);
		unsigned short rightCaptures = countTotalCaptures(rightMove);
		// Populate the list with the nodes of the route that has more captures
		if (leftCaptures >= rightCaptures)
			FindSingleSourceOptimalMoveRec(leftMove, optimalMovesList);
		else
			FindSingleSourceOptimalMoveRec(rightMove, optimalMovesList);
	}
}
//Q2:
SingleSourceMovesList *FindSingleSourceOptimalMove(SingleSourceMovesTree *moves_tree) {
	SingleSourceMovesList* optimalMoves = (SingleSourceMovesList*)calloc(1, sizeof(SingleSourceMovesList));
	checkMemoryAllocation(optimalMoves);
	makeEmptyList(optimalMoves);
	SingleSourceMovesTreeNode* root = moves_tree->source;
	FindSingleSourceOptimalMoveRec(root, optimalMoves);
	return optimalMoves;
}

checkersPos** findAllPlayerGamePieces(Board board, Player player, int* num_of_pieces) {
	int i, j;
	int n = 0; // Number of player's game pieces found so far
	checkersPos** gamePieces = (checkersPos**)calloc(GAME_PIECES_PER_PLAYER, sizeof(checkersPos*)); // Allocate maximum number of pieces and later reallocate
	checkMemoryAllocation(gamePieces);
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == player) {
				gamePieces[n] = (checkersPos*)calloc(1, sizeof(checkersPos));
				checkMemoryAllocation(gamePieces[n]);
				gamePieces[n]->row = rowToChar(i);
				gamePieces[n]->col = colToChar(j);
				n++;
			}
		}
	}
	gamePieces = (checkersPos**)realloc(gamePieces, n * sizeof(checkersPos*)); // Reallocate according to number of actual pieces found
	checkMemoryAllocation(gamePieces);
	*num_of_pieces = n;
	return gamePieces;
}

//Q3:
MultipleSourceMovesList *FindAllPossiblePlayerMoves(Board board, Player player) {
	MultipleSourceMovesList* allPlayerPiecesPossibleMovesList = (MultipleSourceMovesList*)calloc(1, sizeof(MultipleSourceMovesList));
	checkMemoryAllocation(allPlayerPiecesPossibleMovesList);
	int num_of_pieces = 0; // Number of player's game pieces found
	checkersPos** playerGamePieces = findAllPlayerGamePieces(board, player, &num_of_pieces); // First find all player's game pieces
	int i;
	SingleSourceMovesTree* gamePieceMoves; 
	SingleSourceMovesList* gamePieceOptimalMoves; 
	for (i = 0; i < num_of_pieces; i++) {
		gamePieceMoves = FindSingleSourceMoves(board, playerGamePieces[i]); // Get all possible moves per piece
		gamePieceOptimalMoves = FindSingleSourceOptimalMove(gamePieceMoves); // Get the optimal route per piece from aforementioned moves
		insertNodeToTailMultipleSource(allPlayerPiecesPossibleMovesList, createNewListCellMultiple(gamePieceOptimalMoves));
	}
	return allPlayerPiecesPossibleMovesList;
}


char determineMoveDirection(checkersPos* origPos, checkersPos* movePos) {
	if (origPos->col < movePos->col) // e.g. 1 -> 2
		return RIGHT;
	else if (origPos->col > movePos->col) // e.g. 2 -> 1
		return LEFT;
	else // no move, no direction
		return NO_MOVE;
}

void removePiece(Board board, int row, int col) {
	board[row][col] = ' ';
}

void performMoveset(Board board, Player player, SingleSourceMovesList* moveset) {
	checkersPos* originalPos = moveset->head->position;
	SingleSourceMovesListCell* move = moveset->head->next;
	char direction;
	// Destination col & row (where to move)
	int moveCol;
	int moveRow;
	// Remove col & row (in case of capture)
	int removeCol;
	int removeRow;
	while (move != NULL) {
		moveCol = colToInt(move->position->col);
		moveRow = rowToInt(move->position->row);
		if (move->captures != 0) { // There was a capture - remove opponent game piece before making the move
			direction = determineMoveDirection(originalPos, move->position);
			if (direction == 'l')
				removeCol = originalPos->col - 1;
			else if (direction == 'r')
				removeCol = originalPos->col + 1;
			if (player == 'B')
				removeRow = originalPos->row - 1;
			else if (player == 'T')
				removeRow = originalPos->row + 1;
			removePiece(board, removeRow, removeCol);
		}
		// Make the move
		board[moveRow][moveCol] = player;
		move = move->next;
	}
}

//Q4:
void Turn(Board board, Player player) {
	MultipleSourceMovesList* allPossiblePlayerMoves = FindAllPossiblePlayerMoves(board, player);
	MultipleSourceMovesListCell* tempGamePieceMoveset = allPossiblePlayerMoves->head; // Used to iterate over possible moves
	int highest_num_of_captures = 0; // Number of captures per move, used to choose the best move set
	SingleSourceMovesList* chosenGamePieceMoveset = NULL; // The best move set to make based on number of captures
	int tempMovesetPossibleCaptures;
	// Find the move set with the highest possible number of captures
	while (tempGamePieceMoveset != NULL) {
		if (tempGamePieceMoveset->single_source_moves_list != NULL) { // Game piece has a possible move
			tempMovesetPossibleCaptures = tempGamePieceMoveset->single_source_moves_list->tail->captures;
			if (tempMovesetPossibleCaptures > highest_num_of_captures) {
				chosenGamePieceMoveset = tempGamePieceMoveset->single_source_moves_list;
				highest_num_of_captures = tempMovesetPossibleCaptures;
			}
		}
		tempGamePieceMoveset = tempGamePieceMoveset->next; // Move on to next game piece
	}
	// If there are no moves possible whatsoever, do nothing
	if (highest_num_of_captures == 0 && allPossiblePlayerMoves->head == NULL) {
		return;
	}
	// If no moveset with possible captures was found, pick the first one as it doesn't matter
	else if (highest_num_of_captures == 0 && allPossiblePlayerMoves->head != NULL) {
		chosenGamePieceMoveset = allPossiblePlayerMoves->head->single_source_moves_list;
	}
	// Perform the moveset on the board, if there are possible captures the function will handle it
	performMoveset(board, player, chosenGamePieceMoveset);
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
	fclose(bin_file);
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
	SingleSourceMovesList* testListSingle;
	MultipleSourceMovesList* testListMultiple;
	testPos.row = 'A';
	testPos.col = '4';
	printf("Resetting...");
	resetBoard(testBoard);
	printf("Storing...");
	StoreBoard(testBoard, "testfile.bin");
	printf("Loading...");
	LoadBoard("testfile.bin", newTestBoard);
	/* Test Q1
	newTestBoard[5][6] = ' ';
	newTestBoard[3][6] = 'B';
	newTestBoard[6][3] = ' ';
	printBoard(newTestBoard);
	testTree = FindSingleSourceMoves(newTestBoard, &testPos);
	*/
	// Q2 & Q3 Test
	newTestBoard[0][1] = ' ';
	newTestBoard[0][5] = ' ';
	newTestBoard[0][7] = ' ';
	newTestBoard[1][0] = ' ';
	newTestBoard[1][2] = 'B';
	newTestBoard[1][4] = 'B';
	newTestBoard[1][6] = ' ';
	newTestBoard[2][1] = ' ';
	newTestBoard[2][3] = ' ';
	newTestBoard[2][5] = ' ';
	newTestBoard[2][7] = ' ';
	newTestBoard[3][0] = ' ';
	newTestBoard[3][2] = ' ';
	newTestBoard[3][4] = 'B';
	newTestBoard[3][6] = 'B';
	newTestBoard[4][1] = ' ';
	newTestBoard[4][3] = ' ';
	newTestBoard[4][5] = ' ';
	newTestBoard[4][7] = ' ';
	newTestBoard[5][0] = ' ';
	newTestBoard[5][2] = ' ';
	newTestBoard[5][4] = ' ';
	newTestBoard[5][6] = ' ';
	newTestBoard[6][1] = ' ';
	newTestBoard[6][3] = ' ';
	newTestBoard[6][5] = ' ';
	newTestBoard[6][7] = ' ';
	newTestBoard[7][0] = ' ';
	newTestBoard[7][2] = ' ';
	newTestBoard[7][4] = ' ';
	newTestBoard[7][6] = ' ';
	printBoard(newTestBoard);
	testTree = FindSingleSourceMoves(newTestBoard, &testPos);
	testListSingle = FindSingleSourceOptimalMove(testTree);
	testListMultiple = FindAllPossiblePlayerMoves(newTestBoard, 'B');
	Turn(newTestBoard, 'B');
	printf("Done!");
	return 0;
}