#pragma once

#include "../Player.hpp"

#include <string>

namespace coup
{
    class Governor : public Player
    {
    private:
        void cancel_taxes(Player &target);
    public:
        Governor(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Governor() override = default;
        void tax() override;
        
        void undo(UndoableAction action) override;
    };
}