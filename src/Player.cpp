// orel8155@gmail.com

#include "Player.hpp"
#include "Game.hpp"
#include "GameExceptions.hpp"

namespace coup
{
    Player::Player(Game &game, const string &name)
        : game_(game), name_(name), coins_(0), active_(true), blocked_from_economic_(false),
          blocked_from_arrest_(false), last_action_(""), last_target_("")
    {
        game.addPlayer(shared_ptr<Player>(this));
    }

    void Player::gather()
    {
        checkTurn();

        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        addCoins(1);
        last_action_ = "gather";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Player::tax()
    {
        checkTurn();

        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        addCoins(2);
        last_action_ = "tax";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Player::bribe()
    {
        checkTurn();
        addCoins(4);
        last_action_ = "bribe";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Player::arrest(Player &target)
    {
        checkTurn();

        if (blocked_from_arrest_)
        {
            throw InvalidOperation("You are blocked from arrest actions");
        }

        if (last_target_ == target.name() && last_action_ == "arrest")
        {
            throw InvalidOperation("You cannot arrest the same player twice in a row");
        }

        // before arresting, we need to set the last action and target
        last_action_ = "arrest";
        last_target_ = target.name();

        // now we can react to the arrest
        target.react_to_arrest();

        // if the target is not a general or merchant, we can remove 1 coin from the target and add 1 coin to the player
        if (target.role() != "General" && target.role() != "Merchant")
        {
            target.removeCoins(1);
            addCoins(1);
        }

        game_.advanceTurn();
    }

    void Player::sanction(Player &target)
    {
        checkTurn();
        checkCoins(3);
        // pay the sanction
        removeCoins(3);

        // before sanctioning, we need to set the last action and target
        last_action_ = "sanction";
        last_target_ = target.name();

        // block the target from economic actions
        target.setBlocked(true, false);

        // now we can react to the sanction
        target.react_to_sanction();

        game_.advanceTurn();
    }

    void Player::coup(Player &target)
    {
        checkTurn();
        checkCoins(7);

        removeCoins(7);

        game_.removePlayer(target.name());

        last_action_ = "coup";
        last_target_ = target.name();
        game_.advanceTurn();
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

    string Player::get_last_action() const
    {
        return last_action_;
    }

    string Player::get_last_target() const
    {
        return last_target_;
    }

    void Player::setBlocked(bool from_economic, bool from_arrest)
    {
        blocked_from_economic_ = from_economic;
        blocked_from_arrest_ = from_arrest;
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

    void Player::checkTurn() const
    {
        if (game_.turn() != name_)
        {
            throw InvalidTurn("It is not your turn");
        }

        if (!active_)
        {
            throw InvalidOperation("You are not active");
        }

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
    }

    void Player::checkCoins(int amount) const
    {
        if (coins_ < amount)
        {
            throw NotEnoughCoins("You do not have enough coins to perform this action");
        }
    }

    void Player::undo(Player &target)
    {
        if (!can_undo(target.get_last_action()))
        {
            throw InvalidOperation("Player cannot undo this action");
        }

        target.handle_undo(target);
    }

}