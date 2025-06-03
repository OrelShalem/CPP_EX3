//orel8155@gmail.com
/**
 * @file Governor.cpp
 * @brief Implementation of the Governor role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the Governor role.
 * The Governor is a political leader who can collect taxes and cancel taxation actions.
 */

#include "Governor.hpp"
#include "../GameExceptions.hpp"
#include "../Game.hpp"

namespace coup
{
    /**
     * @brief Performs the Governor's special tax collection action
     * 
     * This method allows the Governor to collect 3 coins as tax.
     * If the Governor is blocked from economic actions or has 10 or more coins,
     * the action will fail with an appropriate exception.
     * 
     * @throws InvalidOperation if the Governor is blocked from economic actions
     * @throws InvalidOperation if the Governor has 10 or more coins (must coup instead)
     */
    void Governor::tax()
    {
        // Check if the Governor is currently blocked from performing economic actions
        if (blocked_from_economic_)
        {
            throw InvalidOperation("Governor is blocked from economic actions");
        }
        // Check if the Governor has 10 or more coins, in which case they must perform a coup
        if (coins_ >= 10){
            throw InvalidOperation("Governor must perform a coup he has more than 10 coins");
        }
        
        // Add 3 coins to the Governor's holdings as tax collection
        addCoins(3);
        // Record that the last action performed was 'tax'
        last_action_ = "tax";
        // No target for this action
        last_target_ = "";

        // End the Governor's turn and move to the next player
        game_.advanceTurn();
    }

    /**
     * @brief Cancels taxes collected by a target player
     * 
     * This method allows the Governor to cancel taxes collected by another player.
     * If the target is another Governor, 3 coins are removed. Otherwise, 2 coins are removed.
     * 
     * @param target The player whose tax collection is being canceled
     */
    void Governor::cancel_taxes(Player &target)
    {
        // Check if the target player is also a Governor
        if (target.role() == Role::GOVERNOR)
        {
            target.removeCoins(3); // Remove 3 coins from another Governor
        }
        else
        {
            target.removeCoins(2); // Remove 2 coins from a regular player
        }
    }

    /**
     * @brief Allows the Governor to undo certain game actions
     * 
     * This method gives the Governor the ability to undo tax actions performed by
     * the previous player. It verifies several conditions before allowing the undo.
     * 
     * @param action The type of action to undo (must be TAX for Governor)
     * @throws InvalidOperation if conditions for undoing are not met
     */
    void Governor::undo(UndoableAction action)
    {
        // Check if the action to undo is a tax action
        if (action == UndoableAction::TAX)
        {
            // Get a reference to the previous player who took a turn
            auto &previousPlayer = game_.getPreviousPlayer();
            // Check if there is a previous player available
            if (!previousPlayer)
            {
                throw InvalidOperation("No previous player found to undo tax");
            }
            
            // Important check: The Governor cannot undo their own tax action
            if (previousPlayer->name() == name_)
            {
                throw InvalidOperation("Governor cannot undo his own tax action");
            }
            
            // Verify that the previous player's last action was indeed a tax collection
            if (previousPlayer->get_last_action() != "tax")
            {
                throw InvalidOperation("Previous player's last action was not tax");
            }
            
            // Call the method to cancel taxes for the previous player
            cancel_taxes(*previousPlayer);
        }
        else
        {
            // If the action is not TAX, throw an exception as the Governor can only undo tax actions
            throw InvalidOperation("Governor can't undo this action");
        }
    }
}