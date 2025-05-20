// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    class General : public Player
    {
    public:
        General(Game &game, const string &name);
        ~General() override = default;

        void block_coup(Player &target);

        void react_to_arrest() override;

        string role() const override;
    };
}