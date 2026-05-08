#include"Chess.h"
#include <conio.h>
#include <windows.h>

namespace {
    constexpr int KEY_UP = 1001;
    constexpr int KEY_DOWN = 1002;
    constexpr int KEY_LEFT = 1003;
    constexpr int KEY_RIGHT = 1004;

    void getoxy(int x, int y) {
        COORD pos{ static_cast<SHORT>(x), static_cast<SHORT>(y) };
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    }
}

//player class implementation

//constructor
Player::Player(const string& playerName, bool white)
    : name(playerName), isWhite(white) {}

//destructor
Player::~Player() { cout << "Player " << name << " destroyed." << endl; }
//getters
string Player::getName() const { return name; }
bool Player::getIsWhite() const { return isWhite; }

//setters
void Player::setName(const string& newName) { name = newName; }
void Player::setIsWhite(bool white) { isWhite = white; }

//piece class base implementation

//constructor 
Piece::Piece(char sym, bool white, int r, int c)
    : symbol(sym), 
    isWhite(white),
    row(r), col(c),
    hasMoved(false) {}

//destructor
Piece::~Piece() {}

//getters
char Piece::getSymbol() const { return symbol; }
bool Piece::getIsWhite() const { return isWhite; }
int Piece::getRow() const { return row; }
int Piece::getCol() const { return col; }
bool Piece::getHasMoved() const { return hasMoved; }

//setters
void Piece::setPosition(int r, int c) {
    row = r;
    col = c;
    hasMoved = true;
}

void Piece::setHasMoved(bool moved) { hasMoved = moved; }

bool Piece::isWithinBoard(int r, int c) const {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

// Checks if the target square is empty or occupied by an enemy piece
bool Piece::isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const {
    if (!isWithinBoard(targetRow, targetCol)) return false;

    Piece* target = board[targetRow][targetCol];
    if (target == nullptr) return true;
    return target->isWhite != this->isWhite;
}

// Checks if the path between current position and target position is clear (for Rook, Bishop, Queen)
bool Piece::isPathClear(int targetRow, int targetCol, Piece*** board) const {
    int targetrow = 0, targetcol = 0;

    if (targetRow > row) targetrow = 1;
    else if (targetRow < row) targetrow = -1;

    if (targetCol > col) targetcol = 1;
    else if (targetCol < col) targetcol = -1;

    int currentRow = row + targetrow;
    int currentCol = col + targetcol;

    while (currentRow != targetRow || currentCol != targetCol) {
        if (board[currentRow][currentCol] != nullptr) {
            return false;
        }
        currentRow += targetrow;
        currentCol += targetcol;
    }

    return true;
}
//pawn implementation
Pawn::Pawn(bool white, int row, int col)
    : Piece(white ? 'P' : 'p', white, row, col) {}

bool Pawn::isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard) {
    
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = targetRow - row;
    int colDiff = abs(targetCol - col);
    int direction = isWhite ? -1 : 1;

    // Move forward 1 square (must be empty)
    if (rowDiff == direction && colDiff == 0) {
        return board[targetRow][targetCol] == nullptr;
    }

    // Move forward 2 squares from starting position
    int startingRow = isWhite ? 6 : 1;
    if (row == startingRow && rowDiff == 2 * direction && colDiff == 0) {
        int middleRow = row + direction;
        return board[middleRow][col] == nullptr && board[targetRow][targetCol] == nullptr;
    }

    // Capture diagonally
    if (rowDiff == direction && colDiff == 1) {
        if (board[targetRow][targetCol] != nullptr &&
            board[targetRow][targetCol]->getIsWhite() != this->isWhite) {
            return true;
        }

        // En passant capture
        if (lastBoard != nullptr && board[targetRow][targetCol] == nullptr) {
            // Check if there's an enemy pawn in the square we're capturing from
            Piece* capturePiece = lastBoard[row][targetCol];
            if (capturePiece != nullptr && capturePiece->getSymbol() == (isWhite ? 'p' : 'P')) {
                // The enemy pawn just moved 2 squares (must be in that square in lastBoard)
                return true;
            }
        }
    }

    return false;
}

//rook implementation
Rook::Rook(bool white, int row, int col)
    : Piece(white ? 'R' : 'r', white, row, col) {}

bool Rook::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    if (row != targetRow && col != targetCol) return false;

    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

//knight implementation
Knight::Knight(bool white, int row, int col)
    : Piece(white ? 'N' : 'n', white, row, col) {}

bool Knight::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = abs(targetRow - row);
    int colDiff = abs(targetCol - col);

    if ((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2)) {
        return isEmptyOrEnemy(targetRow, targetCol, board);
    }
    return false;
}

