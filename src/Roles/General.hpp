// orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    /**
     * @brief General class represents the General role in the Coup card game
     * 
     * The General is a military officer with special abilities:
     * - Can block coup actions performed by other players
     * - Has special reaction to arrest attempts
     * - Can undo certain actions in the game
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the General's special abilities.
     */
    class General : public Player
    {
    private:
        /**
         * @brief Blocks a coup action performed by another player
         * 
         * This method allows the General to prevent a coup by paying coins.
         * It returns the eliminated player back to the game.
         * 
         * @param target The player who performed the coup action to be blocked
         */
        void block_coup(Player &target);
    public:
        /**
         * @brief Constructor for the General class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be General)
         */
        General(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the General class
         * Uses the default implementation
         */
        ~General() override = default;

        
        /**
         * @brief Allows the General to undo certain game actions
         * 
         * This method gives the General the ability to reverse specific
         * types of actions that have occurred in the game.
         * 
         * @param action The type of action to undo
         */
        void undo(UndoableAction action) override;
    };
}