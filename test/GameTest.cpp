#include "doctest.h"

#include "../src/Game.hpp"
#include "../src/Player.hpp"
#include "../src/GameExceptions.hpp"
#include <algorithm>
#include <stdexcept>

using namespace coup;
using namespace std;

TEST_CASE("Game: Basic game creation and initial state")
{
    Game game;

    // בדיקה שהמשחק מתחיל במצב נכון
    CHECK_FALSE(game.isGameStarted());
    CHECK_THROWS_AS(game.turn(), GameNotOverException); // אין שחקנים עדיין
    CHECK_THROWS_AS(game.winner(), GameNotOverException);

    // רשימת שחקנים ריקה
    vector<string> players = game.players();
    CHECK(players.empty());
}

TEST_CASE("Game: Creating players with different roles")
{
    Game game;

    // יצירת שחקן אחד מכל תפקיד
    auto governor = game.createPlayer("Gov", Role::GOVERNOR);
    auto merchant = game.createPlayer("Mer", Role::MERCHANT);
    auto judge = game.createPlayer("Jud", Role::JUDGE);
    auto general = game.createPlayer("Gen", Role::GENERAL);
    auto baron = game.createPlayer("Bar", Role::BARON);
    auto spy = game.createPlayer("Spy", Role::SPY);

    // בדיקה שכל השחקנים נוצרו נכון
    CHECK(governor->role() == Role::GOVERNOR);
    CHECK(merchant->role() == Role::MERCHANT);
    CHECK(judge->role() == Role::JUDGE);
    CHECK(general->role() == Role::GENERAL);
    CHECK(baron->role() == Role::BARON);
    CHECK(spy->role() == Role::SPY);

    // בדיקה שכל השחקנים ברשימה
    vector<string> players = game.players();
    CHECK(players.size() == 6);
    CHECK(find(players.begin(), players.end(), "Gov") != players.end());
    CHECK(find(players.begin(), players.end(), "Mer") != players.end());
    CHECK(find(players.begin(), players.end(), "Jud") != players.end());
    CHECK(find(players.begin(), players.end(), "Gen") != players.end());
    CHECK(find(players.begin(), players.end(), "Bar") != players.end());
    CHECK(find(players.begin(), players.end(), "Spy") != players.end());
}

TEST_CASE("Game: Turn management")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // התור הראשון
    CHECK(game.turn() == Role::GOVERNOR);
    CHECK_FALSE(game.isGameStarted()); // עדיין לא התחיל

    // אחרי מהלך ראשון המשחק מתחיל
    player1->gather();
    CHECK(game.isGameStarted());
    CHECK(game.turn() == Role::MERCHANT);

    // המשך התורות
    player2->gather();
    CHECK(game.turn() == Role::JUDGE);

    player3->gather();
    CHECK(game.turn() == Role::GOVERNOR); // חזרה לתחילה
}

TEST_CASE("Game: Player removal and turn advancement")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);
    auto player3 = game.createPlayer("Player3", Role::JUDGE);

    // הסרת שחקן באמצע
    game.removePlayer("Player2");
    CHECK_FALSE(player2->isActive());

    // בדיקה שהשחקן לא ברשימת השחקנים הפעילים
    vector<string> activePlayers = game.players();
    CHECK(activePlayers.size() == 2);
    CHECK(find(activePlayers.begin(), activePlayers.end(), "Player2") == activePlayers.end());

    // בדיקה שהתורות מדלגים על השחקן שהוסר
    player1->gather();                 // Player1 משחק
    CHECK(game.turn() == Role::JUDGE); // דילוג על Player2, ישר ל-Player3

    player3->gather();                    // Player3 משחק
    CHECK(game.turn() == Role::GOVERNOR); // חזרה ל-Player1
}

TEST_CASE("Game: Game over conditions")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // בתחילה המשחק לא נגמר
    CHECK_FALSE(game.isGameOver());
    CHECK_THROWS_AS(game.winner(), GameNotOverException);

    // אחרי שמתחילים לשחק עדיין לא נגמר
    player1->gather();
    CHECK_FALSE(game.isGameOver());

    // הסרת שחקן אחד - המשחק נגמר
    game.removePlayer("Player2");
    CHECK(game.isGameOver());
    CHECK(game.winner() == "Player1");
}

