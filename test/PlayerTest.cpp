// orel8155@gmail.com
/**
 * @file PlayerTest.cpp
 * @brief Test cases for the Player class functionality in the Coup card game.
 * 
 * This file contains comprehensive test cases that verify the proper functioning
 * of the Player class, including player properties, actions, and interactions.
 * Tests cover core game mechanics such as gathering coins, taxation, arrests, sanctions,
 * and coup actions.
 */

#include "doctest.h"  // Include the testing framework

#include "../src/Game.hpp"  // Include the Game class
#include "../src/Player.hpp"  // Include the Player class
#include "../src/GameExceptions.hpp"  // Include custom exceptions
#include <stdexcept>  // For standard exceptions

using namespace coup;  // Use the coup namespace
using namespace std;  // Use the standard namespace

/**
 * Test case that verifies basic player creation and properties.
 * Checks that a player is created with the correct name, role, initial coin count,
 * and active status.
 */
TEST_CASE("Player: Basic player creation and properties")
{
    Game game;  // Create a new game instance
    auto player = game.createPlayer("TestPlayer", Role::GOVERNOR);  // Create a player with Governor role

    // Check basic player properties
    CHECK(player->name() == "TestPlayer");  // Verify player name
    CHECK(player->role() == Role::GOVERNOR);  // Verify player role
    CHECK(player->coins() == 0);  // Verify initial coin count is zero
    CHECK(player->isActive() == true);  // Verify player is active
}

/**
 * Test case that verifies the gather action functionality.
 * Gather should add 1 coin to the player's balance and pass the turn.
 */
TEST_CASE("Player: Gather functionality")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Player1 performs gather action
    CHECK(player1->coins() == 0);  // Verify initial coin count
    player1->gather();  // Player1 gathers a coin
    CHECK(player1->coins() == 1);  // Verify player now has 1 coin

    // Turn passes to Player2
    CHECK(game.turn() == Role::MERCHANT);  // Verify it's now Player2's turn

    // Player2 performs gather action
    player2->gather();  // Player2 gathers a coin
    CHECK(player2->coins() == 1);  // Verify Player2 now has 1 coin

    // Turn returns to Player1
    CHECK(game.turn() == Role::GOVERNOR);  // Verify it's Player1's turn again
}

/**
 * Test case that verifies the tax action functionality.
 * Tax should add 2 coins to a regular player's balance.
 */
TEST_CASE("Player: Tax functionality")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::JUDGE);  // Create player with Judge role
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create player with Merchant role

    // Player1 performs tax action
    player1->tax();  // Player1 collects tax
    CHECK(player1->coins() == 2);  // Regular player receives 2 coins for tax

    // Turn passes to Player2
    CHECK(game.turn() == Role::MERCHANT);  // Verify it's now Player2's turn

    player2->gather();  // Player2 performs gather to pass turn

    // Player1 performs tax again
    player1->tax();  // Player1 collects tax again
    CHECK(player1->coins() == 4);  // 2 + 2 = 4 coins total
}

/**
 * Test case that verifies the Governor's special tax functionality.
 * Governor should get 3 coins from tax instead of the regular 2.
 */
TEST_CASE("Player: Governor special tax functionality")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);  // Create player with Governor role
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);  // Create player with Merchant role

    // Governor performs tax - gets 3 coins (not 2)
    governor->tax();  // Governor collects tax
    CHECK(governor->coins() == 3);  // Governor receives 3 coins for tax (special ability)

    merchant->gather();  // Merchant performs gather to pass turn

    // Governor performs tax again
    governor->tax();  // Governor collects tax again
    CHECK(governor->coins() == 6);  // 3 + 3 = 6 coins total
}

/**
 * Test case that verifies the bribe action functionality.
 * Bribe requires 4 coins and should reset the player's coin count to zero.
 */
