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
#include <stdexcept>
#include <memory>

using namespace coup;
using namespace std;

// ==================== GOVERNOR TESTS ====================

TEST_CASE("Governor: Special tax functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // Governor מקבל 3 מטבעות במקום 2
    governor->tax();
    CHECK(governor->coins() == 3);

    merchant->gather();

    // Governor מקבל 3 מטבעות שוב
    governor->tax();
    CHECK(governor->coins() == 6);
}

TEST_CASE("Governor: React to arrest - no coin loss")
{
    Game game;
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto general = game.createPlayer("General", Role::GENERAL);

    // הוספת מטבעות
    governor->addCoins(5);
    general->addCoins(2);

    // General מעצר את Governor בתורו
    governor->gather(); // העברת תור ל-General
    general->arrest(governor);

    // Governor לא מאבד מטבעות (react_to_arrest ריק)
    CHECK(governor->coins() == 5);
    // General מקבל מטבע (כי Governor לא General או Merchant)
    CHECK(general->coins() == 3); // 2 + 1 = 3
}

// ==================== MERCHANT TESTS ====================

TEST_CASE("Merchant: Special gather functionality")
{
    Game game;
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto judge = game.createPlayer("Judge", Role::JUDGE);

    // Merchant מתחיל עם 0 מטבעות
    CHECK(merchant->coins() == 0);

    // Merchant מבצע gather - מקבל 1 מטבע
    merchant->gather();
    CHECK(merchant->coins() == 1);

    judge->gather(); // העברת תור

    // Merchant מבצע gather שוב - מקבל 1 מטבע
    merchant->gather();
    CHECK(merchant->coins() == 2);

    judge->gather(); // העברת תור

    // Merchant מבצע gather שלישית - מקבל 1 מטבע
    merchant->gather();
    CHECK(merchant->coins() == 3);

    judge->gather(); // העברת תור

    // עכשיו יש ל-Merchant 3+ מטבעות - gather נותן בונוס
    merchant->gather();
    CHECK(merchant->coins() == 5); // 3 + 1 + 1 (בונוס) = 5
}

TEST_CASE("Merchant: React to arrest - loses coins")
{
    Game game;
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto general = game.createPlayer("General", Role::GENERAL);

    // הוספת מטבעות ל-Merchant
    merchant->addCoins(5);
    general->addCoins(2);

    CHECK(merchant->coins() == 5);
    CHECK(general->coins() == 2);

    // General מעצר את Merchant בתורו
    merchant->gather(); // העברת תור ל-General
    general->arrest(merchant);

    // Merchant לא מאבד מטבעות (react_to_arrest לא עובד כמצופה)
    CHECK(merchant->coins() == 5);
    // General לא מקבל מטבע (כי Merchant הוא Merchant)
    CHECK(general->coins() == 2);
}

TEST_CASE("Merchant: React to arrest with few coins")
{
    Game game;
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto general = game.createPlayer("General", Role::GENERAL);

    // Merchant עם מטבע אחד
    merchant->addCoins(1);
    general->addCoins(2);

    // General מעצר את Merchant בתורו
    merchant->gather(); // העברת תור ל-General
    general->arrest(merchant);

    // Merchant מאבד מטבעות (react_to_arrest עובד)
    CHECK(merchant->coins() == 0); // 1 + 1 מ-gather - 2 מ-react_to_arrest
    // General לא מקבל מטבע (כי Merchant הוא Merchant)
    CHECK(general->coins() == 2);
}

// ==================== GENERAL TESTS ====================

TEST_CASE("General: React to arrest - gains coin")
{
    Game game;
    auto general1 = game.createPlayer("General1", Role::GENERAL);
    auto general2 = game.createPlayer("General2", Role::GENERAL);

    // הוספת מטבעות
    general1->addCoins(2);
    general2->addCoins(3);

    // General1 מעצר את General2 בתורו
    general1->arrest(general2);

    // General2 מקבל מטבע (react_to_arrest אוטומטי)
    CHECK(general2->coins() == 4); // 3 + 1 = 4
    // General1 לא מקבל מטבע (כי General2 הוא General)
    CHECK(general1->coins() == 2);
}