//bishop implementation
Bishop::Bishop(bool white, int row, int col)
    : Piece(white ? 'B' : 'b', white, row, col) {}

bool Bishop::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = abs(targetRow - row);
    int colDiff = abs(targetCol - col);

    if (rowDiff != colDiff) return false;

    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

//Queen implementation
Queen::Queen(bool white, int row, int col)
    : Piece(white ? 'Q' : 'q', white, row, col) {}

bool Queen::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = abs(targetRow - row);
    int colDiff = abs(targetCol - col);

    if (row != targetRow && col != targetCol && rowDiff != colDiff) {
        return false;
    }

    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

//king implementation
King::King(bool white, int row, int col)
    : Piece(white ? 'K' : 'k', white, row, col) {}

bool King::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = abs(targetRow - row);
    int colDiff = abs(targetCol - col);

    if (rowDiff > 1 || colDiff > 1) return false;

    return isEmptyOrEnemy(targetRow, targetCol, board);
}

//Board class implementation
Board::Board() : whitesTurn(true) {
	// Allocate memory for the board and previous board
    board = new Piece **[8];
    previousBoard = new Piece **[8];
    for (int i = 0; i < 8; i++) {

		// Allocate memory for each row
        board[i] = new Piece *[8];
        previousBoard[i] = new Piece *[8];

		//setting all squares to nullptr
        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
            previousBoard[i][j] = nullptr;
        }
    }
    initializeBoard();
}
// Destructor to clean up memory
Board::~Board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) {
				delete board[i][j]; // Delete the piece object
                board[i][j] = nullptr;
            }
            if (previousBoard[i][j] != nullptr) {
                delete previousBoard[i][j]; // Delete the piece object
                previousBoard[i][j] = nullptr;
            }
        }
    }

    for (int i = 0; i < 8; i++) {
        delete[] board[i];
        delete[] previousBoard[i];
    }
    delete[] board;
    delete[] previousBoard;
}

void Board::initializeBoard() { //setting up board pieces
    // White pieces
    for (int col = 0; col < 8; col++) {
        placePiece(new Pawn(true, 6, col), 6, col);
    }

    placePiece(new Rook(true, 7, 0), 7, 0);
    placePiece(new Rook(true, 7, 7), 7, 7);
    placePiece(new Knight(true, 7, 1), 7, 1);
    placePiece(new Knight(true, 7, 6), 7, 6);
    placePiece(new Bishop(true, 7, 2), 7, 2);
    placePiece(new Bishop(true, 7, 5), 7, 5);
    placePiece(new Queen(true, 7, 3), 7, 3);
    placePiece(new King(true, 7, 4), 7, 4);

    // Black pieces
    for (int col = 0; col < 8; col++) {
        placePiece(new Pawn(false, 1, col), 1, col);
    }

    placePiece(new Rook(false, 0, 0), 0, 0);
    placePiece(new Rook(false, 0, 7), 0, 7);
    placePiece(new Knight(false, 0, 1), 0, 1);
    placePiece(new Knight(false, 0, 6), 0, 6);
    placePiece(new Bishop(false, 0, 2), 0, 2);
    placePiece(new Bishop(false, 0, 5), 0, 5);
    placePiece(new Queen(false, 0, 3), 0, 3);
    placePiece(new King(false, 0, 4), 0, 4);
}

// Place a piece on the board at a location
void Board::placePiece(Piece* piece, int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        board[row][col] = piece;
    }
}
// Clear a square on the board 
void Board::clearSquare(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) {
        board[row][col] = nullptr; // setting to nullptr
    }
}

void Board::copyBoardState(Piece***, Piece*** to) {
    // Clear previous board
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (to[i][j] != nullptr) {
                delete to[i][j];
                to[i][j] = nullptr;
            }
        }
    }
}

void Board::display(int selectedRow, int selectedCol) const {
    cout << "\n";
    cout << "    A   B   C   D   E   F   G   H\n" << endl;
    cout << "  ---------------------------------" << endl;

    for (int row = 0; row < 8; row++) {
        cout << (8 - row) << " ";

        for (int col = 0; col < 8; col++) {
            bool isSelected = (row == selectedRow && col == selectedCol);
            bool isLight = (row + col) % 2 == 0;

          cout << "| ";

            Piece* piece = board[row][col];
            if (piece != nullptr) {
                char pieceChar = piece->getSymbol();
                cout << pieceChar;
            }
            else cout << " ";
         
            cout << " ";
        }

        cout << "| " << (8 - row) << endl;
        cout << "  ---------------------------------\n";
    }

    cout << "    A   B   C   D   E   F   G   H\n" << endl;
}

