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
        cout << "DEBUG: Governor attempting to undo action" << endl;
        
        if (action == UndoableAction::TAX )
        {
            cout << "DEBUG: Undoing TAX action" << endl;
            
            // בדיקה שיש שחקן קודם
            auto &previousPlayer = game_.getPreviousPlayer();
            if (!previousPlayer)
            {
                cout << "DEBUG: No previous player found" << endl;
                throw InvalidOperation("No previous player found to undo tax");
            }
            
            cout << "DEBUG: Previous player: " << previousPlayer->name() << endl;
            cout << "DEBUG: Current Governor: " << name_ << endl;
            
            // *** בדיקה חשובה: הGovernor לא יכול לבטל tax לעצמו ***
            if (previousPlayer->name() == name_)
            {
                cout << "DEBUG: Governor cannot undo his own tax action" << endl;
                throw InvalidOperation("Governor cannot undo his own tax action");
            }
            
            // בדיקה שהפעולה האחרונה של השחקן הקודם הייתה tax
            cout << "DEBUG: Previous player's last action: " << previousPlayer->get_last_action() << endl;
            if (previousPlayer->get_last_action() != "tax")
            {
                cout << "DEBUG: Previous player's last action was not tax" << endl;
                throw InvalidOperation("Previous player's last action was not tax");
            }
            
            cout << "DEBUG: Cancelling taxes for previous player: " << previousPlayer->name() << endl;
            cancel_taxes(*previousPlayer);
            
            cout << "DEBUG: Successfully cancelled taxes for " << previousPlayer->name() << endl;
        }
        else
        {
            cout << "DEBUG: Invalid action type for Governor undo: " << static_cast<int>(action) << endl;
            throw InvalidOperation("Governor can't undo this action");
        }
    }

}