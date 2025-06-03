// orel8155@gmail.com
/**
 * Main implementation file for the Coup card game
 * This file implements the game's main function, simulation logic, and user interface
 */

// Include necessary headers
#include "Game.hpp"               // Core game logic
#include "Player.hpp"             // Player class definition
#include "Roles/General.hpp"      // Role-specific classes
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "GameExceptions.hpp"     // Custom exceptions
#include "CoupGUI.hpp"            // Graphical user interface
#include <SFML/System/Thread.hpp> // SFML library for threads
#include <iostream>               // Input/output streams
#include <vector>                 // Dynamic arrays
#include <exception>              // Exception handling
#include <random>                 // Random number generation
#include <algorithm>              // Algorithm utilities
#include <thread>                 // Thread support
#include <chrono>                 // Time utilities
#include <unordered_map>          // Hash map
using namespace coup;             // Use the coup namespace
using namespace std;              // Use the standard namespace

/**
 * Converts a Role enum value to its string representation
 * @param role The Role enum to convert
 * @return String representation of the role
 */
string role_to_string(Role role)
{
    switch (role)
    {
    case Role::GENERAL:
        return "General";
    case Role::GOVERNOR:
        return "Governor";
    case Role::SPY:
        return "Spy";
    case Role::BARON:
        return "Baron";
    case Role::JUDGE:
        return "Judge";
    case Role::MERCHANT:
        return "Merchant";
    default:
        return "Unknown";
    }
}

/**
 * GameSimulator class - Simulates automatic play of the Coup game
 * This class provides functionality to run random AI-driven games
 */
class GameSimulator
{
private:
    Game &game;                   // Reference to the game instance
    vector<shared_ptr<Player>> &players;  // Reference to the player list
    mutable mt19937 gen;          // Random number generator
    float coupProbability;        // Probability of attempting a coup action
    int maxTurns;                 // Maximum number of turns before ending the game
    bool verboseMode;             // Whether to print detailed game information

public:
    /**
     * Constructor for the GameSimulator
     * @param g Reference to the Game instance
     * @param players Reference to the vector of Player pointers
     * @param verbose Whether to output detailed game information (default: true)
     */
    GameSimulator(Game &g, vector<shared_ptr<Player>> &players, bool verbose = true)
        : game(g), players(players), gen(random_device{}()), coupProbability(0.5f), maxTurns(300), verboseMode(verbose) {}

    /**
     * Prints the current game status including turn and player information
     * Only prints if verbose mode is enabled
     */
    void printGameStatus() const
    {
        if (!verboseMode)
            return;

        cout << "\n=== Game Status ===" << endl;
        cout << "Current Turn: " << endl;
        cout << role_to_string(game.turn()) << endl;
        cout << "Active Players: ";
        for (const auto &playerName : game.players())
        {
            auto player = game.getPlayerByName(playerName);
            cout << playerName << "(" << player->coins() << " coins) ";
        }
        cout << "\n================\n"
             << endl;
    }

    /**
     * Prints information about an action performed by a player
     * @param playerName Name of the player performing the action
     * @param action Name of the action being performed
     * @param target Optional target player of the action
     * @param success Whether the action was successful
     */
    void printAction(const string &playerName, const string &action, const string &target = "", bool success = true) const
    {
        if (!verboseMode)
            return;

        cout << playerName;
        if (!success)
            cout << " tried and failed to ";
        else
            cout << " performed ";
        cout << action;
        if (!target.empty())
            cout << " on " << target;
        cout << endl;
    }

    /**
     * Selects a random target player for an action
     * @param currentPlayer The player who is performing the action
     * @return Shared pointer to the selected target player, or nullptr if no valid target
     */
    shared_ptr<Player> selectRandomTarget(shared_ptr<Player> &currentPlayer)
    {
        auto activePlayerNames = game.players();

        // Remove the current player from potential targets
        activePlayerNames.erase(
            remove(activePlayerNames.begin(), activePlayerNames.end(), currentPlayer->name()),
            activePlayerNames.end());

        if (activePlayerNames.empty())
            return nullptr;

        // Randomly select a target from the remaining players
        uniform_int_distribution<> dist(0, activePlayerNames.size() - 1);
        string targetName = activePlayerNames[dist(gen)];
        for (const auto &player : players)
        {
            if (player->name() == targetName)
            {
                // Found the player with the matching name
                return player;
            }
        }
        return nullptr; // In case no matching player is found
    }

