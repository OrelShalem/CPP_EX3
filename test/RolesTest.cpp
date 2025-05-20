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

TEST_CASE("Role - Governor")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Governor's tax ability")
    {
        // Governor uses tax to get 3 coins
        governor->tax();
        CHECK(governor->coins() == 3);
        CHECK(governor->get_last_action() == "tax");

        merchant->gather();
    }

    SUBCASE("Governor's arrest ability")
    {
        // Governor arrests merchant
        governor->arrest(*merchant);

        // Merchant is blocked and tries to gather
        CHECK_THROWS_AS(merchant->gather(), coup::InvalidOperation);

        // Merchant reacts to arrest
        merchant->react_to_arrest();

        // Now merchant can act
        merchant->gather();
        CHECK(merchant->coins() == 1);
    }
}

TEST_CASE("Role - Merchant")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Merchant's bribe ability")
    {
        governor->gather();

        // Merchant uses bribe to get 2 coins
        merchant->bribe();
        CHECK(merchant->coins() == 2);
        CHECK(merchant->get_last_action() == "bribe");
    }

    SUBCASE("Merchant's sanction ability")
    {
        governor->gather();
        merchant->gather();

        // Merchant sanctions governor
        governor->sanction(*merchant);

        // Governor tries to use economic action
        CHECK_THROWS_AS(governor->tax(), coup::InvalidOperation);

        // Governor can still coup if they have enough coins
        // Give governor enough coins
        for (int i = 0; i < 7; i++)
        {
            // Use a non-economic action
            governor->arrest(*merchant);
            merchant->react_to_arrest();
            merchant->gather();
        }

        // Governor should be able to coup despite sanctions
        governor->coup(*merchant);
        CHECK_FALSE(merchant->isActive());
    }
}

TEST_CASE("Role - Judge")
{
    Game game;
    auto judge = game.createPlayer<Judge>("Judge1");
    auto governor = game.createPlayer<Governor>("Governor1");

    SUBCASE("Judge's ability to react to arrest")
    {
        judge->gather();

        // Governor arrests judge
        governor->arrest(*judge);

        // Judge is arrested but can use react_to_arrest
        judge->react_to_arrest();

        // Judge should be able to act now
        judge->gather();
        CHECK(judge->coins() == 2);
    }

    SUBCASE("Judge's undo ability")
    {
        judge->gather();
        governor->gather();

        // Judge attempts to undo governor's gather
        CHECK(judge->can_undo("gather"));
        judge->undo(*governor);

        // Governor's coins should be back to 0
        CHECK(governor->coins() == 0);
    }
}

TEST_CASE("Role - General")
{
    Game game;
    auto general = game.createPlayer<General>("General1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("General's coup ability (costs less)")
    {
        // General needs only 5 coins for coup
        for (int i = 0; i < 5; i++)
        {
            general->gather();
            merchant->gather();
        }

        CHECK(general->coins() == 5);

        // General coups merchant with only 5 coins
        general->coup(*merchant);

        // Merchant should be inactive
        CHECK_FALSE(merchant->isActive());

        // General should have 0 coins left
        CHECK(general->coins() == 0);
    }
}

TEST_CASE("Role - Baron")
{
    Game game;
    auto baron = game.createPlayer<Baron>("Baron1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Baron's tax ability (gets more coins)")
    {
        // Baron uses tax to get 4 coins (more than governor)
        baron->tax();
        CHECK(baron->coins() == 4);

        merchant->gather();
    }
}

TEST_CASE("Role - Spy")
{
    Game game;
    auto spy = game.createPlayer<Spy>("Spy1");
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Spy's ability to act in other player's turn")
    {
        // Spy goes first
        spy->gather();
        CHECK(spy->coins() == 1);

        // Governor's turn
        governor->gather();

        // Merchant's turn
        merchant->gather();

        // At this point, all players have acted once

        // Spy can act out of turn
        spy->gather();
        CHECK(spy->coins() == 2);

        // Still governor's turn
        CHECK(game.turn() == "Governor1");
    }
}
