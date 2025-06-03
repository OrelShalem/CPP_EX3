//orel8155@gmail.com
// File: Game.cpp - Implementation of the Coup card game logic

#include "Game.hpp"       // Include the game header file
#include "Player.hpp"     // Include the player class header
#include "GameExceptions.hpp" // Include custom game exceptions
#include "Roles/General.hpp"  // Include role-specific classes
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "Roles/Merchant.hpp"
#include <iostream>       // For standard input/output operations
#include "Player.hpp"     // Duplicate include (could be removed)
using namespace std;      // Using standard namespace

namespace coup
{
    /**
     * Returns the role of the current player whose turn it is
     * @return The Role enum value of the current player
     * @throws GameNotOverException if no players exist in the game
     */
    Role Game::turn() const
    {
        // Check if there are players in the game
        if (players_.empty())
        {
            throw GameNotOverException("Game has not started yet");
        }

        // Return the role of the current player
        return players_[current_player_index_]->role();
    }

    /**
     * Creates a new player with the given name and role and adds them to the game
     * @param name The name for the player
     * @param role The role the player will play
     * @return A shared pointer to the newly created player
     * @throws GameException if the game has already started or if player creation fails
     */
    shared_ptr<Player> Game::createPlayer(const string &name, const Role &role)
    {
        // Cannot add players after game has started
        if (game_started_)
        {
            throw GameException("Game already started");
        }

        // Player name cannot be empty
        if (name.empty())
        {
            throw GameException("Player name cannot be empty");
        }

        // Check if name already exists and create a unique name
        string unique_name = name;
        int counter = 1;

        // Loop until we find a unique name
        while (true)
        {
            bool name_exists = false;
            for (const auto &player : players_)
            {
                if (player->name() == unique_name)
                {
                    name_exists = true;
                    break;
                }
            }

            if (!name_exists)
            {
                break; // Name is unique, exit loop
            }

            // Name exists, create a new name with counter
            counter++;
            unique_name = name + "_" + to_string(counter);
        }

        // Check we don't exceed the maximum player limit
        if (players_.size() >= 6)
        {
            throw GameException("Maximum number of players (6) reached");
        }

        // Create a specific player type based on the role
        switch (role)
        {
        case Role::GENERAL:
        {
            auto player = make_shared<General>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        case Role::GOVERNOR:
        {
            auto player = make_shared<Governor>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        case Role::SPY:
        {
            auto player = make_shared<Spy>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        case Role::BARON:
        {
            auto player = make_shared<Baron>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        case Role::JUDGE:
        {
            auto player = make_shared<Judge>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        case Role::MERCHANT:
        {
            auto player = make_shared<Merchant>(*this, unique_name, role);
            players_.push_back(player);
            return player;
        }
        default:
        {
            throw GameException("Invalid role");
        }
        }
    }

    /**
     * Returns a list of active player names in the game
     * @return A vector of strings containing the names of active players
     */
    vector<string> Game::players() const
    {
        vector<string> active_players;
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                active_players.push_back(player->name());
            }
        }
        return active_players;
    }

    /**
     * Returns the name of the winning player
     * @return String containing the winner's name
     * @throws GameNotOverException if the game is still in progress
     * @throws GameException if no active players are found
     */
    string Game::winner() const
    {
        if (!isGameOver())
        {
            throw GameNotOverException("Game has not ended yet");
        }
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                return player->name();
            }
        }

        throw GameException("No active players found");
    }

    /**
     * Removes a player from active play (does not delete the player)
     * @param player_name The name of the player to remove
     * @throws PlayerNotFound if no player with the given name exists
     */
    void Game::removePlayer(const string &player_name)
    {
        // Search for the player by name
        for (auto &player : players_)
        {
            if (player->name() == player_name)
            {
                player->setActive(false);
                return;
            }
        }
        throw PlayerNotFound("Player not found: " + player_name);
    }

    /**
     * Checks if the current player is active in the game
     * @return true if the current player is active, false otherwise
     */
    bool Game::isPlayerActive()
    {
        try
        {
            auto player = getPlayer();
            return player->isActive();
        }
        catch (const PlayerNotFound &)
        {
            return false;
        }
    }

    /**
     * Advances the turn to the next active player
     * Also handles start-of-turn maintenance like clearing blocks and giving Merchant income
     * @throws GameException if there are no players or not enough players to start
     */
    void Game::advanceTurn()
    {
        if (players_.empty())
        {
            throw GameException("No players in the game");
        }

        // Check there are at least 2 players before starting the game
        if (!game_started_ && players_.size() < 2)
        {
            throw GameException("At least 2 players are required to start the game");
        }

        // Mark the game as started if this is the first turn
        if (!game_started_)
        {
            game_started_ = true;
        }
        
        // Clear any blocks from arresting or economic actions for the current player
        players_[current_player_index_]->get_blocked_from_arresting() = false;
        players_[current_player_index_]->setBlockedFromEconomic(false);

        // Handle Merchant's special income ability at the start of their turn
        try
        {
            if (players_[current_player_index_]->role() == Role::MERCHANT)
            {
                players_[current_player_index_]->newTurnIncome();
            }
        }
        catch (const InvalidOperation &e)
        {
            cerr << e.what() << endl;
        }

        // Store the current player index before updating
        previous_player_index_ = current_player_index_;
        previous_player_ = players_[previous_player_index_];

        // Find the next active player
        do
        {
            current_player_index_ = (current_player_index_ + 1) % players_.size();
        } while (!players_[current_player_index_]->isActive());
    }

    /**
     * Gets the current player whose turn it is
     * @return Reference to shared pointer of the current player
     * @throws GameException if there are no players in the game
     */
    shared_ptr<Player> &Game::getPlayer()
    {
        if (players_.empty())
        {
            throw GameException("No players in the game");
        }
        return players_[current_player_index_];
    }

    /**
     * Gets the index of a player by their name
     * @param name The name of the player to find
     * @return The index of the player in the players vector
     * @throws PlayerNotFound if no player with the given name exists
     */
    size_t Game::getPlayerIndex(const string &name) const
    {
        for (size_t i = 0; i < players_.size(); ++i)
        {
            if (players_[i]->name() == name)
            {
                return i;
            }
        }
        throw PlayerNotFound("Player not found: " + name);
    }

    /**
     * Gets a player object by their name
     * @param name The name of the player to find
     * @return Shared pointer to the found player
     * @throws PlayerNotFound if no player with the given name exists
     */
    shared_ptr<Player> Game::getPlayerByName(const string &name) const
    {
        for (const auto &player : players_)
        {
            if (player->name() == name)
            {
                return player;
            }
        }
        throw PlayerNotFound("Player not found: " + name);
    }
    
    /**
     * Checks if the game is over (only one or fewer active players remain)
     * @return true if the game is over, false otherwise
     */
    bool Game::isGameOver() const
    {
        int active_count = 0;
        for (const auto &player : players_)
        {
            if (player->isActive())
            {
                active_count++;
            }
        }
        return active_count <= 1 && game_started_;
    }
}