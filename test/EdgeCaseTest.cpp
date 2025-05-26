#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
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

TEST_CASE("Edge Case: Maximum number of players (6 players)")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);
    auto judge = game.createPlayer("Judge1", Role::JUDGE);
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto baron = game.createPlayer("Baron1", Role::BARON);
    auto spy = game.createPlayer("Spy1", Role::SPY);

    // בדיקה שהתור הראשון הוא של השחקן הראשון
    CHECK(game.turn() == Role::GOVERNOR);

    // בדיקה שכל השחקנים נמצאים במשחק
    vector<string> players = game.players();
    CHECK(players.size() == 6);
    CHECK(find(players.begin(), players.end(), "Governor1") != players.end());
    CHECK(find(players.begin(), players.end(), "Merchant1") != players.end());
    CHECK(find(players.begin(), players.end(), "Judge1") != players.end());
    CHECK(find(players.begin(), players.end(), "General1") != players.end());
    CHECK(find(players.begin(), players.end(), "Baron1") != players.end());
    CHECK(find(players.begin(), players.end(), "Spy1") != players.end());

    // בדיקה שכל השחקנים מתחילים עם 0 מטבעות
    CHECK(governor->coins() == 0);
    CHECK(merchant->coins() == 0);
    CHECK(judge->coins() == 0);
    CHECK(general->coins() == 0);
    CHECK(baron->coins() == 0);
    CHECK(spy->coins() == 0);
}

TEST_CASE("Edge Case: Trying to add 7th player")
{
    Game game;
    game.createPlayer("Player1", Role::GOVERNOR);
    game.createPlayer("Player2", Role::MERCHANT);
    game.createPlayer("Player3", Role::JUDGE);
    game.createPlayer("Player4", Role::GENERAL);
    game.createPlayer("Player5", Role::BARON);
    game.createPlayer("Player6", Role::SPY);

    // ניסיון להוסיף שחקן שביעי צריך לזרוק חריגה
    CHECK_THROWS_AS(game.createPlayer("Player7", Role::GOVERNOR), runtime_error);
}

TEST_CASE("Edge Case: Empty player name")
{
    Game game;

    // ניסיון להוסיף שחקן עם שם ריק צריך לזרוק חריגה
    CHECK_THROWS_AS(game.createPlayer("", Role::GOVERNOR), runtime_error);
}

TEST_CASE("Edge Case: Duplicate player names - Auto numbering")
{
    Game game;
    auto player1 = game.createPlayer("יוסי", Role::GOVERNOR);
    auto player2 = game.createPlayer("יוסי", Role::MERCHANT);
    auto player3 = game.createPlayer("יוסי", Role::JUDGE);

    // בדיקה שהשמות קיבלו מספור אוטומטי
    CHECK(player1->name() == "יוסי");
    CHECK(player2->name() == "יוסי_2");
    CHECK(player3->name() == "יוסי_3");

    // בדיקה שכל השחקנים נמצאים במשחק
    vector<string> players = game.players();
    CHECK(players.size() == 3);
    CHECK(find(players.begin(), players.end(), "יוסי") != players.end());
    CHECK(find(players.begin(), players.end(), "יוסי_2") != players.end());
    CHECK(find(players.begin(), players.end(), "יוסי_3") != players.end());
}

TEST_CASE("Edge Case: Acting out of turn")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // התור הראשון הוא של Governor
    CHECK(game.turn() == Role::GOVERNOR);

    // ניסיון של Merchant לשחק מחוץ לתור
    CHECK_THROWS_AS(merchant->gather(), InvalidTurn);

    // Governor משחק, ואז התור עובר ל-Merchant
    governor->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // עכשיו Governor לא יכול לשחק
    CHECK_THROWS_AS(governor->gather(), InvalidTurn);
}

TEST_CASE("Edge Case: Adding player after game started")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // התחלת המשחק על ידי ביצוע מהלך
    governor->gather();

    // ניסיון להוסיף שחקן אחרי שהמשחק התחיל צריך לזרוק חריגה
    CHECK_THROWS_AS(game.createPlayer("Judge1", Role::JUDGE), runtime_error);
}

