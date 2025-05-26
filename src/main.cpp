// orel8155@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "Roles/General.hpp"
#include "Roles/Merchant.hpp"
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "GameExceptions.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <exception>
#include <random>
#include <algorithm>
#include <thread>
#include <chrono>
#include <unordered_map>
using namespace coup;
using namespace std;

// פונקציה להמרת Role למחרוזת
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

class GameSimulator
{
private:
    Game &game;
    vector<shared_ptr<Player>> &players;
    mutable mt19937 gen;
    float coupProbability;
    int maxTurns;
    bool verboseMode;

public:
    GameSimulator(Game &g, vector<shared_ptr<Player>> &players, bool verbose = true)
        : game(g), players(players), gen(random_device{}()), coupProbability(0.5f), maxTurns(300), verboseMode(verbose) {}

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

    shared_ptr<Player> selectRandomTarget(shared_ptr<Player> &currentPlayer)
    {
        auto activePlayerNames = game.players();

        // הסר את השחקן הנוכחי
        activePlayerNames.erase(
            remove(activePlayerNames.begin(), activePlayerNames.end(), currentPlayer->name()),
            activePlayerNames.end());

        if (activePlayerNames.empty())
            return nullptr;

        uniform_int_distribution<> dist(0, activePlayerNames.size() - 1);
        string targetName = activePlayerNames[dist(gen)];
        for (const auto &player : players)
        {
            if (player->name() == targetName)
            {
                // מצא את השחקן המתאים לפי השם
                return player;
            }
        }
        return nullptr; // במקרה שלא נמצא שחקן מתאים
    }

    bool shouldAttemptCoup() const
    {
        uniform_real_distribution<> dist(0.0, 1.0);
        return dist(gen) < coupProbability;
    }

    bool shouldAttemptSpecialAction() const
    {
        uniform_real_distribution<> dist(0.0, 1.0);
        return dist(gen) < 0.4f; // 40% סיכוי לפעולה מיוחדת
    }

