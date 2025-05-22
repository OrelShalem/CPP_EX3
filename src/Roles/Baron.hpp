// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class Baron : public Player
    {
    public:
        Baron(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Baron() override = default;

        void invest();

        void react_to_sanction() override;
    };
}