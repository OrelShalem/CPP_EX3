// orel8155@gmail.com
#include "Merchant.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
#include <iostream>
namespace coup
{

    void Merchant::gather()
    {
        checkAndClearSanction();
        checkTurn();

        if (blocked_from_economic_)
        {
            throw InvalidOperation("Merchant is blocked from economic actions");
        }

        bool had_three_coins = coins_ >= 3;

        addCoins(1);

        if (had_three_coins)
        {
            cout << "Merchant gathered 3 coins and got 1 more" << endl;
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
}