    /**
     * Determines whether a player should attempt a coup action based on probability
     * @return true if coup should be attempted, false otherwise
     */
    bool shouldAttemptCoup() const
    {
        uniform_real_distribution<> dist(0.0, 1.0);
        return dist(gen) < coupProbability;
    }

    /**
     * Determines whether a player should attempt a special action based on probability
     * @return true if special action should be attempted, false otherwise
     */
    bool shouldAttemptSpecialAction() const
    {
        uniform_real_distribution<> dist(0.0, 1.0);
        return dist(gen) < 0.4f; // 40% chance for special action
    }

    /**
     * Executes an action that requires a target player
     * @param player The player performing the action
     * @param action The action to perform ("arrest", "sanction", or "coup")
     */
    void executeTargetAction(shared_ptr<Player> &player, const string &action)
    {
        auto target = selectRandomTarget(player);
        if (!target)
            return;

        try
        {
            if (action == "arrest")
            {
                player->arrest(target);
                printAction(player->name(), "arrest", target->name());
            }
            else if (action == "sanction")
            {
                player->sanction(*target);
                printAction(player->name(), "sanction", target->name());
            }
            else if (action == "coup")
            {
                string targetName = target->name();
                player->coup(target);
                printAction(player->name(), "coup", targetName);
            }
        }
        catch (const GameException &e)
        {
            printAction(player->name(), action, target ? target->name() : "", false);
            if (verboseMode)
            {
                cout << "ERROR: " << e.what() << endl;
            }
            throw;
        }
    }

    /**
     * Executes a basic action that doesn't require a target
     * @param player The player performing the action
     * @param action The action to perform ("gather", "tax", or "bribe")
     */
    void executeBasicAction(shared_ptr<Player> &player, const string &action)
    {
        try
        {
            if (action == "gather")
            {
                player->gather();
                printAction(player->name(), "gather");
            }
            else if (action == "tax")
            {
                player->tax();
                printAction(player->name(), "tax");
            }
            else if (action == "bribe")
            {
                player->bribe();
                printAction(player->name(), "bribe");
            }
        }
        catch (const GameException &e)
        {
            printAction(player->name(), action, "", false);
            if (verboseMode)
            {
                cout << "ERROR: " << e.what() << endl;
            }
            throw;
        }
    }

