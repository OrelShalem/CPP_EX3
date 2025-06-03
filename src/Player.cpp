// orel8155@gmail.com

#include "Player.hpp"
#include "Game.hpp"
#include "GameExceptions.hpp"
#include <iostream>

namespace coup
{
    Player::Player(Game &game, const string &name, Role role)
        : game_(game), name_(name), coins_(0), active_(true), blocked_from_economic_(false),
          blocked_from_arresting_(false), last_action_(""), last_target_(""), role_(role)
    {
    }

    void Player::checkTurn() const
    {
        if (game_.turn() != role_)
        {
            throw InvalidTurn("It is not your turn");
        }

        if (!active_)
        {
            throw InvalidOperation("You are not active");
        }
    }


    void Player::gather()
    {
        cout<<"gather"<<endl;
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        checkTurn();
        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        

        addCoins(1);
        last_action_ = "gather";
        last_target_ = "";

        // מחיקת הפעולה הקודמת מהמפה (אם הייתה)

        game_.advanceTurn();
        cout<<"advance turn"<<endl;
    }

    void Player::tax()
    {
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        checkTurn();
        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        
        game_.removeCoinsFromBank(2); // remove 2 coins from the bank
        addCoins(2);
        last_action_ = "tax";
        last_target_ = "";

        // רישום הפעולה במפה של השחקנים

        game_.advanceTurn();

    }

    void Player::bribe()
    {
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        
        
        if (coins_ < 4)
        {
            throw InvalidOperation("You do not have enough coins to bribe");
        }
        removeCoins(4);
        last_action_ = "bribe";
        last_target_ = "";

   
        // if the player is sanctioned, we need to clear the blocked economic actions
        if (game_.getPlayerByName(name_)->blocked_from_economic())
        {
            game_.getPlayerByName(name_)->setBlockedFromEconomic(false);
        }
    }

    void Player::arrest(shared_ptr<Player> &target)
    {
        cout << "DEBUG Player::arrest: Start - " << name_ << " arresting " << target->name() << endl;
        
        if (blocked_from_arresting_)
        {
            cout << "DEBUG Player::arrest: Player " << name_ << " is blocked from arrest" << endl;
            throw InvalidOperation("You are blocked from arrest actions");
        }

        // בדיקה אם השחקן כבר נעצר
        if (game_.getArrestedPlayerName() == target->name())
        {
            cout << "DEBUG Player::arrest: Cannot arrest " << target->name() << " - already arrested" << endl;
            throw InvalidOperation("You cannot arrest the same player twice in a row");
        }

        if (coins_ >= 10)
        {
            cout << "DEBUG Player::arrest: Cannot arrest - " << name_ << " has " << coins_ << " coins, must coup" << endl;
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }

        cout << "DEBUG Player::arrest: Setting action and target" << endl;
        // before arresting, we need to set the last action and target
        last_action_ = "arrest";
        last_target_ = target->name();
        game_.setArrestedPlayerName(target->name());

        

        cout << "DEBUG Player::arrest: Calling target->react_to_arrest()" << endl;
        // now we can react to the arrest
        target->react_to_arrest();

        cout << "DEBUG Player::arrest: Checking roles - target role: " << (int)target->role() << endl;
        // if the target is not a general or merchant, we can remove 1 coin from the target and add 1 coin to the player
        if (target->role() != Role::GENERAL && target->role() != Role::MERCHANT)
        {
            cout << "DEBUG Player::arrest: Target is not General/Merchant, transferring coin" << endl;
            target->removeCoins(1);
            addCoins(1);
        }
        else {
            cout << "DEBUG Player::arrest: Target is General/Merchant, no coin transfer" << endl;
        }

        cout << "DEBUG Player::arrest: Advancing turn" << endl;
        game_.advanceTurn();

        
        
        cout << "DEBUG Player::arrest: Completed successfully" << endl;
    }

