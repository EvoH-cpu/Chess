#include "Chess.h"

//Console utilities 
void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // default grey on black
}

//Player

Player::Player(const string& playerName, bool white)
    : name(playerName), isWhite(white) {}

Player::~Player() {}

string Player::getName()    const { return name; }
bool   Player::getIsWhite() const { return isWhite; }
void   Player::setName(const string& n) { name = n; }
void   Player::setIsWhite(bool w) { isWhite = w; }

//Piece base

Piece::Piece(char sym, bool white, int r, int c)
    : symbol(sym), isWhite(white), row(r), col(c), hasMoved(false) {}

Piece::~Piece() {}

char Piece::getSymbol() const { return symbol; }
bool Piece::getIsWhite() const { return isWhite; }
int  Piece::getRow() const { return row; }
int  Piece::getCol() const { return col; }
bool Piece::getHasMoved() const { return hasMoved; }

void Piece::setPosition(int r, int c) { row = r; col = c; hasMoved = true; }
void Piece::setHasMoved(bool moved) { hasMoved = moved; }

bool Piece::isWithinBoard(int r, int c) const {
    return r >= 0 && r < 8 && c >= 0 && c < 8;
}

bool Piece::isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    Piece* t = board[targetRow][targetCol];
    return t == nullptr || t->isWhite != this->isWhite;
}

bool Piece::isPathClear(int targetRow, int targetCol, Piece*** board) const {
    int dr = (targetRow > row) ? 1 : (targetRow < row) ? -1 : 0;
    int dc = (targetCol > col) ? 1 : (targetCol < col) ? -1 : 0;

    int r = row + dr, c = col + dc;
    while (r != targetRow || c != targetCol) {
        if (board[r][c] != nullptr) return false;
        r += dr; c += dc;
    }
    return true;
}

//Piece subclasses 

Pawn::Pawn(bool white, int row, int col)
    : Piece(white ? 'P' : 'p', white, row, col) {}

bool Pawn::isValidMove(int targetRow, int targetCol, Piece*** board, Piece*** lastBoard) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int rowDiff = targetRow - row;
    int colDiff = abs(targetCol - col);
    int direction = isWhite ? -1 : 1;

    if (rowDiff == direction && colDiff == 0)
        return board[targetRow][targetCol] == nullptr;

    int startingRow = isWhite ? 6 : 1;
    if (row == startingRow && rowDiff == 2 * direction && colDiff == 0) {
        int mid = row + direction;
        return board[mid][col] == nullptr && board[targetRow][targetCol] == nullptr;
    }

    if (rowDiff == direction && colDiff == 1) {
        if (board[targetRow][targetCol] != nullptr &&
            board[targetRow][targetCol]->getIsWhite() != this->isWhite)
            return true;

        if (lastBoard != nullptr && board[targetRow][targetCol] == nullptr) {
            Piece* cap = lastBoard[row][targetCol];
            if (cap != nullptr && cap->getSymbol() == (isWhite ? 'p' : 'P'))
                return true;
        }
    }
    return false;
}

Rook::Rook(bool white, int row, int col)
    : Piece(white ? 'R' : 'r', white, row, col) {}

bool Rook::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (row != targetRow && col != targetCol) return false;
    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

Knight::Knight(bool white, int row, int col)
    : Piece(white ? 'N' : 'n', white, row, col) {}

bool Knight::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int dr = abs(targetRow - row), 
        dc = abs(targetCol - col);

    if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2))
        return isEmptyOrEnemy(targetRow, targetCol, board);
    return false;
}

Bishop::Bishop(bool white, int row, int col)
    : Piece(white ? 'B' : 'b', white, row, col) {}

bool Bishop::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {

    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (abs(targetRow - row) != abs(targetCol - col)) return false;
    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

Queen::Queen(bool white, int row, int col)
    : Piece(white ? 'Q' : 'q', white, row, col) {}

bool Queen::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    int dr = abs(targetRow - row), dc = abs(targetCol - col);
    if (row != targetRow && col != targetCol && dr != dc) return false;
    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

King::King(bool white, int row, int col)
    : Piece(white ? 'K' : 'k', white, row, col) {}

bool King::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (abs(targetRow - row) > 1 || abs(targetCol - col) > 1) return false;
    return isEmptyOrEnemy(targetRow, targetCol, board);
}

