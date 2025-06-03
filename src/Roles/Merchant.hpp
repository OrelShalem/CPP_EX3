// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class Merchant : public Player
    {
    public:
        Merchant(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Merchant() override = default;
        
        void newTurnIncome() override;
        void react_to_arrest() override;
    };
}