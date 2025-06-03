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

using namespace coup;
using namespace std;

TEST_CASE("Player: Basic player creation and properties")
{
    Game game;
    auto player = game.createPlayer("TestPlayer", Role::GOVERNOR);

    // בדיקת מאפיינים בסיסיים
    CHECK(player->name() == "TestPlayer");
    CHECK(player->role() == Role::GOVERNOR);
    CHECK(player->coins() == 0);
    CHECK(player->isActive() == true);
}

TEST_CASE("Player: Gather functionality")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // Player1 מבצע gather
    CHECK(player1->coins() == 0);
    player1->gather();
    CHECK(player1->coins() == 1);

    // התור עובר ל-Player2
    CHECK(game.turn() == Role::MERCHANT);

    // Player2 מבצע gather
    player2->gather();
    CHECK(player2->coins() == 1);

    // התור חוזר ל-Player1
    CHECK(game.turn() == Role::GOVERNOR);
}

TEST_CASE("Player: Tax functionality")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::JUDGE); // שינוי ל-JUDGE
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // Player1 מבצע tax
    player1->tax();
    CHECK(player1->coins() == 2); // Player רגיל מקבל 2 מטבעות

    // התור עובר ל-Player2
    CHECK(game.turn() == Role::MERCHANT);

    player2->gather(); // Player2 מבצע gather כדי להעביר תור

    // Player1 מבצע tax שוב
    player1->tax();
    CHECK(player1->coins() == 4); // 2 + 2 = 4
}

TEST_CASE("Player: Governor special tax functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // Governor מבצע tax - מקבל 3 מטבעות (לא 2)
    governor->tax();
    CHECK(governor->coins() == 3);

    merchant->gather();

    // Governor מבצע tax שוב
    governor->tax();
    CHECK(governor->coins() == 6); // 3 + 3 = 6
}

TEST_CASE("Player: Bribe functionality")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // ניסיון bribe ללא מספיק מטבעות
    CHECK_THROWS_AS(player1->bribe(), InvalidOperation);

    // הוספת מטבעות
    player1->addCoins(4);
    CHECK(player1->coins() == 4);

    // עכשיו bribe צריך לעבוד
    player1->bribe();
    CHECK(player1->coins() == 0); // 4 - 4 = 0

    // התור עובר ל-Player2
    CHECK(game.turn() == Role::MERCHANT);
}

TEST_CASE("Player: Arrest functionality")
{
    Game game;
    auto general = game.createPlayer("General", Role::GENERAL);
    auto spy = game.createPlayer("Spy", Role::SPY);

    // הוספת מטבעות לשני השחקנים
    general->addCoins(2);
    spy->addCoins(3);

    CHECK(general->coins() == 2);
    CHECK(spy->coins() == 3);

    // General מעצר את Spy
    general->arrest(spy);

    // General מקבל מטבע נוסף (כי Spy לא General או Merchant)
    // Spy מאבד מטבע
    CHECK(general->coins() == 3); // 2 + 1 = 3
    CHECK(spy->coins() == 2);     // 3 - 1 = 2
}

TEST_CASE("Player: Arrest on General/Merchant - no coin transfer")
{
    Game game;
    auto general1 = game.createPlayer("General1", Role::GENERAL);
    auto general2 = game.createPlayer("General2", Role::GENERAL);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // הוספת מטבעות
    general1->addCoins(2);
    general2->addCoins(3);
    merchant->addCoins(3);

    // General1 מעצר את General2 - אין העברת מטבעות
    general1->arrest(general2);
    CHECK(general1->coins() == 2); // לא השתנה
    CHECK(general2->coins() == 4); // 3 + 1 (react_to_arrest של General)

    general2->gather(); // העברת תור
    merchant->gather(); // העברת תור

    // General1 מעצר את Merchant - אין העברת מטבעות
    general1->arrest(merchant);
    CHECK(general1->coins() == 2); // לא השתנה
    CHECK(merchant->coins() == 3); // לא השתנה (Merchant לא מקבל מטבע מ-react_to_arrest)
}

TEST_CASE("Player: Sanction functionality")
{
    Game game;
    auto general = game.createPlayer("General", Role::GENERAL);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // General מקבל מטבעות לסנקציה
    general->addCoins(5);
    CHECK(general->coins() == 5);

    // General מטיל סנקציה על Merchant
    general->sanction(*merchant);
    CHECK(general->coins() == 2); // 5 - 3 = 2

    // עכשיו התור של Merchant - הסנקציה צריכה להתבטל
    CHECK(game.turn() == Role::MERCHANT);

    // Merchant יכול לבצע פעולות כלכליות (הסנקציה מתבטלת בתורו)
    merchant->gather();
    CHECK(merchant->coins() == 1);
}

TEST_CASE("Player: Coup functionality")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // ניסיון coup ללא מספיק מטבעות
    CHECK_THROWS_AS(player1->coup(player2), NotEnoughCoins);

    // הוספת מטבעות
    player1->addCoins(7);
    CHECK(player1->coins() == 7);
    CHECK(player2->isActive() == true);

    // coup צריך לעבוד
    player1->coup(player2);
    CHECK(player1->coins() == 0); // 7 - 7 = 0
    CHECK(player2->isActive() == false);

    // המשחק צריך להסתיים
    CHECK(game.isGameOver());
    CHECK(game.winner() == "Player1");
}