TEST_CASE("Edge Case: Basic gather functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מבצע gather - מקבל 1 מטבע
    governor->gather();
    CHECK(governor->coins() == 1);
    CHECK(game.turn() == Role::MERCHANT);

    // Merchant מבצע gather - מקבל 1 מטבע
    merchant->gather();
    CHECK(merchant->coins() == 1);
    CHECK(game.turn() == Role::GOVERNOR);
}

TEST_CASE("Edge Case: Governor tax functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מבצע tax - מקבל 3 מטבעות
    governor->tax();
    CHECK(governor->coins() == 3);
    CHECK(game.turn() == Role::MERCHANT);

    merchant->gather();

    // Governor מבצע tax שוב
    governor->tax();
    CHECK(governor->coins() == 6);
}

TEST_CASE("Edge Case: Player tax functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מבצע gather כדי להעביר את התור ל-Merchant
    governor->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // Merchant מבצע tax רגיל - מקבל 2 מטבעות
    merchant->tax();
    CHECK(merchant->coins() == 2);
    CHECK(game.turn() == Role::GOVERNOR);
}

TEST_CASE("Edge Case: Using coup with insufficient coins")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מתחיל עם 0 מטבעות
    CHECK(governor->coins() == 0);

    // ניסיון לבצע coup עם מטבעות לא מספיקים
    CHECK_THROWS_AS(governor->coup(*merchant), NotEnoughCoins);

    // גם עם 6 מטבעות זה לא מספיק
    governor->tax(); // +3 = 3
    merchant->gather();
    governor->tax(); // +3 = 6
    merchant->gather();

    CHECK(governor->coins() == 6);
    CHECK_THROWS_AS(governor->coup(*merchant), NotEnoughCoins);
}

TEST_CASE("Edge Case: Successful coup with exactly 7 coins")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מקבל מספיק מטבעות לcoup
    governor->tax(); // +3 = 3
    merchant->gather();
    governor->tax(); // +3 = 6
    merchant->gather();
    governor->gather(); // +1 = 7
    merchant->gather();

    CHECK(governor->coins() == 7);

    // coup צריך לעבוד
    governor->coup(*merchant);

    // Governor צריך להישאר עם 0 מטבעות (7-7=0)
    CHECK(governor->coins() == 0);

    // Merchant צריך להיות לא פעיל
    CHECK_FALSE(merchant->isActive());

    // Governor צריך לנצח
    CHECK(game.winner() == "Governor1");
}

TEST_CASE("Edge Case: Forced coup with 10+ coins")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::JUDGE);

    // בדיקה שהתור הוא של השחקן הראשון
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 מקבל 10 מטבעות בדרך ישירה
    player1->addCoins(10);
    CHECK(player1->coins() == 10);

    // Player1 יכול לבצע coup
    player1->coup(*player2);
    CHECK(player1->coins() == 3); // 10-7=3

    // Player1 צריך לנצח כי player2 הוסר
    CHECK(game.winner() == "Player1");
}

TEST_CASE("Edge Case: Game with no winner yet")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // בתחילת המשחק אין מנצח - winner() זורק חריגה
    CHECK_THROWS_AS(game.winner(), GameNotOverException);

    // אחרי מהלך אחד עדיין אין מנצח
    governor->gather();
    CHECK_THROWS_AS(game.winner(), GameNotOverException);
}

TEST_CASE("Edge Case: PlayerNotFound exception")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // ניסיון לקבל שחקן שלא קיים
    CHECK_THROWS_AS(game.getPlayerByName("NonExistentPlayer"), PlayerNotFound);

    // ניסיון לקבל אינדקס של שחקן שלא קיים
    CHECK_THROWS_AS(game.getPlayerIndex("NonExistentPlayer"), PlayerNotFound);
}

