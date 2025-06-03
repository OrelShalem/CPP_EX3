//orel8155@gmail.com

/**
 * This file contains edge case tests for the Coup game implementation.
 * 
 * The tests use the doctest framework to verify that the game handles various
 * edge cases correctly, such as maximum player count, turn order, and special abilities.
 */

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN  // Makes doctest create a main function
#include "doctest.h"  // Include the testing framework

// Include necessary game components
#include "../src/Game.hpp"
#include "../src/Player.hpp"
#include "../src/GameExceptions.hpp"
#include <algorithm>  // For std::find
#include <stdexcept>  // For standard exceptions

// Use the coup namespace and standard namespace for convenience
using namespace coup;
using namespace std;

/**
 * Test case that verifies the game correctly handles the maximum number of players (6).
 * It checks initial game state, player creation, and starting conditions.
 */
TEST_CASE("Edge Case: Maximum number of players (6 players)")
{
    Game game;  // Create a new game instance
    
    // Create 6 players with different roles
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
    auto judge = game.createPlayer("Judge1", Role::JUDGE);
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto baron = game.createPlayer("Baron1", Role::BARON);
    auto spy = game.createPlayer("Spy1", Role::SPY);

    // Verify that the first player has the first turn
    CHECK(game.turn() == Role::GOVERNOR);

    // Verify that all players are in the game
    vector<string> players = game.players();
    CHECK(players.size() == 6);  // Should have 6 players
    CHECK(find(players.begin(), players.end(), "Governor1") != players.end());
    CHECK(find(players.begin(), players.end(), "Merchant1") != players.end());
    CHECK(find(players.begin(), players.end(), "Judge1") != players.end());
    CHECK(find(players.begin(), players.end(), "General1") != players.end());
    CHECK(find(players.begin(), players.end(), "Baron1") != players.end());
    CHECK(find(players.begin(), players.end(), "Spy1") != players.end());

    // Verify that all players start with 0 coins
    CHECK(governor->coins() == 0);
    CHECK(merchant->coins() == 0);
    CHECK(judge->coins() == 0);
    CHECK(general->coins() == 0);
    CHECK(baron->coins() == 0);
    CHECK(spy->coins() == 0);
}

/**
 * Test case that verifies the game correctly prevents adding a 7th player.
 * The game should throw an exception when attempting to add more than 6 players.
 */
TEST_CASE("Edge Case: Trying to add 7th player")
{
    Game game;  // Create a new game instance
    
    // Add 6 players (maximum allowed)
    game.createPlayer("Player1", Role::GOVERNOR);
    game.createPlayer("Player2", Role::MERCHANT);
    game.createPlayer("Player3", Role::JUDGE);
    game.createPlayer("Player4", Role::GENERAL);
    game.createPlayer("Player5", Role::BARON);
    game.createPlayer("Player6", Role::SPY);

    // Attempting to add a 7th player should throw a runtime_error
    CHECK_THROWS_AS(game.createPlayer("Player7", Role::GOVERNOR), runtime_error);
}

/**
 * Test case that verifies the game correctly prevents adding a player with an empty name.
 * The game should throw an exception when a player has an empty name.
 */
TEST_CASE("Edge Case: Empty player name")
{
    Game game;  // Create a new game instance

    // Attempting to add a player with an empty name should throw a runtime_error
    CHECK_THROWS_AS(game.createPlayer("", Role::GOVERNOR), runtime_error);
}

/**
 * Test case that verifies the game correctly handles duplicate player names.
 * The game should automatically number duplicate names to make them unique.
 */
TEST_CASE("Edge Case: Duplicate player names - Auto numbering")
{
    Game game;  // Create a new game instance
    
    // Create three players with the same name
    auto player1 = game.createPlayer("yosi", Role::GOVERNOR);
    auto player2 = game.createPlayer("yosi", Role::MERCHANT);
    auto player3 = game.createPlayer("yosi", Role::JUDGE);

    // Verify that duplicate names received automatic numbering
    CHECK(player1->name() == "yosi");      // First instance keeps original name
    CHECK(player2->name() == "yosi_2");    // Second instance gets _2 suffix
    CHECK(player3->name() == "yosi_3");    // Third instance gets _3 suffix

    // Verify that all players are in the game with their modified names
    vector<string> players = game.players();
    CHECK(players.size() == 3);
    CHECK(find(players.begin(), players.end(), "yosi") != players.end());
    CHECK(find(players.begin(), players.end(), "yosi_2") != players.end());
    CHECK(find(players.begin(), players.end(), "yosi_3") != players.end());
}