TEST_CASE("Game: Player lookup functions")
{
    Game game;
    auto player1 = game.createPlayer("Alice", Role::GOVERNOR);
    auto player2 = game.createPlayer("Bob", Role::MERCHANT);

    // getPlayerByName
    auto foundPlayer = game.getPlayerByName("Alice");
    CHECK(foundPlayer->name() == "Alice");
    CHECK(foundPlayer->role() == Role::GOVERNOR);

    foundPlayer = game.getPlayerByName("Bob");
    CHECK(foundPlayer->name() == "Bob");
    CHECK(foundPlayer->role() == Role::MERCHANT);

    // שחקן שלא קיים
    CHECK_THROWS_AS(game.getPlayerByName("Charlie"), PlayerNotFound);

    // getPlayerIndex
    CHECK(game.getPlayerIndex("Alice") == 0);
    CHECK(game.getPlayerIndex("Bob") == 1);
    CHECK_THROWS_AS(game.getPlayerIndex("Charlie"), PlayerNotFound);
}



TEST_CASE("Game: isPlayerActive function")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // בתחילה השחקן הנוכחי פעיל
    CHECK(game.isPlayerActive());

    // אחרי הסרת השחקן שלא בתור
    game.removePlayer("Player2");
    // השחקן הנוכחי עדיין פעיל
    CHECK(game.isPlayerActive());

    // אחרי הסרת השחקן הנוכחי
    game.removePlayer("Player1");
    // אין שחקנים פעילים - המשחק נגמר
    CHECK_FALSE(game.isPlayerActive());
}

TEST_CASE("Game: Error handling - invalid operations")
{
    Game game;

    // הסרת שחקן שלא קיים
    CHECK_THROWS_AS(game.removePlayer("NonExistent"), PlayerNotFound);

    // יצירת שחקן עם שם ריק
    CHECK_THROWS_AS(game.createPlayer("", Role::GOVERNOR), runtime_error);

    // יצירת יותר מ-6 שחקנים
    game.createPlayer("P1", Role::GOVERNOR);
    game.createPlayer("P2", Role::MERCHANT);
    game.createPlayer("P3", Role::JUDGE);
    game.createPlayer("P4", Role::GENERAL);
    game.createPlayer("P5", Role::BARON);
    game.createPlayer("P6", Role::SPY);

    CHECK_THROWS_AS(game.createPlayer("P7", Role::GOVERNOR), runtime_error);
}

TEST_CASE("Game: Adding players after game started")
{
    Game game;
    auto player1 = game.createPlayer("Player1", Role::GOVERNOR);
    auto player2 = game.createPlayer("Player2", Role::MERCHANT);

    // התחלת המשחק
    player1->gather();
    CHECK(game.isGameStarted());

    // ניסיון להוסיף שחקן אחרי שהמשחק התחיל
    CHECK_THROWS_AS(game.createPlayer("Player3", Role::JUDGE), runtime_error);
}

TEST_CASE("Game: Duplicate player names handling")
{
    Game game;

    // יצירת שחקנים עם שמות זהים
    auto player1 = game.createPlayer("John", Role::GOVERNOR);
    auto player2 = game.createPlayer("John", Role::MERCHANT);
    auto player3 = game.createPlayer("John", Role::JUDGE);

    // בדיקה שהשמות קיבלו מספור אוטומטי
    CHECK(player1->name() == "John");
    CHECK(player2->name() == "John_2");
    CHECK(player3->name() == "John_3");

    // בדיקה שכולם ברשימה
    vector<string> players = game.players();
    CHECK(players.size() == 3);
    CHECK(find(players.begin(), players.end(), "John") != players.end());
    CHECK(find(players.begin(), players.end(), "John_2") != players.end());
    CHECK(find(players.begin(), players.end(), "John_3") != players.end());
}

TEST_CASE("Game: Complex scenario - multiple eliminations")
{
    Game game;
    auto p1 = game.createPlayer("P1", Role::GOVERNOR);
    auto p2 = game.createPlayer("P2", Role::MERCHANT);
    auto p3 = game.createPlayer("P3", Role::JUDGE);
    auto p4 = game.createPlayer("P4", Role::GENERAL);

    // הסרת שחקנים ברצף
    game.removePlayer("P2");
    game.removePlayer("P4");

    // בדיקה שנשארו רק 2 שחקנים פעילים
    vector<string> activePlayers = game.players();
    CHECK(activePlayers.size() == 2);
    CHECK(find(activePlayers.begin(), activePlayers.end(), "P1") != activePlayers.end());
    CHECK(find(activePlayers.begin(), activePlayers.end(), "P3") != activePlayers.end());

    // בדיקה שהתורות עובדים נכון
    CHECK(game.turn() == Role::GOVERNOR); // P1
    p1->gather();
    CHECK(game.turn() == Role::JUDGE); // P3 (דילוג על P2 ו-P4)
    p3->gather();
    CHECK(game.turn() == Role::GOVERNOR); // חזרה ל-P1
}
