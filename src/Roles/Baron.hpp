//orel8155@gmail.com
#pragma once

#include "../Player.hpp"

namespace coup
{
    /**
     * @brief Baron class represents the Baron role in the Coup card game
     * 
     * The Baron is a character with special economic abilities:
     * - Can invest coins to gain more coins
     * - Has a unique reaction to economic sanctions
     * 
     * This class inherits from the base Player class and overrides
     * specific methods to implement the Baron's special abilities.
     */
    class Baron : public Player
    {
    public:
        /**
         * @brief Constructor for the Baron class
         * 
         * @param game Reference to the Game object that manages the game state
         * @param name The name of the player
         * @param role The role of the player (should be Baron)
         */
        Baron(Game &game, const string &name, Role role) : Player(game, name, role) {};
        
        /**
         * @brief Destructor for the Baron class
         * Uses the default implementation
         */
        ~Baron() override = default;

        /**
         * @brief Performs the Baron's special investment action
         * 
         * Allows the Baron to invest coins and receive more in return,
         * effectively generating profit. This is a unique ability
         * that only the Baron role can perform.
         */
        void invest() override;

        /**
         * @brief Defines how the Baron reacts to economic sanctions
         * 
         * Due to the Baron's economic influence, they react differently
         * to sanctions compared to other roles. This method implements
         * that special reaction.
         */
        void react_to_sanction() override;
    };
}