#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../src/Game.hpp"
#include "../src/Player.hpp"
#include "../src/Roles/Governor.hpp"
#include "../src/Roles/Merchant.hpp"
#include "../src/Roles/Judge.hpp"
#include "../src/Roles/General.hpp"
#include "../src/Roles/Baron.hpp"
#include "../src/Roles/Spy.hpp"
#include "../src/GameExceptions.hpp"
#include <algorithm>

using namespace coup;
using namespace std;

TEST_CASE("Edge Case: Maximum number of players (6 players)")
{
    // Test adding maximum allowed players (6)
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");
    auto general = game.createPlayer<General>("General1");
    auto baron = game.createPlayer<Baron>("Baron1");
    auto spy = game.createPlayer<Spy>("Spy1");

    // First turn should be Governor1
    CHECK(game.turn() == "Governor1");

    // Test all players are in the game
    vector<string> players = game.players();
    CHECK(players.size() == 6);
    CHECK(find(players.begin(), players.end(), "Governor1") != players.end());
    CHECK(find(players.begin(), players.end(), "Merchant1") != players.end());
    CHECK(find(players.begin(), players.end(), "Judge1") != players.end());
    CHECK(find(players.begin(), players.end(), "General1") != players.end());
    CHECK(find(players.begin(), players.end(), "Baron1") != players.end());
    CHECK(find(players.begin(), players.end(), "Spy1") != players.end());
}

TEST_CASE("Edge Case: Adding player after game started")
{
    // Test that we can't add players after the game has started
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");

    // Start the game by making a move
    governor->gather();

    // Trying to add a player now should throw an exception
    CHECK_THROWS_AS(game.createPlayer<Merchant>("Merchant1"), runtime_error);
}

TEST_CASE("Edge Case: Minimum number of players (2 players)")
{
    // Test the game with only 2 players (minimum required)
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // First player's turn
    governor->gather();
    CHECK(governor->coins() == 1);

    // Second player's turn
    merchant->gather();
    CHECK(merchant->coins() == 1);

    // First player again
    governor->gather();
    CHECK(governor->coins() == 2);
}

TEST_CASE("Edge Case: Forced coup when above coin limit")
{
    // In this test, we check the coup operation works correctly
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Give governor 10+ coins
    governor->tax(); // +3
    merchant->gather();

    governor->tax(); // +3 = 6
    merchant->gather();

    governor->tax(); // +3 = 9
    merchant->gather();

    // Now coup should work
    governor->coup(*merchant);

    // Governor should have 2 coins left (9-7=2)
    CHECK(governor->coins() == 2);

    // Merchant should be inactive
    CHECK_FALSE(merchant->isActive());

    // Governor should win
    CHECK(game.winner() == "Governor1");
}

TEST_CASE("Edge Case: Last player standing wins")
{
    // Test that the winner is correctly determined
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Governor gets enough coins
    governor->tax(); // +3
    merchant->gather();

    governor->tax(); // +3 = 6
    merchant->gather();

    governor->gather(); // +1 = 7
    merchant->gather();

    // Governor coups merchant
    governor->coup(*merchant);

    // Governor should be the winner as last player
    CHECK(game.winner() == "Governor1");
}

TEST_CASE("Edge Case: Trying to play after being couped")
{
    // Test that a player can't act after being couped
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");

    // Governor gets enough coins
    governor->tax(); // +3
    merchant->gather();
    judge->gather();

    governor->tax(); // +3 = 6
    merchant->gather();
    judge->gather();

    governor->gather(); // +1 = 7
    merchant->gather();
    judge->gather();

    // Governor coups merchant
    governor->coup(*merchant);

    // Merchant tries to act after being couped
    CHECK_THROWS(merchant->gather());
}

TEST_CASE("Edge Case: Acting out of turn")
{
    // Test that a player can't act out of turn
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // It's Governor's turn first
    CHECK(game.turn() == "Governor1");

    // Merchant tries to play out of turn
    CHECK_THROWS_AS(merchant->gather(), coup::InvalidTurn);

    // Governor plays, then it's Merchant's turn
    governor->gather();
    CHECK(game.turn() == "Merchant1");
}

TEST_CASE("Edge Case: Using coup with insufficient coins")
{
    // Test that coup requires at least 7 coins
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Give governor 6 coins (not enough for coup)
    governor->tax(); // +3
    merchant->gather();

    governor->tax(); // +3 = 6
    merchant->gather();

    CHECK(governor->coins() == 6);

    // Governor tries coup with insufficient coins
    CHECK_THROWS_AS(governor->coup(*merchant), coup::NotEnoughCoins);
}
