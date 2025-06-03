// orel8155@gmail.com
/**
 * @file Judge.cpp
 * @brief Implementation of the Judge role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the Judge role.
 * The Judge is a judicial officer who can undo bribe actions and has immunity to sanctions.
 * 
 * @author Orel
 */
#include "Judge.hpp"
#include "../Game.hpp"
#include "../GameExceptions.hpp"
namespace coup
{
    /**
     * @brief Checks if the Judge can undo a specific action
     * 
     * This method determines whether the Judge has the authority to undo
     * a particular action based on its type. Currently, Judges can only
     * undo bribe actions.
     * 
     * @param action The name of the action to check
     * @return true if the action can be undone by a Judge, false otherwise
     */
    bool Judge::can_undo(const string &action) const
    {
        return action == "bribe";
    }

    /**
     * @brief Cancels a bribe action performed by another player
     * 
     * This method allows the Judge to undo a bribe action. After canceling
     * the bribe, the game advances to the next player's turn.
     * 
     * @param target The player whose bribe action is being canceled
     */
    void Judge::cancel_bribe(Player &target)
    {
        // After canceling the bribe, advance to the next player's turn
        game_.advanceTurn();
    }

    /**
     * @brief Defines how the Judge reacts when targeted by a sanction
     * 
     * This method implements the Judge's immunity to sanctions. When someone
     * attempts to sanction a Judge, the action fails and the sanctioning player
     * must pay 1 coin back to the bank as a penalty.
     * 
     * @throws InvalidOperation indicating that Judges cannot be sanctioned
     */
    void Judge::react_to_sanction()
    {
        throw InvalidOperation("Cannot sanction Judge, pay 1 coin back to the bank");
    }

    /**
     * @brief Allows the Judge to undo specific game actions
     * 
     * This method implements the Judge's special ability to undo bribe actions.
     * It searches for the player who performed the bribe, validates that the Judge
     * can legitimately undo the action, and then calls cancel_bribe to reverse it.
     * 
     * @param action The type of action to undo (from UndoableAction enum)
     * @throws InvalidOperation if the Judge cannot undo the specified action
     */
    void Judge::undo(UndoableAction action)
    {
        if (action == UndoableAction::BRIBE)
        {
            
            // Search for the player who performed a bribe instead of relying on previous_player_
            shared_ptr<Player> briberPlayer = nullptr;
            auto players = game_.players();
            
            // Iterate through all players to find who last performed a bribe action
            for (const auto& playerName : players) {
                auto player = game_.getPlayerByName(playerName);
                if (player->get_last_action() == "bribe") {
                    briberPlayer = player;
                    break;
                }
            }
            
            // Verify that a player who performed bribe was found
            if (!briberPlayer)
            {
                throw InvalidOperation("No player found who performed bribe");
            }
            
            // Verify that the Judge is not trying to undo their own bribe
            if (briberPlayer->name() == name_){
                throw InvalidOperation("Judge cannot undo his own bribe");
            }
            
            // Call the method to cancel the bribe for the identified player
            cancel_bribe(*briberPlayer);
        }
        else
        {
            // If the action is not a bribe, throw an exception as the Judge can only undo bribes
            throw InvalidOperation("Judge cannot undo this action");
        }
    }

}