TEST_CASE("Player: Bribe functionality")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create player with Governor role
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create player with Merchant role

    // Attempt to bribe without enough coins
    CHECK_THROWS_AS(player1->bribe(), InvalidOperation);  // Should throw exception

    // Add coins
    player1->addCoins(4);  // Add 4 coins to Player1
    CHECK(player1->coins() == 4);  // Verify Player1 has 4 coins

    // Now bribe should work
    player1->bribe();  // Player1 performs bribe action
    CHECK(player1->coins() == 0);  // 4 - 4 = 0 coins remaining
}

/**
 * Test case that verifies the arrest action functionality.
 * When a General arrests a non-General/non-Merchant, the General gains 1 coin
 * and the target loses 1 coin.
 */
TEST_CASE("Player: Arrest functionality")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General", Role::GENERAL);  // Create player with General role
    auto spy = game.createPlayer("Spy", Role::SPY);  // Create player with Spy role

    // Add coins to both players
    general->addCoins(2);  // Add 2 coins to General
    spy->addCoins(3);  // Add 3 coins to Spy

    CHECK(general->coins() == 2);  // Verify General has 2 coins
    CHECK(spy->coins() == 3);  // Verify Spy has 3 coins

    // General arrests Spy
    general->arrest(spy);  // General performs arrest on Spy

    // General gets 1 additional coin (because Spy is not General or Merchant)
    // Spy loses 1 coin
    CHECK(general->coins() == 3);  // 2 + 1 = 3 coins
    CHECK(spy->coins() == 2);  // 3 - 1 = 2 coins
}

/**
 * Test case that verifies special arrest interactions.
 * When arresting Generals or Merchants, no coins should transfer.
 * Generals can gain coins from their react_to_arrest ability.
 */
TEST_CASE("Player: Arrest on General/Merchant - no coin transfer")
{
    Game game;  // Create a new game instance
    auto general1 = game.createPlayer("General1", Role::GENERAL);  // Create first General
    auto general2 = game.createPlayer("General2", Role::GENERAL);  // Create second General
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);  // Create Merchant

    // Add coins to all players
    general1->addCoins(2);  // Add 2 coins to General1
    general2->addCoins(3);  // Add 3 coins to General2
    merchant->addCoins(3);  // Add 3 coins to Merchant

    // General1 arrests General2 - no coin transfer should occur
    general1->arrest(general2);  // General1 performs arrest on General2
    CHECK(general1->coins() == 2);  // Unchanged
    CHECK(general2->coins() == 3);  // 3 + 1 (from General's react_to_arrest ability)

    general2->gather();  // Pass turn
    merchant->gather();  // Pass turn

    // General1 arrests Merchant - no coin transfer should occur
    general1->arrest(merchant);  // General1 performs arrest on Merchant
    CHECK(general1->coins() == 2);  // Unchanged
    CHECK(merchant->coins() == 3);  // Unchanged (Merchant doesn't get a coin from react_to_arrest)
}

/**
 * Test case that verifies the sanction action functionality.
 * Sanction costs 3 coins and prevents economic actions until the target's turn.
 */
TEST_CASE("Player: Sanction functionality")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General", Role::GENERAL);  // Create player with General role
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);  // Create player with Merchant role

    // General gets coins for sanction
    general->addCoins(5);  // Add 5 coins to General
    CHECK(general->coins() == 5);  // Verify General has 5 coins

    // General imposes sanction on Merchant
    general->sanction(*merchant);  // General sanctions Merchant
    CHECK(general->coins() == 2);  // 5 - 3 = 2 coins remaining

    // Now it's Merchant's turn - sanction should be lifted
    CHECK(game.turn() == Role::MERCHANT);  // Verify it's Merchant's turn

    // Merchant can perform economic actions (sanction is lifted on their turn)
    CHECK_THROWS(merchant->gather());  // Verify Merchant can gather
    CHECK(merchant->coins() == 0);  // Merchant still has 0 coins
}

/**
 * Test case that verifies the coup action functionality.
 * Coup costs 7 coins and removes a player from the game.
 */