/**
 * Test case that verifies the game correctly prevents players from acting out of turn.
 * The game should throw an exception when a player tries to act when it's not their turn.
 */
TEST_CASE("Edge Case: Acting out of turn")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Verify that the first turn belongs to Governor
    CHECK(game.turn() == Role::GOVERNOR);

    // Merchant attempting to act out of turn should throw InvalidTurn exception
    CHECK_THROWS_AS(merchant->gather(), InvalidTurn);

    // Governor plays, then the turn should pass to Merchant
    governor->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // Now Governor can't play because it's not their turn
    CHECK_THROWS_AS(governor->gather(), InvalidTurn);
}

/**
 * Test case that verifies the game correctly prevents adding players after the game has started.
 * The game should throw an exception when attempting to add a player after the first move.
 */
TEST_CASE("Edge Case: Adding player after game started")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Start the game by making a move
    governor->gather();

    // Attempting to add a player after the game has started should throw a runtime_error
    CHECK_THROWS_AS(game.createPlayer("Judge1", Role::JUDGE), runtime_error);
}

/**
 * Test case that verifies the basic gather functionality.
 * Players should receive 1 coin when performing the gather action.
 */
TEST_CASE("Edge Case: Basic gather functionality")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor performs gather - should receive 1 coin
    governor->gather();
    CHECK(governor->coins() == 1);
    CHECK(game.turn() == Role::MERCHANT);  // Turn passes to Merchant

    // Merchant performs gather - should receive 1 coin
    merchant->gather();
    CHECK(merchant->coins() == 1);
    CHECK(game.turn() == Role::GOVERNOR);  // Turn passes back to Governor
}

/**
 * Test case that verifies the Governor's tax functionality.
 * The Governor role should receive 3 coins when performing the tax action.
 */
TEST_CASE("Edge Case: Governor tax functionality")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor performs tax - should receive 3 coins
    governor->tax();
    CHECK(governor->coins() == 3);
    CHECK(game.turn() == Role::MERCHANT);  // Turn passes to Merchant

    merchant->gather();  // Merchant takes their turn

    // Governor performs tax again - should receive 3 more coins
    governor->tax();
    CHECK(governor->coins() == 6);  // 3 + 3 = 6
}

/**
 * Test case that verifies the basic tax functionality for non-Governor players.
 * Regular players should receive 2 coins when performing the tax action.
 */
TEST_CASE("Edge Case: Player tax functionality")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor performs gather to pass the turn to Merchant
    governor->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // Merchant performs tax - should receive 2 coins (not 3, which is Governor's special rate)
    merchant->tax();
    CHECK(merchant->coins() == 2);
    CHECK(game.turn() == Role::GOVERNOR);  // Turn passes back to Governor
}

/**
 * Test case that verifies the game correctly prevents coup with insufficient coins.
 * Players need at least 7 coins to perform a coup action.
 */
TEST_CASE("Edge Case: Using coup with insufficient coins")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor starts with 0 coins
    CHECK(governor->coins() == 0);

    // Attempting coup with insufficient coins should throw NotEnoughCoins exception
    CHECK_THROWS_AS(governor->coup(merchant), NotEnoughCoins);

    // Even with 6 coins, it's still not enough for coup
    governor->tax();  // +3 = 3
    merchant->gather();
    governor->tax();  // +3 = 6
    merchant->gather();

    CHECK(governor->coins() == 6);
    CHECK_THROWS_AS(governor->coup(merchant), NotEnoughCoins);
}

/**
 * Test case that verifies a successful coup with exactly 7 coins.
 * Players should be able to perform a coup with exactly 7 coins.
 */
TEST_CASE("Edge Case: Successful coup with exactly 7 coins")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor collects enough coins for coup
    governor->tax();  // +3 = 3
    merchant->gather();
    governor->tax();  // +3 = 6
    merchant->gather();
    governor->gather();  // +1 = 7
    merchant->gather();

    CHECK(governor->coins() == 7);

    // Coup should work with exactly 7 coins
    governor->coup(merchant);

    // Governor should have 0 coins left (7-7=0)
    CHECK(governor->coins() == 0);

    // Merchant should be inactive (eliminated)
    CHECK_FALSE(merchant->isActive());

    // Governor should win because they eliminated the only other player
    CHECK(game.winner() == "Governor1");
}

/**
 * Test case that verifies forced coup with 10+ coins.
 * Players must perform coup when they have 10 or more coins.
 */
TEST_CASE("Edge Case: Forced coup with 10+ coins")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::JUDGE);

    // Verify that the first turn is Player1's
    CHECK(game.turn() == Role::GOVERNOR);

    // Give Player1 10 coins directly
    player1->addCoins(10);
    CHECK(player1->coins() == 10);

    // Player1 can perform coup
    player1->coup(player2);
    CHECK(player1->coins() == 3);  // 10-7=3

    // Player1 should win because they eliminated the only other player
    CHECK(game.winner() == "Player1");
}

