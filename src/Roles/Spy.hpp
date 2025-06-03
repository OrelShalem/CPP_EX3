#pragma once

#include "../Player.hpp"

namespace coup
{
    class Spy : public Player
    {
    private:
        void block_arrest(Player &target) { target.get_blocked_from_arresting() = true; }

    public:
        Spy(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Spy() override = default;

        /**
         * חוסם את היכולת של השחקן לבצע מעצר בתור הבא שלו
         * @param target השחקן שיחסם
         */
        

        /**
         * מאפשר למרגל לראות את מספר המטבעות של שחקן אחר
         * וגם חוסם את יכולת המעצר שלו לתור הבא
         * @param target השחקן שרוצים לראות את המטבעות שלו
         * @return מספר המטבעות של השחקן
         */
        int view_coins(Player &target) override;
    };
}