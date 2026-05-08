#include "Chess.h"

//Console utilities 
void gotoxy(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void setColor(int fg, int bg) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (bg << 4) | (fg & 0x0F));
}

void resetColor() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // default grey on black
}

void hideConsoleCursor() {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(h, &info);
}

//Player
Player::Player(const string& playerName, bool white) : name(playerName), isWhite(white) {}
Player::~Player() {}
string Player::getName() const { return name; }
bool   Player::getIsWhite() const { return isWhite; }
void   Player::setName(const string& n) { name = n; }
void   Player::setIsWhite(bool w) { isWhite = w; }

//Piece base class implentation
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

//checking target square 
bool Piece::isEmptyOrEnemy(int targetRow, int targetCol, Piece*** board) const {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    Piece* t = board[targetRow][targetCol];
    return t == nullptr || t->isWhite != this->isWhite;
}

// checking path to the target square
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

//pawn class implentation
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

//rook class implementation
Rook::Rook(bool white, int row, int col)
    : Piece(white ? 'R' : 'r', white, row, col) {}

bool Rook::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (row != targetRow && col != targetCol) return false;
    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

//knight class implementation
Knight::Knight(bool white, int row, int col)
    : Piece(white ? 'N' : 'n', white, row, col) {}

bool Knight::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;

    int dr = abs(targetRow - row), dc = abs(targetCol - col);
    if ((dr == 2 && dc == 1) || (dr == 1 && dc == 2))
        return isEmptyOrEnemy(targetRow, targetCol, board);
    return false;
}

//bishop class implementation
Bishop::Bishop(bool white, int row, int col)
    : Piece(white ? 'B' : 'b', white, row, col) {}

bool Bishop::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (abs(targetRow - row) != abs(targetCol - col)) return false;
    return isPathClear(targetRow, targetCol, board) &&
        isEmptyOrEnemy(targetRow, targetCol, board);
}

//queen class implementation
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

//king class implementation
King::King(bool white, int row, int col)
    : Piece(white ? 'K' : 'k', white, row, col) {}

bool King::isValidMove(int targetRow, int targetCol, Piece*** board, Piece***) {
    if (!isWithinBoard(targetRow, targetCol)) return false;
    if (targetRow == row && targetCol == col) return false;
    if (abs(targetRow - row) > 1 || abs(targetCol - col) > 1) return false;
    return isEmptyOrEnemy(targetRow, targetCol, board);
}

//Board setup 
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

//placing single piece on board
void Board::placePiece(Piece* piece, int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        board[row][col] = piece;
}

//setting target square to nullptr
void Board::clearSquare(int row, int col) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8)
        board[row][col] = nullptr;
}


void Board::copyBoardState(Piece*** from, Piece*** to) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (to[i][j]) { delete to[i][j]; to[i][j] = nullptr; }
            if (from[i][j]) {
                char sym = from[i][j]->getSymbol();
                bool w = from[i][j]->getIsWhite();
                int r = from[i][j]->getRow();
                int c = from[i][j]->getCol();

                // Create a new object piece of the same type
                if (sym == 'P' || sym == 'p') to[i][j] = new Pawn(w, r, c);
                else if (sym == 'R' || sym == 'r') to[i][j] = new Rook(w, r, c);
                else if (sym == 'N' || sym == 'n') to[i][j] = new Knight(w, r, c);
                else if (sym == 'B' || sym == 'b') to[i][j] = new Bishop(w, r, c);
                else if (sym == 'Q' || sym == 'q') to[i][j] = new Queen(w, r, c);
                else if (sym == 'K' || sym == 'k') to[i][j] = new King(w, r, c);

                if (to[i][j]) to[i][j]->setHasMoved(from[i][j]->getHasMoved());
            }
        }
    }
}

// check if move leaves own king in check
bool Board::KingInCheck(int srcRow, int srcCol, int tgtRow, int tgtCol) {
    Piece* moving = board[srcRow][srcCol]; // get the piece being moved
    if (!moving) return true;

    Piece* captured = board[tgtRow][tgtCol]; // piece at target square
    Piece* enPassantCaptured = nullptr;

    //checking en passant condition
    bool isEnPassant = (moving->getSymbol() == (moving->getIsWhite() ? 'P' : 'p')) &&
        (tgtCol != srcCol) && (captured == nullptr);


    if (isEnPassant) {
        enPassantCaptured = board[srcRow][tgtCol];
        board[srcRow][tgtCol] = nullptr;
    }

    int oldRow = moving->getRow();
    int oldCol = moving->getCol();
    bool oldMoved = moving->getHasMoved();


    board[tgtRow][tgtCol] = moving;
    board[srcRow][srcCol] = nullptr;
    moving->setPosition(tgtRow, tgtCol);

    bool inCheck = isInCheck(moving->getIsWhite());

    // restore
    board[srcRow][srcCol] = moving;
    board[tgtRow][tgtCol] = captured;
    if (isEnPassant) board[srcRow][tgtCol] = enPassantCaptured;

    moving->setPosition(oldRow, oldCol);
    moving->setHasMoved(oldMoved);

    return inCheck;
}

