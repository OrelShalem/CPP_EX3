// orel8155@gmail.com
#include "Spy.hpp"
#include "../GameExceptions.hpp"

namespace coup
{
    Spy::Spy(Game &game, const string &name) : Player(game, name)
    {
    }

    void Spy::block_arrest(Player &target)
    {
        // not need to check turn because spy can block arrest at any time
        target.setBlocked(false, true);
    }

    string Spy::role() const
    {
        return "Spy";
    }

}