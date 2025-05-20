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
#include <random>
#include <string>
#include <vector>

using namespace coup;
using namespace std;

TEST_CASE("Integration - Full Game Simulation")
{
    // Test a full game with multiple players and turns
    Game game;

    // Create all role types
    auto governor = game.createPlayer<Governor>("Governor");
    auto merchant = game.createPlayer<Merchant>("Merchant");
    auto judge = game.createPlayer<Judge>("Judge");
    auto general = game.createPlayer<General>("General");
    auto baron = game.createPlayer<Baron>("Baron");
    auto spy = game.createPlayer<Spy>("Spy");

    // Start the game with a few turns
    governor->gather(); // player 1
    merchant->gather(); // player 2
    judge->gather();    // player 3
    general->gather();  // player 4
    baron->gather();    // player 5
    spy->gather();      // player 6

    // Check each player has 1 coin
    CHECK(governor->coins() == 1);
    CHECK(merchant->coins() == 1);
    CHECK(judge->coins() == 1);
    CHECK(general->coins() == 1);
    CHECK(baron->coins() == 1);
    CHECK(spy->coins() == 1);

    // Continue with more complex actions
    governor->tax(); // player 1 gets 3 more coins
    CHECK(governor->coins() == 4);

    merchant->bribe(); // player 2 gets 2 coins
    CHECK(merchant->coins() == 3);

    judge->gather(); // player 3 gets 1 coin
    CHECK(judge->coins() == 2);

    general->gather(); // player 4 gets 1 coin
    CHECK(general->coins() == 2);

    baron->tax(); // player 5 (baron) gets 4 coins with tax
    CHECK(baron->coins() == 5);

    spy->gather(); // player 6 gets 1 coin
    CHECK(spy->coins() == 2);

    // Governor arrests merchant
    governor->arrest(*merchant);

    // Merchant is blocked and can't gather
    CHECK_THROWS_AS(merchant->gather(), coup::InvalidOperation);

    // Merchant reacts to arrest
    merchant->react_to_arrest();

    // Now merchant can act again
    merchant->gather();
    CHECK(merchant->coins() == 4);

    // Judge tries to undo baron's tax
    judge->undo(*baron);

    // Baron should have fewer coins
    CHECK(baron->coins() == 1); // 5 - 4

    // General can coup with only 5 coins
    for (int i = 0; i < 3; i++)
    {
        general->gather();
        baron->gather();
        spy->gather();
        governor->gather();
        merchant->gather();
        judge->gather();
    }

    CHECK(general->coins() == 5);

    // General coups spy
    general->coup(*spy);
    CHECK_FALSE(spy->isActive());

    // Verify spy is removed from active players
    vector<string> active_players = game.players();
    CHECK(find(active_players.begin(), active_players.end(), "Spy") == active_players.end());

    // Continue until only one player remains
    // Baron uses tax to get coins faster
    baron->tax();
    CHECK(baron->coins() == 5);

    // Governor gets more coins
    governor->tax();
    CHECK(governor->coins() == 8);

    // Governor coups baron
    governor->coup(*baron);
    CHECK_FALSE(baron->isActive());

    // Turn goes to merchant
    CHECK(game.turn() == "Merchant");

    // Merchant bribe to get coins faster
    merchant->bribe();
    CHECK(merchant->coins() == 10);

    // Merchant must coup now (has 10 coins)
    merchant->coup(*judge);
    CHECK_FALSE(judge->isActive());

    // Turn goes to general
    CHECK(game.turn() == "General");

    // General uses remaining coins for coup
    general->coup(*governor);
    CHECK_FALSE(governor->isActive());

    // Merchant vs General, merchant goes first
    merchant->gather();
    CHECK(merchant->coins() == 4); // 10 - 7 + 1 = 4

    // General gather
    general->gather();

    // Continue until one has enough for coup
    for (int i = 0; i < 3; i++)
    {
        merchant->gather();
        general->gather();
    }

    // Merchant coups general
    merchant->coup(*general);
    CHECK_FALSE(general->isActive());

    // Merchant should be the winner
    CHECK(game.winner() == "Merchant");
    CHECK(game.isGameOver());
}

TEST_CASE("Integration - Multiple Role Interactions")
{
    Game game;

    // Create a few different roles
    auto governor = game.createPlayer<Governor>("Governor");
    auto merchant = game.createPlayer<Merchant>("Merchant");
    auto judge = game.createPlayer<Judge>("Judge");
    auto general = game.createPlayer<General>("General");

    // First round of actions
    governor->tax();   // +3 coins
    merchant->bribe(); // +2 coins
    judge->gather();   // +1 coin
    general->gather(); // +1 coin

    // Governor tries to arrest judge
    governor->arrest(*judge);

    // Judge is blocked, but can react
    CHECK_THROWS_AS(judge->gather(), coup::InvalidOperation);

    // Judge reacts to arrest
    judge->react_to_arrest();

    // Judge can now act and tries to undo merchant's bribe
    judge->undo(*merchant);

    // Merchant should have 0 coins now
    CHECK(merchant->coins() == 0);

    // Merchant sanctions governor
    merchant->sanction(*governor);

    // Governor is sanctioned and can't gather or tax
    CHECK_THROWS_AS(governor->gather(), coup::InvalidOperation);
    CHECK_THROWS_AS(governor->tax(), coup::InvalidOperation);

    // But governor can still arrest
    governor->arrest(*general);

    // General is blocked
    CHECK_THROWS_AS(general->gather(), coup::InvalidOperation);

    // Spy can act out of turn
    auto spy = game.createPlayer<Spy>("Spy");
    spy->gather();
    spy->gather(); // Spy can act twice

    CHECK(spy->coins() == 2);

    // General reacts to arrest
    general->react_to_arrest();

    // Continue normal turns
    general->gather();
    spy->gather(); // normal turn

    // Give governor enough coins for coup using arrest
    for (int i = 0; i < 7; i++)
    {
        governor->arrest(*spy);
        spy->react_to_arrest();
        merchant->gather();
        judge->gather();
        general->gather();
        spy->gather();
    }

    // Governor can still coup despite sanctions
    governor->coup(*spy);
    CHECK_FALSE(spy->isActive());
}