    void executeTargetAction(shared_ptr<Player> &player, const string &action)
    {
        auto target = selectRandomTarget(player);
        if (!target)
            return;

        try
        {
            if (action == "arrest")
            {
                player->arrest(*target);
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
                player->coup(*target);
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

    void executeSpecialAction(shared_ptr<Player> &player)
    {
        try
        {
            Role role = player->role();

            if (role == Role::BARON)
            {
                player->invest();
                printAction(player->name(), "invest (special)");
                return;
            }
            else if (role == Role::SPY)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    player->block_arrest(*target);
                    printAction(player->name(), "block_arrest (special)", target->name());
                    return;
                }
            }
            else if (role == Role::GOVERNOR)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    player->undo(*target);
                    printAction(player->name(), "undo tax (special)", target->name());
                    return;
                }
            }
            else if (role == Role::JUDGE)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    player->undo(*target);
                    printAction(player->name(), "undo bribe (special)", target->name());
                    return;
                }
            }
            else if (role == Role::GENERAL)
            {
                auto target = selectRandomTarget(player);
                if (target)
                {
                    player->block_coup(*target);
                    printAction(player->name(), "block_coup (special)", target->name());
                    return;
                }
            }

            // אם לא הצליח לבצע פעולה מיוחדת, בצע gather
            executeBasicAction(player, "gather");
        }
        catch (const GameException &e)
        {
            // fallback
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

    void performRandomTurn(shared_ptr<Player> &player)
    {
        // אם יש 10+ מטבעות - חובה coup
        if (player->coins() >= 10)
        {
            auto target = selectRandomTarget(player);
            if (target)
            {
                try
                {
                    player->coup(*target);
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

        // אם יש 7+ מטבעות ונבחר לעשות coup
        if (player->coins() >= 7 && shouldAttemptCoup())
        {
            auto target = selectRandomTarget(player);
            if (target)
            {
                try
                {
                    player->coup(*target);
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

        // בחר סוג פעולה אקראית
        uniform_int_distribution<> actionTypeDist(0, 2);
        int actionType = actionTypeDist(gen);

        try
        {
            if (actionType == 0)
            {
                vector<string> basicActions = {"gather", "tax", "bribe"};
                uniform_int_distribution<> dist(0, basicActions.size() - 1);
                executeBasicAction(player, basicActions[dist(gen)]);
            }
            else if (actionType == 1)
            {
                vector<string> targetActions = {"arrest", "sanction", "coup"};
                uniform_int_distribution<> dist(0, targetActions.size() - 1);
                executeTargetAction(player, targetActions[dist(gen)]);
            }
            else
            {
                executeSpecialAction(player);
            }
        }
        catch (const GameException &e)
        {
            // fallback אחרון
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

    void increaseAggression() // goal: make the game more aggressive
    {
        coupProbability = min(0.9f, coupProbability + 0.2f);
        if (verboseMode)
        {
            cout << ">>> Increasing aggression! Coup probability raised to " << (coupProbability * 100) << "% <<<" << endl;
        }
    }

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

                // מעקב אחר מספר התורות לכל שחקן
                playerTurns[currentPlayerName]++;

                // בדיקה אם אותו שחקן מקבל תורות רבים ברצף
                if (currentPlayerName == lastPlayer)
                {
                    samePlayerCount++;
                    if (samePlayerCount >= 15 && game.players().size() > 1)
                    {
                        cout << "\n⚠️ Detected stalemate: " << currentPlayerName << " played "
                             << samePlayerCount << " consecutive turns!" << endl;

                        // מצא את השחקן עם הכי הרבה מטבעות
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

                        // הסר את כל השחקנים חוץ מהמנצח
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

                // הגבר אגרסיביות כל 25 תורות
                if ((currentTurn + 1) % 25 == 0)
                {
                    increaseAggression();
                }

                // השהיה קצרה לקריאה
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

    void setVerbose(bool verbose)
    {
        verboseMode = verbose;
    }
};

void runDemoGame()
{
    cout << "=== Game Manual Demo ===" << endl;

    Game demoGame;
    auto governor = demoGame.createPlayer("Moses", Role::GOVERNOR);
    auto spy = demoGame.createPlayer("Joseph", Role::SPY);
    auto baron = demoGame.createPlayer("Michael", Role::BARON);
    auto general = demoGame.createPlayer("Roy", Role::GENERAL);
    auto judge = demoGame.createPlayer("Gilad", Role::JUDGE);

    try
    {
        // Short demo
        cout << "Players: ";
        for (const auto &name : demoGame.players())
        {
            cout << name << " ";
        }
        cout << endl;

        governor->gather();
        spy->gather();
        baron->gather();
        general->gather();
        judge->gather();

        cout << "After gather round:" << endl;
        cout << "Moses: " << governor->coins() << " coins" << endl;
        cout << "Joseph: " << spy->coins() << " coins" << endl;

        governor->tax(); // Governor gets 3
        cout << "Moses after tax: " << governor->coins() << " coins" << endl;
    }
    catch (const exception &e)
    {
        cout << "Demo error: " << e.what() << endl;
    }
}

// void runMultipleRandomGames(int numGames)
// {
//     cout << "\n=== Running " << numGames << " random games ===" << endl;

//     map<string, int> wins;
//     int successfulGames = 0;

//     for (int i = 0; i < numGames; i++)
//     {
//         cout << "\n--- Game " << (i + 1) << " ---" << endl;

//         Game game;
//         auto general = game.createPlayer("Dan", Role::GENERAL);
//         auto merchant = game.createPlayer("Ron", Role::MERCHANT);
//         auto governor = game.createPlayer("Liat", Role::GOVERNOR);
//         auto spy = game.createPlayer("Noa", Role::SPY);
//         auto baron = game.createPlayer("Amit", Role::BARON);
//         auto judge = game.createPlayer("Michal", Role::JUDGE);

//         vector<shared_ptr<Player>> &players = game.getPlayers();

//         GameSimulator simulator(game, players, false); // Without verbose in multiple games

//         if (simulator.runRandomGame())
//         {
//             successfulGames++;
//             string winner = game.winner();
//             wins[winner]++;
//             cout << "Winner: " << winner << endl;
//         }
//     }

//     cout << "\n=== Summary of " << numGames << " games ===" << endl;
//     cout << "Games completed: " << successfulGames << "/" << numGames << endl;
//     cout << "Wins by player:" << endl;
//     for (const auto &pair : wins)
//     {
//         cout << pair.first << ": " << pair.second << " wins" << endl;
//     }
// }

int main()
{
    try
    {
        // Short manual demo
        runDemoGame();

        cout << "\n"
             << string(50, '=') << endl;

        // One detailed random game
        Game game;
        auto general = game.createPlayer("Dan", Role::GENERAL);
        auto merchant = game.createPlayer("Ron", Role::MERCHANT);
        auto governor = game.createPlayer("Liat", Role::GOVERNOR);
        auto spy = game.createPlayer("Noa", Role::SPY);
        auto baron = game.createPlayer("Amit", Role::BARON);
        auto judge = game.createPlayer("Michal", Role::JUDGE);

        vector<shared_ptr<Player>> &players = game.getPlayers();

        GameSimulator simulator(game, players, true); // With verbose

        simulator.runRandomGame();

        cout << "\n"
             << string(50, '=') << endl;

        // // Multiple games
        // char choice;
        // cout << "Run 10 additional games? (y/n): ";
        // cin >> choice;

        // if (choice == 'y' || choice == 'Y')
        // {
        //     runMultipleRandomGames(10);
        // }
    }
    catch (const exception &e)
    {
        cerr << "General error: " << e.what() << endl;
        return 1;
    }

    return 0;
}