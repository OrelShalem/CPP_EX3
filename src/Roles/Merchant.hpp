// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class Merchant : public Player
    {
    public:
        Merchant(Game &game, const string &name);
        ~Merchant() override = default;

        void gather() override;

        void react_to_arrest() override;

        string role() const override;
    };
}