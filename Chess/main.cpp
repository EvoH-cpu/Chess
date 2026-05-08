#include "Chess.h"

int main() {
    try {
        Game game;
        game.run();
    }
    catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }
    return 0;
}
