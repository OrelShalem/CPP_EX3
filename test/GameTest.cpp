//orel8155@gmail.com
/**
 * @file GameTest.cpp
 * @brief Test cases for the Game class functionality in the Coup card game.
 * 
 * This file contains comprehensive test cases that verify the proper functioning
 * of the Game class, including game state management, player creation, turn order,
 * and error handling.
 */

#include "doctest.h"  // Include the testing framework

#include "../src/Game.hpp"  // Include the Game class
#include "../src/Player.hpp"  // Include the Player class
#include "../src/GameExceptions.hpp"  // Include custom exceptions
#include <algorithm>  // For algorithms like std::find
#include <stdexcept>  // For standard exceptions

using namespace coup;  // Use the coup namespace
using namespace std;  // Use the standard namespace

/**
 * Test case that verifies a newly created game is in the correct initial state.
 * A new game should not be started, should have no turns, no winners, and no players.
 */
TEST_CASE("Game: Basic game creation and initial state")
{
    Game game;  // Create a new game instance

    // Check that the game starts in the correct state
    CHECK_FALSE(game.isGameStarted());  // Game should not be started initially
    CHECK_THROWS_AS(game.turn(), GameNotOverException);  // Should throw exception as no players exist yet
    CHECK_THROWS_AS(game.winner(), GameNotOverException);  // Should throw exception as game isn't over

    // Player list should be empty
    vector<string> players = game.players();  // Get the list of players
    CHECK(players.empty());  // Verify the list is empty
}

/**
 * Test case that verifies players of different roles can be created correctly.
 * Each player should have the correct role and be present in the player list.
 */
TEST_CASE("Game: Creating players with different roles")
{
    Game game;  // Create a new game instance

    // Create one player of each role
    auto governor = game.createPlayer("Gov", Role::GOVERNOR);  // Create a Governor
    auto merchant = game.createPlayer("Mer", Role::MERCHANT);  // Create a Merchant
    auto judge = game.createPlayer("Jud", Role::JUDGE);  // Create a Judge
    auto general = game.createPlayer("Gen", Role::GENERAL);  // Create a General
    auto baron = game.createPlayer("Bar", Role::BARON);  // Create a Baron
    auto spy = game.createPlayer("Spy", Role::SPY);  // Create a Spy

    // Verify each player has the correct role
    CHECK(governor->role() == Role::GOVERNOR);  // Governor should have GOVERNOR role
    CHECK(merchant->role() == Role::MERCHANT);  // Merchant should have MERCHANT role
    CHECK(judge->role() == Role::JUDGE);  // Judge should have JUDGE role
    CHECK(general->role() == Role::GENERAL);  // General should have GENERAL role
    CHECK(baron->role() == Role::BARON);  // Baron should have BARON role
    CHECK(spy->role() == Role::SPY);  // Spy should have SPY role

    // Verify all players are in the player list
    vector<string> players = game.players();  // Get the list of players
    CHECK(players.size() == 6);  // Should have 6 players
    CHECK(find(players.begin(), players.end(), "Gov") != players.end());  // "Gov" should be in the list
    CHECK(find(players.begin(), players.end(), "Mer") != players.end());  // "Mer" should be in the list
    CHECK(find(players.begin(), players.end(), "Jud") != players.end());  // "Jud" should be in the list
    CHECK(find(players.begin(), players.end(), "Gen") != players.end());  // "Gen" should be in the list
    CHECK(find(players.begin(), players.end(), "Bar") != players.end());  // "Bar" should be in the list
    CHECK(find(players.begin(), players.end(), "Spy") != players.end());  // "Spy" should be in the list
}

/**
 * Test case that verifies turn management works correctly.
 * The game should advance turns in the correct order and start after the first move.
 */
TEST_CASE("Game: Turn management")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player
    auto player3 = game.createPlayer("Player3", Role::JUDGE);  // Create third player

    // First turn
    CHECK(game.turn() == Role::GOVERNOR);  // First player (GOVERNOR) should have the first turn
    CHECK_FALSE(game.isGameStarted());  // Game hasn't started yet

    // After first move, game starts
    player1->gather();  // Player1 performs an action
    CHECK(game.isGameStarted());  // Game should now be started
    CHECK(game.turn() == Role::MERCHANT);  // Turn should move to the second player

    // Continue turns
    player2->gather();  // Player2 performs an action
    CHECK(game.turn() == Role::JUDGE);  // Turn should move to the third player

    player3->gather();  // Player3 performs an action
    CHECK(game.turn() == Role::GOVERNOR);  // Turn should cycle back to the first player
}

