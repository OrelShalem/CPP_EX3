// orel8155@gmail.com
#include "Merchant.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"


namespace coup
{

    
    void Merchant::react_to_arrest()
    {
        try
        {
            if (coins_ >= 2)
            {
                removeCoins(2);
                game_.addCoinsToBank(2); // add the coins to the bank
            }
        }
        catch (const NotEnoughCoins &)
        {
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