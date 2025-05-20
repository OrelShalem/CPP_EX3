// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class Judge : public Player
    {
    public:
        Judge(Game &game, const string &name);
        ~Judge() override = default;

        bool can_undo(const string &action) const override;
        void handle_undo(Player &target) override;
        void react_to_sanction() override;

        string role() const override;
    };
}