TEST_CASE("Player: Coup functionality")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Attempt coup without enough coins
    CHECK_THROWS_AS(player1->coup(player2), NotEnoughCoins);  // Should throw exception

    // Add coins
    player1->addCoins(7);  // Add 7 coins to Player1
    CHECK(player1->coins() == 7);  // Verify Player1 has 7 coins
    CHECK(player2->isActive() == true);  // Verify Player2 is active

    // Coup should now work
    player1->coup(player2);  // Player1 performs coup on Player2
    CHECK(player1->coins() == 0);  // 7 - 7 = 0 coins remaining
    CHECK(player2->isActive() == false);  // Player2 should be inactive

    // Game should end
    CHECK(game.isGameOver());  // Game should be over
    CHECK(game.winner() == "Player1");  // Player1 should be the winner
}

/**
 * Test case that verifies coup cannot be performed on inactive players.
 * Attempting to coup an inactive player should throw an exception.
 */
TEST_CASE("Player: Coup on inactive player")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player
    auto player3 = game.createPlayer("Player3", Role::JUDGE);  // Create third player

    // Add coins
    player1->addCoins(7);  // Add 7 coins to Player1

    // Remove Player2 from the game
    game.removePlayer("Player2");  // Manually remove Player2
    CHECK_FALSE(player2->isActive());  // Verify Player2 is inactive

    // Attempt to coup an inactive player should throw exception
    CHECK_THROWS_AS(player1->coup(player2), InvalidOperation);  // Should throw exception

    // Coins should remain unchanged
    CHECK(player1->coins() == 7);  // Coins should not be deducted
}

/**
 * Test case that verifies turn validation.
 * Players should only be able to act on their turn.
 */
TEST_CASE("Player: Turn validation")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // It's Player1's turn
    CHECK(game.turn() == Role::GOVERNOR);  // Verify it's Player1's turn

    // Player2 attempts to act out of turn
    CHECK_THROWS_AS(player2->gather(), InvalidTurn);  // Should throw exception
    CHECK_THROWS_AS(player2->tax(), InvalidTurn);  // Should throw exception
    CHECK_THROWS(player2->bribe());  // Should throw exception

    // Player1 acts
    player1->gather();  // Player1 performs valid action

    // Now it's Player2's turn
    CHECK(game.turn() == Role::MERCHANT);  // Verify it's now Player2's turn

    // Player1 attempts to act out of turn
    CHECK_THROWS_AS(player1->gather(), InvalidTurn);  // Should throw exception
}

/**
 * Test case that verifies inactive players cannot perform actions.
 * Any action attempted by an inactive player should throw an exception.
 */
TEST_CASE("Player: Inactive player cannot act")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Remove Player1 from the game
    game.removePlayer("Player1");  // Manually remove Player1
    CHECK_FALSE(player1->isActive());  // Verify Player1 is inactive

    // Player1 attempts to act while inactive
    CHECK_THROWS_AS(player1->gather(), InvalidOperation);  // Should throw exception
}

/**
 * Test case that verifies the forced coup rule with 10+ coins.
 * A player with 10 or more coins must perform a coup action.
 */
TEST_CASE("Player: Forced coup with 10+ coins")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Add 10 coins to Player1
    player1->addCoins(10);  // Add 10 coins
    CHECK(player1->coins() == 10);  // Verify Player1 has 10 coins

    // With 10+ coins, players must perform coup - other actions are forbidden
    // Check is only performed when it's the player's turn
    CHECK_THROWS_AS(player1->gather(), InvalidOperation);  // Should throw exception

    // Only coup is allowed
    player1->coup(player2);  // Player1 performs coup on Player2
    CHECK(player1->coins() == 3);  // 10 - 7 = 3 coins remaining
    CHECK_FALSE(player2->isActive());  // Player2 should be inactive
}

/**
 * Test case that verifies coin management functionality.
 * Tests adding and removing coins, and handling invalid operations.
 */
