#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h> 
#include <windows.h>
using namespace std;

class Player {
    string name;
    bool isWhite;  // true for white, false for black

public:
    // Constructor
    Player(const string& playerName, bool white);

    // Destructor
    ~Player();
    // Getter functions
    string getName() const;
    bool getIsWhite() const;
};

// Base class for all chess pieces - demonstrates inheritance and encapsulation
class Piece {
protected:
    char symbol;           // Character representation
	bool isWhite;          // checks white piece or black piece
    int row;               // Current row position (0-7)
    int col;               // Current column position (0-7)
    bool hasMoved;         // Track if piece has moved (for en passant, castling)

public:
    // Constructor
    Piece(char sym, bool white, int r, int c);

    // Virtual destructor
    virtual ~Piece();

    // Getter functions
    char getSymbol() const;
    bool getIsWhite() const;
    int getRow() const;
    int getCol() const;
    bool getHasMoved() const;

    // Setter functions
    void setPosition(int r, int c);
    void setHasMoved(bool moved);
   
    // Pure virtual function
    virtual bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) = 0;

	//checks boundary of board
    bool isWithinBoard(int r, int c) const;

	//checks if target square is empty or occupied by enemy piece
    bool isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const;

	//checks for clear path between current position and target position 
    bool isPathClear(int targetRow, int targetCol, Piece*** board) const;
};

// Pawn class
class Pawn : public Piece {
public:
    // Constructor
    Pawn(bool white, int row, int col);
    // pawn move
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

// Rook class 
class Rook : public Piece {
public:
    // Constructor
    Rook(bool white, int row, int col);
    // rook move
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

// Knight class
class Knight : public Piece {
public:
    // Constructor
    Knight(bool white, int row, int col);
    // knight move
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

// Bishop class
class Bishop : public Piece {
public:
    // Constructor
    Bishop(bool white, int row, int col);
    // bishop move 
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

// Queen class
class Queen : public Piece {
public:
    Queen(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};

// King class
class King : public Piece {
public:
    King(bool white, int row, int col);
    bool isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard = nullptr) override;
};


// Board class
class Board {
private:
    
    Piece*** board;
    Piece*** previousBoard;  // Store previous board state

    // Tracks players turn
    bool whitesTurn;

    // initialize board with starting positions
    void initializeBoard();

    // place a piece at a specific position
    void placePiece(Piece* piece, int row, int col);

    // clear a square on the board
    void clearSquare(int row, int col);

    // Private helper to copy board state
    void copyBoardState(Piece*** from, Piece*** to);

public:
    // Constructor
    Board();

    // Destructor 
    ~Board();

    // Display board
    void display(int selectedRow = -1, int selectedCol = -1) const;

    // Move a piece
    bool movePiece(int sourceRow, int sourceCol, int targetRow, int targetCol);

    // Check if a specific square contains a piece
    bool hasPiece(int row, int col) const;

    // Get the piece at a specific square
    Piece* getPiece(int row, int col) const;

    // Check if a king is currently in check
    bool isInCheck(bool isWhiteKing) const;

    // Check if a king is in checkmate
    bool isCheckmate(bool isWhiteKing) const;

    // Get the current turn
    bool getWhitesTurn() const;

    // Switch turn to the other player
    void switchTurn();

    // Find the king piece for a given color
    King* findKing(bool isWhite) const;

    // Check if opponent can attack a given square
    bool canOpponentAttack(int row, int col, bool isWhiteAttacking) const;
};


// Game class 
class Game {
private:
    Board* board;           // The game board
    bool gameActive;        // Flag to track if game is still running
    bool gameWon;           // Flag to track if someone won
    bool isWhiteWinner;     // Flag to track which player won

    int cursorRow;          // Current cursor position (row)
    int cursorCol;          // Current cursor position (column)
    int selectedRow;        // Selected piece row (-1 if none)
    int selectedCol;        // Selected piece column (-1 if none)


    // get input from user
    int getKeyInput();

    // piece selection and movement
    bool processMove(int key);

    // single game turn
    bool playTurn();

    //  display game status and instructions
    void displayGameStatus() const;

    // Clear screen function
    void clearScreen() const;

public:
    // Constructor 
    Game();

    // Destructor 
    ~Game();

    // Main game loop 
    void run();

    // Check if game is still active
    bool isGameActive() const;

};