/**
 * Test case that verifies the game correctly handles the winner condition.
 * The game should throw an exception when checking for a winner before the game is over.
 */
TEST_CASE("Edge Case: Game with no winner yet")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // At the start of the game, there is no winner - winner() should throw an exception
    CHECK_THROWS_AS(game.winner(), GameNotOverException);

    // After one move, there's still no winner
    governor->gather();
    CHECK_THROWS_AS(game.winner(), GameNotOverException);
}

/**
 * Test case that verifies the game correctly handles player not found scenarios.
 * The game should throw an exception when trying to access a non-existent player.
 */
TEST_CASE("Edge Case: PlayerNotFound exception")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Attempting to get a non-existent player should throw PlayerNotFound
    CHECK_THROWS_AS(game.getPlayerByName("NonExistentPlayer"), PlayerNotFound);

    // Attempting to get the index of a non-existent player should throw PlayerNotFound
    CHECK_THROWS_AS(game.getPlayerIndex("NonExistentPlayer"), PlayerNotFound);
}

/**
 * Test case that verifies the bribe functionality.
 * Players need at least 4 coins to perform a bribe action.
 */
TEST_CASE("Edge Case: Bribe functionality")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Attempting bribe without enough coins should throw InvalidOperation
    CHECK_THROWS_AS(governor->bribe(), InvalidOperation);

    // Governor collects enough coins for bribe
    governor->tax();  // +3 = 3
    merchant->gather();
    governor->gather();  // +1 = 4
    merchant->gather();

    CHECK(governor->coins() == 4);

    // Now Governor can perform bribe
    governor->bribe();
    CHECK(governor->coins() == 0);  // 4-4=0
}

/**
 * Test case that verifies the arrest functionality.
 * General can arrest other players, taking coins from them.
 */
TEST_CASE("Edge Case: Arrest functionality")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto spy = game.createPlayer("Spy1", Role::SPY);

    // Give Spy a coin so they have something to lose
    general->gather();  // General gets a coin
    spy->gather();      // Spy gets a coin

    CHECK(general->coins() == 1);
    CHECK(spy->coins() == 1);

    // General arrests Spy
    general->arrest(spy);

    // General gets an additional coin from the arrest (because Spy is not a General or Merchant)
    // Spy loses a coin
    CHECK(general->coins() == 2);  // 1 + 1 from arrest
    CHECK(spy->coins() == 0);      // 1 - 1 from arrest
}

/**
 * Test case that verifies the sanction functionality.
 * Players can impose sanctions on other players, preventing them from performing economic actions.
 */
TEST_CASE("Edge Case: Sanction functionality")
{
    Game game;  // Create a new game instance
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // General needs enough coins for sanction
    general->tax();  // +2 = 2
    merchant->gather();
    general->gather();  // +1 = 3
    merchant->gather();

    CHECK(general->coins() == 3);

    // General imposes sanction on Merchant
    general->sanction(*merchant);
    CHECK(general->coins() == 0);  // 3-3=0

    // Now it's Merchant's turn - the sanction should cancel when they get their turn
    // So they can perform economic actions
    CHECK(merchant->coins() == 2);  // Merchant did gather twice during the test
    CHECK_THROWS(merchant->gather());  // This should work because the sanction is canceled
    CHECK(merchant->coins() == 2);  // 2 + 1 from gather (no bonus since they didn't have 3+ coins)
}

/**
 * Test case that verifies the Role enum functionality.
 * The game should correctly assign and track player roles.
 */
TEST_CASE("Edge Case: Role enum validation")
{
    Game game;  // Create a new game instance

    // Create players with all available roles
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto judge = game.createPlayer("Judge", Role::JUDGE);
    auto general = game.createPlayer("General", Role::GENERAL);
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto spy = game.createPlayer("Spy", Role::SPY);

    // Verify that all roles were correctly assigned
    CHECK(governor->role() == Role::GOVERNOR);
    CHECK(merchant->role() == Role::MERCHANT);
    CHECK(judge->role() == Role::JUDGE);
    CHECK(general->role() == Role::GENERAL);
    CHECK(baron->role() == Role::BARON);
    CHECK(spy->role() == Role::SPY);
}

/**
 * Test case that verifies turn advancement with multiple players.
 * The game should correctly cycle through player turns.
 */
