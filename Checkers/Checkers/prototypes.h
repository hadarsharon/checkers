#ifndef __PROTOTYPES_H
#define __PROTOTYPES_H

#include "structs.h"

void checkMemoryAllocation(void* ptr);
void checkFile(FILE* f);
int colToInt(char col);
int rowToInt(char row);
char colToChar(int row);
char rowToChar(int row);
void freeTreeNode(SingleSourceMovesTreeNode* treeNode);
BOOL isMovePossible(int row, int col, char piece, char direction);
BOOL isEnemy(checkersPos movePos, char piece, Board board);
BOOL hasNoMove(checkersPos* pos);
checkersPos findNextCell(checkersPos curPos, char piece, char direction);
checkersPos findCaptureCell(checkersPos capturePos, char piece, char direction);
BOOL isEmpty(checkersPos movePos, char piece, Board board);
void copyBoard(Board src_board, Board dest_board);
checkersPos findNextMove(SingleSourceMovesTreeNode* curNode, BOOL* canCapture, char piece, char direction);
SingleSourceMovesTreeNode* FindSingleSourceMovesRec(SingleSourceMovesTreeNode* tempNode, char piece);
char findPiece(checkersPos* src, Board board);
void makeEmptyList(SingleSourceMovesList* lst);
int isListEmptySingle(const SingleSourceMovesList* lst);
int isListEmptyMultiple(const MultipleSourceMovesList* lst);
SingleSourceMovesListCell* createNewListCellSingle(SingleSourceMovesTreeNode* node);
MultipleSourceMovesListCell* createNewListCellMultiple(SingleSourceMovesList* lst);
void insertNodeToTailSingleSource(SingleSourceMovesList* lst, SingleSourceMovesListCell* newNode);
void insertNodeToTailMultipleSource(MultipleSourceMovesList* lst, MultipleSourceMovesListCell* newNode);
SingleSourceMovesTree *FindSingleSourceMoves(Board board, checkersPos *src);
unsigned short countTotalCaptures(SingleSourceMovesTreeNode* treeNode);
void FindSingleSourceOptimalMoveRec(SingleSourceMovesTreeNode* treeNode, SingleSourceMovesList* optimalMovesList);
SingleSourceMovesList *FindSingleSourceOptimalMove(SingleSourceMovesTree *moves_tree);
checkersPos** findAllPlayerGamePieces(Board board, Player player, int* num_of_pieces);
MultipleSourceMovesList *FindAllPossiblePlayerMoves(Board board, Player player);
char determineMoveDirection(checkersPos* origPos, checkersPos* movePos);
void removePiece(Board board, int row, int col);
void printGame(Player player, checkersPos originalPos, checkersPos nextPos);
void printTurn(Player player);
void performMoveset(Board board, Player player, SingleSourceMovesList* moveset);
void Turn(Board board, Player player);
void StoreBoard(Board board, char *filename);
void LoadBoard(char *filename, Board board);
BOOL hasWinningPiece(Board board, Player player);
BOOL noMoreGamePieces(Board board, Player player);
BOOL isGameOver(Board board, char* winner);
void printBoard(Board board);
void PlayGame(Board board, Player starting_player);
void resetBoard(Board board);

#endif __PROTOTYPES_H