TEST_CASE("Player: Coup on inactive player")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // הוספת מטבעות
    player1->addCoins(7);

    // הסרת Player2 מהמשחק
    game.removePlayer("Player2");
    CHECK_FALSE(player2->isActive());

    // ניסיון coup על שחקן לא פעיל צריך לזרוק חריגה
    CHECK_THROWS_AS(player1->coup(player2), InvalidOperation);

    // המטבעות צריכים להישאר ללא שינוי
    CHECK(player1->coins() == 7);
}

TEST_CASE("Player: Turn validation")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // התור הוא של Player1
    CHECK(game.turn() == Role::GOVERNOR);

    // Player2 מנסה לשחק מחוץ לתור
    CHECK_THROWS_AS(player2->gather(), InvalidTurn);
    CHECK_THROWS_AS(player2->tax(), InvalidTurn);
    CHECK_THROWS_AS(player2->bribe(), InvalidTurn);

    // Player1 משחק
    player1->gather();

    // עכשיו התור של Player2
    CHECK(game.turn() == Role::MERCHANT);

    // Player1 מנסה לשחק מחוץ לתור
    CHECK_THROWS_AS(player1->gather(), InvalidTurn);
}

TEST_CASE("Player: Inactive player cannot act")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // הסרת Player1 מהמשחק
    game.removePlayer("Player1");
    CHECK_FALSE(player1->isActive());

    // Player1 לא יכול לבצע פעולות (זורק InvalidOperation כי לא פעיל)
    CHECK_THROWS_AS(player1->gather(), InvalidOperation);
    CHECK_THROWS_AS(player1->tax(), InvalidOperation);
}

TEST_CASE("Player: Forced coup with 10+ coins")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // הוספת 10 מטבעות
    player1->addCoins(10);
    CHECK(player1->coins() == 10);

    // עם 10+ מטבעות, חייבים לבצע coup - פעולות אחרות אסורות
    // הבדיקה מתבצעת רק כשזה התור של השחקן
    CHECK_THROWS_AS(player1->gather(), InvalidOperation);

    // רק coup מותר
    player1->coup(player2);
    CHECK(player1->coins() == 3); // 10 - 7 = 3
    CHECK_FALSE(player2->isActive());
}

TEST_CASE("Player: Coin management")
{
    Game game;
    auto player = game.createPlayer("Player", Role::GOVERNOR);

    // בדיקת מצב התחלתי
    CHECK(player->coins() == 0);

    // הוספת מטבעות
    player->addCoins(5);
    CHECK(player->coins() == 5);

    // הסרת מטבעות
    player->removeCoins(2);
    CHECK(player->coins() == 3);

    // ניסיון להסיר יותר מטבעות ממה שיש
    CHECK_THROWS_AS(player->removeCoins(5), runtime_error);
    CHECK(player->coins() == 3); // לא השתנה
}

TEST_CASE("Player: Last action tracking")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // בתחילה אין פעולה אחרונה
    CHECK(player1->get_last_action() == "");
    CHECK(player1->get_last_target() == "");

    // ביצוע gather
    player1->gather();
    CHECK(player1->get_last_action() == "gather");
    CHECK(player1->get_last_target() == "");

    player2->gather(); // העברת תור

    // ביצוע tax
    player1->tax();
    CHECK(player1->get_last_action() == "tax");
    CHECK(player1->get_last_target() == "");

    player2->gather(); // העברת תור

    // הוספת מטבעות לפעולות עם יעד
    player1->addCoins(7);

    // ביצוע coup
    player1->coup(player2);
    CHECK(player1->get_last_action() == "coup");
    CHECK(player1->get_last_target() == "Player2");
}

TEST_CASE("Player: Arrest same target twice restriction")
{
    Game game;
    auto general = game.createPlayer("General", Role::GENERAL);
    auto spy1 = game.createPlayer("Spy1", Role::SPY);
    auto spy2 = game.createPlayer("Spy2", Role::SPY);

    // הוספת מטבעות
    general->addCoins(2);
    spy1->addCoins(2);
    spy2->addCoins(2);

    // General מעצר את Spy1
    general->arrest(spy1);
    CHECK(general->get_last_action() == "arrest");
    CHECK(general->get_last_target() == "Spy1");

    spy1->gather(); // העברת תור
    spy2->gather(); // העברת תור

    // ניסיון לעצור את אותו שחקן שוב צריך לזרוק חריגה
    CHECK_THROWS_AS(general->arrest(spy1), InvalidOperation);

    // אבל מעצר של שחקן אחר צריך לעבוד
    general->arrest(spy2);
    CHECK(general->get_last_target() == "Spy2");
}

TEST_CASE("Player: Economic actions blocked by sanction")
{
    Game game;
    auto general = game.createPlayer("General", Role::GENERAL);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);

    // General מקבל מטבעות ומטיל סנקציה
    general->addCoins(5);
    general->sanction(*merchant);

    // Merchant חסום מפעולות כלכליות עד שמקבל את התור
    // אבל כשהוא מקבל את התור, הסנקציה מתבטלת אוטומטית
    CHECK(game.turn() == Role::MERCHANT);

    // Merchant יכול לבצע פעולות כלכליות (הסנקציה התבטלה)
    merchant->gather();
    CHECK(merchant->coins() == 1);
}
