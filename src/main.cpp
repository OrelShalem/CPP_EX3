// orel8155@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "Roles/General.hpp"
#include "Roles/Merchant.hpp"
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "GameExceptions.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <exception>

using namespace coup;
using namespace std;

// Function to print game status
void printGameStatus(Game &game)
{
    auto role = game.getPlayer();
    cout << "\n--- Current Game Status ---" << endl;
    cout << "Current turn: " << role->name() << endl;
    cout << "Active players: ";
    for (const auto &player : game.players())
    {
        cout << player << " ";
    }
    cout << "\n-----------------------\n"
         << endl;
}

int main()
{
    try
    {
        // Create a new game
        Game game;

        // Create players with different roles
        cout << "Creating players with different roles..." << endl;
        auto general = game.createPlayer("Dan", Role::GENERAL);
        auto merchant = game.createPlayer("Ron", Role::MERCHANT);
        auto governor = game.createPlayer("Liat", Role::GOVERNOR);
        auto spy = game.createPlayer("Noa", Role::SPY);
        auto baron = game.createPlayer("Amit", Role::BARON);
        auto judge = game.createPlayer("Michal", Role::JUDGE);

        cout << "Game starts with 6 players!" << endl;

        // --- ROUNDS 1-2: Initial resource gathering ---
        cout << "\n=== ROUNDS 1-2: Initial resource gathering ===" << endl;

        // First round - each player gathers coins
        printGameStatus(game);
        general->gather();
        cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

        printGameStatus(game);
        merchant->gather();
        cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;

        printGameStatus(game);
        governor->gather();
        cout << "Liat (Governor) gathered a coin. Coins: " << governor->coins() << endl;

        printGameStatus(game);
        spy->gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

        printGameStatus(game);
        baron->gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

        printGameStatus(game);
        judge->gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge->coins() << endl;

        // Second round - continue gathering
        printGameStatus(game);
        general->gather();
        cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

        printGameStatus(game);
        merchant->gather();
        cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;
        cout << "Next turn Ron will have 3 coins and will get an extra coin when gathering (special ability)." << endl;

        printGameStatus(game);
        governor->gather();
        cout << "Liat (Governor) gathered a coin. Coins: " << governor->coins() << endl;

        printGameStatus(game);
        spy->gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

        printGameStatus(game);
        baron->gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

        printGameStatus(game);
        judge->gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge->coins() << endl;

        // --- ROUNDS 3-4: Demonstrating special abilities ---
        cout << "\n=== ROUNDS 3-4: Demonstrating special abilities ===" << endl;

        // General uses tax
        printGameStatus(game);
        general->tax();
        cout << "Dan (General) used tax to collect 2 coins. Coins: " << general->coins() << endl;

        // Merchant gathers (should get an extra coin with 3+ coins)
        printGameStatus(game);
        merchant->gather();
        cout << "Ron (Merchant) gathered a coin plus a bonus coin (special ability). Coins: " << merchant->coins() << endl;

        // Governor uses tax (gets 3 coins instead of 2)
        printGameStatus(game);
        governor->tax();
        cout << "Liat (Governor) used tax to collect 3 coins (special ability). Coins: " << governor->coins() << endl;

        // Spy's turn - here we can check coins first, then arrest
        printGameStatus(game);
        // המרגל צופה במטבעות בתור שלה, ואז מבצעת מעצר
        cout << "Noa (Spy) checking Amit's coins during her turn and blocked his arrest: " << std::static_pointer_cast<Spy>(spy)->view_coins(*baron) << endl;
        spy->gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

        // Baron's turn - gather one coin
        printGameStatus(game);
        baron->gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

        // Judge's turn
        printGameStatus(game);
        judge->gather();
        cout << "Michal (Judge) gathered a coin. Coins: " << judge->coins() << endl;

        // General's turn
        printGameStatus(game);
        general->gather();
        cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

        // Merchant's turn
        printGameStatus(game);
        merchant->gather();
        cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;

        // Governor's turn
        printGameStatus(game);
        governor->gather();
        cout << "Liat (Governor) gathered a coin. Coins: " << governor->coins() << endl;

        // Spy's turn
        printGameStatus(game);
        spy->gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

        // Baron's turn - gather another coin to reach 3
        printGameStatus(game);
        baron->gather();
        cout << "Amit (Baron) gathered another coin. Coins: " << baron->coins() << endl;

        // Judge's turn
        printGameStatus(game);
        judge->gather();
        cout << "Michal (Judge) gathered a coin. Coins222: " << judge->coins() << endl;
        // general turn
        printGameStatus(game);
        general->gather();
        cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

        // // Baron finally has 3 coins and can now invest
        // printGameStatus(game);
        // std::static_pointer_cast<Baron>(baron)->invest();
        // cout << "Amit (Baron) invested 3 coins to receive 6 (special ability). Coins: " << baron->coins() << endl;

        // --- ROUNDS 5-6: מצבי קונפליקט והתגוננות ---
        cout << "\n=== ROUNDS 5-6: Conflicts and Defense ===" << endl;

        printGameStatus(game);
        merchant->gather();
        cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;

        printGameStatus(game);
        // נציב (ליאת) מבטל פעולת מס של הגנרל (יכולת מיוחדת) - כעת זה בתור שלה
        governor->undo(*general);
        cout << "Liat (Governor) cancelled Dan's tax action (special ability)." << endl;
        cout << "Dan (General) now has " << general->coins() << " coins." << endl;

        printGameStatus(game);
        // מרגל (נועה) מונע מהברון להשתמש במעצר בתור הבא
        std::static_pointer_cast<Spy>(spy)->block_arrest(*baron);
        cout << "Noa (Spy) blocked Amit's arrest ability (special ability)." << endl;
        spy->gather();
        cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

        printGameStatus(game);
        baron->gather();
        cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

        printGameStatus(game);
        judge->sanction(*merchant);
        cout << "Michal (Judge) sanctioned Ron (Merchant) for 3 coins. Coins: " << judge->coins() << endl;
        cout << "Ron (Merchant) is now blocked from economic actions." << endl;

        // סוחר מנסה לבצע gather אבל נחסם בגלל החרם
        printGameStatus(game);
        general->gather();
        cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

        printGameStatus(game);
        try
        {
            merchant->gather();
            cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;
        }
        catch (const InvalidOperation &e)
        {
            cout << "Ron (Merchant) tried to gather coins but was blocked by sanction: " << e.what() << endl;
            // צריך לקדם את התור באופן ידני כי הפעולה נכשלה
            game.advanceTurn();
        }

        // הסוחר מנסה לשחד
        printGameStatus(game);
        merchant->bribe();
        cout << "Ron (Merchant) paid 4 coins for bribe. Coins: " << merchant->coins() << endl;

        // שופט מבטל פעולת שוחד של הסוחר
        printGameStatus(game);
        judge->undo(*merchant);
        cout << "Michal (Judge) cancelled Ron's bribe (special ability)." << endl;

        // --- ROUNDS 7-8: הפיכות והדחות ---
        cout << "\n=== ROUNDS 7-8: Coups and Eliminations ===" << endl;

        // צבירת מטבעות לגנרל
        while (general->coins() < 7)
        {
            printGameStatus(game);
            general->gather();
            cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;

            // תורות של שאר השחקנים בינתיים
            printGameStatus(game);
            merchant->gather();
            cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;

            printGameStatus(game);
            governor->gather();
            cout << "Liat (Governor) gathered a coin. Coins: " << governor->coins() << endl;

            printGameStatus(game);
            spy->gather();
            cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

            printGameStatus(game);
            baron->gather();
            cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

            printGameStatus(game);
            judge->gather();
            cout << "Michal (Judge) gathered a coin. Coins: " << judge->coins() << endl;
        }

        // גנרל מבצע coup על השופט
        printGameStatus(game);
        general->coup(*judge);
        cout << "Dan (General) performed coup on Michal (Judge)! Michal is out of the game." << endl;

        // צבירת מטבעות לברון
        while (baron->coins() < 7)
        {
            printGameStatus(game);
            merchant->gather();
            cout << "Ron (Merchant) gathered a coin. Coins: " << merchant->coins() << endl;

            printGameStatus(game);
            governor->gather();
            cout << "Liat (Governor) gathered a coin. Coins: " << governor->coins() << endl;

            printGameStatus(game);
            spy->gather();
            cout << "Noa (Spy) gathered a coin. Coins: " << spy->coins() << endl;

            printGameStatus(game);
            baron->gather();
            cout << "Amit (Baron) gathered a coin. Coins: " << baron->coins() << endl;

            printGameStatus(game);
            general->gather();
            cout << "Dan (General) gathered a coin. Coins: " << general->coins() << endl;
        }

        // ברון מנסה לבצע coup על הנציב
        printGameStatus(game);
        baron->coup(*governor);
        cout << "Amit (Baron) attempts to coup Liat (Governor)..." << endl;

        // גנרל חוסם את ההפיכה
        std::static_pointer_cast<General>(general)->block_coup(*baron);
        cout << "Dan (General) blocks the coup by paying 5 coins (special ability)!" << endl;
        cout << "Liat (Governor) remains in the game." << endl;
        cout << "Dan (General) now has " << general->coins() << " coins." << endl;

        // --- FORCED COUP DEMONSTRATION (10+ coins) ---
        cout << "\n=== FORCED COUP DEMONSTRATION (10+ COINS) ===" << endl;

        // Continue until merchant has 10 coins
        while (merchant->coins() < 10)
        {
            printGameStatus(game);
            auto current = game.getPlayer();
            current->gather();
            cout << current->name() << " gathered a coin. Coins: " << current->coins() << endl;
        }

        // Merchant must perform coup with 10+ coins
        printGameStatus(game);
        cout << "Ron (Merchant) has 10+ coins and MUST perform a coup!" << endl;
        merchant->coup(*spy);
        cout << "Ron (Merchant) performed a forced coup on Noa (Spy)!" << endl;
        cout << "Noa (Spy) is out of the game." << endl;

        // --- PLAY UNTIL WINNER ---
        cout << "\n=== CONTINUING UNTIL WINNER ===" << endl;

        // Continue playing until there's a winner
        while (game.players().size() > 1)
        {
            printGameStatus(game);
            auto current = game.getPlayer();

            if (current->coins() >= 7)
            {
                // מוצא מטרה תקפה לקופ
                shared_ptr<Player> target = nullptr;
                for (const auto &player_name : game.players())
                {
                    if (player_name != current->name())
                    {
                        for (auto p : {general, merchant, governor, baron})
                        {
                            if (p && p->isActive() && p->name() == player_name)
                            {
                                target = p;
                                break;
                            }
                        }
                        if (target)
                            break;
                    }
                }

                if (target)
                {
                    current->coup(*target);
                    cout << current->name() << " performed a coup on " << target->name() << "!" << endl;
                    cout << target->name() << " is out of the game." << endl;
                }
            }
            else
            {
                current->gather();
                cout << current->name() << " gathered a coin. Coins: " << current->coins() << endl;
            }
        }

        // Announce the winner
        printGameStatus(game);
        cout << "\nThe winner is: " << game.winner() << "!" << endl;
    }
    catch (const exception &e)
    {
        cerr << "An error occurred: " << e.what() << endl;
    }

    return 0;
}