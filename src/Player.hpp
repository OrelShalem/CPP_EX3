// orel8155@gmail.com

#pragma once
#include <string>
#include <memory>

using namespace std;

namespace coup
{
    class Game;

    enum class Role // to store the role of the player
    {
        GENERAL,
        GOVERNOR,
        SPY,
        BARON,
        JUDGE,
        MERCHANT
    };

    class Player
    {
    protected:
        Game &game_;                   // to store the game
        string name_;                  // to store the name of the player
        int coins_;                    // to store the number of coins the player has
        bool active_;                  // to check if the player is active
        bool blocked_from_economic_;   // to check if the player is blocked from economic actions
        bool blocked_from_arrest_;     // to check if the player is blocked from arrest
        int sanction_turns_remaining_; // מספר התורות שנשארו לסנקציה
        string last_action_;           // to store the last action the player made
        string last_target_;           // to store the last target of the action
        Role role_;                    // to store the role of the player
    public:
        Player(Game &game, const string &name, Role role);
        virtual ~Player() = default; // destructor is virtual because it is a base class and we want to call the destructor of the derived class and default because we don't want to do anything in the destructor

        // basic functions
        virtual void gather();
        virtual void tax();
        virtual void bribe();
        virtual void arrest(Player &player);
        virtual void sanction(Player &player);
        void coup(Player &player);

        // actions
        virtual void react_to_arrest() {}   // to react to an arrest
        virtual void react_to_sanction() {} // to react to a sanction
        virtual bool can_undo(const string &action [[maybe_unused]]) const { return false; }
        virtual void handle_undo(Player &target [[maybe_unused]]) {}

        virtual void undo(Player &target);

        // info
        int coins() const;     // to get the number of coins the player has
        string name() const;   // to get the name of the player
        bool isActive() const; // to check if the player is active

        string &get_last_action();
        string &get_last_target();

        Role role() const; // to get the role of the player (this is a pure virtual function and must be implemented by the derived classes)

        // inner functions
        void setBlocked(bool from_economic, bool from_arrest);
        void setSanctionTurns(int turns);
        void setActive(bool active);
        void addCoins(int amount);
        void removeCoins(int amount);

        // validations
        void checkTurn() const;
        void checkCoins(int amount) const;
        void checkAndClearSanction(); // פונקציה חדשה לטיפול בסנקציות

        // פונקציה חדשה שמשלבת בדיקת סנקציה ובדיקת תור
        void checkTurnAndClearSanction();

        // פעולות מיוחדות - virtual methods עם הצהרה בלבד
        virtual void invest();
        virtual void block_arrest(Player &target);
        virtual void block_coup(Player &target);
    };

}