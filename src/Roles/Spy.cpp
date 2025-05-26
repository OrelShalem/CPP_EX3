// orel8155@gmail.com
#include "Spy.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"

namespace coup
{
    void Spy::block_arrest(Player &target)
    {
        // בדיקה שהפעולה האחרונה של השחקן היא מעצר
        if (target.get_last_action() != "arrest")
        {
            throw InvalidOperation("Can only block arrest action");
        }

        // בדיקה שקיימת פעולת מעצר ממתינה
        if (!game_.hasPending("arrest", target.name(), target.get_last_target()))
        {
            throw InvalidOperation("No pending arrest action to block");
        }

        // חסימת מעצר לתור הבא
        target.setBlocked(false, true);

        // ניקוי הפעולה הממתינה
        game_.clearPendingFor("arrest");
    }

    int Spy::view_coins(Player &target)
    {
        // חסימת מעצר לתור הבא
        block_arrest(target);

        return target.coins();
    }
}