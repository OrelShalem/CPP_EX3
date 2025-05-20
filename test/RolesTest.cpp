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

TEST_CASE("Roles - Basic Initialization")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");
    auto general = game.createPlayer<General>("General1");
    auto baron = game.createPlayer<Baron>("Baron1");
    auto spy = game.createPlayer<Spy>("Spy1");

    // Check role names
    CHECK(governor->role() == "Governor");
    CHECK(merchant->role() == "Merchant");
    CHECK(judge->role() == "Judge");
    CHECK(general->role() == "General");
    CHECK(baron->role() == "Baron");
    CHECK(spy->role() == "Spy");
}

TEST_CASE("Roles - Basic Actions")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    // Governor tax
    governor->tax();
    CHECK(governor->coins() == 3);
    CHECK(governor->get_last_action() == "tax");

    // Merchant gather
    merchant->gather();
    CHECK(merchant->get_last_action() == "gather");
}

TEST_CASE("Roles - Turn Order")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");
    auto judge = game.createPlayer<Judge>("Judge1");

    // Check initial turn
    CHECK(game.turn() == "Governor1");

    // First round
    governor->gather();
    CHECK(game.turn() == "Merchant1");

    merchant->gather();
    CHECK(game.turn() == "Judge1");

    judge->gather();
    CHECK(game.turn() == "Governor1");
}
