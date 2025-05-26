// orel8155@gmail.com
#include "General.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"
namespace coup
{

    void General::block_coup(Player &target)
    {
        // בדיקה שהשחקן שביצע את ה-coup אכן ביצע coup
        if (target.get_last_action() != "coup")
        {
            throw InvalidOperation("Can only block coup action");
        }

        // בדיקה שיש מספיק מטבעות
        checkCoins(5);

        // בדיקה שקיימת פעולת coup ממתינה
        string victim_name = target.get_last_target();

        // מוודאים שהשחקן ביצע coup בתור האחרון שלו
        if (!game_.hasPending("coup", target.name(), victim_name))
        {
            throw InvalidOperation("No pending coup action to block");
        }

        // מורידים מטבעות רק לאחר שוידאנו שהפעולה חוקית
        removeCoins(5);

        // מחפשים את השחקן ומחזירים אותו למשחק
        try
        {
            // מוצאים את השחקן שהופל ומפעילים אותו מחדש
            auto victim = game_.getPlayerByName(victim_name);
            victim->setActive(true);
        }
        catch (const std::exception &e)
        {
            throw InvalidOperation("Failed to restore player: " + string(e.what()));
        }

        // ניקוי הפעולה הממתינה
        game_.clearPendingFor(target.name());
    }

    void General::react_to_arrest()
    {
        addCoins(1);
    }

}