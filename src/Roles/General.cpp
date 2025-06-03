// orel8155@gmail.com
#include "General.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"
namespace coup
{

    void General::block_coup(Player &target)
    {
        

        // בדיקה שיש מספיק מטבעות
        checkCoins(5);

        // מורידים מטבעות רק לאחר שוידאנו שהפעולה חוקית
        removeCoins(5);

        // מחפשים את השחקן ומחזירים אותו למשחק
        auto victim = game_.getLastPlayerCouped();
        if (victim)
        {
            victim->setActive(true);
            game_.getArrestedPlayer() = nullptr; // מנקים את השחקן שנעצר
        }
        else
        {
            throw InvalidOperation("General can't undo this action");
        }
        
    }

    void General::react_to_arrest()
    {
       
    }

    void General::undo(UndoableAction action)
    {
        if (action == UndoableAction::COUP)
        {
            block_coup(*game_.getCurrentPlayer());
        }
        else
        {
            throw InvalidOperation("General can't undo this action");
        }
    }
}
