// orel8155@gmail.com
#include "Judge.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
namespace coup
{

    Judge::Judge(Game &game, const string &name) : Player(game, name)
    {
    }

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

        target.removeCoins(4);
    }

    void Judge::react_to_sanction()
    {

        if (!last_action_.empty())
        {
            try
            {
                auto sactioner = game_.getPlayer(last_target_);
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

    string Judge::role() const
    {
        return "Judge";
    }
}