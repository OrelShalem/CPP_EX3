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
        cout << "DEBUG: Judge::cancel_bribe called for " << target.name() << endl;
        // next turn
        game_.advanceTurn();
        cout << "Judge has undone the bribe action of " << target.name() << endl;
        cout << "DEBUG: Judge::cancel_bribe completed successfully" << endl;
    }

    void Judge::react_to_sanction()
    {
        cout << "DEBUG Judge::react_to_sanction: Judge " << name() << " reacting to sanction - throwing exception" << endl;
        throw InvalidOperation("Cannot sanction Judge, pay 1 coin back to the bank");
    }

    void Judge::undo(UndoableAction action)
    {
        if (action == UndoableAction::BRIBE)
        {
            cout << "DEBUG: Judge looking for player who bribed..." << endl;
            
            // חיפוש השחקן שביצע bribe במקום לסמוך על previous_player_
            shared_ptr<Player> briberPlayer = nullptr;
            auto players = game_.players();
            
            for (const auto& playerName : players) {
                auto player = game_.getPlayerByName(playerName);
                if (player->get_last_action() == "bribe") {
                    briberPlayer = player;
                    cout << "DEBUG: Found briber: " << player->name() << endl;
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
            
            cout << "DEBUG: Calling cancel_bribe for " << briberPlayer->name() << endl;
            cancel_bribe(*briberPlayer);
        }
        else
        {
            throw InvalidOperation("Judge cannot undo this action");
        }
    }

}