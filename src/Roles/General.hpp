// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class General : public Player
    {
    public:
        General(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~General() override = default;

        void block_coup(Player &target) override;

        void react_to_arrest() override;
    };
}