// orel8155@gmail.com
#include "Spy.hpp"
#include "../Game.hpp"

namespace coup
{
    
    int Spy::view_coins(Player &target)
    {
        // חסימת מעצר לתור הבא
        block_arrest(target);

        return target.coins();
    }
}