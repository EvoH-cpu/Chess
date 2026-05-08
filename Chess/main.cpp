#include "Chess.h"

//main Function
int main() {
    //custom exceptions
    try {
        Game game;
        game.run();
    }
    // Catch any exceptions thrown during the game
	catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }
    return 0;
}