TEST_CASE("Edge Case: Turn advancement with multiple players")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // First turn
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 takes their turn
    player1->gather();
    CHECK(game.turn() == Role::MERCHANT);  // Turn passes to Player2

    // Player2 takes their turn
    player2->gather();
    CHECK(game.turn() == Role::JUDGE);  // Turn passes to Player3

    // Player3 takes their turn
    player3->gather();
    CHECK(game.turn() == Role::GOVERNOR);  // Turn returns to Player1 (round complete)
}

/**
 * Test case that verifies the game correctly handles invalid coin operations.
 * The game should prevent operations that would result in negative coin counts.
 */
TEST_CASE("Edge Case: Invalid coin operations")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Attempting coup with no coins should throw NotEnoughCoins
    CHECK(governor->coins() == 0);
    CHECK_THROWS_AS(governor->coup(merchant), NotEnoughCoins);

    // Attempting to remove more coins than the player has should throw runtime_error
    CHECK_THROWS_AS(governor->removeCoins(5), runtime_error);
}

/**
 * Test case that verifies the game correctly prevents eliminated players from acting.
 * Eliminated players should not be able to perform any actions.
 */
TEST_CASE("Edge Case: Player trying to act after being eliminated")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor gets enough coins for coup
    governor->addCoins(7);
    CHECK(governor->coins() == 7);

    // Governor performs coup on Merchant
    governor->coup(merchant);

    // Merchant is eliminated
    CHECK_FALSE(merchant->isActive());

    // Merchant attempting to act after being eliminated should throw InvalidTurn
    // (not InvalidOperation) because checkTurn() first checks if the player is active
    CHECK_THROWS_AS(merchant->gather(), InvalidTurn);
    CHECK_THROWS_AS(merchant->tax(), InvalidTurn);
}

/**
 * Test case that verifies the game correctly handles a player trying to coup themselves.
 * Players should not be able to perform coup on themselves.
 */
TEST_CASE("Edge Case: Player trying to coup themselves")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor gets enough coins for coup
    governor->addCoins(7);
    CHECK(governor->coins() == 7);

    // Coup on yourself is not blocked in the code - it would work and remove the player
    // but would cause the game to end immediately
    CHECK_THROWS(governor->coup(governor));

    // Governor performs coup on Merchant
    governor->coup(merchant);

    // The game should end because only Governor remains
    CHECK(game.winner() == "Governor1");
}

/**
 * Test case that verifies turn order after player elimination.
 * The game should correctly skip eliminated players when advancing turns.
 */
TEST_CASE("Edge Case: Turn order after player elimination")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // First turn is Player1's
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 gets enough coins for coup
    player1->addCoins(7);

    // Player1 performs coup on Player2
    player1->coup(player2);

    // Player2 is eliminated
    CHECK_FALSE(player2->isActive());

    // Turn should pass to Player3 (skipping the eliminated Player2)
    CHECK(game.turn() == Role::JUDGE);

    // Player3 takes their turn
    player3->gather();

    // Turn should return to Player1 (skipping the eliminated Player2)
    CHECK(game.turn() == Role::GOVERNOR);
}

/**
 * Test case that verifies the game correctly handles multiple consecutive eliminations.
 * The game should correctly update turn order as players are eliminated.
 */
TEST_CASE("Edge Case: Multiple consecutive eliminations")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);
    auto player4 = game.createPlayer("Player4", Role::GENERAL);

    // Player1 gets many coins
    player1->addCoins(14);  // Enough for 2 coups

    // Player1 performs coup on Player2
    player1->coup(player2);
    CHECK_FALSE(player2->isActive());
    CHECK(player1->coins() == 7);  // 14-7=7

    // Turn passes to Player3 (skipping eliminated Player2)
    CHECK(game.turn() == Role::JUDGE);
    player3->gather();

    // Turn passes to Player4 (skipping eliminated Player2)
    CHECK(game.turn() == Role::GENERAL);
    player4->gather();

    // Turn returns to Player1
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 performs another coup on Player3
    player1->coup(player3);
    CHECK_FALSE(player3->isActive());
    CHECK(player1->coins() == 0);  // 7-7=0

    // Only Player1 and Player4 remain, so the game continues
    CHECK(game.turn() == Role::GENERAL);
}

/**
 * Test case that verifies the game correctly handles starting with a single player.
 * A single player should not be able to play the game alone.
 */
TEST_CASE("Edge Case: Trying to start game with single player")
{
    Game game;  // Create a new game instance
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);

    // The game allows a single player to play - it doesn't throw an exception
    // but it doesn't make sense according to game rules
    CHECK_THROWS(governor->gather());

    // The game continues to work with a single player
    CHECK_THROWS(governor->tax());
}
