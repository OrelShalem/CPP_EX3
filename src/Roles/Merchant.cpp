// orel8155@gmail.com
#include "Merchant.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"

namespace coup
{
    Merchant::Merchant(Game &game, const string &name) : Player(game, name)
    {
    }

    void Merchant::gather()
    {
        checkTurn();

        if (blocked_from_economic_)
        {
            throw InvalidOperation("Merchant is blocked from economic actions");
        }

        addCoins(1);

        if (coins_ >= 3)
        {
            addCoins(1);
        }

        last_action_ = "gather";
        last_target_ = "";
        game_.advanceTurn();
    }

    void Merchant::react_to_arrest()
    {
        try
        {
            if (coins_ >= 2)
            {
                removeCoins(2);
            }
            else if (coins_ >= 1)
            {
                removeCoins(1);
            }
        }
        catch (const NotEnoughCoins &)
        {
        }
    }

    string Merchant::role() const
    {
        return "Merchant";
    }
}