bool Board::movePiece(int sourceRow, int sourceCol, int targetRow, int targetCol) {
    if (sourceRow < 0 || sourceRow >= 8 || sourceCol < 0 || sourceCol >= 8) {
        return false;
    }

    Piece* movingPiece = board[sourceRow][sourceCol];
    if (movingPiece == nullptr) {
        return false;
    }

    if (movingPiece->getIsWhite() != whitesTurn) {
        return false;
    }

    if (targetRow < 0 || targetRow >= 8 || targetCol < 0 || targetCol >= 8) {
        return false;
    }

    // Check for en passant capture
    bool isEnPassant = false;
    if (movingPiece->getSymbol() == (whitesTurn ? 'P' : 'p')) {
        if (targetRow < 0 || targetRow >= 8) return false;
        if (board[targetRow][targetCol] == nullptr && sourceCol != targetCol) {
            // This could be en passant
            isEnPassant = true;
        }
    }

    if (!movingPiece->isValidMove(targetRow, targetCol, board, previousBoard)) {
        return false;
    }

    // Save previous board state
    copyBoardState(board, previousBoard);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board[i][j] != nullptr) {
                // Just mark that a piece was there (don't copy actual piece)
                previousBoard[i][j] = board[i][j];
            }
        }
    }

    // en passant capture
    if (isEnPassant) {
        if (board[sourceRow][targetCol] != nullptr) {
            delete board[sourceRow][targetCol];
            board[sourceRow][targetCol] = nullptr;
        }
    }

    // Execute the move
    if (board[targetRow][targetCol] != nullptr) {
        delete board[targetRow][targetCol];
    }

    board[targetRow][targetCol] = movingPiece;
    board[sourceRow][sourceCol] = nullptr;
    movingPiece->setPosition(targetRow, targetCol);

    return true;
}

// Check if a specific square contains a piece
bool Board::hasPiece(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return false;
    return board[row][col] != nullptr;
}

Piece* Board::getPiece(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return nullptr;
    return board[row][col];
}

bool Board::getWhitesTurn() const { return whitesTurn; }

// Switch turn to the other player
void Board::switchTurn() { whitesTurn = !whitesTurn;}

// Find the king piece for a given color
King* Board::findKing(bool isWhite) const {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Piece* piece = board[row][col];
            if (piece != nullptr && piece->getIsWhite() == isWhite) {
                King* king = dynamic_cast<King*>(piece);
                if (king != nullptr) {
                    return king;
                }
            }
        }
    }
    return nullptr;
}
// Check if opponent can attack a given square
bool Board::canOpponentAttack(int row, int col, bool isWhiteAttacking) const {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece* piece = board[r][c];
            if (piece != nullptr && piece->getIsWhite() == isWhiteAttacking) {
                if (piece->isValidMove(row, col, board)) {
                    return true;
                }
            }
        }
    }
    return false;
}

//king check state
bool Board::isInCheck(bool isWhiteKing) const {
    King* king = findKing(isWhiteKing);
    if (king == nullptr) return false;

    return canOpponentAttack(king->getRow(), king->getCol(), !isWhiteKing);
}

// check for checkmate 
bool Board::isCheckmate(bool isWhiteKing) const {
    if (!isInCheck(isWhiteKing)) return false;

    King* king = findKing(isWhiteKing);
    if (king == nullptr) return false;

	//get king's current position
    int kingRow = king->getRow();
    int kingCol = king->getCol();

    for (int r = kingRow - 1; r <= kingRow + 1; r++) {
        for (int c = kingCol - 1; c <= kingCol + 1; c++) {
            //check for boundary 
            if (r >= 0 && r < 8 && c >= 0 && c < 8) {
                //if (r == kingRow && c == kingCol) continue;

                Piece* targetPiece = board[r][c];
                if (targetPiece == nullptr || targetPiece->getIsWhite() != isWhiteKing) {
                    Piece* capturePiece = board[r][c];
                    board[r][c] = king;
                    board[kingRow][kingCol] = nullptr;
                    king->setPosition(r, c);

                    bool check = isInCheck(isWhiteKing);

                    board[kingRow][kingCol] = king;
                    board[r][c] = capturePiece;
                    king->setPosition(kingRow, kingCol);

                    if (!check) return false;
                }
            }
        }
    }

    return true;
}

//Game class implementation
Game::Game() : gameActive(true), gameWon(false), isWhiteWinner(false),
cursorRow(0), cursorCol(0), selectedRow(-1), selectedCol(-1) {
    board = new Board();
}

// Destructor to clean up memory
Game::~Game() {
    if (board != nullptr) {
        delete board;
        board = nullptr;
    }
}

void Game::clearScreen() const { system("cls"); }

