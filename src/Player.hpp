// orel8155@gmail.com

#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "GameExceptions.hpp"
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

    enum class UndoableAction
    {
        GATHER,
        TAX,
        BRIBE,
        ARREST,
        SANCTION,
        COUP
    };

    class Player
    {
    protected:
        Game &game_;                   // to store the game
        string name_;                  // to store the name of the player
        int coins_;                    // to store the number of coins the player has
        bool active_;                  // to check if the player is active
        bool blocked_from_economic_;   // to check if the player is blocked from economic actions
        bool blocked_from_arresting_;  // to check if the player is blocked from arresting
        string last_action_;           // to store the last action the player made
        string last_target_;           // to store the last target of the action
        Role role_;                    // to store the role of the player
    public:
        Player(Game &game, const string &name, Role role);
        virtual ~Player() = default; // destructor is virtual because it is a base class and we want to call the destructor of the derived class and default because we don't want to do anything in the destructor

        // basic functions
        void gather();
        virtual void tax();
        virtual void bribe();
        virtual void arrest(shared_ptr<Player> &player);
        virtual void sanction(Player &player);
        void coup(shared_ptr<Player> &player);
        virtual void newTurnIncome() { throw InvalidOperation("Player doesn't have new turn income"); }
        virtual void react_to_arrest() {}
        virtual void react_to_sanction() { cout << "Player " << name_ << " cannot gather and tax" << endl; }
        virtual void undo(UndoableAction action) { throw InvalidOperation("Player doesn't have undo function"); }
        virtual int view_coins(Player &target) { throw InvalidOperation("Player doesn't have view coins function"); }
        virtual void invest() { throw InvalidOperation("Player doesn't have invest function"); }

        // info
        int coins() const;                                                    // to get the number of coins the player has
        void setCoins(int amount) { coins_ = amount; }
        string name() const;                                                  // to get the name of the player
        bool isActive() const;                                                // to check if the player is active
        bool blocked_from_economic() const { return blocked_from_economic_; } // לבדוק אם השחקן חסום מפעולות כלכליות
        bool& get_blocked_from_arresting() { return blocked_from_arresting_; }
        string &get_last_action();
        string &get_last_target();

        Role role() const; // to get the role of the player (this is a pure virtual function and must be implemented by the derived classes)

        // inner functions
        void setBlockedFromEconomic(bool blocked)
        {
            blocked_from_economic_ = blocked;
        }
        void setActive(bool active);
        void addCoins(int amount);
        void removeCoins(int amount);

        // validations
        void checkTurn() const;
        void checkCoins(int amount) const;
        bool mustPerformCoup() const { return coins_ >= 10; } // לבדוק אם השחקן חייב לבצע coup

    };

}