//Board
Board::Board() : whitesTurn(true) {
    board = new Piece * *[8];
    previousBoard = new Piece * *[8];
    for (int i = 0; i < 8; i++) {
        board[i] = new Piece * [8];
        previousBoard[i] = new Piece * [8];
        for (int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
            previousBoard[i][j] = nullptr;
        }
    }
    initializeBoard();
}

Board::~Board() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
        
             delete board[i][j];   
            if (previousBoard[i][j]) { delete previousBoard[i][j]; previousBoard[i][j] = nullptr; }
        }
        delete[] board[i];
        delete[] previousBoard[i];
    }
    delete[] board;
    delete[] previousBoard;
}

void Board::initializeBoard() {
    for (int col = 0; col < 8; col++) placePiece(new Pawn(true, 6, col), 6, col);
    placePiece(new Rook(true, 7, 0), 7, 0);
    placePiece(new Rook(true, 7, 7), 7, 7);
    placePiece(new Knight(true, 7, 1), 7, 1);
    placePiece(new Knight(true, 7, 6), 7, 6);
    placePiece(new Bishop(true, 7, 2), 7, 2);
    placePiece(new Bishop(true, 7, 5), 7, 5);
    placePiece(new Queen(true, 7, 3), 7, 3);
    placePiece(new King(true, 7, 4), 7, 4);

    for (int col = 0; col < 8; col++) placePiece(new Pawn(false, 1, col), 1, col);
    placePiece(new Rook(false, 0, 0), 0, 0);
    placePiece(new Rook(false, 0, 7), 0, 7);
    placePiece(new Knight(false, 0, 1), 0, 1);
    placePiece(new Knight(false, 0, 6), 0, 6);
    placePiece(new Bishop(false, 0, 2), 0, 2);
    placePiece(new Bishop(false, 0, 5), 0, 5);
    placePiece(new Queen(false, 0, 3), 0, 3);
    placePiece(new King(false, 0, 4), 0, 4);
}

void Board::placePiece(Piece* piece, int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        board[row][col] = piece;
}

void Board::clearSquare(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        board[row][col] = nullptr;
}

void Board::copyBoardState(Piece***, Piece*** to) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++) {
            if (to[i][j]) { delete to[i][j]; to[i][j] = nullptr; }
        }
}


//  Foreground colors used:
//    White pieces      : 15 (bright white)
//    Black pieces      : 11 (bright cyan)  — distinguishes them clearly
//    Cursor cell       :  2 (green)  when nothing is selected
//                        14 (yellow) when this cell is selected
//    Board border/text :  7 (grey)

void Board::display(int curRow, int curCol,
    int selRow, int selCol,
    int originX, int originY) const
{
   
    // Column labels
    gotoxy(originX, originY);
    setColor(7);
    cout << "    A   B   C   D   E   F   G   H";

    // Top border
    gotoxy(originX, originY + 1);
    cout << "  ---------------------------------";

    for (int row = 0; row < 8; row++) {
        int consoleY = originY + 2 + row * 2;

        // Rank number
        gotoxy(originX, consoleY);
        setColor(7);
        cout << (8 - row) << " ";

        for (int col = 0; col < 8; col++) {
            bool isCursor = (row == curRow && col == curCol);
            bool isSelected = (row == selRow && col == selCol);

            // Cell separator
            setColor(7);
            cout << "|";

            // Determine cell highlight color
            if (isSelected) setColor(14); // yellow  
            else if (isCursor) setColor(10); // bright green = cursor
            else  setColor(7);

            cout <<" ";

            Piece* piece = board[row][col];
            if (piece != nullptr) {
                // Pick piece
                if (isSelected || isCursor) {
                    // keep highlight color for the character
                }
                else {
                    setColor(piece->getIsWhite() ? 15 : 11); // white=bright, black=cyan
                }
                cout << piece->getSymbol();
            }
            else {
                cout << " ";
            }

            // Space after character (keep highlight if cursor/selected)
            if (isSelected)    setColor(14);
            else if (isCursor) setColor(10);
            else               setColor(7);
            cout << " ";
        }

        setColor(7);
        cout << "| " << (8 - row);

        // Separator row
        gotoxy(originX, consoleY + 1);
        cout << "  ---------------------------------";
    }

    // Bottom label
    gotoxy(originX, originY + 18);
    setColor(7);
    cout << "    A   B   C   D   E   F   G   H";

    resetColor();
}

