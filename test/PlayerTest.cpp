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

TEST_CASE("Player - Basic Functions")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Player initialization")
    {
        // Check initial state
        CHECK(governor->name() == "Governor1");
        CHECK(governor->role() == "Governor");
        CHECK(governor->coins() == 0);
        CHECK(governor->isActive());

        CHECK(merchant->name() == "Merchant1");
        CHECK(merchant->role() == "Merchant");
        CHECK(merchant->coins() == 0);
        CHECK(merchant->isActive());
    }

    SUBCASE("Player gather action")
    {
        governor->gather();
        CHECK(governor->coins() == 1);
        CHECK(governor->get_last_action() == "gather");

        merchant->gather();
        CHECK(merchant->coins() == 1);
        CHECK(merchant->get_last_action() == "gather");
    }

    SUBCASE("Player coup action")
    {
        // Give governor enough coins for coup
        for (int i = 0; i < 7; i++)
        {
            governor->gather();
            merchant->gather();
        }

        // Governor coups merchant
        governor->coup(*merchant);

        // Check results
        CHECK_FALSE(merchant->isActive());
        CHECK(governor->coins() == 0); // 7-7=0
        CHECK(governor->get_last_action() == "coup");
        CHECK(governor->get_last_target() == "Merchant1");
    }
}

TEST_CASE("Player - Coins Management")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");

    SUBCASE("Adding coins")
    {
        CHECK(governor->coins() == 0);

        governor->gather(); // +1
        CHECK(governor->coins() == 1);

        governor->tax(); // +3
        CHECK(governor->coins() == 4);
    }

    SUBCASE("Removing coins")
    {
        // Give governor 7 coins
        for (int i = 0; i < 7; i++)
        {
            governor->gather();
        }

        CHECK(governor->coins() == 7);

        // Create a target for coup
        auto target = game.createPlayer<Merchant>("Target");
        target->gather(); // player 2's turn

        // Coup costs 7 coins
        governor->coup(*target);
        CHECK(governor->coins() == 0);
    }

    SUBCASE("Not enough coins")
    {
        // Give governor 6 coins (not enough for coup)
        for (int i = 0; i < 6; i++)
        {
            governor->gather();
        }

        CHECK(governor->coins() == 6);

        // Create a target for coup
        auto target = game.createPlayer<Merchant>("Target");
        target->gather(); // player 2's turn

        // Try to coup with insufficient coins
        CHECK_THROWS_AS(governor->coup(*target), coup::NotEnoughCoins);
    }
}

TEST_CASE("Player - Role Specific Actions")
{
    Game game;

    SUBCASE("Governor tax ability")
    {
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");

        // Governor uses tax to get 3 coins
        governor->tax();
        CHECK(governor->coins() == 3);
        CHECK(governor->get_last_action() == "tax");

        merchant->gather();
    }

    SUBCASE("Merchant bribe ability")
    {
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");

        governor->gather();

        // Merchant uses bribe to get 2 coins
        merchant->bribe();
        CHECK(merchant->coins() == 2);
        CHECK(merchant->get_last_action() == "bribe");
    }

    SUBCASE("Judge arrest ability")
    {
        auto judge = game.createPlayer<Judge>("Judge1");
        auto governor = game.createPlayer<Governor>("Governor1");
        auto merchant = game.createPlayer<Merchant>("Merchant1");

        judge->gather();
        governor->gather();

        // Merchant gets some coins
        merchant->bribe(); // +2 coins
        CHECK(merchant->coins() == 2);

        judge->gather();
        governor->gather();

        // Judge arrests merchant
        merchant->arrest(*judge);

        // Judge should be blocked from actions
        judge->gather();
        governor->gather();

        // Judge should be able to react to arrest
        judge->react_to_arrest();

        // After reacting, Judge should be unblocked
        CHECK(judge->isActive());
    }
}

TEST_CASE("Player - Status Changes")
{
    Game game;
    auto governor = game.createPlayer<Governor>("Governor1");
    auto merchant = game.createPlayer<Merchant>("Merchant1");

    SUBCASE("Deactivating a player")
    {
        CHECK(merchant->isActive());

        // Give governor enough coins for coup
        for (int i = 0; i < 7; i++)
        {
            governor->gather();
            merchant->gather();
        }

        // Governor coups merchant
        governor->coup(*merchant);

        // Merchant should be inactive
        CHECK_FALSE(merchant->isActive());
    }

    SUBCASE("Blocked player actions")
    {
        // Setup a judge who can block
        auto judge = game.createPlayer<Judge>("Judge1");

        // Give everyone a turn
        governor->gather();
        merchant->gather();
        judge->gather();

        // Governor blocks merchant with arrest
        governor->arrest(*merchant);

        // Merchant is blocked and tries to gather
        CHECK_THROWS_AS(merchant->gather(), coup::InvalidOperation);

        // Judge gets a turn
        judge->gather();

        // Governor gets another turn
        governor->gather();

        // Merchant should be able to react to the block
        merchant->react_to_arrest();

        // Now merchant should be able to act again
        merchant->gather();
        CHECK(merchant->coins() == 2); // 1 from earlier + 1 now
    }
}
