#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h> 
#include <windows.h>
using namespace std;

// Console utility 
void gotoxy(int x, int y);
void setColor(int color);
void resetColor();

// Arrow-key scan codes returned by _getch() after the 0xE0 / 0x00 prefix
#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC   27

class Player {
    string name;
    bool isWhite;

public:
    Player(const string& playerName, bool white);
    ~Player();
    string getName() const;
    bool getIsWhite() const;
    void setName(const string& newName);
    void setIsWhite(bool white);
};

// Base class for all chess pieces
class Piece {
protected:
    char symbol;
    bool isWhite;
    int row;
    int col;
    bool hasMoved;

public:
    Piece(char sym, bool white, int r, int c);
    virtual ~Piece();

    char getSymbol() const;
    bool getIsWhite() const;
    int getRow() const;
    int getCol() const;
    bool getHasMoved() const;

    void setPosition(int r, int c);
    void setHasMoved(bool moved);

    virtual bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) = 0;

    bool isWithinBoard(int r, int c) const;
    bool isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const;
    bool isPathClear(int targetRow, int targetCol, Piece*** board) const;
};

class Pawn : public Piece {
public:
    Pawn(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

class Rook : public Piece {
public:
    Rook(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

class Knight : public Piece {
public:
    Knight(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

class Bishop : public Piece {
public:
    Bishop(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

class Queen : public Piece {
public:
    Queen(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

class King : public Piece {
public:
    King(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};


class Board {
private:
    Piece*** board;
    Piece*** previousBoard;
    bool whitesTurn;

    void initializeBoard();
    void placePiece(Piece* piece, int row, int col);
    void clearSquare(int row, int col);
    void copyBoardState(Piece*** from, Piece*** to);

public:
    Board();
    ~Board();

    // boardOriginX/Y: top-left console position where the board is drawn
    void display(int cursorRow, int cursorCol,
                 int selectedRow = -1, int selectedCol = -1,
                 int boardOriginX = 4, int boardOriginY = 3) const;

    bool movePiece(int sourceRow, int sourceCol, int targetRow, int targetCol);
    bool hasPiece(int row, int col) const;
    Piece* getPiece(int row, int col) const;
    bool isInCheck(bool isWhiteKing) const;
    bool isCheckmate(bool isWhiteKing) const;
    bool getWhitesTurn() const;
    void switchTurn();
    King* findKing(bool isWhite) const;
    bool canOpponentAttack(int row, int col, bool isWhiteAttacking) const;
};


class Game {
private:
    Board* board;
    bool gameActive;
    bool gameWon;
    bool isWhiteWinner;

    int cursorRow;
    int cursorCol;
    int selectedRow;
    int selectedCol;

    // Console origin where the board starts
    static const int BOARD_ORIGIN_X = 4;
    static const int BOARD_ORIGIN_Y = 3;

    int  getKeyInput();
    bool processMove(int key);
    bool playTurn();
    void displayGameStatus() const;
    void clearScreen() const;
    void drawCursor() const;         

public:
    Game();
    ~Game();
    void run();
    bool isGameActive() const;
};