bool Board::movePiece(int srcRow, int srcCol, int tgtRow, int tgtCol) {
    if (srcRow < 0 || srcRow >= 8 || srcCol < 0 || srcCol >= 8) return false;

    Piece* moving = board[srcRow][srcCol];
    if (moving == nullptr) return false;
    if (moving->getIsWhite() != whitesTurn) return false;
    if (tgtRow < 0 || tgtRow >= 8 || tgtCol < 0 || tgtCol >= 8) return false;

    bool isEnPassant = false;
    if (moving->getSymbol() == (whitesTurn ? 'P' : 'p')) {
        if (board[tgtRow][tgtCol] == nullptr && srcCol != tgtCol)
            isEnPassant = true;
    }

    if (!moving->isValidMove(tgtRow, tgtCol, board, previousBoard)) return false;

    // Save previous board state
    copyBoardState(board, previousBoard);
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            previousBoard[i][j] = board[i][j];

    if (isEnPassant && board[srcRow][tgtCol] != nullptr) {
        delete board[srcRow][tgtCol];
        board[srcRow][tgtCol] = nullptr;
    }

    if (board[tgtRow][tgtCol] != nullptr) delete board[tgtRow][tgtCol];

    board[tgtRow][tgtCol] = moving;
    board[srcRow][srcCol] = nullptr;
    moving->setPosition(tgtRow, tgtCol);
    return true;
}

bool   Board::hasPiece(int r, int c) const { return (r >= 0 && r < 8 && c >= 0 && c < 8) && board[r][c] != nullptr; }
Piece* Board::getPiece(int r, int c) const { return (r >= 0 && r < 8 && c >= 0 && c < 8) ? board[r][c] : nullptr; }
bool   Board::getWhitesTurn()        const { return whitesTurn; }
void   Board::switchTurn() { whitesTurn = !whitesTurn; }

King* Board::findKing(bool white) const {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            Piece* p = board[r][c];
            if (p && p->getIsWhite() == white) {
                King* k = dynamic_cast<King*>(p);
                if (k) return k;
            }
        }
    return nullptr;
}

bool Board::canOpponentAttack(int row, int col, bool isWhiteAttacking) const {
    for (int r = 0; r < 8; r++)
        for (int c = 0; c < 8; c++) {
            Piece* p = board[r][c];
            if (p && p->getIsWhite() == isWhiteAttacking)
                if (p->isValidMove(row, col, board)) return true;
        }
    return false;
}

bool Board::isInCheck(bool isWhiteKing) const {
    King* k = findKing(isWhiteKing);
    if (!k) return false;
    return canOpponentAttack(k->getRow(), k->getCol(), !isWhiteKing);
}

bool Board::isCheckmate(bool isWhiteKing) const {
    if (!isInCheck(isWhiteKing)) return false;
    King* king = findKing(isWhiteKing);
    if (!king) return false;

    int kr = king->getRow(), kc = king->getCol();
    for (int r = kr - 1; r <= kr + 1; r++) {
        for (int c = kc - 1; c <= kc + 1; c++) {
            if (r < 0 || r >= 8 || c < 0 || c >= 8) continue;
            Piece* cap = board[r][c];
            if (cap && cap->getIsWhite() == isWhiteKing) continue;

            board[r][c] = king;
            board[kr][kc] = nullptr;
            king->setPosition(r, c);

            bool stillCheck = isInCheck(isWhiteKing);

            board[kr][kc] = king;
            board[r][c] = cap;
            king->setPosition(kr, kc);

            if (!stillCheck) return false;
        }
    }
    return true;
}

//Game class implementation

Game::Game()
    : gameActive(true), gameWon(false), isWhiteWinner(false),
    cursorRow(0), cursorCol(0), selectedRow(-1), selectedCol(-1){

    board = new Board();
}

Game::~Game() { delete board; board = nullptr; }

void Game::clearScreen() const { system("cls"); }

// drawCursor
void Game::drawCursor() const {
   int cx = BOARD_ORIGIN_X + 3 + cursorCol * 4 + 1;
    int cy = BOARD_ORIGIN_Y + 2 + cursorRow * 2;
    gotoxy(cx, cy);
}

