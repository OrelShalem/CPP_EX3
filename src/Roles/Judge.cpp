// orel8155@gmail.com
#include "Judge.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
#include <iostream>
namespace coup
{

    bool Judge::can_undo(const string &action) const
    {
        return action == "bribe";
    }

    void Judge::handle_undo(Player &target)
    {
        if (!can_undo(target.get_last_action()))
        {
            throw InvalidOperation("Judge cannot undo this action");
        }

        // בדיקה אם קיימת פעולת bribe ממתינה
        if (!game_.hasPending("bribe", target.name(), ""))
        {
            throw InvalidOperation("No pending bribe action to undo");
        }

        // next turn
        game_.advanceTurn();
        cout << "Judge has undone the bribe action of " << target.name() << endl;
        target.get_last_action() = "";
        target.get_last_target() = "";

        // ניקוי הפעולה הממתינה
        game_.clearPendingFor("bribe");
    }

    void Judge::react_to_sanction()
    {

        if (!last_action_.empty())
        {
            try
            {
                auto sactioner = game_.getPlayer();
                if (sactioner->get_last_action() == "sanction" &&
                    sactioner->get_last_target() == name_)
                {
                    sactioner->removeCoins(1);
                }
            }
            catch (const PlayerNotFound &)
            {
            }
            catch (const NotEnoughCoins &)
            {
            }
        }
    }

}