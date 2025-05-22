// #include "doctest.h"

// #include "../src/Game.hpp"
// #include "../src/Player.hpp"
// #include "../src/Roles/Governor.hpp"
// #include "../src/Roles/Merchant.hpp"
// #include "../src/Roles/Judge.hpp"
// #include "../src/GameExceptions.hpp"
// #include <algorithm>

// using namespace coup;
// using namespace std;

// TEST_CASE("Game - Basic Functionality")
// {
//     // Test basic game functionality
//     Game game;

//     SUBCASE("Game initialization")
//     {
//         // Verify the game is initialized correctly
//         CHECK_THROWS_AS(game.winner(), coup::GameNotOverException);
//         CHECK_FALSE(game.isGameOver());
//         CHECK_FALSE(game.isGameStarted());

//         // No players initially
//         CHECK(game.players().empty());
//     }

//     SUBCASE("Adding players")
//     {
//         auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//         auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

//         // Check players were added
//         vector<string> players = game.players();
//         CHECK(players.size() == 2);
//         CHECK(players[0] == "Governor1");
//         CHECK(players[1] == "Merchant1");

//         // Game should still not be started until a move is made
//         CHECK_FALSE(game.isGameStarted());
//     }

//     SUBCASE("Game start and turns")
//     {
//         auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//         auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
//         auto judge = game.createPlayer("Judge1", Role::JUDGE);

//         // First turn should be Governor1
//         CHECK(game.turn() == "Governor1");

//         // Make a move to start the game
//         governor->gather();

//         // Game should be started now
//         CHECK(game.isGameStarted());

//         // Turn should advance to Merchant1
//         CHECK(game.turn() == "Merchant1");

//         // Merchant makes a move
//         merchant->gather();

//         // Turn advances to Judge1
//         CHECK(game.turn() == "Judge1");

//         // Judge makes a move
//         judge->gather();

//         // Turn cycles back to Governor1
//         CHECK(game.turn() == "Governor1");
//     }
// }

// TEST_CASE("Game - Player Turn Validation")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
//     auto judge = game.createPlayer("Judge1", Role::JUDGE);

//     // It's Governor's turn
//     CHECK(game.turn() == "Governor1");

//     // Merchant tries to play out of turn
//     CHECK_THROWS_AS(merchant->gather(), coup::InvalidTurn);

//     // Judge tries to play out of turn
//     CHECK_THROWS_AS(judge->gather(), coup::InvalidTurn);

//     // Governor plays, now it's Merchant's turn
//     governor->gather();
//     CHECK(game.turn() == "Merchant1");
// }

// TEST_CASE("Game - Simple Actions")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

//     // Initial coins
//     CHECK(governor->coins() == 0);

//     // Gather adds 1 coin
//     governor->gather();
//     CHECK(governor->coins() == 1);

//     merchant->gather();
//     CHECK(merchant->coins() == 1);

//     governor->gather();
//     CHECK(governor->coins() == 2);
// }
