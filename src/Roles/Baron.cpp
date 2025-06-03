// orel8155@gmail.com
#include "Baron.hpp"
#include "../Game.hpp"
#include <iostream>
namespace coup
{

    void Baron::invest()
    {
        removeCoins(3);
        addCoins(6);

        last_action_ = "invest";
        last_target_ = "";

        game_.advanceTurn();
    }

    void Baron::react_to_sanction()
    {
        cout << "DEBUG Baron::react_to_sanction: Baron " << name() << " reacting to sanction, gaining 1 coin" << endl;
        addCoins(1);
        cout << "DEBUG Baron::react_to_sanction: Baron " << name() << " now has " << coins() << " coins" << endl;
    }
}