/**
 * Test case that verifies player removal and turn advancement work correctly.
 * When a player is removed, they should be marked inactive and skipped in turn order.
 */
TEST_CASE("Game: Player removal and turn advancement")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player
    auto player3 = game.createPlayer("Player3", Role::JUDGE);  // Create third player

    // Remove a player in the middle
    game.removePlayer("Player2");  // Remove Player2
    CHECK_FALSE(player2->isActive());  // Player2 should be marked as inactive

    // Verify the player is not in the active players list
    vector<string> activePlayers = game.players();  // Get the list of active players
    CHECK(activePlayers.size() == 2);  // Should have 2 active players
    CHECK(find(activePlayers.begin(), activePlayers.end(), "Player2") == activePlayers.end());  // "Player2" should not be in the list

    // Verify turns skip the removed player
    player1->gather();  // Player1 takes their turn
    CHECK(game.turn() == Role::JUDGE);  // Turn should skip Player2 and go to Player3

    player3->gather();  // Player3 takes their turn
    CHECK(game.turn() == Role::GOVERNOR);  // Turn should cycle back to Player1
}

/**
 * Test case that verifies game over conditions work correctly.
 * The game should end when only one player remains, who is declared the winner.
 */
TEST_CASE("Game: Game over conditions")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Initially the game is not over
    CHECK_FALSE(game.isGameOver());  // Game should not be over
    CHECK_THROWS_AS(game.winner(), GameNotOverException);  // Should throw exception as game isn't over

    // After starting play, still not over
    player1->gather();  // Player1 performs an action
    CHECK_FALSE(game.isGameOver());  // Game should still not be over

    // After removing one player, game is over
    game.removePlayer("Player2");  // Remove Player2
    CHECK(game.isGameOver());  // Game should be over
    CHECK(game.winner() == "Player1");  // Player1 should be the winner
}

/**
 * Test case that verifies player lookup functions work correctly.
 * The game should be able to find players by name and determine their index.
 */
TEST_CASE("Game: Player lookup functions")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Alice", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Bob", Role::MERCHANT);  // Create second player

    // Test getPlayerByName
    auto foundPlayer = game.getPlayerByName("Alice");  // Look up player by name
    CHECK(foundPlayer->name() == "Alice");  // Should find player with name "Alice"
    CHECK(foundPlayer->role() == Role::GOVERNOR);  // Should have GOVERNOR role

    foundPlayer = game.getPlayerByName("Bob");  // Look up another player
    CHECK(foundPlayer->name() == "Bob");  // Should find player with name "Bob"
    CHECK(foundPlayer->role() == Role::MERCHANT);  // Should have MERCHANT role

    // Non-existent player
    CHECK_THROWS_AS(game.getPlayerByName("Charlie"), PlayerNotFound);  // Should throw exception for non-existent player

    // Test getPlayerIndex
    CHECK(game.getPlayerIndex("Alice") == 0);  // "Alice" should be at index 0
    CHECK(game.getPlayerIndex("Bob") == 1);  // "Bob" should be at index 1
    CHECK_THROWS_AS(game.getPlayerIndex("Charlie"), PlayerNotFound);  // Should throw exception for non-existent player
}

/**
 * Test case that verifies the isPlayerActive function works correctly.
 * The function should correctly indicate whether the current player is active.
 */
TEST_CASE("Game: isPlayerActive function")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Initially the current player is active
    CHECK(game.isPlayerActive());  // Current player should be active

    // After removing the player not in turn
    game.removePlayer("Player2");  // Remove Player2
    // Current player should still be active
    CHECK(game.isPlayerActive());  // Current player should still be active

    // After removing the current player
    game.removePlayer("Player1");  // Remove Player1
    // No active players - game is over
    CHECK_FALSE(game.isPlayerActive());  // No players should be active
}

/**
 * Test case that verifies error handling for invalid operations.
 * The game should throw appropriate exceptions for invalid operations.
 */