//Status panel drawn below the board
void Game::displayGameStatus() const {
    int statusY = BOARD_ORIGIN_Y + 20;

    setColor(7);
    for (int i = 0; i < 5; i++) {
        gotoxy(0, statusY + i);
    }

    gotoxy(0, statusY);

    bool whiteTurn = board->getWhitesTurn();
    setColor(whiteTurn ? 15 : 11);
    cout << "  Current Turn: " << (whiteTurn ? "WHITE" : "BLACK") << "   ";

    if (board->isInCheck(whiteTurn)) {
        setColor(12); // bright red
        cout << "  *** CHECK! ***";
    }

    setColor(7);
    gotoxy(0, statusY + 1);
    cout << "  Arrow Keys: move cursor  |  ENTER: select / move  |  Q: quit";

    gotoxy(0, statusY + 2);
    cout << "  Cursor: " << (char)('A' + cursorCol) << (8 - cursorRow);
    if (selectedRow != -1)
        cout << "  |  Selected: " << (char)('A' + selectedCol) << (8 - selectedRow);
    else cout << " |  (no piece selected)          ";

    gotoxy(0, statusY + 3);
    cout << "  P=Pawn  R=Rook  N=Knight  B=Bishop  Q=Queen  K=King";
    gotoxy(0, statusY + 4);
    cout << "  UPPER=White  lower=Black   " << "  [Green=cursor]  [Yellow = selected]";

    resetColor();
}

// Key reading
int Game::getKeyInput() {
    int key = _getch();
    // Arrow keys send 0xE0 (224) or 0x00 as a prefix
    if (key == 0xE0 || key == 0x00) {
        key = _getch();   // read actual scan code
        return -(key);    // return negative so we can tell it apart
    }
    return key;
}

//Process one keypress
bool Game::processMove(int key) {
    if (key == -KEY_UP || key == 'w' || key == 'W') {
        cursorRow = (cursorRow - 1 + 8) % 8;
    }
    else if (key == -KEY_DOWN || key == 's' || key == 'S') {
        cursorRow = (cursorRow + 1) % 8;
    }
    else if (key == -KEY_LEFT || key == 'a' || key == 'A') {
        cursorCol = (cursorCol - 1 + 8) % 8;
    }
    else if (key == -KEY_RIGHT || key == 'd' || key == 'D') {
        cursorCol = (cursorCol + 1) % 8;
    }
    else if (key == KEY_ENTER || key == '\n' || key == '\r') {
        if (selectedRow == -1) {
            // Select piece
            Piece* piece = board->getPiece(cursorRow, cursorCol);
            if (piece && piece->getIsWhite() == board->getWhitesTurn()) {
                selectedRow = cursorRow;
                selectedCol = cursorCol;
            }
        }
        else {
            // Attempt move
            if (board->movePiece(selectedRow, selectedCol, cursorRow, cursorCol)) {
                selectedRow = selectedCol = -1;

                bool opponent = !board->getWhitesTurn();
                if (board->isCheckmate(opponent)) {
                    gameActive = false;
                    gameWon = true;
                    isWhiteWinner = board->getWhitesTurn();
                    return false;
                }
                board->switchTurn();
            }
            else {
                // Invalid move — deselect
                selectedRow = selectedCol = -1;
            }
        }
    }
    else if (key == 'q' || key == 'Q' || key == KEY_ESC) {
        gameActive = false;
        return false;
    }

    return true;
}

// Single turn (redraw only what changed)
bool Game::playTurn() {
    board->display(cursorRow, cursorCol,
        selectedRow, selectedCol,
        BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    displayGameStatus();
    drawCursor();  // park blinking cursor inside current cell

    int key = getKeyInput();
    return processMove(key);
}

// Main game loop
void Game::run() {
    clearScreen();

    gotoxy(0, 0);
    setColor(15);
    cout << " WELCOME TO CHESS GAME ";
    setColor(7);
    cout << "\n  Arrow Keys / WASD: move cursor \n  ENTER: select & move \n  Q: quit\n" << endl;
    cout << "  Press ENTER to start...";
    resetColor();

    // Wait for ENTER
    while (true) { int k = _getch(); if (k == KEY_ENTER || k == '\r') break; }

    clearScreen();

    gotoxy(0, 0);
    setColor(15);
    cout << "\t CHESS BOARD" << endl;
    resetColor();

    while (gameActive) {
        if (!playTurn()) break;
    }

    clearScreen();
    gotoxy(0, 0);
    setColor(14);
    cout << "\n\n GAME OVER!\n\n";
    if (gameWon) {
        setColor(15);
        string winner = isWhiteWinner ? "WHITE" : "BLACK";
        cout << "   Congratulations! " << winner << " WINS!\n\n";
    }
    else {
        setColor(7);
        cout << "   Game ended. Thanks for playing!\n\n";
    }
    resetColor();
}

bool Game::isGameActive() const { return gameActive; }
