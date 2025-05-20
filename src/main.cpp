// orel8155@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "Roles/General.hpp"
#include "Roles/Merchant.hpp"
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <exception>

using namespace coup;
using namespace std;

// Function to print game status
void printGameStatus(Game &game)
{
    cout << "\n--- Current Game Status ---" << endl;
    cout << "Current turn: " << game.turn() << endl;
    cout << "Active players: ";
    for (const auto &player : game.players())
    {
        cout << player << " ";
    }
    cout << "\n-----------------------\n"
         << endl;
}

int main()
{
    // Create a new game
    Game game;

    // Create players with different roles
    General general{game, "Dan"};
    Merchant merchant{game, "Ron"};
    Governor governor{game, "Liat"};
    Spy spy{game, "Noa"};
    Baron baron{game, "Amit"};
    Judge judge{game, "Michal"};

    cout << "Game starts with 6 players!" << endl;

    try
    {
        // First round - each player gathers coins
        printGameStatus(game);
        general.gather();
        cout << "Dan (General) gathered a coin. Coins: " << general.coins() << endl;

        printGameStatus(game);
        merchant.gather();
        cout << "Ron (Merchant) gathered a coin. Coins: " << merchant.coins() << endl;

        printGameStatus(game);
        governor.gather();
        cout << "Liat (Governor) gathered a coin. Coins: " << governor.coins() << endl;

        printGameStatus(game);
        spy.gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy.coins() << endl;

        printGameStatus(game);
        baron.gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron.coins() << endl;

        printGameStatus(game);
        judge.gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge.coins() << endl;

        // Second round - using special abilities
        printGameStatus(game);
        general.tax(); // General takes 3 coins as tax
        cout << "Dan (General) took tax. Coins: " << general.coins() << endl;

        printGameStatus(game);
        merchant.bribe(); // Merchant takes 2 coins as bribe
        cout << "Ron (Merchant) took bribe. Coins: " << merchant.coins() << endl;

        printGameStatus(game);
        governor.gather(); // Governor gathers coins
        cout << "Liat (Governor) gathered a coin. Coins: " << governor.coins() << endl;

        printGameStatus(game);
        spy.arrest(governor); // Spy arrests the Governor (this already calls governor.react_to_arrest() internally)
        cout << "Noa (Spy) arrested Liat (Governor)" << endl;

        printGameStatus(game);
        baron.gather(); // Baron gathers a coin instead of sanction (not enough coins for sanction)
        cout << "Amit (Baron) gathered a coin. Coins: " << baron.coins() << endl;

        printGameStatus(game);
        judge.gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge.coins() << endl;

        // Third round
        printGameStatus(game);
        general.gather();
        cout << "Dan (General) gathered a coin. Coins: " << general.coins() << endl;

        printGameStatus(game);
        merchant.tax(); // Merchant tries to take tax despite the sanction
        cout << "Ron (Merchant) took tax despite the sanction. Coins: " << merchant.coins() << endl;

        printGameStatus(game);
        governor.gather(); // Governor's turn - gathering a coin
        cout << "Liat (Governor) gathered a coin. Coins: " << governor.coins() << endl;

        printGameStatus(game);
        spy.gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy.coins() << endl;

        printGameStatus(game);
        baron.gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron.coins() << endl;

        printGameStatus(game);
        judge.gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge.coins() << endl;

        // One more round
        printGameStatus(game);
        general.gather();
        cout << "Dan (General) gathered a coin. Coins: " << general.coins() << endl;

        printGameStatus(game);
        // Check if merchant has enough coins for coup
        if (merchant.coins() >= 7)
        {
            merchant.coup(spy); // Merchant performs coup on Spy
            cout << "Ron (Merchant) performed coup on Noa (Spy)" << endl;
        }
        else
        {
            merchant.gather();
            cout << "Ron (Merchant) gathered a coin. Coins: " << merchant.coins() << endl;
        }

        printGameStatus(game);
        governor.gather();
        cout << "Liat (Governor) gathered a coin. Coins: " << governor.coins() << endl;

        // At this point, merchant might have performed a coup on Spy
        // Continue with players who are still active
        printGameStatus(game);
        try
        {
            if (game.isPlayerActive("Noa"))
            {
                spy.gather();
                cout << "Noa (Spy) gathered a coin. Coins: " << spy.coins() << endl;
            }
        }
        catch (const exception &e)
        {
            cout << "Spy is no longer active" << endl;
        }

        printGameStatus(game);
        baron.gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron.coins() << endl;

        printGameStatus(game);
        judge.gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge.coins() << endl;

        // Display coin status before continuing
        cout << "\n--- Coin status before final rounds ---" << endl;
        cout << "Dan (General): " << general.coins() << " coins" << endl;
        cout << "Ron (Merchant): " << merchant.coins() << " coins" << endl;
        cout << "Liat (Governor): " << governor.coins() << " coins" << endl;
        if (game.isPlayerActive("Noa"))
        {
            cout << "Noa (Spy): " << spy.coins() << " coins" << endl;
        }
        cout << "Amit (Baron): " << baron.coins() << " coins" << endl;
        cout << "Michal (Judge): " << judge.coins() << " coins" << endl;

        // Continue playing until we have a winner
        cout << "\n--- Continuing game until only one player remains ---" << endl;

        // Play additional rounds until only one player remains
        while (game.players().size() > 1)
        {
            printGameStatus(game);
            string current_player = game.turn();

            // Determine the current player and perform their action
            if (current_player == "Dan")
            {
                if (general.coins() >= 7)
                {
                    // Enough coins for coup - find someone to coup
                    if (game.isPlayerActive("Amit"))
                    {
                        general.coup(baron);
                        cout << "Dan (General) performed coup on Amit (Baron)" << endl;
                    }
                    else if (game.isPlayerActive("Michal"))
                    {
                        general.coup(judge);
                        cout << "Dan (General) performed coup on Michal (Judge)" << endl;
                    }
                    else if (game.isPlayerActive("Liat"))
                    {
                        general.coup(governor);
                        cout << "Dan (General) performed coup on Liat (Governor)" << endl;
                    }
                    else if (game.isPlayerActive("Ron"))
                    {
                        general.coup(merchant);
                        cout << "Dan (General) performed coup on Ron (Merchant)" << endl;
                    }
                }
                else
                {
                    general.gather();
                    cout << "Dan (General) gathered a coin. Coins: " << general.coins() << endl;
                }
            }
            else if (current_player == "Ron")
            {
                if (merchant.coins() >= 7)
                {
                    // Enough coins for coup
                    if (game.isPlayerActive("Amit"))
                    {
                        merchant.coup(baron);
                        cout << "Ron (Merchant) performed coup on Amit (Baron)" << endl;
                    }
                    else if (game.isPlayerActive("Michal"))
                    {
                        merchant.coup(judge);
                        cout << "Ron (Merchant) performed coup on Michal (Judge)" << endl;
                    }
                    else if (game.isPlayerActive("Liat"))
                    {
                        merchant.coup(governor);
                        cout << "Ron (Merchant) performed coup on Liat (Governor)" << endl;
                    }
                    else if (game.isPlayerActive("Dan"))
                    {
                        merchant.coup(general);
                        cout << "Ron (Merchant) performed coup on Dan (General)" << endl;
                    }
                }
                else
                {
                    merchant.gather();
                    cout << "Ron (Merchant) gathered a coin. Coins: " << merchant.coins() << endl;
                }
            }
            else if (current_player == "Liat")
            {
                if (governor.coins() >= 7)
                {
                    // Enough coins for coup
                    if (game.isPlayerActive("Amit"))
                    {
                        governor.coup(baron);
                        cout << "Liat (Governor) performed coup on Amit (Baron)" << endl;
                    }
                    else if (game.isPlayerActive("Michal"))
                    {
                        governor.coup(judge);
                        cout << "Liat (Governor) performed coup on Michal (Judge)" << endl;
                    }
                    else if (game.isPlayerActive("Ron"))
                    {
                        governor.coup(merchant);
                        cout << "Liat (Governor) performed coup on Ron (Merchant)" << endl;
                    }
                    else if (game.isPlayerActive("Dan"))
                    {
                        governor.coup(general);
                        cout << "Liat (Governor) performed coup on Dan (General)" << endl;
                    }
                }
                else
                {
                    governor.gather();
                    cout << "Liat (Governor) gathered a coin. Coins: " << governor.coins() << endl;
                }
            }
            else if (current_player == "Amit")
            {
                if (baron.coins() >= 7)
                {
                    // Enough coins for coup
                    if (game.isPlayerActive("Michal"))
                    {
                        baron.coup(judge);
                        cout << "Amit (Baron) performed coup on Michal (Judge)" << endl;
                    }
                    else if (game.isPlayerActive("Liat"))
                    {
                        baron.coup(governor);
                        cout << "Amit (Baron) performed coup on Liat (Governor)" << endl;
                    }
                    else if (game.isPlayerActive("Ron"))
                    {
                        baron.coup(merchant);
                        cout << "Amit (Baron) performed coup on Ron (Merchant)" << endl;
                    }
                    else if (game.isPlayerActive("Dan"))
                    {
                        baron.coup(general);
                        cout << "Amit (Baron) performed coup on Dan (General)" << endl;
                    }
                }
                else
                {
                    baron.gather();
                    cout << "Amit (Baron) gathered a coin. Coins: " << baron.coins() << endl;
                }
            }
            else if (current_player == "Michal")
            {
                if (judge.coins() >= 7)
                {
                    // Enough coins for coup
                    if (game.isPlayerActive("Amit"))
                    {
                        judge.coup(baron);
                        cout << "Michal (Judge) performed coup on Amit (Baron)" << endl;
                    }
                    else if (game.isPlayerActive("Liat"))
                    {
                        judge.coup(governor);
                        cout << "Michal (Judge) performed coup on Liat (Governor)" << endl;
                    }
                    else if (game.isPlayerActive("Ron"))
                    {
                        judge.coup(merchant);
                        cout << "Michal (Judge) performed coup on Ron (Merchant)" << endl;
                    }
                    else if (game.isPlayerActive("Dan"))
                    {
                        judge.coup(general);
                        cout << "Michal (Judge) performed coup on Dan (General)" << endl;
                    }
                }
                else
                {
                    judge.gather();
                    cout << "Michal (Judge) gathered a coin. Coins: " << judge.coins() << endl;
                }
            }
        }

        // Show the final status and winner
        printGameStatus(game);
        cout << "\nThe winner is: " << game.winner() << "!" << endl;

        // Exit cleanly to avoid segfault from shared_ptr issues
        exit(0);
    }
    catch (const exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }

    return 0;
}