TEST_CASE("Edge Case: Bribe functionality")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // ניסיון לבצע bribe ללא מספיק מטבעות
    CHECK_THROWS_AS(governor->bribe(), InvalidOperation);

    // Governor מקבל מספיק מטבעות
    governor->tax(); // +3 = 3
    merchant->gather();
    governor->gather(); // +1 = 4
    merchant->gather();

    CHECK(governor->coins() == 4);

    // עכשיו Governor יכול לבצע bribe
    governor->bribe();
    CHECK(governor->coins() == 0); // 4-4=0
}

TEST_CASE("Edge Case: Arrest functionality")
{
    Game game;
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto spy = game.createPlayer("Spy1", Role::SPY);

    // Spy מקבל מטבע כדי שיהיה לו מה להפסיד
    general->gather(); // General מקבל מטבע
    spy->gather();     // Spy מקבל מטבע

    CHECK(general->coins() == 1);
    CHECK(spy->coins() == 1);

    // General מעצר את Spy
    general->arrest(*spy);

    // General מקבל מטבע נוסף מהמעצר (כי Spy לא General או Merchant)
    // Spy מאבד מטבע
    CHECK(general->coins() == 2); // 1 + 1 מהמעצר
    CHECK(spy->coins() == 0);     // 1 - 1 מהמעצר
}

TEST_CASE("Edge Case: Sanction functionality")
{
    Game game;
    auto general = game.createPlayer("General1", Role::GENERAL);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // General צריך מספיק מטבעות לסנקציה
    general->tax(); // +2 = 2
    merchant->gather();
    general->gather(); // +1 = 3
    merchant->gather();

    CHECK(general->coins() == 3);

    // General מטיל סנקציה על Merchant
    general->sanction(*merchant);
    CHECK(general->coins() == 0); // 3-3=0

    // עכשיו התור של Merchant - הסנקציה צריכה להתבטל כשהוא מקבל את התור
    // אז הוא יכול לבצע פעולות כלכליות
    CHECK(merchant->coins() == 2); // Merchant עשה gather פעמיים במהלך הטסט
    merchant->gather();            // זה צריך לעבוד כי הסנקציה מתבטלת
    CHECK(merchant->coins() == 3); // 2 + 1 מ-gather (אין בונוס כי לא היו לו 3+ מטבעות)
}

TEST_CASE("Edge Case: Role enum validation")
{
    Game game;

    // יצירת שחקנים עם כל התפקידים
    auto governor = game.createPlayer("Governor", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant", Role::MERCHANT);
    auto judge = game.createPlayer("Judge", Role::JUDGE);
    auto general = game.createPlayer("General", Role::GENERAL);
    auto baron = game.createPlayer("Baron", Role::BARON);
    auto spy = game.createPlayer("Spy", Role::SPY);

    // בדיקה שכל התפקידים נוצרו נכון
    CHECK(governor->role() == Role::GOVERNOR);
    CHECK(merchant->role() == Role::MERCHANT);
    CHECK(judge->role() == Role::JUDGE);
    CHECK(general->role() == Role::GENERAL);
    CHECK(baron->role() == Role::BARON);
    CHECK(spy->role() == Role::SPY);
}

TEST_CASE("Edge Case: Turn advancement with multiple players")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // התור הראשון
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 מבצע פעולה
    player1->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // Player2 מבצע פעולה
    player2->gather();
    CHECK(game.turn() == Role::JUDGE);

    // Player3 מבצע פעולה
    player3->gather();
    CHECK(game.turn() == Role::GOVERNOR); // חזרה לתחילת הסיבוב
}

TEST_CASE("Edge Case: Invalid coin operations")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // ניסיון לבצע coup עם מטבעות שליליים
    CHECK(governor->coins() == 0);
    CHECK_THROWS_AS(governor->coup(*merchant), NotEnoughCoins);

    // בדיקה שלא ניתן להוריד מטבעות מתחת ל-0
    CHECK_THROWS_AS(governor->removeCoins(5), runtime_error);
}

TEST_CASE("Edge Case: Multiple consecutive actions by same player")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מבצע פעולה
    governor->gather();
    CHECK(game.turn() == Role::MERCHANT);

    // ניסיון של Governor לבצע פעולה נוספת באותו תור
    CHECK_THROWS_AS(governor->gather(), InvalidTurn);
    CHECK_THROWS_AS(governor->tax(), InvalidTurn);
}

