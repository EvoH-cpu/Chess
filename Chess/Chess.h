#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <conio.h> 
#include <windows.h>
using namespace std;

// Console utility 
void gotoxy(int x, int y);
void setColor(int color);
void setColor(int fg, int bg);
void resetColor();
void hideConsoleCursor();

// Arrow-key scan codes returned by _getch() after the 0xE0 / 0x00 prefix
#define KEY_UP    72
#define KEY_DOWN  80
#define KEY_LEFT  75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_ESC   27

// Player class 
class Player {
    string name;
    bool isWhite;

public:
    //constructor
    Player(const string& playerName, bool white);
    //destructor
    ~Player();
    //getters
    string getName() const;
    bool getIsWhite() const;
    //setters
    void setName(const string& newName);
    void setIsWhite(bool white);
};

// Base class Piece
class Piece {
protected:
    char symbol;
    bool isWhite;
    int row;
    int col;
    bool hasMoved;

public:
    //constructor
    Piece(char sym, bool white, int r, int c);
    //destructor
    virtual ~Piece();

    //getters
    char getSymbol() const;
    bool getIsWhite() const;
    int getRow() const;
    int getCol() const;
    bool getHasMoved() const;

    //setters
    void setPosition(int r, int c);
    void setHasMoved(bool moved);

    //check for valid move 
    virtual bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) = 0;

    //checking validity functions 
    bool isWithinBoard(int r, int c) const;
    bool isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const;
    bool isPathClear(int targetRow, int targetCol, Piece*** board) const;
};

// Derived pawn class
class Pawn : public Piece {
public:
    Pawn(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Derived Rook class
class Rook : public Piece {
public:
    Rook(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Derived Knight class
class Knight : public Piece {
public:
    Knight(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Derived Bishop class
class Bishop : public Piece {
public:
    Bishop(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Derived Queen class
class Queen : public Piece {
public:
    Queen(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Derived King class
class King : public Piece {
public:
    King(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

//Base Class Board
class Board {
private:
    Piece*** board;
    Piece*** previousBoard;
    bool whitesTurn;

    void initializeBoard();
    void placePiece(Piece* piece, int row, int col);
    void clearSquare(int row, int col);
    void copyBoardState(Piece*** from, Piece*** to);

    bool KingInCheck(int srcRow, int srcCol, int tgtRow, int tgtCol);
    bool LegalMove(bool isWhite);

public:
    Board();
    ~Board();

    // Display the board
    void display(int cursorRow, int cursorCol,
        int selectedRow = -1, int selectedCol = -1,
        const vector<pair<int, int>>* hints = nullptr,
        int boardOriginX = 4, int boardOriginY = 3) const;

    // Move piece from source to target location
    bool movePiece(int sourceRow, int sourceCol, int targetRow, int targetCol);
    bool hasPiece(int row, int col) const;
    // Get piece at specific location
    Piece* getPiece(int row, int col) const;
    // Check if King is in check
    bool isInCheck(bool isWhiteKing) const;
    // Check if King is in checkmate
    bool isCheckmate(bool isWhiteKing);

    bool getWhitesTurn() const;
    void switchTurn();
    // Find King of given color
    King* findKing(bool isWhite) const;
    bool canOpponentAttack(int row, int col, bool isWhiteAttacking) const;

    vector<pair<int, int>> getLegalMoves(int row, int col);
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

    vector<pair<int, int>> legalMoves;

    // Console origin where the board starts
    static const int BOARD_ORIGIN_X = 4;
    static const int BOARD_ORIGIN_Y = 3;

    int  getKeyInput(); // Get user input for movement using arrow keys 
    bool processMove(int key); // Process the move based on user input
    bool playTurn();
    void displayGameStatus() const;
    void clearScreen() const; //clears the screan
    void drawCursor() const;

public:
    Game(); //constructor
    ~Game(); //destructor

    void run();
    bool isGameActive() const;
};