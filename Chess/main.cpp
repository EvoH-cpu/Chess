#include "Chess.h"

// Main function
int main() {
    try {
        Game* game = new Game();
        game->run();
        delete game;
        game = nullptr;
    }
    catch (exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }
    return 0;
}
