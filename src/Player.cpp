// orel8155@gmail.com

#include "Player.hpp"
#include "Game.hpp"
#include "GameExceptions.hpp"

namespace coup
{
    Player::Player(Game &game, const string &name, Role role)
        : game_(game), name_(name), coins_(0), active_(true), blocked_from_economic_(false),
          blocked_from_arrest_(false), sanction_turns_remaining_(0), last_action_(""), last_target_(""), role_(role)
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

    void Player::checkAndClearSanction()
    {
        // כאשר שחקן מקבל את תורו, יש לבדוק אם יש עליו סנקציה ולהסיר אותה
        if (blocked_from_economic_ && sanction_turns_remaining_ > 0)
        {
            sanction_turns_remaining_ = 0;
            blocked_from_economic_ = false;
        }
    }

    void Player::checkTurnAndClearSanction()
    {
        // ראשית מנקים סנקציות
        checkAndClearSanction();
        // אז בודקים את התור
        checkTurn();
    }

    void Player::gather()
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }

        addCoins(1);
        last_action_ = "gather";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Player::tax()
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }

        addCoins(2);
        last_action_ = "tax";
        last_target_ = "";

        // רישום הפעולה כממתינה
        game_.registerPendingAction("tax", name_, "");

        game_.advanceTurn();
    }

    void Player::bribe()
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
        if (coins_ < 4)
        {
            throw InvalidOperation("You do not have enough coins to bribe");
        }
        removeCoins(4);
        last_action_ = "bribe";
        last_target_ = "";

        // רישום הפעולה כממתינה
        game_.registerPendingAction("bribe", name_, "");

        game_.advanceTurn();
    }

    void Player::arrest(Player &target)
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }

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

        // רישום הפעולה כממתינה
        game_.registerPendingAction("arrest", name_, target.name());

        // now we can react to the arrest
        target.react_to_arrest();

        // if the target is not a general or merchant, we can remove 1 coin from the target and add 1 coin to the player
        if (target.role() != Role::GENERAL && target.role() != Role::MERCHANT)
        {
            target.removeCoins(1);
            addCoins(1);
        }

        game_.advanceTurn();
    }

    void Player::sanction(Player &target)
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
        checkCoins(3);
        // pay the sanction
        removeCoins(3);

        // before sanctioning, we need to set the last action and target
        last_action_ = "sanction";
        last_target_ = target.name();

        // רישום הפעולה כממתינה
        game_.registerPendingAction("sanction", name_, target.name());

        // block the target from economic actions
        target.setBlocked(true, false);

        // הגדרת הסנקציה לתור הבא (משתמשים ב-1 כדי לסמן שיש סנקציה פעילה)
        target.setSanctionTurns(1);

        // now we can react to the sanction
        target.react_to_sanction();

        game_.advanceTurn();
    }

    void Player::coup(Player &target)
    {
        checkTurnAndClearSanction(); // משתמשים בפונקציה המשולבת

        checkCoins(7);

        // בודק שהשחקן היעד פעיל לפני הסרתו
        if (!target.isActive())
        {
            throw InvalidOperation("Cannot coup an inactive player");
        }

        // מסיר מטבעות רק לאחר שוידאנו שהפעולה חוקית
        removeCoins(7);

        // מעביר את שם השחקן היעד לפונקציה removePlayer
        game_.removePlayer(target.name());

        last_action_ = "coup";
        last_target_ = target.name();

        // רישום הפעולה כממתינה רק אם השחקן הוא לא גנרל
        // (כי רק גנרל יכול לבטל coup)
        if (target.role() != Role::GENERAL)
        {
            game_.registerPendingAction("coup", name_, target.name());
        }

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

    string &Player::get_last_action()
    {
        return last_action_;
    }

    string &Player::get_last_target()
    {
        return last_target_;
    }

    void Player::setBlocked(bool from_economic, bool from_arrest)
    {
        blocked_from_economic_ = from_economic;
        blocked_from_arrest_ = from_arrest;
    }

    void Player::setSanctionTurns(int turns)
    {
        sanction_turns_remaining_ = turns;
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

    void Player::undo(Player &target)
    {
        if (!can_undo(target.get_last_action()))
        {
            throw InvalidOperation("Player cannot undo this action");
        }

        target.handle_undo(target);
    }

    Role Player::role() const
    {
        return role_;
    }

    // מימושי ברירת מחדל לפעולות מיוחדות
    void Player::invest()
    {
        throw InvalidOperation("This player cannot invest");
    }

    void Player::block_arrest(Player &target)
    {
        (void)target; // למנוע warning על פרמטר לא בשימוש
        throw InvalidOperation("This player cannot block arrest");
    }

    void Player::block_coup(Player &target)
    {
        (void)target; // למנוע warning על פרמטר לא בשימוש
        throw InvalidOperation("This player cannot block coup");
    }
}