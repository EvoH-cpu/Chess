#include "Chess.h"


// Main function - initializes and runs the chess game
int main() {
    try {
        // Create a new game instance
        Game* game = new Game();

        // Run the game loop 
        game->run();


        // Clean up allocated memory
        delete game;
        game = nullptr;

    }
    catch (exception& e) {
        // Exception handling for any runtime errors
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0; 
}