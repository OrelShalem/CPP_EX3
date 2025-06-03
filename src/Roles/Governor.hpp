// orel8155@gmail.com
#pragma once  // Ensures this header file is included only once during compilation

#include "../Player.hpp"  // Include the base Player class that Governor inherits from

#include <string>  // Include the string library for string operations

namespace coup
{
    /**
     * @brief Governor class represents the Governor role in the Coup card game
     * 
     * The Governor is a political leader with special abilities:
     * - Can collect taxes (3 coins) as a special economic action
     * - Can cancel taxation actions performed by other players
     * - Can undo certain tax-related actions in the game
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the Governor's special abilities.
     */
    class Governor : public Player
    {
    private:
        /**
         * @brief Cancels taxes collected by a target player
         * 
         * This method allows the Governor to cancel taxes collected by another player.
         * If the target is another Governor, 3 coins are removed. Otherwise, 2 coins are removed.
         * 
         * @param target The player whose tax collection is being canceled
         */
        void cancel_taxes(Player &target);
    public:
        /**
         * @brief Constructor for the Governor class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be Governor)
         */
        Governor(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the Governor class
         * Uses the default implementation
         */
        ~Governor() override = default;
        
        /**
         * @brief Performs the Governor's special tax collection action
         * 
         * This method allows the Governor to collect 3 coins as tax.
         * Overrides the base class tax method with Governor-specific implementation.
         */
        void tax() override;
        
        /**
         * @brief Allows the Governor to undo certain tax-related game actions
         * 
         * This method gives the Governor the ability to undo tax actions performed by
         * other players, canceling their effects.
         * 
         * @param action The type of action to undo (should be TAX for Governor)
         */
        void undo(UndoableAction action) override;
    };
}