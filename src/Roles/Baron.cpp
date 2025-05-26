// orel8155@gmail.com
#include "Baron.hpp"
#include "../Game.hpp"
namespace coup
{

    void Baron::invest()
    {
        checkAndClearSanction();
        checkTurn();
        checkCoins(3);

        removeCoins(3);
        addCoins(6);

        last_action_ = "invest";
        last_target_ = "";

        game_.registerPendingAction("invest", name_, "");

        game_.advanceTurn();
    }

    void Baron::react_to_sanction()
    {
        addCoins(1);
    }
}