    /**
     * Executes a special action based on the player's role
     * @param player The player performing the action
     */
    void executeSpecialAction(shared_ptr<Player> &player)
    {
        try
        {
            Role role = player->role();

            if (role == Role::BARON)
            {
                auto baron = dynamic_cast<Baron *>(player.get());
                if (baron)
                {
                    
                }
                return;
            }
            else if (role == Role::SPY)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    auto spy = dynamic_cast<Spy *>(player.get());
                    if (spy)
                    {
                        spy->undo(UndoableAction::ARREST);
                        printAction(player->name(), "block_arrest (special)", target->name());
                    }
                    return;
                }
            }
            else if (role == Role::GOVERNOR)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    auto governor = dynamic_cast<Governor *>(player.get());
                    if (governor && target->get_last_action() == "tax")
                    {
                        governor->undo(UndoableAction::TAX);
                        printAction(player->name(), "cancel_taxes (special)", target->name());
                    }
                    return;
                }
            }
            else if (role == Role::JUDGE)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    auto judge = dynamic_cast<Judge *>(player.get());
                    if (judge && target->get_last_action() == "bribe")
                    {
                        judge->undo(UndoableAction::BRIBE);
                        printAction(player->name(), "cancel_bribe (special)", target->name());
                    }
                    return;
                }
            }
            else if (role == Role::GENERAL)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    auto general = dynamic_cast<General *>(player.get());
                    if (general)
                    {
                        general->undo(UndoableAction::COUP);
                        printAction(player->name(), "block_coup (special)", target->name());
                    }
                    return;
                }
            }

            // If special action failed, perform gather as fallback
            executeBasicAction(player, "gather");
        }
        catch (const GameException &e)
        {
            // fallback action
            try
            {
                executeBasicAction(player, "gather");
            }
            catch (...)
            {
                if (verboseMode)
                {
                    cout << player->name() << " couldn't perform any action!" << endl;
                }
            }
        }
    }

    /**
     * Performs a random turn for the given player based on game state
     * @param player The player whose turn it is
     */
    void performRandomTurn(shared_ptr<Player> &player)
    {
        // If player has 10+ coins, they must perform a coup
        if (player->coins() >= 10)
        {
            auto target = selectRandomTarget(player);
            if (target)
            {
                try
                {
                    player->coup(target);
                    printAction(player->name(), "coup (mandatory)", target->name());
                    return;
                }
                catch (const GameException &e)
                {
                    printAction(player->name(), "coup (mandatory)", target->name(), false);
                    if (verboseMode)
                    {
                        cout << "ERROR during mandatory coup: " << e.what() << endl;
                    }
                }
            }
        }

        // If player has 7+ coins and randomly decides to coup
        if (player->coins() >= 7 && shouldAttemptCoup())
        {
            auto target = selectRandomTarget(player);
            if (target)
            {
                try
                {
                    player->coup(target);
                    printAction(player->name(), "coup", target->name());
                    return;
                }
                catch (const GameException &e)
                {
                    printAction(player->name(), "coup", target->name(), false);
                    if (verboseMode)
                    {
                        cout << "ERROR during coup: " << e.what() << endl;
                    }
                }
            }
        }

        // Choose a random action type
        uniform_int_distribution<> actionTypeDist(0, 2);
        int actionType = actionTypeDist(gen);

        try
        {
            if (actionType == 0)
            {
                // Basic actions (gather, tax, bribe)
                vector<string> basicActions = {"gather", "tax", "bribe"};
                uniform_int_distribution<> dist(0, basicActions.size() - 1);
                executeBasicAction(player, basicActions[dist(gen)]);
            }
            else if (actionType == 1)
            {
                // Target actions (arrest, sanction, coup)
                vector<string> targetActions = {"arrest", "sanction", "coup"};
                uniform_int_distribution<> dist(0, targetActions.size() - 1);
                executeTargetAction(player, targetActions[dist(gen)]);
            }
            else
            {
                // Role-specific special actions
                executeSpecialAction(player);
            }
        }
        catch (const GameException &e)
        {
            // Final fallback - try to gather
            try
            {
                player->gather();
                printAction(player->name(), "gather (fallback)");
            }
            catch (...)
            {
                if (verboseMode)
                {
                    cout << player->name() << " couldn't perform any action!" << endl;
                }
            }
        }
    }

    /**
     * Increases the probability that players will attempt coup actions
     * Used to make games more aggressive as they progress
     */
    void increaseAggression() 
    {
        coupProbability = min(0.9f, coupProbability + 0.2f);
        if (verboseMode)
        {
            cout << ">>> Increasing aggression! Coup probability raised to " << (coupProbability * 100) << "% <<<" << endl;
        }
    }

    /**
     * Runs a complete game with random AI players
     * @return true if the game completed normally, false if it hit the turn limit
     */
    bool runRandomGame()
    {
        cout << "\n🎮 Starting random game with " << players.size() << " players!" << endl;
        printGameStatus();

        int currentTurn = 0;
        string lastPlayer = "";
        int samePlayerCount = 0;
        unordered_map<string, int> playerTurns;

        while (!game.isGameOver() && currentTurn < maxTurns)
        {
            try
            {
                string currentPlayerName = game.getPlayer()->name();
                auto currentPlayer = game.getPlayer();

                // Track number of turns for each player
                playerTurns[currentPlayerName]++;

                // Check if the same player is getting multiple consecutive turns (potential deadlock)
                if (currentPlayerName == lastPlayer)
                {
                    samePlayerCount++;
                    if (samePlayerCount >= 15 && game.players().size() > 1)
                    {
                        cout << "\n⚠️ Detected stalemate: " << currentPlayerName << " played "
                             << samePlayerCount << " consecutive turns!" << endl;

                        // Find the player with the most coins to declare as winner
                        string winnerName = "";
                        int maxCoins = -1;

                        for (const auto &name : game.players())
                        {
                            int coins = game.getPlayerByName(name)->coins();
                            if (coins > maxCoins)
                            {
                                maxCoins = coins;
                                winnerName = name;
                            }
                        }

                        // Remove all players except the winner
                        for (const auto &name : game.players())
                        {
                            if (name != winnerName)
                            {
                                game.removePlayer(name);
                            }
                        }

                        cout << "👑 " << winnerName << " wins with " << maxCoins << " coins!" << endl;
                        break;
                    }
                }
                else
                {
                    samePlayerCount = 0;
                    lastPlayer = currentPlayerName;
                    cout << "\n🔄 Switching to player: " << currentPlayerName << endl;
                }

                if (verboseMode)
                {
                    cout << "\n--- Turn " << (currentTurn + 1) << ": " << currentPlayerName
                         << " (" << role_to_string(currentPlayer->role()) << ", " << currentPlayer->coins() << " coins) ---" << endl;
                }

                performRandomTurn(currentPlayer);

                if (verboseMode && (currentTurn + 1) % 10 == 0)
                {
                    printGameStatus();
                }

                // Increase aggression every 25 turns
                if ((currentTurn + 1) % 25 == 0)
                {
                    increaseAggression();
                }

                // Short delay for readability
                if (verboseMode)
                {
                    this_thread::sleep_for(chrono::milliseconds(200));
                }
            }
            catch (const GameException &e)
            {
                if (verboseMode)
                {
                    cout << "Error: " << e.what() << endl;
                }
            }

            currentTurn++;
        }

        if (game.isGameOver())
        {
            cout << "\n🏆 Game over! The winner is: " << game.winner() << "! 🏆" << endl;
            printGameStatus();
            return true;
        }
        else
        {
            cout << "\n⏰ Game reached turn limit (" << maxTurns << ")" << endl;
            return false;
        }
    }

    /**
     * Sets the verbose mode for the simulator
     * @param verbose true for detailed output, false for minimal output
     */
    void setVerbose(bool verbose)
    {
        verboseMode = verbose;
    }
};

