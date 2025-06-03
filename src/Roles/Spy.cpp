// orel8155@gmail.com
/**
 * @file Spy.cpp
 * @brief Implementation of the Spy role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the Spy role.
 * The Spy is an intelligence agent who can secretly gather information about other players.
 * 
 * @author Orel
 */
#include "Spy.hpp"
#include "../Game.hpp"

namespace coup
{
    /**
     * @brief Allows the Spy to view another player's coin count
     * 
     * This method implements the Spy's special ability to secretly check how many
     * coins another player has. As a side effect, this action also blocks the target
     * from being arrested in the next turn, representing the Spy providing cover.
     * 
     * @param target The player whose coin count is being viewed
     * @return The number of coins the target player currently has
     */
    int Spy::view_coins(Player &target)
    {
        // Block the target from being arrested in the next turn
        block_arrest(target);

        // Return the target player's coin count
        return target.coins();
    }
}