#include "Governor.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"

namespace coup
{

    void Governor::tax()
    {

        if (blocked_from_economic_)
        {
            throw InvalidOperation("Governor is blocked from economic actions");
        }
        if (coins_ >= 10){
            throw InvalidOperation("Governor must perform a coup he has more than 10 coins");
        }
        addCoins(3);
        last_action_ = "tax";
        last_target_ = "";


        game_.advanceTurn();
    }

    void Governor::cancel_taxes(Player &target)
    {

        if (target.role() == Role::GOVERNOR)
        {
            target.removeCoins(3); // governor remove 3 coins
        }
        else
        {
            target.removeCoins(2); // regular player remove 2 coins
        }
    }

  
    void Governor::undo(UndoableAction action)
    {
        
        if (action == UndoableAction::TAX )
        {
            
            // בדיקה שיש שחקן קודם
            auto &previousPlayer = game_.getPreviousPlayer();
            if (!previousPlayer)
            {
                throw InvalidOperation("No previous player found to undo tax");
            }
            
            
            // *** בדיקה חשובה: הGovernor לא יכול לבטל tax לעצמו ***
            if (previousPlayer->name() == name_)
            {
                throw InvalidOperation("Governor cannot undo his own tax action");
            }
            
            // בדיקה שהפעולה האחרונה של השחקן הקודם הייתה tax
            if (previousPlayer->get_last_action() != "tax")
            {
                throw InvalidOperation("Previous player's last action was not tax");
            }
            
            cancel_taxes(*previousPlayer);
            
        }
        else
        {
            throw InvalidOperation("Governor can't undo this action");
        }
    }

}