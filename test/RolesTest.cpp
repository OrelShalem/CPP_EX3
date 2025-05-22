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

// TEST_CASE("Roles - Basic Initialization")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
//     auto judge = game.createPlayer("Judge1", Role::JUDGE);
//     auto general = game.createPlayer("General1", Role::GENERAL);
//     auto baron = game.createPlayer("Baron1", Role::BARON);
//     auto spy = game.createPlayer("Spy1", Role::SPY);

//     // Check role names
//     CHECK(governor->role() == Role::GOVERNOR);
//     CHECK(merchant->role() == Role::MERCHANT);
//     CHECK(judge->role() == Role::JUDGE);
//     CHECK(general->role() == Role::GENERAL);
//     CHECK(baron->role() == Role::BARON);
//     CHECK(spy->role() == Role::SPY);
// }

// TEST_CASE("Roles - Basic Actions")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

//     // Governor tax
//     governor->tax();
//     CHECK(governor->coins() == 3);
//     CHECK(governor->get_last_action() == "tax");

//     // Merchant gather
//     merchant->gather();
//     CHECK(merchant->get_last_action() == "gather");
// }

// TEST_CASE("Roles - Turn Order")
// {
//     Game game;
//     auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
//     auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
//     auto judge = game.createPlayer("Judge1", Role::JUDGE);

//     // Check initial turn
//     CHECK(game.turn() == "Governor1");

//     // First round
//     governor->gather();
//     CHECK(game.turn() == "Merchant1");

//     merchant->gather();
//     CHECK(game.turn() == "Judge1");

//     judge->gather();
//     CHECK(game.turn() == "Governor1");
// }
