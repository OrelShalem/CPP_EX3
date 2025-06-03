//orel8155@gmail.com

/**
 * @file Player.hpp
 * @brief Header file defining the Player class for the Coup card game
 * @details This file contains the base Player class which all specific role classes inherit from
 */

#pragma once  // Ensure this header is only included once during compilation
#include <string>   // For string manipulation
#include <memory>   // For smart pointers (shared_ptr)
#include "GameExceptions.hpp"  // For game-specific exceptions
using namespace std;  // Using the standard namespace

namespace coup
{
    // Forward declaration of Game class to avoid circular dependency
    class Game;

    /**
     * @enum Role
     * @brief Defines the possible character roles in the game
     */
    enum class Role 
    {
        GENERAL,   // Military leader role
        GOVERNOR,  // Political leader role
        SPY,       // Intelligence agent role
        BARON,     // Aristocratic role
        JUDGE,     // Legal authority role
        MERCHANT   // Business role
    };

    /**
     * @enum UndoableAction
     * @brief Defines the actions that can be potentially undone in the game
     */
    enum class UndoableAction
    {
        GATHER,    // Collecting coins
        TAX,       // Collecting tax
        BRIBE,     // Bribing another player
        ARREST,    // Arresting another player
        SANCTION,  // Imposing sanctions
        COUP       // Eliminating another player
    };

    /**
     * @class Player
     * @brief Base class for all player types in the Coup game
     * @details Provides common functionality for all roles and defines the interface
     * that specific role classes must implement
     */
    class Player
    {
    protected:
        Game &game_;                   // Reference to the game instance
        string name_;                  // Player's name
        int coins_;                    // Player's coin count
        bool active_;                  // Whether player is active in the game
        bool blocked_from_economic_;   // Whether player is blocked from economic actions
        bool blocked_from_arresting_;  // Whether player is blocked from arresting
        string last_action_;           // Player's most recent action
        string last_target_;           // Target of player's most recent action
        Role role_;                    // Player's role in the game
    public:
        /**
         * @brief Constructor for Player
         * @param game Reference to the Game instance
         * @param name Player's name
         * @param role Player's role
         */
        Player(Game &game, const string &name, Role role);
        
        /**
         * @brief Virtual destructor for proper cleanup of derived classes
         */
        virtual ~Player() = default;

        // Basic game action functions
        /**
         * @brief Collect one coin from the treasury
         */
        void gather();
        
        /**
         * @brief Collect taxes (role-specific implementation)
         */
        virtual void tax();
        
        /**
         * @brief Attempt to bribe another player (role-specific)
         */
        virtual void bribe();
        
        /**
         * @brief Arrest another player (role-specific)
         * @param player Pointer to the player being arrested
         */
        virtual void arrest(shared_ptr<Player> &player);
        
        /**
         * @brief Impose sanctions on another player (role-specific)
         * @param player Reference to the player being sanctioned
         */
        virtual void sanction(Player &player);
        
        /**
         * @brief Eliminate another player from the game
         * @param player Pointer to the player being eliminated
         */
        void coup(shared_ptr<Player> &player);
        
        /**
         * @brief Special income at the start of a turn (role-specific)
         */
        virtual void newTurnIncome() { throw InvalidOperation("Player doesn't have new turn income"); }
        
        /**
         * @brief Response when player is arrested (role-specific)
         */
        virtual void react_to_arrest() {}
        
        /**
         * @brief Response when player is sanctioned (role-specific)
         */
        virtual void react_to_sanction() {}
        
        /**
         * @brief Undo a previous action (role-specific)
         * @param action The action to undo
         */
        virtual void undo(UndoableAction action) { throw InvalidOperation("Player doesn't have undo function"); }
        
        /**
         * @brief View another player's coins (role-specific)
         * @param target The player whose coins are being viewed
         * @return The number of coins the target has
         */
        virtual int view_coins(Player &target) { throw InvalidOperation("Player doesn't have view coins function"); }
        
        /**
         * @brief Make an investment (role-specific)
         */
        virtual void invest() { throw InvalidOperation("Player doesn't have invest function"); }

        // Information accessor methods
        /**
         * @brief Get the player's coin count
         * @return Number of coins the player has
         */
        int coins() const
        {
            return coins_;
        }
        
        /**
         * @brief Set the player's coin count
         * @param amount The new coin amount
         */
        void setCoins(int amount) { coins_ = amount; }
        
        /**
         * @brief Get the player's name
         * @return Player's name as a string
         */
        string name() const
        {
            return name_;
        }
        
        /**
         * @brief Check if the player is active in the game
         * @return True if player is active, false otherwise
         */
        bool isActive() const
        {
            return active_;
        }
        
        /**
         * @brief Check if player is blocked from economic actions
         * @return True if blocked, false otherwise
         */
        bool blocked_from_economic() const { return blocked_from_economic_; }
        
        /**
         * @brief Get reference to the arrest blocking status
         * @return Reference to the blocked_from_arresting_ flag
         */
        bool& get_blocked_from_arresting() { return blocked_from_arresting_; }
        
        /**
         * @brief Get the player's last action
         * @return Reference to the last action string
         */
        string &get_last_action()
        {
            return last_action_;
        }
        
        /**
         * @brief Get the target of the player's last action
         * @return Reference to the last target string
         */
        string &get_last_target()
        {
            return last_target_;
        }

        /**
         * @brief Get the player's role
         * @return The player's role enum value
         */
        Role role() const
        {
            return role_;
        }

        // Internal utility functions
        /**
         * @brief Set whether the player is blocked from economic actions
         * @param blocked True to block, false to unblock
         */
        void setBlockedFromEconomic(bool blocked)
        {
            blocked_from_economic_ = blocked;
        }
        
        /**
         * @brief Set whether the player is active in the game
         * @param active True for active, false for inactive
         */
        void setActive(bool active){
            active_ = active;
        }
        
        /**
         * @brief Add coins to the player
         * @param amount Number of coins to add
         */
        void addCoins(int amount){
            coins_ += amount;
        }
        
        /**
         * @brief Remove coins from the player
         * @param amount Number of coins to remove
         * @throws NotEnoughCoins if player has insufficient funds
         */
        void removeCoins(int amount){
            if (coins_ < amount)
            {
                throw NotEnoughCoins("You do not have enough coins to remove");
            }
            coins_ -= amount;
        }

        // Validation functions
        /**
         * @brief Check if it's this player's turn
         * @throws InvalidTurn if it's not the player's turn
         */
        void checkTurn() const;
        
        /**
         * @brief Check if player has enough coins for an action
         * @param amount Minimum coins required
         * @throws NotEnoughCoins if player has insufficient funds
         */
        void checkCoins(int amount) const
        {
            if (coins_ < amount)
            {
                throw NotEnoughCoins("You do not have enough coins to perform this action");
            }
        }
        
        /**
         * @brief Check if player must perform a coup (has 10+ coins)
         * @return True if player must coup, false otherwise
         */
        bool mustPerformCoup() const { return coins_ >= 10; }
    };
}