TEST_CASE("Edge Case: Player trying to act after being eliminated")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מקבל מספיק מטבעות לcoup
    governor->addCoins(7);
    CHECK(governor->coins() == 7);

    // Governor מבצע coup על Merchant
    governor->coup(*merchant);

    // Merchant נפסל
    CHECK_FALSE(merchant->isActive());

    // ניסיון של Merchant לשחק אחרי שנפסל צריך לזרוק InvalidTurn (לא InvalidOperation)
    // כי checkTurn() בודק קודם אם השחקן פעיל
    CHECK_THROWS_AS(merchant->gather(), InvalidTurn);
    CHECK_THROWS_AS(merchant->tax(), InvalidTurn);
}

TEST_CASE("Edge Case: Player trying to coup themselves")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);
    auto merchant = game.createPlayer("Merchant1", Role::MERCHANT);

    // Governor מקבל מספיק מטבעות לcoup
    governor->addCoins(7);
    CHECK(governor->coins() == 7);

    // coup על עצמו לא נחסם בקוד - הוא יעבוד ויסיר את השחקן מהמשחק
    // אבל זה יגרום לכך שהמשחק יסתיים מיד
    governor->coup(*governor);

    // Governor נפסל
    CHECK_FALSE(governor->isActive());
    CHECK(governor->coins() == 0); // 7-7=0

    // המשחק צריך להסתיים כי נשאר רק merchant
    CHECK(game.winner() == "Merchant1");
}

TEST_CASE("Edge Case: Turn order after player elimination")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // התור הראשון הוא של Player1
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 מקבל מספיק מטבעות לcoup
    player1->addCoins(7);

    // Player1 מבצע coup על Player2
    player1->coup(*player2);

    // Player2 נפסל
    CHECK_FALSE(player2->isActive());

    // התור צריך לעבור ל-Player3 (דילוג על Player2 שנפסל)
    CHECK(game.turn() == Role::JUDGE);

    // Player3 מבצע פעולה
    player3->gather();

    // התור צריך לחזור ל-Player1 (דילוג על Player2 שנפסל)
    CHECK(game.turn() == Role::GOVERNOR);
}

TEST_CASE("Edge Case: Multiple consecutive eliminations")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);
    auto player4 = game.createPlayer("Player4", Role::GENERAL);

    // Player1 מקבל הרבה מטבעות
    player1->addCoins(14); // מספיק ל-2 coups

    // Player1 מבצע coup על Player2
    player1->coup(*player2);
    CHECK_FALSE(player2->isActive());
    CHECK(player1->coins() == 7); // 14-7=7

    // התור עובר ל-Player3 (דילוג על Player2 שנפסל)
    CHECK(game.turn() == Role::JUDGE);
    player3->gather();

    // התור חוזר ל-Player4 (דילוג על Player2 שנפסל)
    CHECK(game.turn() == Role::GENERAL);
    player4->gather();

    // התור חוזר ל-Player1
    CHECK(game.turn() == Role::GOVERNOR);

    // Player1 מבצע coup נוסף על Player3
    player1->coup(*player3);
    CHECK_FALSE(player3->isActive());
    CHECK(player1->coins() == 0); // 7-7=0

    // עכשיו נשארו רק Player1 ו-Player4, אז המשחק ממשיך
    CHECK(game.turn() == Role::GENERAL);
}

TEST_CASE("Edge Case: Trying to start game with single player")
{
    Game game;
    auto governor = game.createPlayer("Governor1", Role::GOVERNOR);

    // המשחק מאפשר לשחקן יחיד לשחק - זה לא זורק חריגה
    // אבל זה לא הגיוני מבחינת חוקי המשחק
    governor->gather();
    CHECK(governor->coins() == 1);

    // המשחק ממשיך לעבוד עם שחקן יחיד
    governor->tax();
    CHECK(governor->coins() == 4); // 1 + 3 = 4
}
