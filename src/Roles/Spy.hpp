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
        int view_coins(const Player &target) const;
        string role() const override;
    };
}