TEST_CASE("Game: Error handling - invalid operations")
{
    Game game;  // Create a new game instance

    // Removing a non-existent player
    CHECK_THROWS_AS(game.removePlayer("NonExistent"), PlayerNotFound);  // Should throw exception for non-existent player

    // Creating a player with an empty name
    CHECK_THROWS_AS(game.createPlayer("", Role::GOVERNOR), runtime_error);  // Should throw exception for empty name

    // Creating more than 6 players
    game.createPlayer("P1", Role::GOVERNOR);  // Create player 1
    game.createPlayer("P2", Role::MERCHANT);  // Create player 2
    game.createPlayer("P3", Role::JUDGE);  // Create player 3
    game.createPlayer("P4", Role::GENERAL);  // Create player 4
    game.createPlayer("P5", Role::BARON);  // Create player 5
    game.createPlayer("P6", Role::SPY);  // Create player 6

    CHECK_THROWS_AS(game.createPlayer("P7", Role::GOVERNOR), runtime_error);  // Should throw exception for 7th player
}

/**
 * Test case that verifies players cannot be added after the game has started.
 * The game should throw an exception when attempting to add players after the first move.
 */
TEST_CASE("Game: Adding players after game started")
{
    Game game;  // Create a new game instance
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);  // Create first player
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);  // Create second player

    // Start the game
    player1->gather();  // Player1 performs an action
    CHECK(game.isGameStarted());  // Game should be started

    // Attempt to add a player after the game has started
    CHECK_THROWS_AS(game.createPlayer("Player3", Role::JUDGE), runtime_error);  // Should throw exception
}

/**
 * Test case that verifies handling of duplicate player names.
 * The game should automatically number duplicate names to make them unique.
 */
TEST_CASE("Game: Duplicate player names handling")
{
    Game game;  // Create a new game instance

    // Create players with identical names
    auto player1 = game.createPlayer("John", Role::GOVERNOR);  // Create first player named "John"
    auto player2 = game.createPlayer("John", Role::MERCHANT);  // Create second player named "John"
    auto player3 = game.createPlayer("John", Role::JUDGE);  // Create third player named "John"

    // Verify names were automatically numbered
    CHECK(player1->name() == "John");  // First player should keep original name
    CHECK(player2->name() == "John_2");  // Second player should be renamed to "John_2"
    CHECK(player3->name() == "John_3");  // Third player should be renamed to "John_3"

    // Verify all players are in the list
    vector<string> players = game.players();  // Get the list of players
    CHECK(players.size() == 3);  // Should have 3 players
    CHECK(find(players.begin(), players.end(), "John") != players.end());  // "John" should be in the list
    CHECK(find(players.begin(), players.end(), "John_2") != players.end());  // "John_2" should be in the list
    CHECK(find(players.begin(), players.end(), "John_3") != players.end());  // "John_3" should be in the list
}

/**
 * Test case that verifies a complex scenario with multiple player eliminations.
 * The game should correctly maintain turn order when multiple players are removed.
 */
TEST_CASE("Game: Complex scenario - multiple eliminations")
{
    Game game;  // Create a new game instance
    auto p1 = game.createPlayer("P1", Role::GOVERNOR);  // Create first player
    auto p2 = game.createPlayer("P2", Role::MERCHANT);  // Create second player
    auto p3 = game.createPlayer("P3", Role::JUDGE);  // Create third player
    auto p4 = game.createPlayer("P4", Role::GENERAL);  // Create fourth player

    // Remove players in sequence
    game.removePlayer("P2");  // Remove Player2
    game.removePlayer("P4");  // Remove Player4

    // Verify only 2 active players remain
    vector<string> activePlayers = game.players();  // Get the list of active players
    CHECK(activePlayers.size() == 2);  // Should have 2 active players
    CHECK(find(activePlayers.begin(), activePlayers.end(), "P1") != activePlayers.end());  // "P1" should be in the list
    CHECK(find(activePlayers.begin(), activePlayers.end(), "P3") != activePlayers.end());  // "P3" should be in the list

    // Verify turns work correctly
    CHECK(game.turn() == Role::GOVERNOR);  // P1 should have the turn
    p1->gather();  // P1 performs an action
    CHECK(game.turn() == Role::JUDGE);  // Turn should skip P2 and P4, going to P3
    p3->gather();  // P3 performs an action
    CHECK(game.turn() == Role::GOVERNOR);  // Turn should cycle back to P1
}
