// orel8155@gmail.com
#include "Merchant.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
#include <iostream>

namespace coup
{

    
    void Merchant::react_to_arrest()
    {
        cout << "DEBUG Merchant::react_to_arrest: Merchant " << name() << " reacting to arrest" << endl;
        try
        {
            if (coins_ >= 2)
            {
                cout << "DEBUG Merchant::react_to_arrest: Removing 2 coins from " << name() << " (had " << coins_ << " coins)" << endl;
                removeCoins(2);
                game_.addCoinsToBank(2); // add the coins to the bank
                cout << "DEBUG Merchant::react_to_arrest: Merchant " << name() << " now has " << coins() << " coins" << endl;
            }
            else
            {
                cout << "DEBUG Merchant::react_to_arrest: Merchant " << name() << " doesn't have enough coins (" << coins_ << ")" << endl;
            }
        }
        catch (const NotEnoughCoins &)
        {
            cout << "DEBUG Merchant::react_to_arrest: NotEnoughCoins exception thrown for " << name() << endl;
            throw InvalidOperation("Merchant cannot react to arrest");
        }
    }

    void Merchant::newTurnIncome()
    {
        if (coins_ > 3)
        {
            game_.removeCoinsFromBank(1);
            addCoins(1);
        }
        else
        {
            throw InvalidOperation("Merchant isn't eligible for new turn income");
        }
    }
}