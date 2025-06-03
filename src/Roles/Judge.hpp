// orel8155@gmail.com
/**
 * @file Judge.hpp
 * @brief Header file for the Judge role in the Coup card game
 * 
 * This file contains the declaration of the Judge class which represents
 * a judicial role in the game with special abilities related to bribe actions
 * and immunity to sanctions.
 */
#pragma once  // Ensures this header file is included only once during compilation

#include "../Player.hpp"  // Include the base Player class that Judge inherits from

namespace coup
{
    /**
     * @brief Judge class represents the Judge role in the Coup card game
     * 
     * The Judge is a judicial officer with special abilities:
     * - Can undo bribe actions performed by other players
     * - Has immunity to sanctions imposed by other players
     * - Maintains judicial authority in the game
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the Judge's special abilities.
     */
    class Judge : public Player
    {
    private:
        /**
         * @brief Cancels a bribe action performed by a target player
         * 
         * This method allows the Judge to undo a bribe action taken by another player.
         * 
         * @param target The player whose bribe action is being canceled
         */
        void cancel_bribe(Player &target);
    public:
        /**
         * @brief Constructor for the Judge class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be Judge)
         */
        Judge(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the Judge class
         * Uses the default implementation
         */
        ~Judge() override = default;

        /**
         * @brief Checks if the Judge can undo a specific action
         * 
         * This method determines if the Judge has the authority to undo
         * a particular action based on its type.
         * 
         * @param action The name of the action to check
         * @return true if the action can be undone by a Judge, false otherwise
         */
        bool can_undo(const string &action) const;
        
        /**
         * @brief Defines how the Judge reacts when targeted by a sanction
         * 
         * This method implements the Judge's immunity to sanctions. It overrides
         * the base class method to provide Judge-specific behavior.
         */
        void react_to_sanction() override;
        
        /**
         * @brief Allows the Judge to undo specific game actions
         * 
         * This method gives the Judge the ability to undo actions performed by
         * other players, particularly bribe actions.
         * 
         * @param action The type of action to undo (from UndoableAction enum)
         */
        void undo(UndoableAction action) override;
    };
}