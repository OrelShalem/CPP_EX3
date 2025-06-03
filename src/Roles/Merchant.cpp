// orel8155@gmail.com
/**
 * @file Merchant.cpp
 * @brief Implementation of the Merchant role in the Coup card game
 * 
 * This file contains the implementation of special abilities for the Merchant role.
 * The Merchant is a wealthy trader who has special economic abilities and reactions.
 * 
 * @author Orel
 */
#include "Merchant.hpp"  // Include the Merchant class header
#include "../Game.hpp"   // Include the Game class for game state management
#include "../GameExceptions.hpp"  // Include custom exceptions for the game


namespace coup
{
    /**
     * @brief Defines how the Merchant reacts when arrested
     * 
     * This method implements the Merchant's response to being arrested.
     * When arrested, the Merchant pays a 2-coin fine to the bank if they have
     * enough coins. If they don't have enough coins, they can't properly react
     * to the arrest and an exception is thrown.
     * 
     * This represents the Merchant's ability to use wealth to mitigate legal problems.
     */
    void Merchant::react_to_arrest()
    {
        try
        {
            // Check if the Merchant has enough coins to pay the arrest fine
            if (coins_ >= 2)
            {
                removeCoins(2);  // Remove 2 coins from the Merchant
                game_.addCoinsToBank(2); // Add the 2 coins to the game's bank
            }
        }
        catch (const NotEnoughCoins &)
        {
            // If the Merchant doesn't have enough coins, they can't properly react
            throw InvalidOperation("Merchant cannot react to arrest");
        }
    }

    /**
     * @brief Provides additional income to the Merchant at the start of their turn
     * 
     * This method implements the Merchant's special ability to earn passive income.
     * If the Merchant has more than 3 coins, they receive an additional coin from
     * the bank at the start of their turn, representing business profits.
     * 
     * @throws InvalidOperation if the Merchant doesn't have enough coins for the passive income
     */
    void Merchant::newTurnIncome()
    {
        // Check if the Merchant has enough coins to qualify for passive income
        if (coins_ > 3)
        {
            game_.removeCoinsFromBank(1);  // Take 1 coin from the bank
            addCoins(1);  // Add the coin to the Merchant's holdings
        }
        else
        {
            // If the Merchant doesn't have enough coins, they can't receive passive income
            throw InvalidOperation("Merchant isn't eligible for new turn income");
        }
    }
}