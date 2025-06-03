//orel8155@gmail.com

/**
 * @file Player.cpp
 * @brief Implementation file for the Player class in the Coup card game
 */

#include "Player.hpp"    // Include the Player class header
#include "Game.hpp"      // Include the Game class header
#include "GameExceptions.hpp" // Include custom exceptions

namespace coup
{
    /**
     * Constructor for Player class
     * Initializes a new player with the given game, name, and role
     * 
     * @param game Reference to the game instance
     * @param name The player's name
     * @param role The role of the player (General, Governor, etc.)
     */
    Player::Player(Game &game, const string &name, Role role)
        : game_(game), name_(name), coins_(0), active_(true), blocked_from_economic_(false),
          blocked_from_arresting_(false), last_action_(""), last_target_(""), role_(role)
    {
    }

    /**
     * Checks if it's the player's turn and if they're active
     * Throws exceptions if conditions aren't met
     */
    void Player::checkTurn() const
    {
        // Verify it's this player's turn based on role
        if (game_.turn() != role_)
        {
            throw InvalidTurn("It is not your turn");
        }

        // Verify the player is still active in the game
        if (!active_)
        {
            throw InvalidOperation("You are not active");
        }
    }

    /**
     * Gather action - collect one coin from the treasury
     * Basic economic action available to all roles
     */
    void Player::gather()
    {
        // Players with 10 or more coins must coup instead of gathering
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        
        checkTurn(); // Verify it's this player's turn
        
        // Check if player is under economic sanctions
        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        addCoins(1); // Add one coin to player's balance
        last_action_ = "gather"; // Record the action taken
        last_target_ = ""; // No target for gather action

        // Clear previous action from game state

        game_.advanceTurn(); // Move to the next player's turn
    }

    /**
     * Tax action - collect two coins from the treasury
     * Basic economic action available to all roles
     */
    void Player::tax()
    {
        // Players with 10 or more coins must coup instead of taxing
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        
        checkTurn(); // Verify it's this player's turn
        
        // Check if player is under economic sanctions
        if (blocked_from_economic_)
        {
            throw InvalidOperation("You are blocked from economic actions");
        }

        game_.removeCoinsFromBank(2); // Remove 2 coins from the game bank
        addCoins(2); // Add two coins to player's balance
        last_action_ = "tax"; // Record the action taken
        last_target_ = ""; // No target for tax action

        // Record the action in the player's action map

        game_.advanceTurn(); // Move to the next player's turn
    }

    /**
     * Bribe action - pay coins to gain advantage
     * Costs 4 coins and removes economic sanctions
     */
    void Player::bribe()
    {
        // Players with 10 or more coins must coup instead of bribing
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }
        
        // Check if player has enough coins for bribe
        if (coins_ < 4)
        {
            throw InvalidOperation("You do not have enough coins to bribe");
        }
        
        removeCoins(4); // Pay 4 coins for the bribe
        last_action_ = "bribe"; // Record the action taken
        last_target_ = ""; // No target for bribe action

        // If the player is under economic sanctions, remove them
        if (game_.getPlayerByName(name_)->blocked_from_economic())
        {
            game_.getPlayerByName(name_)->setBlockedFromEconomic(false);
        }
    }

    /**
     * Arrest action - arrests another player
     * Effects vary based on the arrested player's role
     * 
     * @param target Pointer to the player being arrested
     */
    void Player::arrest(shared_ptr<Player> &target)
    {
        // Check if player is blocked from arresting
        if (blocked_from_arresting_)
        {
            throw InvalidOperation("You are blocked from arrest actions");
        }

        // Check if the target is already arrested
        if (game_.getArrestedPlayerName() == target->name())
        {
            throw InvalidOperation("You cannot arrest the same player twice in a row");
        }

        // Players with 10 or more coins must coup instead of arresting
        if (coins_ >= 10)
        {
            throw InvalidOperation("Must perform coup when you have 10 or more coins");
        }

        // Record the action and target
        last_action_ = "arrest";
        last_target_ = target->name();
        game_.setArrestedPlayerName(target->name());

        // Allow the target to react to being arrested
        target->react_to_arrest();

        // Generals and Merchants have special immunity to monetary effects
        if (target->role() != Role::GENERAL && target->role() != Role::MERCHANT)
        {
            target->removeCoins(1); // Take 1 coin from target
            addCoins(1); // Add 1 coin to arresting player
        }

        game_.advanceTurn(); // Move to the next player's turn
    }

    /**
     * Sanction action - imposes economic sanctions on another player
     * Costs 3 coins and blocks target from economic actions
     * 
     * @param target Reference to the player being sanctioned
     */
    void Player::sanction(Player &target)
    {
        // Players with 10 or more coins must coup instead of sanctioning
        if (coins_ >= 10)
        {
            throw InvalidOperation("You must perform a coup when you have 10 or more coins");
        }
        
        checkCoins(3); // Verify player has at least 3 coins
        
        // Pay for the sanction
        removeCoins(3);
        game_.addCoinsToBank(3); // Return coins to the game bank
        
        // Record the action and target
        last_action_ = "sanction";
        last_target_ = target.name();

        // Apply economic sanctions to the target
        target.setBlockedFromEconomic(true);

        // Allow the target to react to sanctions
        try 
        {
            target.react_to_sanction();
        }
        catch (const InvalidOperation &e)
        {
            // Additional penalty if reaction fails
            removeCoins(1);
            game_.addCoinsToBank(1); // Return coin to the game bank
        }

        game_.advanceTurn(); // Move to the next player's turn
    }

    /**
     * Coup action - eliminates another player from the game
     * Costs 7 coins and removes the target from active play
     * 
     * @param target Pointer to the player being eliminated
     */
    void Player::coup(shared_ptr<Player> &target)
    {
        checkCoins(7); // Verify player has at least 7 coins

        // Verify the target is active before removing
        if (!target->isActive())
        {
            throw InvalidOperation("Cannot coup an inactive player");
        }
        
        // Prevent self-coup
        if (this == target.get())
        {
            throw InvalidOperation("You cannot coup yourself");
        }

        // Pay for the coup action
        removeCoins(7);

        // Mark the target as inactive
        target->setActive(false);

        // Record the action and target
        last_action_ = "coup";
        last_target_ = target->name();

        game_.advanceTurn(); // Move to the next player's turn
        
        // Update the game's record of the last player to be couped
        game_.getLastPlayerCouped() = target;
    }
}