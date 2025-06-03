// orel8155@gmail.com
#include "Judge.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
#include <iostream>
namespace coup
{

    bool Judge::can_undo(const string &action) const
    {
        return action == "bribe";
    }
    // undo bribe
    void Judge::cancel_bribe(Player &target)
    {
        // next turn
        game_.advanceTurn();
    }

    void Judge::react_to_sanction()
    {
        throw InvalidOperation("Cannot sanction Judge, pay 1 coin back to the bank");
    }

    void Judge::undo(UndoableAction action)
    {
        if (action == UndoableAction::BRIBE)
        {
            
            // חיפוש השחקן שביצע bribe במקום לסמוך על previous_player_
            shared_ptr<Player> briberPlayer = nullptr;
            auto players = game_.players();
            
            for (const auto& playerName : players) {
                auto player = game_.getPlayerByName(playerName);
                if (player->get_last_action() == "bribe") {
                    briberPlayer = player;
                    break;
                }
            }
            
            if (!briberPlayer)
            {
                throw InvalidOperation("No player found who performed bribe");
            }
            if (briberPlayer->name() == name_){
                throw InvalidOperation("Judge cannot undo his own bribe");
            }
            
            cancel_bribe(*briberPlayer);
        }
        else
        {
            throw InvalidOperation("Judge cannot undo this action");
        }
    }

}