// #include "Game.hpp"
// #include "Player.hpp"
// #include "Roles/General.hpp"
// #include "Roles/Merchant.hpp"
// #include "Roles/Governor.hpp"
// #include "Roles/Spy.hpp"
// #include "Roles/Baron.hpp"
// #include "Roles/Judge.hpp"
// #include "GameExceptions.hpp"

// #include <iostream>
// #include <stdexcept>
// #include <vector>
// #include <string>
// #include <memory>

// using namespace coup;
// using namespace std;

// // פונקציה להמרת Role למחרוזת
// string role_to_string(Role role)
// {
//     switch (role)
//     {
//     case Role::GENERAL:
//         return "General";
//     case Role::GOVERNOR:
//         return "Governor";
//     case Role::SPY:
//         return "Spy";
//     case Role::BARON:
//         return "Baron";
//     case Role::JUDGE:
//         return "Judge";
//     case Role::MERCHANT:
//         return "Merchant";
//     default:
//         return "Unknown";
//     }
// }

// // פונקציה לתצוגת מצב המשחק
// void printGameStatus(Game &game)
// {
//     cout << "\n=== מצב המשחק ===" << endl;
//     cout << "תור נוכחי: " << role_to_string(game.turn()) << endl;
//     cout << "שחקנים פעילים: ";
//     for (const auto &playerName : game.players())
//     {
//         auto player = game.getPlayerByName(playerName);
//         cout << playerName << " (" << player->coins() << " מטבעות) ";
//     }
//     cout << "\n================\n"
//          << endl;
// }

// // פונקציה המציגה פעולה
// void printAction(const string &playerName, const string &action, const string &target = "")
// {
//     cout << playerName << " מבצע " << action;
//     if (!target.empty())
//     {
//         cout << " על " << target;
//     }
//     cout << endl;
// }

// // פונקציה המריצה משחק מודרך
// void runGuidedGame()
// {
//     cout << "===== מתחילים משחק coup מודרך =====" << endl;

//     // יצירת משחק
//     Game game;

//     // יצירת שחקנים עם תפקידים שונים
//     auto dan = make_shared<General>(game, "דן");
//     auto yuval = make_shared<Governor>(game, "יובל");
//     auto yarden = make_shared<Spy>(game, "ירדן");
//     auto ronen = make_shared<Baron>(game, "רונן");
//     auto shira = make_shared<Judge>(game, "שירה");
//     auto tomer = make_shared<Merchant>(game, "תומר");

//     cout << "המשחק מתחיל עם שישה שחקנים:" << endl;
//     cout << "דן (General), יובל (Governor), ירדן (Spy), רונן (Baron), שירה (Judge), תומר (Merchant)" << endl;

//     try
//     {
//         // שלב 1: תורות ראשונים - איסוף מטבעות
//         cout << "\n=== שלב 1: איסוף מטבעות ===\n"
//              << endl;

//         cout << "תור 1:" << endl;
//         printAction(dan->name(), "gather");
//         dan->gather();
//         printGameStatus(game);

//         cout << "תור 2:" << endl;
//         printAction(yuval->name(), "gather");
//         yuval->gather();
//         printGameStatus(game);

//         cout << "תור 3:" << endl;
//         printAction(yarden->name(), "gather");
//         yarden->gather();
//         printGameStatus(game);

//         cout << "תור 4:" << endl;
//         printAction(ronen->name(), "gather");
//         ronen->gather();
//         printGameStatus(game);

//         cout << "תור 5:" << endl;
//         printAction(shira->name(), "gather");
//         shira->gather();
//         printGameStatus(game);

//         cout << "תור 6:" << endl;
//         printAction(tomer->name(), "gather");
//         tomer->gather();
//         printGameStatus(game);

//         // שלב 2: שימוש בפעולות מיוחדות
//         cout << "\n=== שלב 2: פעולות מיוחדות ===\n"
//              << endl;

//         cout << "תור 7:" << endl;
//         printAction(dan->name(), "tax");
//         dan->tax(); // הגנרל אוסף מס - 3 מטבעות
//         printGameStatus(game);

//         cout << "תור 8:" << endl;
//         printAction(yuval->name(), "arrest", tomer->name());
//         yuval->arrest(tomer); // המושל מבצע מעצר על תומר
//         printGameStatus(game);

//         cout << "תור 9:" << endl;
//         printAction(yarden->name(), "gather");
//         yarden->gather(); // המרגל אוסף מטבע
//         printGameStatus(game);

//         cout << "תור 10:" << endl;
//         printAction(ronen->name(), "arrest", yuval->name());
//         ronen->arrest(yuval); // הברון מבצע מעצר על יובל
//         printGameStatus(game);

//         cout << "תור 11:" << endl;
//         printAction(shira->name(), "gather");
//         shira->gather(); // השופט אוסף מטבע
//         printGameStatus(game);

//         cout << "תור 12:" << endl;
//         printAction(tomer->name(), "bribe");
//         tomer->bribe(); // הסוחר משחד במחיר 4 מטבעות
//         printGameStatus(game);

