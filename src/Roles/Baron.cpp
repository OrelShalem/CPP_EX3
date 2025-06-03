//orel8155@gmail.com

/**
 * @file Baron.cpp
 * @brief Implementation file for the Baron role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the Baron role.
 * The Baron is a wealthy businessman who can invest money for profit
 * and has special resistance to economic sanctions.
 */

#include "Baron.hpp"     // Include the Baron class header
#include "../Game.hpp"   // Include the Game class header for game state management

namespace coup
{
    /**
     * @brief Performs the Baron's special 'invest' action
     * 
     * This function allows the Baron to invest 3 coins and receive 6 coins in return,
     * effectively doubling their investment. After the action is performed,
     * the turn advances to the next player.
     * 
     * The action flow:
     * 1. Remove 3 coins from the Baron's holdings
     * 2. Add 6 coins to the Baron's holdings (net gain of 3)
     * 3. Record the action for history tracking
     * 4. Advance to the next player's turn
     */
    void Baron::invest()
    {
        removeCoins(3);      // Pay 3 coins for the investment
        addCoins(6);         // Receive 6 coins back (net profit of 3)

        last_action_ = "invest";  // Record that the last action was 'invest'
        last_target_ = "";        // No target for this action

        game_.advanceTurn();      // End turn and move to next player
    }

    /**
     * @brief Special reaction to economic sanctions
     * 
     * This function defines how the Baron reacts when economic sanctions are imposed.
     * Due to their economic influence, the Baron gains 1 coin when sanctioned,
     * instead of being negatively affected like other roles might be.
     */
    void Baron::react_to_sanction()
    {
        addCoins(1);     // Baron gains 1 coin when sanctioned
    }
}
