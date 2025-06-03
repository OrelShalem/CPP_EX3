// orel8155@gmail.com

#include "Game.hpp"
#include "Player.hpp"
#include "GameExceptions.hpp"
#include "Roles/General.hpp"
#include "Roles/Governor.hpp"
#include "Roles/Spy.hpp"
#include "Roles/Baron.hpp"
#include "Roles/Judge.hpp"
#include "Roles/Merchant.hpp"
#include <iostream>
#include "Player.hpp"
using namespace std;
namespace coup
{

    Role Game::turn() const // to get the current player
    {

        if (players_.empty())
        {
            throw GameNotOverException("Game has not started yet");
        }

        return players_[current_player_index_]->role();
    }

    shared_ptr<Player> Game::createPlayer(const string &name, const Role &role)
    {
        if (game_started_)
        {
            throw GameException("Game already started");
        }

        if (name.empty())
        {
            throw GameException("Player name cannot be empty");
        }

        // בדיקה אם השם כבר קיים ויצירת שם ייחודי
        string unique_name = name;
        int counter = 1;

        // בדיקה אם השם כבר קיים
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
                break; // השם ייחודי, יוצאים מהלולאה
            }

            // השם קיים, יוצרים שם חדש עם מספר
            counter++;
            unique_name = name + "_" + to_string(counter);
        }

        // בדיקה שלא חורגים ממספר השחקנים המקסימלי
        if (players_.size() >= 6)
        {
            throw GameException("Maximum number of players (6) reached");
        }

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

    vector<string> Game::players() const // to get the players
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

    string Game::winner() const // to get the winner
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

    void Game::removePlayer(const string &player_name)
    {
        // מחפשים את השחקן לפי השם
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

    void Game::advanceTurn()
    {
        cout << "DEBUG Game::advanceTurn: Starting advanceTurn" << endl;
        
        if (players_.empty())
        {
            cout << "DEBUG Game::advanceTurn: No players in the game" << endl;
            throw GameException("No players in the game");
        }

        // בדיקה שיש לפחות 2 שחקנים לפני תחילת המשחק
        if (!game_started_ && players_.size() < 2)
        {
            cout << "DEBUG Game::advanceTurn: Not enough players to start game (" << players_.size() << ")" << endl;
            throw GameException("At least 2 players are required to start the game");
        }

        if (!game_started_)
        {
            cout << "DEBUG Game::advanceTurn: Starting game for first time" << endl;
            game_started_ = true;
        }
        
        cout << "DEBUG Game::advanceTurn: Current player index before advance: " << current_player_index_ << endl;
        cout << "DEBUG Game::advanceTurn: Current player name: " << players_[current_player_index_]->name() << endl;
        
        // מנקה את החסימה מ-arrest של השחקן הנוכחי
        players_[current_player_index_]->get_blocked_from_arresting() = false;
        players_[current_player_index_]->setBlockedFromEconomic(false);


        try
        {
            cout << "DEBUG Game::advanceTurn: Calling newTurnIncome for " << players_[current_player_index_]->name() << endl;
            if (players_[current_player_index_]->role() == Role::MERCHANT)
            {
                players_[current_player_index_]->newTurnIncome();
            }
            cout << "DEBUG Game::advanceTurn: newTurnIncome completed successfully" << endl;
        }
        catch (const InvalidOperation &e)
        {
            cout << "DEBUG Game::advanceTurn: newTurnIncome threw exception: " << e.what() << endl;
            cerr << e.what() << endl;
        }
        // שמירת האינדקס הנוכחי לפני העדכון
        previous_player_index_ = current_player_index_;
        previous_player_ = players_[previous_player_index_];

        cout << "DEBUG Game::advanceTurn: Looking for next active player..." << endl;
        // עדכון האינדקס לשחקן הבא
        do
        {
            current_player_index_ = (current_player_index_ + 1) % players_.size(); // to advance the turn to the next player
            cout << "DEBUG Game::advanceTurn: Checking player at index " << current_player_index_ << " (" << players_[current_player_index_]->name() << ") - active: " << players_[current_player_index_]->isActive() << endl;
        } while (!players_[current_player_index_]->isActive());
        
        cout << "DEBUG Game::advanceTurn: New current player index: " << current_player_index_ << endl;
        cout << "DEBUG Game::advanceTurn: New current player name: " << players_[current_player_index_]->name() << endl;
        
        
        
        cout << "DEBUG Game::advanceTurn: advanceTurn completed successfully" << endl;
    }

    shared_ptr<Player> &Game::getPlayer()
    {
        if (players_.empty())
        {
            throw GameException("No players in the game");
        }
        return players_[current_player_index_];
    }

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

    bool Game::isGameStarted() const
    {
        // בודק האם המשחק התחיל וגם שיש מספר תקין של שחקנים (בין 2 ל-6)
        return game_started_ && isValidPlayerCount();
    }

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

    void Game::blockPlayerFromArrest(const string &player_name)
    {
        // מוסיף את שם השחקן לרשימת החסומים מ-arrest
        blocked_from_arrest_players_.push_back(player_name);
    }

    void Game::clearBlockedFromArrestPlayers()
    {
        // מנקה את רשימת השחקנים החסומים מ-arrest
        blocked_from_arrest_players_.clear();
    }

   

}