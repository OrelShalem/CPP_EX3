// orel8155@gmail.com
/**
 * @file Merchant.hpp
 * @brief Header file for the Merchant role in the Coup card game
 * 
 * This file contains the declaration of the Merchant class which represents
 * a wealthy trader role in the game with special economic abilities.
 */
#pragma once  // Ensures this header file is included only once during compilation

#include "../Player.hpp"  // Include the base Player class that Merchant inherits from

namespace coup
{
    /**
     * @brief Merchant class represents the Merchant role in the Coup card game
     * 
     * The Merchant is a wealthy trader with special abilities:
     * - Can earn passive income at the start of their turn
     * - Has special reactions to being arrested
     * - Maintains economic advantages in the game
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the Merchant's special abilities.
     */
    class Merchant : public Player
    {
    public:
        /**
         * @brief Constructor for the Merchant class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be Merchant)
         */
        Merchant(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the Merchant class
         * Uses the default implementation
         */
        ~Merchant() override = default;
        
        /**
         * @brief Provides additional income to the Merchant at the start of their turn
         * 
         * This method overrides the base class method to implement the Merchant's
         * special ability to earn passive income when they have more than 3 coins.
         */
        void newTurnIncome() override;
        
        /**
         * @brief Defines how the Merchant reacts when arrested
         * 
         * This method overrides the base class method to implement the Merchant's
         * special reaction to being arrested - paying a fine from their wealth.
         */
        void react_to_arrest() override;
    };
}