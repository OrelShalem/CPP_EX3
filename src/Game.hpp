#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory>
#include "Player.hpp"
#include <climits>
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
        vector<shared_ptr<Player>> players_;           // to store the players
        size_t current_player_index_;                  // to know the current player
        size_t previous_player_index_;                  // to know the previous player
        bool game_started_;                            // to check if the game has started
        shared_ptr<Player> current_player_;            // to store the current player
        shared_ptr<Player> previous_player_;            // to store the previous player
        shared_ptr<Player> arrested_player_;           // to store the arrested player
        vector<string> blocked_from_arrest_players_;   // וקטור שישמור שמות של שחקנים שחסומים מביצוע arrest
        string player_get_arrested;
        shared_ptr<Player> last_player_couped;                  // משתנה שישמור את השחקן שקיבל מעצר
        int bank_balance_;
    public:
        Game() : current_player_index_(0), previous_player_index_(0), game_started_(false), current_player_(nullptr), previous_player_(nullptr), arrested_player_(nullptr), player_get_arrested(""), last_player_couped(nullptr), bank_balance_(1000000) {} // constructor
        ~Game() = default;                                                                                                                                           // destructor

        // basic functions
        Role turn() const;              // to get the current player
        vector<string> players() const; // to get the players
        string winner() const;          // to get the winner

        shared_ptr<Player> createPlayer(const string &name, const Role &role);
        // helper functions

        void removePlayer(const string &player_name); // to remove a player from the game. the parameter is const because we don't want to change the player
        bool isPlayerActive();                        // to check if the player is active. the parameter is const because we don't want to change the name and the function is const because it doesn't change the game
        void advanceTurn();                           // to advance the turn to the next player
                                                      // to clear a pending action from the game

        // getters
        shared_ptr<Player> &getPlayer();
        size_t getPlayerIndex(const string &name) const;
        shared_ptr<Player> getPlayerByName(const string &name) const;
        vector<shared_ptr<Player>> &getPlayers() { return players_; };
        shared_ptr<Player> &getArrestedPlayer() { return arrested_player_; };
        shared_ptr<Player> &getCurrentPlayer() { return current_player_; };
        shared_ptr<Player> &getPreviousPlayer() { return previous_player_; };
        size_t getPreviousPlayerIndex() { return previous_player_index_; };
        size_t getCurrentPlayerIndex() { return current_player_index_; };

        // שיטות חדשות לטיפול במשתנה player_get_arrested
        void setArrestedPlayerName(const string &name) { player_get_arrested = name; }
        string getArrestedPlayerName() const { return player_get_arrested; }

        // check if the game has started correctly
        bool isGameStarted() const;
        bool isGameOver() const;

        // בדיקה שמספר השחקנים הוא תקין (בין 2 ל-6)
        bool isValidPlayerCount() const
        {
            size_t player_count = players_.size();
            return player_count >= 2 && player_count <= 6;
        }

        // פונקציות חדשות לניהול חסימת arrest
        void blockPlayerFromArrest(const string &player_name);
        void clearBlockedFromArrestPlayers();
        bool isPlayerBlockedFromArrest(const string &player_name) const;

       

        // פונקציה ייעודית לקבלת שחקנים שביצעו פעולת מס - עבור הממשק הגרפי
        shared_ptr<Player> &getLastPlayerCouped() { return last_player_couped; }
        void addCoinsToBank(int amount) {bank_balance_ += amount; }
        void removeCoinsFromBank(int amount) {bank_balance_ -= amount; }
    };

}