/**
 * Creates and runs a random game with predefined players
 * This function sets up a game with one player of each role
 */
void runRandomGame()
{
    cout << "\n=== Running Random Game ===" << endl;
    
    Game game;
    auto general = game.createPlayer("Dan", Role::GENERAL);
    auto merchant = game.createPlayer("Ron", Role::MERCHANT);
    auto governor = game.createPlayer("Liat", Role::GOVERNOR);
    auto spy = game.createPlayer("Noa", Role::SPY);
    auto baron = game.createPlayer("Amit", Role::BARON);
    auto judge = game.createPlayer("Michal", Role::JUDGE);

    vector<shared_ptr<Player>> players = {general, merchant, governor, spy, baron, judge};

    GameSimulator simulator(game, players, true);
    simulator.runRandomGame();
}

/**
 * Main function - Entry point of the program
 * Provides a menu to either run the GUI or a random game
 * @return 0 for successful execution, 1 for errors
 */
int main()
{
    cout << "=== Welcome to Coup! ===" << endl;
    cout << "Please choose an option:" << endl;
    cout << "1. Run GUI" << endl;
    cout << "2. Run random game" << endl;
    cout << "Choose an option (1-2): ";
    
    int choice;
    cin >> choice;
    
    try
    {
        if (choice == 1)
        {
            // Run the graphical user interface
            CoupGUI gui;
            gui.run();            
        }
        else if (choice == 2)
        {
            // Run a random simulation game
            runRandomGame();
        }
        else
        {
            cout << "Invalid choice!" << endl;
            return 1;
        }

        
    }
    catch (const exception &e)
    {
        cerr << "General error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}