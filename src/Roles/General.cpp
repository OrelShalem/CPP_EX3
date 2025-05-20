// orel8155@gmail.com
#include "General.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"
namespace coup
{

    General::General(Game &game, const string &name) : Player(game, name)
    {
    }

    void General::block_coup(Player &target)
    {
        checkCoins(5);

        if (target.get_last_action() != "coup")
        {
            throw InvalidOperation("Can only block coup action");
        }

        removeCoins(5);

        auto victim_name = target.get_last_target();
        auto victim = game_.getPlayer(victim_name);
        victim->setActive(true);
    }

    void General::react_to_arrest()
    {
        addCoins(1);
    }

    string General::role() const
    {
        return "General";
    }
}