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

// טסט קטן שבודק רק את הפונקציונליות הבסיסית של שחקנים
TEST_CASE("Integration - Basic Player Setup")
{
    // Test a game with just a few basic actions
    Game game;

    // Create some players
    auto governor = game.createPlayer<Governor>("Governor");
    auto merchant = game.createPlayer<Merchant>("Merchant");
    auto judge = game.createPlayer<Judge>("Judge");

    // Start the game with a few turns
    governor->gather(); // player 1: +1
    merchant->gather(); // player 2: +1
    judge->gather();    // player 3: +1

    // Check each player has the expected coins
    CHECK(governor->coins() == 1);
    CHECK(merchant->coins() == 1);
    CHECK(judge->coins() == 1);

    // Continue with more simple actions
    governor->tax(); // player 1 gets 3 more coins = 4 total
    CHECK(governor->coins() == 4);

    merchant->gather(); // player 2 gets 1 more coin = 2 total
    CHECK(merchant->coins() == 2);

    judge->gather(); // player 3 gets 1 coin = 2 total
    CHECK(judge->coins() == 2);
}

// טסט שבודק רק את הסדר של התורות
TEST_CASE("Integration - Turn Order And Basic Actions")
{
    Game game;

    // Create a few different roles
    auto governor = game.createPlayer<Governor>("Governor");
    auto merchant = game.createPlayer<Merchant>("Merchant");
    auto judge = game.createPlayer<Judge>("Judge");

    // First turn should be Governor's
    CHECK(game.turn() == "Governor");

    // First round of actions
    governor->tax(); // +3 coins
    CHECK(game.turn() == "Merchant");

    merchant->gather(); // +1 coin
    CHECK(game.turn() == "Judge");

    judge->gather(); // +1 coin
    CHECK(game.turn() == "Governor");

    // Second round
    governor->gather(); // +1 coin
    CHECK(game.turn() == "Merchant");

    merchant->gather(); // +1 coin
    CHECK(game.turn() == "Judge");

    judge->gather(); // +1 coin
    CHECK(game.turn() == "Governor");
}

// טסט שבודק פעולות בסיסיות של תפקידים שונים
TEST_CASE("Integration - Basic Role Actions")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor");
    auto baron = game.createPlayer<Baron>("Baron");

    // Governor tax
    governor->tax();
    CHECK(governor->coins() == 3);

    // Baron tax - gives 2 coins
    baron->tax();
    CHECK(baron->coins() == 2);

    // Governor gather
    governor->gather();
    CHECK(governor->coins() == 4);

    // Baron gather
    baron->gather();
    CHECK(baron->coins() == 3);
}

// טסט פשוט לבדיקת coup
TEST_CASE("Integration - Simple Coup")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor");
    auto merchant = game.createPlayer<Merchant>("Merchant");

    // Governor gets enough coins for coup
    governor->tax(); // +3
    merchant->gather();

    governor->tax(); // +3 = 6
    merchant->gather();

    governor->tax(); // +3 = 9
    merchant->gather();

    // Governor coups merchant
    governor->coup(*merchant);

    // Check merchant is inactive
    CHECK_FALSE(merchant->isActive());

    // Check governor has 2 coins left after coup
    CHECK(governor->coins() == 2);

    // Governor wins as the last player
    CHECK(game.winner() == "Governor");
}