vector<pair<int, int>> Board::getLegalMoves(int row, int col) {
    vector<pair<int, int>> moves;
    Piece* p = getPiece(row, col);
    if (!p) return moves;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (p->isValidMove(r, c, board, previousBoard)) {
                if (!KingInCheck(row, col, r, c)) {
                    moves.emplace_back(r, c);
                }
            }
        }
    }
    return moves;
}
//checking for legal moves for current player
bool Board::LegalMove(bool isWhite) {
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            Piece* p = board[r][c];
            if (p && p->getIsWhite() == isWhite) {
                for (int tr = 0; tr < 8; tr++) {
                    for (int tc = 0; tc < 8; tc++) {
                        if (p->isValidMove(tr, tc, board, previousBoard) &&
                            !KingInCheck(r, c, tr, tc)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

//  Colors used:
//    Board squares  : light blue / light green
//    White pieces   : white text
//    Black pieces   : black text
//    Cursor/Select  : yellow background
//    Hints          : grey dots


//displaying the board
void Board::display(int curRow, int curCol,
    int selRow, int selCol,
    const vector<pair<int, int>>* hints,
    int originX, int originY) const
{
    const int LIGHT_BLUE = 9;
    const int LIGHT_GREEN = 10;
    const int YELLOW = 14;
    const int GREY = 8;

    // Column labels
    gotoxy(originX, originY);
    setColor(7);
    cout << "    A   B   C   D   E   F   G   H";

    // Top border
    gotoxy(originX, originY + 1);
    setColor(7);
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

            bool isHint = false;
            if (hints) {
                for (const auto& h : *hints) {
                    if (h.first == row && h.second == col) { isHint = true; break; }
                }
            }

            // Cell separator
            setColor(7);
            cout << "|";

            int bg = ((row + col) % 2 == 0) ? LIGHT_BLUE : LIGHT_GREEN;
            if (isCursor || isSelected) bg = GREY;

            // Cell background
            setColor(7, bg);
            cout << " ";

            Piece* piece = board[row][col];
            if (piece != nullptr) {
                int fg = piece->getIsWhite() ? 15 : 0;
                setColor(fg, bg);
                cout << piece->getSymbol();
            }
            else if (isHint) {
                setColor(8, bg);              // dark grey
                cout << (char)219;            // full block = big dot
            }
            else {
                setColor(7, bg);
                cout << " ";
            }

            // space after
            setColor(7, bg);
            cout << " ";
        }

        setColor(7);
        cout << "| " << (8 - row);

        // Separator row
        gotoxy(originX, consoleY + 1);
        setColor(7);
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
    if (KingInCheck(srcRow, srcCol, tgtRow, tgtCol)) return false;

    copyBoardState(board, previousBoard);

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

bool Board::isCheckmate(bool isWhiteKing) {
    if (!isInCheck(isWhiteKing)) return false;
    return !LegalMove(isWhiteKing);
}

//Game class implementation

Game::Game()
    : gameActive(true), gameWon(false), isWhiteWinner(false),
    cursorRow(0), cursorCol(0), selectedRow(-1), selectedCol(-1) {

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
        cout << "                                                            ";
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
    cout << "  UPPER=White  lower=Black   " << "  [Yellow=cursor/selected]";

    resetColor();
}

// Key reading
int Game::getKeyInput() {
    int key = _getch();
    if (key == 0xE0 || key == 0x00) {
        key = _getch();
        return -(key);
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
            Piece* piece = board->getPiece(cursorRow, cursorCol);
            if (piece && piece->getIsWhite() == board->getWhitesTurn()) {
                selectedRow = cursorRow;
                selectedCol = cursorCol;
                legalMoves = board->getLegalMoves(selectedRow, selectedCol);
            }
        }
        else {
            if (board->movePiece(selectedRow, selectedCol, cursorRow, cursorCol)) {
                selectedRow = selectedCol = -1;
                legalMoves.clear();

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
                selectedRow = selectedCol = -1;
                legalMoves.clear();
            }
        }
    }
    else if (key == 'q' || key == 'Q' || key == KEY_ESC) {
        gameActive = false;
        return false;
    }

    return true;
}
//check active status
bool Game::isGameActive() const { return gameActive; }

// Single turn
bool Game::playTurn() {
    board->display(cursorRow, cursorCol,
        selectedRow, selectedCol,
        &legalMoves,
        BOARD_ORIGIN_X, BOARD_ORIGIN_Y);
    displayGameStatus();
    drawCursor();

    int key = getKeyInput();
    return processMove(key);
}

// Main game loop
void Game::run() {
    clearScreen();
    hideConsoleCursor();

    gotoxy(0, 0);
    setColor(15);
    cout << " WELCOME TO CHESS GAME " << endl;
    setColor(7);

    //storing player name
    string player1Name, player2Name;
    cout << "\nEnter Player 1 Name: ";
    cin >> player1Name;
    cout << "\nEnter Player 2 Name: ";
    cin >> player2Name;

    //creating player objects
    Player player1(player1Name, true);
    Player player2(player2Name, false);

    clearScreen();

    cout << "\tInstructions:\n" << endl;
    cout << "\n\tArrow Keys / WASD: move cursor \n\tENTER: select & move \n\tQ: quit\n" << endl;
    cout << "\tPress ENTER to start...";
    resetColor();

    //running Game loop
    while (true) { int k = _getch(); if (k == KEY_ENTER || k == '\r') break; }

    clearScreen();

    gotoxy(0, 0);
    setColor(15);
    cout << "\t    CHESS BOARD" << endl;
    resetColor();

    while (gameActive) {
        if (!playTurn()) break;
    }

    clearScreen();
    gotoxy(0, 0);
    setColor(14);
    cout << "\n\n GAME OVER!\n" << endl;

    //displaying result of game
    if (gameWon) {
        setColor(15);

        // Display winner name
        string winner = isWhiteWinner ? player1.getName() : player2.getName();
        cout << "   Congratulations! " << winner << " WINS!\n" << endl;
    }
    else {
        setColor(7);
        cout << "   Game ended. Thanks for playing The Game!\n\n";
    }
    resetColor();
}