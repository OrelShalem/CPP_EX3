#pragma once

#include "../Player.hpp"

namespace coup
{
    class Governor : public Player
    {
    public:
        Governor(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Governor() override = default;

        void tax() override;
        void react_to_arrest() override;

        bool can_undo(const string &action) const override;
        void handle_undo(Player &target) override;
    };

}