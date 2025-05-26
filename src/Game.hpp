#pragma once

#include <vector>
#include <string>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include "Player.hpp"
using namespace std;

namespace coup
{

    class Player; // forward declaration

    struct Pending
    {
        string action;
        string actor;
        string target;
    };

    class Game
    {
    private:
        vector<shared_ptr<Player>> players_; // to store the players
        size_t current_player_index_;        // to know the current player
        bool game_started_;                  // to check if the game has started
        shared_ptr<Player> current_player_;  // to store the current player
        vector<Pending> pending_actions_;    // to store the pending actions
    public:
        Game() : current_player_index_(0), game_started_(false), current_player_(nullptr), pending_actions_() {} // constructor
        ~Game() = default;                                                                                       // destructor

        // basic functions
        Role turn() const;              // to get the current player
        vector<string> players() const; // to get the players
        string winner() const;          // to get the winner

        shared_ptr<Player> createPlayer(const string &name, const Role &role);
        // helper functions

        void removePlayer(const string &player_name); // to remove a player from the game. the parameter is const because we don't want to change the player
        bool isPlayerActive();                        // to check if the player is active. the parameter is const because we don't want to change the name and the function is const because it doesn't change the game
        void advanceTurn();                           // to advance the turn to the next player
        void registerPendingAction(const string &action, const string &actor, const string &target)
        {
            pending_actions_.push_back({action, actor, target});
        }; // to add a pending action to the game
        bool hasPending(const string &action, const string &actor, const string &target) const; // to check if there are pending actions
        void clearPendingFor(const string &actor);                                              // to clear a pending action from the game

        // getters
        shared_ptr<Player> &getPlayer();
        size_t getPlayerIndex(const string &name) const;
        shared_ptr<Player> getPlayerByName(const string &name) const;
        vector<shared_ptr<Player>> &getPlayers(){return players_;};

        // check if the game has started correctly
        bool isGameStarted() const;
        bool isGameOver() const;
    };

}