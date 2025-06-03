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
        if (blocked_from_arresting_)
        {
            throw InvalidOperation("You are blocked from arrest actions");
        }

        // בדיקה אם השחקן כבר נעצר
        if (game_.getArrestedPlayerName() == target->name())
        {
            throw InvalidOperation("You cannot arrest the same player twice in a row");
        }

        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }

        // before arresting, we need to set the last action and target
        last_action_ = "arrest";
        last_target_ = target->name();
        game_.setArrestedPlayerName(target->name());

        // now we can react to the arrest
        target->react_to_arrest();

        // if the target is not a general or merchant, we can remove 1 coin from the target and add 1 coin to the player
        if (target->role() != Role::GENERAL && target->role() != Role::MERCHANT)
        {
            target->removeCoins(1);
            addCoins(1);
        }

        game_.advanceTurn();
    }

    void Player::sanction(Player &target)
    {
        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
        checkCoins(3);
        
        // pay the sanction
        removeCoins(3);
        game_.addCoinsToBank(3); // add the coins to the bank
        
        // before sanctioning, we need to set the last action and target
        last_action_ = "sanction";
        last_target_ = target.name();

        // block the target from economic actions
        target.setBlockedFromEconomic(true);

        // now we can react to the sanction
        try 
        {
            target.react_to_sanction();
        }
        catch (const InvalidOperation &e)
        {
            removeCoins(1);
            game_.addCoinsToBank(1); // add the coin to the bank
        }

        game_.advanceTurn();
    }

    void Player::coup(shared_ptr<Player> &target)
    {
        checkCoins(7);

        // בודק שהשחקן היעד פעיל לפני הסרתו
        if (!target->isActive())
        {
            throw InvalidOperation("Cannot coup an inactive player");
        }
        // check if the target is the same as the player
        if (this == target.get())
        {
            throw InvalidOperation("You cannot coup yourself");
        }

        // מסיר מטבעות רק לאחר שוידאנו שהפעולה חוקית
        removeCoins(7);

        // מעביר את שם השחקן היעד לפונקציה removePlayer
        target->setActive(false); // מסמן את השחקן היעד כלא פעיל

        last_action_ = "coup";
        last_target_ = target->name();

        game_.advanceTurn();
        
        game_.getLastPlayerCouped() = target; // עדכון השחקן האחרון שעבר coup
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