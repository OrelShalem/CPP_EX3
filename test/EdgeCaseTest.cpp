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

    // Game should be valid
    governor->gather(); // player 1
    merchant->gather(); // player 2
    governor->gather(); // player 1 again

    CHECK(governor->coins() == 2);
    CHECK(merchant->coins() == 1);
}

TEST_CASE("Edge Case: Player with maximum coins (10)")
{
    // Test player behavior when reaching maximum coins (forced coup)
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Give governor 10 coins
    for (int i = 0; i < 5; i++)
    {
        governor->gather(); // player 1
        merchant->gather(); // player 2
    }

    CHECK(governor->coins() == 5);
    CHECK(merchant->coins() == 5);

    // Give governor 5 more coins to reach 10
    governor->tax(); // +3 coins
    merchant->gather();
    governor->gather(); // +1 coin
    merchant->gather();
    governor->gather(); // +1 coin, now has 10

    CHECK(governor->coins() == 10);

    // Now governor must coup, gather should throw exception
    CHECK_THROWS_AS(governor->gather(), coup::InvalidOperation);

    // Governor must perform coup now
    governor->coup(*merchant);

    // Now merchant should be inactive
    CHECK_FALSE(merchant->isActive());

    // Governor should have 7 coins left (10-3)
    CHECK(governor->coins() == 7);
}

TEST_CASE("Edge Case: Last player standing wins")
{
    // Test that the winner is correctly determined
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");

    // Give enough coins to coup
    for (int i = 0; i < 4; i++)
    {
        governor->gather();
        merchant->gather();
        judge->gather();
    }

    // Governor coups merchant
    governor->coup(*merchant);

    // Now winner should not be declared yet
    CHECK_THROWS_AS(game.winner(), coup::GameNotOverException);

    // Judge coups governor
    judge->coup(*governor);

    // Now judge should be the winner
    CHECK(game.winner() == "Judge1");
}

TEST_CASE("Edge Case: Trying to play after being couped")
{
    // Test that a player can't act after being couped
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");

    // Give enough coins to coup
    for (int i = 0; i < 4; i++)
    {
        governor->gather();
        merchant->gather();
        judge->gather();
    }

    // Governor coups merchant
    governor->coup(*merchant);

    // Merchant tries to play but should throw exception
    CHECK_THROWS_AS(merchant->gather(), coup::InvalidOperation);
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
    for (int i = 0; i < 3; i++)
    {
        governor->gather(); // +1
        merchant->gather();
        governor->gather(); // +1
        merchant->gather();
    }

    CHECK(governor->coins() == 6);

    // Governor tries coup with insufficient coins
    CHECK_THROWS_AS(governor->coup(*merchant), coup::NotEnoughCoins);
}

TEST_CASE("Edge Case: Trying to operate on non-existent player")
{
    // Test operations on non-existent players
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Create a player outside the game
    Game anotherGame;
    auto outsidePlayer = anotherGame.createPlayer<Judge>("Judge1");

    // Governor tries to coup a player from another game
    governor->gather(); // get a coin
    merchant->gather();
    governor->gather(); // get a coin
    merchant->gather();
    governor->gather(); // get a coin
    merchant->gather();
    governor->tax(); // get 3 coins

    CHECK(governor->coins() >= 7); // has enough coins for coup

    // Should throw exception when trying to coup player from another game
    CHECK_THROWS_AS(governor->coup(*outsidePlayer), coup::PlayerNotFound);
}
