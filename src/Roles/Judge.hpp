// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class Judge : public Player
    {
    private:
        void cancel_bribe(Player &target);
    public:
        Judge(Game &game, const string &name, Role role) : Player(game, name, role) {};
        ~Judge() override = default;

        bool can_undo(const string &action) const;
        
        void react_to_sanction() override;
        void undo(UndoableAction action) override;
    };
}