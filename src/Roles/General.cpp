//orel8155@gmail.com
/**
 * @file General.cpp
 * @brief Implementation of the General role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the General role.
 * The General is a military officer who can block coup actions and has special
 * resistance to arrest.
 * 
 * @author Orel
 */

#include "General.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"
namespace coup
{
    /**
     * @brief Blocks a coup action performed by another player
     * 
     * This method allows the General to prevent a coup by paying 5 coins.
     * It returns the eliminated player back to the game.
     * 
     * @param target The player who performed the coup action to be blocked
     * @throws InsufficientCoins if the General doesn't have 5 coins
     * @throws InvalidOperation if there's no player to restore
     */
    void General::block_coup(Player &target)
    {
        // Check if the General has enough coins (5) to perform this action
        checkCoins(5);

        // Remove the coins from the General's holdings after confirming the action is valid
        removeCoins(5);

        // Find the player who was last eliminated by a coup and return them to the game
        auto victim = game_.getLastPlayerCouped();
        if (victim)
        {
            // Set the eliminated player back to active status
            victim->setActive(true);
            // Clear the arrested player reference in the game state
            game_.getArrestedPlayer() = nullptr;
        }
        else
        {
            // If no player was recently eliminated, throw an exception
            throw InvalidOperation("General can't undo this action");
        }
    }


    /**
     * @brief Undoes specific game actions based on the action type
     * 
     * This method allows the General to reverse certain actions in the game.
     * Currently only supports undoing coup actions.
     * 
     * @param action The type of action to undo (from UndoableAction enum)
     * @throws InvalidOperation if the General can't undo the specified action
     */
    void General::undo(UndoableAction action)
    {
        if (action == UndoableAction::COUP)
        {
            // If the action is a coup, call block_coup to reverse it
            block_coup(*game_.getCurrentPlayer());
        }
        else
        {
            // If the action is not a coup, throw an exception
            throw InvalidOperation("General can't undo this action");
        }
    }
}