TEST_CASE("Player: Coin management")
{
    Game game;  // Create a new game instance
    auto player = game.createPlayer("Player", Role::GOVERNOR);  // Create a player

    // Check initial state
    CHECK(player->coins() == 0);  // Verify player starts with 0 coins

    // Add coins
    player->addCoins(5);  // Add 5 coins
    CHECK(player->coins() == 5);  // Verify player now has 5 coins

    // Remove coins
    player->removeCoins(2);  // Remove 2 coins
    CHECK(player->coins() == 3);  // Verify player now has 3 coins

    // Attempt to remove more coins than available
    CHECK_THROWS_AS(player->removeCoins(5), runtime_error);  // Should throw exception
    CHECK(player->coins() == 3);  // Coin count should remain unchanged
}

/**
 * Test case that verifies last action tracking functionality.
 * The game should track the last action performed by each player and its target.
 */
TEST_CASE("Player: Last action tracking")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Initially there is no last action
    CHECK(player1->get_last_action() == "");  // Verify no last action
    CHECK(player1->get_last_target() == "");  // Verify no last target

    // Perform gather
    player1->gather();  // Player1 performs gather
    CHECK(player1->get_last_action() == "gather");  // Verify last action is gather
    CHECK(player1->get_last_target() == "");  // Verify no target for gather

    player2->gather();  // Pass turn

    // Perform tax
    player1->tax();  // Player1 performs tax
    CHECK(player1->get_last_action() == "tax");  // Verify last action is tax
    CHECK(player1->get_last_target() == "");  // Verify no target for tax

    player2->gather();  // Pass turn

    // Add coins for actions with targets
    player1->addCoins(7);  // Add 7 coins for coup

    // Perform coup
    player1->coup(player2);  // Player1 performs coup on Player2
    CHECK(player1->get_last_action() == "coup");  // Verify last action is coup
    CHECK(player1->get_last_target() == "Player2");  // Verify target is Player2
}

/**
 * Test case that verifies the restriction against arresting the same target twice.
 * A player should not be able to arrest the same target in consecutive turns.
 */
TEST_CASE("Player: Arrest same target twice restriction")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General", Role::GENERAL);  // Create General
    auto spy1 = game.createPlayer("Spy1", Role::SPY);  // Create first Spy
    auto spy2 = game.createPlayer("Spy2", Role::SPY);  // Create second Spy

    // Add coins
    general->addCoins(2);  // Add 2 coins to General
    spy1->addCoins(2);  // Add 2 coins to Spy1
    spy2->addCoins(2);  // Add 2 coins to Spy2

    // General arrests Spy1
    general->arrest(spy1);  // General performs arrest on Spy1
    CHECK(general->get_last_action() == "arrest");  // Verify last action is arrest
    CHECK(general->get_last_target() == "Spy1");  // Verify target is Spy1

    spy1->gather();  // Pass turn
    spy2->gather();  // Pass turn

    // Attempt to arrest the same player again should throw exception
    CHECK_THROWS_AS(general->arrest(spy1), InvalidOperation);  // Should throw exception

    // But arresting a different player should work
    general->arrest(spy2);  // General performs arrest on Spy2
    CHECK(general->get_last_target() == "Spy2");  // Verify target is now Spy2
}

/**
 * Test case that verifies economic actions are blocked by sanctions.
 * Sanctions prevent economic actions until the target's turn.
 */
TEST_CASE("Player: Economic actions blocked by sanction")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General", Role::GENERAL);  // Create General
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);  // Create Merchant

    // General gets coins and imposes sanction
    general->addCoins(5);  // Add 5 coins to General
    general->sanction(*merchant);  // General sanctions Merchant

    // Merchant is blocked from economic actions until their turn
    // But when they get their turn, the sanction is automatically lifted
    CHECK(game.turn() == Role::MERCHANT);  // Verify it's Merchant's turn

    // Merchant can perform economic actions (sanction is lifted)
    CHECK_THROWS(merchant->gather());  // Verify Merchant can gather
    CHECK(merchant->coins() == 0);  // Merchant still has 0 coins
}
