// #include "doctest.h"

// #include "../src/Game.hpp"
// #include "../src/Player.hpp"
// #include "../src/Roles/Governor.hpp"
// #include "../src/Roles/Merchant.hpp"
// #include "../src/Roles/Judge.hpp"
// #include "../src/Roles/General.hpp"
// #include "../src/Roles/Baron.hpp"
// #include "../src/Roles/Spy.hpp"
// #include "../src/GameExceptions.hpp"
// #include <algorithm>

// using namespace coup;
// using namespace std;

// TEST_CASE("Player - Initialization")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

//     // Check initial state
//     CHECK(governor->name() == "Governor1");
//     CHECK(governor->role() == Role::GOVERNOR);
//     CHECK(governor->coins() == 0);
//     CHECK(governor->isActive());

//     CHECK(merchant->name() == "Merchant1");
//     CHECK(merchant->role() == Role::MERCHANT);
//     CHECK(merchant->coins() == 0);
//     CHECK(merchant->isActive());
// }

// TEST_CASE("Player - Basic Actions")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

//     // Governor gather
//     governor->gather();
//     CHECK(governor->coins() == 1);
//     CHECK(governor->get_last_action() == "gather");

//     // Merchant gather
//     merchant->gather();
//     CHECK(merchant->coins() == 1);
//     CHECK(merchant->get_last_action() == "gather");

//     // Second round
//     governor->tax();
//     CHECK(governor->coins() == 4); // 1+3=4

//     merchant->gather();
//     CHECK(merchant->coins() == 2); // 1+1=2
// }

// TEST_CASE("Player - Turn Order")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
//     auto judge = game.createPlayer("Judge1", Role::JUDGE);

//     // First turn is Governor's
//     CHECK(game.turn() == "Governor1");

//     // Governor's action
//     governor->gather();

//     // Now it's Merchant's turn
//     CHECK(game.turn() == "Merchant1");

//     // Merchant's action
//     merchant->gather();

//     // Now Judge's turn
//     CHECK(game.turn() == "Judge1");

//     // Judge's action
//     judge->gather();

//     // Back to Governor
//     CHECK(game.turn() == "Governor1");
// }