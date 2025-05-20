// orel8155@gmail.com
#include "Spy.hpp"
#include "../GameExceptions.hpp"

namespace coup
{
    Spy::Spy(Game &game, const string &name) : Player(game, name)
    {
    }

    int Spy::view_coins(const Player &target) const
    {
        return target.coins();
    }

    void Spy::block_arrest(Player &target)
    {
        if (target.get_last_action() != "arrest")
        {
            throw InvalidOperation("Spy can only block arrest");
        }
        // not need to check turn because spy can block arrest at any time
        target.setBlocked(false, true);
    }

    string Spy::role() const
    {
        return "Spy";
    }

}