// Display game status and instructions
void Game::displayGameStatus() const {
    cout << endl;
    cout << "\t\t Arrow Keys: Move | ENTER: Select " << endl;

    string currentPlayer = board->getWhitesTurn() ? "WHITE" : "BLACK";
    cout << "Current Turn: " << currentPlayer << "\n";

    if (board->isInCheck(board->getWhitesTurn())) {
        cout << currentPlayer << " IS IN CHECK! ***\n";
    }

    cout << "\nControls:" << endl;
    cout << "- Arrow Keys: Move cursor (UP/DOWN/LEFT/RIGHT)" << endl;
    cout << "- ENTER: Select piece or confirm move" << endl;
    cout << "- Q: Quit game" << endl;
    cout << "- Piece Symbols: P=Pawn, R=Rook, N=Knight, B=Bishop, Q=Queen, K=King" << endl;
    cout << "- Uppercase: White, Lowercase: Black" << endl << endl;
}

int Game::getKeyInput() {
    int ch = _getch();

    if (ch == 0 || ch == 224) {
        int arrow = _getch();
        switch (arrow) {
            case 72: return KEY_UP;
            case 80: return KEY_DOWN;
            case 75: return KEY_LEFT;
            case 77: return KEY_RIGHT;
            default: return arrow;
        }
    }

    if (ch == 13) return '\n';
    return ch;
}

bool Game::processMove(int key) {
    // Arrow key handling 
    if (key == KEY_UP || key == 'w' || key == 'W') cursorRow = (cursorRow - 1 + 8) % 8;  // Up
    else if (key == KEY_DOWN || key == 's' || key == 'S') cursorRow = (cursorRow + 1) % 8;  // Down
    else if (key == KEY_LEFT || key == 'a' || key == 'A') cursorCol = (cursorCol - 1 + 8) % 8;  // Left
    else if (key == KEY_RIGHT || key == 'd' || key == 'D') cursorCol = (cursorCol + 1) % 8;  // Right
    else if (key == '\n' || key == '\r') {  // ENTER key
        if (selectedRow == -1 && selectedCol == -1) {
            // Select a piece
            Piece* piece = board->getPiece(cursorRow, cursorCol);
            if (piece != nullptr && piece->getIsWhite() == board->getWhitesTurn()) {
                selectedRow = cursorRow;
                selectedCol = cursorCol;
            }
        }
        else {
            // Move the selected piece
            if (board->movePiece(selectedRow, selectedCol, cursorRow, cursorCol)) {
                cout << "Move successful!\n";
                selectedRow = -1;
                selectedCol = -1;

                // Check for checkmate
                bool opponentIsWhite = !board->getWhitesTurn();
                if (board->isCheckmate(opponentIsWhite)) {
                    gameActive = false;
                    gameWon = true;
                    isWhiteWinner = board->getWhitesTurn();
                    return false;
                }
				// Switch turn to the other player
                board->switchTurn();
                return true;
            }
            else {
                cout << "Invalid move! Try again.\n";
                //setting row and col to invalid 
                selectedRow = -1;
                selectedCol = -1;
            }
        }
    }
    else if (key == 'q' || key == 'Q') {
        gameActive = false;
        return false;
    }

    return true;
}

bool Game::playTurn() {

    displayGameStatus();
    board->display(selectedRow, selectedCol);

    getoxy(0, 20);
    cout << "Cursor at: " << (char)('A' + cursorCol) << (8 - cursorRow);
    if (selectedRow != -1 && selectedCol != -1) {
        cout << " | Selected: " << (char)('A' + selectedCol) << (8 - selectedRow);
    }
    cout << "      ";

    getoxy(0, 22);
    cout << "Enter move (Arrow keys/WASD, ENTER to select, Q to quit): ";
    int key = getKeyInput();

    return processMove(key);
}
// Main menu
void Game::run() {
    clearScreen();
    cout << "\t\tWELCOME TO CHESS GAME" << endl;
  
    cout << "\nInstructions:" << endl;
    cout << "- Use WASD to move the cursor around the board\n";
    cout << "- Press ENTER to select a piece, then move cursor to target square\n";
    cout << "- Press ENTER again to move the piece\n";
    cout << "- The board shows: Light squares (white), Dark squares (black), Green = Selected\n";
    cout << "- En Passant is supported for pawn captures\n";
    cout << "- Type Q to quit\n" << endl;

    cout << "Press ENTER to start...";
    cin.get();

    while (gameActive) {
        clearScreen();
        if (!playTurn()) {
            break;
        }
    }

    clearScreen();
    cout << endl;
    cout << "           GAME OVER!                    " << endl;

    if (gameWon) {
        string winner = isWhiteWinner ? "WHITE" : "BLACK";
        cout << "Congratulations! " << winner << " WINS!" << endl;
    }
    else  cout << "Game ended. Thanks for playing!" << endl;
   
    cout << endl;
}

bool Game::isGameActive() const {  return gameActive;}