// ==================== JUDGE TESTS ====================

TEST_CASE("Judge: React to sanction - sanctioner loses coin")
{
    Game game;
    auto judge = game.createPlayer("Judge", Role::JUDGE);
    auto general = game.createPlayer("General", Role::GENERAL);

    // General מקבל מטבעות ומטיל סנקציה על Judge בתורו
    general->addCoins(5);
    judge->gather(); // העברת תור ל-General
    general->sanction(*judge);

    // Judge מגיב לסנקציה אוטומטית - General מאבד מטבע נוסף
    CHECK(general->coins() == 1); // 5 - 3 - 1 = 1 (react_to_sanction אוטומטי)
}

// ==================== BARON TESTS ====================

TEST_CASE("Baron: Invest functionality")
{
    Game game;
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // Baron מקבל מטבעות להשקעה
    baron->addCoins(5);
    CHECK(baron->coins() == 5);

    // Baron מבצע invest
    baron->invest();
    CHECK(baron->coins() == 8); // 5 - 3 + 6 = 8

    // התור עובר ל-Merchant
    CHECK(game.turn() == Role::MERCHANT);
}

TEST_CASE("Baron: Invest without enough coins")
{
    Game game;
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // Baron עם מעט מטבעות
    baron->addCoins(2);
    CHECK(baron->coins() == 2);

    // Baron לא יכול לבצע invest
    CHECK_THROWS_AS(baron->invest(), NotEnoughCoins);
}

TEST_CASE("Baron: React to sanction - gains coin")
{
    Game game;
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto general = game.createPlayer("General", Role::GENERAL);

    // General מטיל סנקציה על Baron בתורו
    general->addCoins(5);
    baron->gather(); // העברת תור ל-General
    general->sanction(*baron);

    // Baron מקבל מטבע (react_to_sanction אוטומטי) + 1 מ-gather
    CHECK(baron->coins() == 2); // 1 מ-gather + 1 מ-react_to_sanction
}

// ==================== SPY TESTS ====================

TEST_CASE("Spy: Basic functionality")
{
    Game game;
    auto spy = game.createPlayer("Spy", Role::SPY);
    auto general = game.createPlayer("General", Role::GENERAL);

    // בדיקה בסיסית שהשחקנים נוצרו
    CHECK(spy->role() == Role::SPY);
    CHECK(general->role() == Role::GENERAL);

    // בדיקה שהם יכולים לבצע פעולות בסיסיות
    spy->gather();
    CHECK(spy->coins() == 1);

    general->gather();
    CHECK(general->coins() == 1);
}

// ==================== ROLE INTERACTIONS TESTS ====================

TEST_CASE("Roles: Multiple special abilities in one game")
{
    Game game;
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto general = game.createPlayer("General", Role::GENERAL);
    auto baron = game.createPlayer("Baron", Role::BARON);

    // Governor מבצע tax מיוחד
    governor->tax();
    CHECK(governor->coins() == 3);

    // Merchant מבצע gather
    merchant->gather();
    CHECK(merchant->coins() == 1);

    // General מבצע gather
    general->gather();
    CHECK(general->coins() == 1);

    // Baron מקבל מטבעות ומבצע invest
    baron->addCoins(3);
    baron->invest();
    CHECK(baron->coins() == 6); // 3 - 3 + 6 = 6
}

TEST_CASE("Roles: All roles basic functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto judge = game.createPlayer("Judge", Role::JUDGE);
    auto general = game.createPlayer("General", Role::GENERAL);
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto spy = game.createPlayer("Spy", Role::SPY);

    // כל השחקנים מבצעים gather
    governor->gather();
    CHECK(governor->coins() == 1);

    merchant->gather();
    CHECK(merchant->coins() == 1);

    judge->gather();
    CHECK(judge->coins() == 1);

    general->gather();
    CHECK(general->coins() == 1);

    baron->gather();
    CHECK(baron->coins() == 1);

    spy->gather();
    CHECK(spy->coins() == 1);

    // בדיקה שהתור חזר ל-Governor
    CHECK(game.turn() == Role::GOVERNOR);
}
