// orel8155@gmail.com
/**
 * @file Spy.hpp
 * @brief Header file for the Spy role in the Coup card game
 * 
 * This file contains the declaration of the Spy class which represents
 * an intelligence agent role in the game with special information gathering abilities.
 */
#pragma once  // Ensures this header file is included only once during compilation

#include "../Player.hpp"  // Include the base Player class that Spy inherits from

namespace coup
{
    /**
     * @brief Spy class represents the Spy role in the Coup card game
     * 
     * The Spy is an intelligence agent with special abilities:
     * - Can view other players' coin counts
     * - Can block other players from arresting in their next turn
     * - Specializes in gathering information and providing cover
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the Spy's special abilities.
     */
    class Spy : public Player
    {
    private:
        /**
         * @brief Blocks a player from arresting anyone in their next turn
         * 
         * This private method sets a flag on the target player that prevents
         * them from making arrest actions in their next turn.
         * 
         * @param target The player to block from arresting
         */
        void block_arrest(Player &target) { target.get_blocked_from_arresting() = true; }

    public:
        /**
         * @brief Constructor for the Spy class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be Spy)
         */
        Spy(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the Spy class
         * Uses the default implementation
         */
        ~Spy() override = default;

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
        int view_coins(Player &target) override;
    };
}