//         // שלב 3: סנקציות ואיסוף נוסף
//         cout << "\n=== שלב 3: סנקציות ואיסוף נוסף ===\n"
//              << endl;

//         cout << "תור 13:" << endl;
//         printAction(dan->name(), "gather");
//         dan->gather();
//         printGameStatus(game);

//         cout << "תור 14:" << endl;
//         printAction(yuval->name(), "tax");
//         yuval->tax(); // המושל אוסף מס
//         printGameStatus(game);

//         cout << "תור 15:" << endl;
//         printAction(yarden->name(), "sanction", dan->name());
//         yarden->sanction(*dan); // המרגל מטיל סנקציה על דן
//         printGameStatus(game);

//         cout << "תור 16:" << endl;
//         printAction(ronen->name(), "gather");
//         ronen->gather();
//         printGameStatus(game);

//         cout << "תור 17:" << endl;
//         printAction(shira->name(), "sanction", ronen->name());
//         shira->sanction(*ronen); // השופט מטיל סנקציה על רונן
//         printGameStatus(game);

//         cout << "תור 18:" << endl;
//         printAction(tomer->name(), "gather");
//         tomer->gather();
//         printGameStatus(game);

//         // שלב 4: קו ופעולות סיום
//         cout << "\n=== שלב 4: קו (הדחה) ===\n"
//              << endl;

//         // איסוף מטבעות נוספים לקראת קו
//         cout << "איסוף מטבעות נוספים לקראת קו..." << endl;

//         cout << "תור 19:" << endl;
//         printAction(dan->name(), "tax");
//         dan->tax();
//         printGameStatus(game);

//         cout << "תור 20:" << endl;
//         printAction(yuval->name(), "gather");
//         yuval->gather();
//         printGameStatus(game);

//         cout << "תור 21:" << endl;
//         printAction(yarden->name(), "gather");
//         yarden->gather();
//         printGameStatus(game);

//         cout << "תור 22:" << endl;
//         printAction(ronen->name(), "tax");
//         ronen->tax();
//         printGameStatus(game);

//         cout << "תור 23:" << endl;
//         printAction(shira->name(), "tax");
//         shira->tax();
//         printGameStatus(game);

//         cout << "תור 24:" << endl;
//         printAction(tomer->name(), "tax");
//         tomer->tax();
//         printGameStatus(game);

//         // מספיק מטבעות לקו
//         cout << "\nבשלב זה חלק מהשחקנים צברו מספיק מטבעות להדחה (קו)" << endl;

//         cout << "תור 25:" << endl;
//         printAction(dan->name(), "coup", yarden->name());
//         dan->coup(*yarden); // הגנרל מבצע קו על ירדן
//         printGameStatus(game);

//         cout << "תור 26:" << endl;
//         printAction(yuval->name(), "gather");
//         yuval->gather();
//         printGameStatus(game);

//         cout << "תור 27:" << endl;
//         // ירדן הודח, התור עובר לרונן
//         printAction(ronen->name(), "gather");
//         ronen->gather();
//         printGameStatus(game);

//         cout << "תור 28:" << endl;
//         printAction(shira->name(), "coup", ronen->name());
//         shira->coup(*ronen); // השופט מבצע קו על רונן
//         printGameStatus(game);

//         cout << "תור 29:" << endl;
//         printAction(tomer->name(), "gather");
//         tomer->gather();
//         printGameStatus(game);

//         cout << "תור 30:" << endl;
//         printAction(dan->name(), "gather");
//         dan->gather();
//         printGameStatus(game);

//         cout << "תור 31:" << endl;
//         printAction(yuval->name(), "coup", tomer->name());
//         yuval->coup(*tomer); // המושל מבצע קו על תומר
//         printGameStatus(game);

//         // נותרו רק שניים
//         cout << "\n=== שלב 5: גמר ===\n"
//              << endl;
//         cout << "נותרו רק שני שחקנים: דן ושירה" << endl;

//         cout << "תור 32:" << endl;
//         printAction(dan->name(), "tax");
//         dan->tax();
//         printGameStatus(game);

//         cout << "תור 33:" << endl;
//         printAction(shira->name(), "gather");
//         shira->gather();
//         printGameStatus(game);

//         cout << "תור 34:" << endl;
//         printAction(dan->name(), "gather");
//         dan->gather();
//         printGameStatus(game);

//         cout << "תור 35:" << endl;
//         printAction(shira->name(), "tax");
//         shira->tax();
//         printGameStatus(game);

//         cout << "תור 36:" << endl;
//         printAction(dan->name(), "coup", shira->name());
//         dan->coup(*shira); // הגנרל מבצע קו על השופט
//         printGameStatus(game);

//         // סיום המשחק
//         cout << "\n=== המשחק הסתיים! ===\n"
//              << endl;
//         cout << "המנצח: " << game.winner() << endl;
//     }
//     catch (const GameException &e)
//     {
//         cout << "\nשגיאה במשחק: " << e.what() << endl;
//     }
//     catch (const std::exception &e)
//     {
//         cout << "\nשגיאה כללית: " << e.what() << endl;
//     }
// }

// int main()
// {
//     runGuidedGame();
//     return 0;
// }