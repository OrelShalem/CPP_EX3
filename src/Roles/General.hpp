// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class General : public Player
    {
    private:
        void block_coup(Player &target);
    public:
        General(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~General() override = default;

        

        void react_to_arrest() override;
        void undo(UndoableAction action) override;
    };
}