    void Player::sanction(Player &target)
    {
        cout << "DEBUG Player::sanction: Start - " << name_ << " sanctioning " << target.name() << endl;
        
        if (coins_ >= 10)
        {
            cout << "DEBUG Player::sanction: Cannot sanction - " << name_ << " has " << coins_ << " coins, must coup" << endl;
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
        cout << "DEBUG Player::sanction: Checking if has enough coins (need 3)" << endl;
        checkCoins(3);
        
        cout << "DEBUG Player::sanction: Removing 3 coins from " << name_ << endl;
        // pay the sanction
        removeCoins(3);
        game_.addCoinsToBank(3); // add the coins to the bank
        
        cout << "DEBUG Player::sanction: Setting action and target" << endl;
        // before sanctioning, we need to set the last action and target
        last_action_ = "sanction";
        last_target_ = target.name();

        

        cout << "DEBUG Player::sanction: Blocking target from economic actions" << endl;
        // block the target from economic actions
        target.setBlockedFromEconomic(true);

       

        cout << "DEBUG Player::sanction: Calling target.react_to_sanction()" << endl;
        // now we can react to the sanction
        try 
        {
            target.react_to_sanction();
            cout << "DEBUG Player::sanction: react_to_sanction completed normally" << endl;
        }
        catch (const InvalidOperation &e)
        {
            cout << "DEBUG Player::sanction: react_to_sanction threw exception: " << e.what() << endl;
            removeCoins(1);
            game_.addCoinsToBank(1); // add the coin to the bank
        }

        cout << "DEBUG Player::sanction: Advancing turn" << endl;
        game_.advanceTurn();

     
        
        cout << "DEBUG Player::sanction: Completed successfully" << endl;
    }

    void Player::coup(shared_ptr<Player> &target)
    {
        cout << "DEBUG Player::coup: Starting coup by " << name_ << " against " << target->name() << endl;
        // checkTurn();

        cout << "DEBUG Player::coup: Checking if has enough coins (need 7)" << endl;
        checkCoins(7);

        // בודק שהשחקן היעד פעיל לפני הסרתו
        if (!target->isActive())
        {
            cout << "DEBUG Player::coup: Target " << target->name() << " is inactive, cannot coup" << endl;
            throw InvalidOperation("Cannot coup an inactive player");
        }

        cout << "DEBUG Player::coup: Removing 7 coins from " << name_ << endl;
        // מסיר מטבעות רק לאחר שוידאנו שהפעולה חוקית
        removeCoins(7);

       

        cout << "DEBUG Player::coup: Setting target " << target->name() << " as inactive" << endl;
        // מעביר את שם השחקן היעד לפונקציה removePlayer
        target->setActive(false); // מסמן את השחקן היעד כלא פעיל

        cout << "DEBUG Player::coup: Setting action and target" << endl;
        last_action_ = "coup";
        last_target_ = target->name();

        cout << "DEBUG Player::coup: Removing player " << target->name() << endl;
        cout << "DEBUG Player::coup: Advancing turn after coup" << endl;
        game_.advanceTurn();
        
        cout << "DEBUG Player::coup: Setting last player couped" << endl;
        game_.getLastPlayerCouped() = target; // עדכון השחקן האחרון שעבר coup
        
        cout << "DEBUG Player::coup: Completed successfully" << endl;
    }

    int Player::coins() const
    {
        return coins_;
    }

    string Player::name() const
    {
        return name_;
    }

    bool Player::isActive() const
    {
        return active_;
    }

    string &Player::get_last_action()
    {
        return last_action_;
    }

    string &Player::get_last_target()
    {
        return last_target_;
    }



    void Player::setActive(bool active)
    {
        active_ = active;
    }

    void Player::addCoins(int amount)
    {
        coins_ += amount;
    }

    void Player::removeCoins(int amount)
    {
        if (coins_ < amount)
        {
            throw NotEnoughCoins("You do not have enough coins to remove");
        }
        coins_ -= amount;
    }

    void Player::checkCoins(int amount) const
    {
        if (coins_ < amount)
        {
            throw NotEnoughCoins("You do not have enough coins to perform this action");
        }
    }

    Role Player::role() const
    {
        return role_;
    }

}