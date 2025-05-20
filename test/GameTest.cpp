#include "doctest.h"

#include "../src/Game.hpp"
#include "../src/Player.hpp"
#include "../src/Roles/Governor.hpp"
#include "../src/Roles/Merchant.hpp"
#include "../src/Roles/Judge.hpp"
#include "../src/GameExceptions.hpp"
#include <algorithm>

using namespace coup;
using namespace std;

TEST_CASE("Game - Basic Functionality")
{
    // Test basic game functionality
    Game game;

    SUBCASE("Game initialization")
    {
        // Verify the game is initialized correctly
        CHECK_THROWS_AS(game.winner(), coup::GameNotOverException);
        CHECK_FALSE(game.isGameOver());
        CHECK_FALSE(game.isGameStarted());

        // No players initially
        CHECK(game.players().empty());
    }

    SUBCASE("Adding players")
    {
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");

        // Check players were added
        vector<string> players = game.players();
        CHECK(players.size() == 2);
        CHECK(players[0] == "Governor1");
        CHECK(players[1] == "Merchant1");

        // Game should still not be started until a move is made
        CHECK_FALSE(game.isGameStarted());
    }

    SUBCASE("Game start and turns")
    {
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");
        auto judge = game.createPlayer<Judge>("Judge1");

        // First turn should be Governor1
        CHECK(game.turn() == "Governor1");

        // Make a move to start the game
        governor->gather();

        // Game should be started now
        CHECK(game.isGameStarted());

        // Turn should advance to Merchant1
        CHECK(game.turn() == "Merchant1");

        // Merchant makes a move
        merchant->gather();

        // Turn advances to Judge1
        CHECK(game.turn() == "Judge1");

        // Judge makes a move
        judge->gather();

        // Turn cycles back to Governor1
        CHECK(game.turn() == "Governor1");
    }

    SUBCASE("Game over and winner")
    {
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");

        // Get enough coins for coup
        for (int i = 0; i < 7; i++)
        {
            governor->gather();
            merchant->gather();
        }

        // Governor coups merchant
        governor->coup(*merchant);

        // Governor should be the winner as the last player standing
        CHECK(game.isGameOver());
        CHECK(game.winner() == "Governor1");
    }
}

TEST_CASE("Game - Player Interactions")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");

    SUBCASE("Player turn validation")
    {
        // It's Governor's turn
        CHECK(game.turn() == "Governor1");

        // Merchant tries to play out of turn
        CHECK_THROWS_AS(merchant->gather(), coup::InvalidTurn);

        // Judge tries to play out of turn
        CHECK_THROWS_AS(judge->gather(), coup::InvalidTurn);

        // Governor plays, now it's Merchant's turn
        governor->gather();
        CHECK(game.turn() == "Merchant1");
    }

    SUBCASE("Player removal")
    {
        // Give enough coins for coup
        governor->gather();
        merchant->gather();
        judge->gather();
        governor->tax(); // +3 coins
        merchant->gather();
        judge->gather();
        governor->gather(); // +1 coin
        merchant->gather();
        judge->gather();
        governor->gather(); // +1 coin, has 7 now

        // Governor coups merchant
        governor->coup(*merchant);

        // Merchant should be removed from active players
        vector<string> players = game.players();
        CHECK(players.size() == 2);
        CHECK(find(players.begin(), players.end(), "Merchant1") == players.end());

        // Turn should go to the next active player - Judge
        CHECK(game.turn() == "Judge1");
    }
}

TEST_CASE("Game - Player Actions")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Gather action")
    {
        // Initial coins
        CHECK(governor->coins() == 0);

        // Gather adds 1 coin
        governor->gather();
        CHECK(governor->coins() == 1);

        merchant->gather();
        CHECK(merchant->coins() == 1);

        governor->gather();
        CHECK(governor->coins() == 2);
    }

    SUBCASE("Tax action")
    {
        // Governor uses tax ability
        governor->tax(); // +3 coins
        CHECK(governor->coins() == 3);

        merchant->gather();
        CHECK(merchant->coins() == 1);
    }

    SUBCASE("Coup action")
    {
        // Give enough coins for coup
        for (int i = 0; i < 3; i++)
        {
            governor->tax(); // +3 coins each time
            merchant->gather();
        }

        CHECK(governor->coins() >= 7);

        // Governor coups merchant
        governor->coup(*merchant);

        // Merchant should be inactive
        CHECK_FALSE(merchant->isActive());

        // Governor should lose 7 coins
        CHECK(governor->coins() == 2); // 9-7=2
    }
}
