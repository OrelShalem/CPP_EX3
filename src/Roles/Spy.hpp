#pragma once

#include "../Player.hpp"

namespace coup
{
    class Spy : public Player
    {
    public:
        Spy(Game &game, const string &name);
        ~Spy() override = default;

        void block_arrest(Player &target);

        string role() const override;
    };
}