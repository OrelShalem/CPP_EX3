#include "Governor.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"

namespace coup
{

    void Governor::tax()
    {
        checkTurn();
        if (blocked_from_economic_)
        {
            throw InvalidOperation("Governor is blocked from economic actions");
        }
        addCoins(3);
        last_action_ = "tax";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Governor::react_to_arrest()
    {
        // Governor doesn't lose coins when arrested
        // No need to advance turn here as it's already done in the arrest function
    }

    bool Governor::can_undo(const string &action) const
    {
        return action == "tax"; // can undo tax
    }

    void Governor::handle_undo(Player &target)
    {
        if (!isActive())
        {
            throw InvalidOperation("Governor is not active");
        }

        if (!can_undo(target.get_last_action()))
        {
            throw InvalidOperation("Governor cannot undo this action");
        }

        if (target.role() == Role::GOVERNOR)
        {
            target.removeCoins(3); // governor remove 3 coins
        }
        else
        {
            target.removeCoins(2); // regular player remove 2 coins
        }
    }

}