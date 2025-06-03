//orel8155@gmail.com
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <map>
#include "Game.hpp"
#include "Player.hpp"

/**
 * Game state enumeration
 * Represents the current state of the GUI
 */
enum class GuiState
{
    SETUP,        // Setup screen for configuring the game
    PLAYING,      // Active gameplay state
    WIN_SCREEN    // Winner display screen
};

/**
 * Main GUI class for Coup game
 * Handles all graphics, user interaction, and game state management
 */
class CoupGUI
{
private:
    // SFML Window
    sf::RenderWindow window;

    // Resources
    sf::Font mainFont;
    sf::Font titleFont;
    sf::Texture backgroundTexture;
    sf::Texture coinTexture;
    std::map<coup::Role, sf::Color> roleColors;

    // GUI elements
    sf::RectangleShape background;
    sf::RectangleShape gameBoard;
    sf::RectangleShape logPanel;
    sf::RectangleShape actionPanel;
    sf::RectangleShape specialActionsPanel; // Panel for special actions

    // GUI state
    GuiState currentState;
    sf::Clock globalClock;
    std::vector<std::string> logMessages;
    std::queue<std::string> notificationQueue;
    bool debugMode;

    /**
     * Player setup data structure
     * Stores configuration information for each player during setup
     */
    struct PlayerSetup {
        std::string name;
        coup::Role role;
        sf::Text nameText;
        sf::RectangleShape nameInput;
        sf::RectangleShape roleButtons[6]; // Buttons for all 6 roles
        sf::Text roleButtonTexts[6];
        bool nameCompleted;
        bool roleSelected;
    };
    
    // Setup screen elements
    std::vector<PlayerSetup> setupPlayers;
    int numPlayersToSetup;
    int currentSetupStep; // 0 = player count selection, 1+ = player configuration
    sf::RectangleShape playerCountButtons[5]; // Buttons for selecting 2-6 players
    sf::Text playerCountTexts[5];
    sf::RectangleShape startGameButton;
    sf::Text startGameButtonText;
    std::string currentInputText;
    int activeInputPlayer; // Which player is currently inputting name
    
    // Active game elements
    std::shared_ptr<coup::Game> game;
    std::vector<std::shared_ptr<coup::Player>> realPlayers;
    std::string currentPlayerName;

    // Target selection state
    bool waitingForTarget;
    std::string pendingActionName; // Name of action waiting to be executed
    
    /**
     * Special action state enumeration
     * Represents the current state of special action selection process
     */
    enum class SpecialActionState
    {
        NONE,                   // No special action in progress
        SELECTING_PERFORMER,    // Selecting which player performs the action
        SELECTING_TARGET        // Selecting target for the action
    };
    SpecialActionState specialActionState;
    std::string pendingSpecialAction;        // Name of special action
    coup::Role pendingSpecialActionRole;     // Required role for the action
    std::shared_ptr<coup::Player> selectedPerformer; // Player selected to perform the action

    /**
     * Player button structure
     * Used for creating clickable player selection buttons
     */
    struct PlayerButton
    {
        std::string playerName;
        sf::RectangleShape button;
        sf::Text buttonText;
    };
    std::vector<PlayerButton> playerButtons;

    /**
     * Special action structure
     * Defines out-of-turn special actions
     */
    struct SpecialAction
    {
        std::string name;
        coup::Role requiredRole;
        sf::RectangleShape button;
        sf::Text buttonText;
    };
    std::vector<SpecialAction> specialActions;

    /**
     * Basic action structure
     * Defines standard in-turn actions
     */
    struct BasicAction
    {
        std::string name;
        sf::RectangleShape button;
        sf::Text buttonText;
        sf::Color buttonColor;
    };
    std::vector<BasicAction> basicActions;

    // Reset button
    sf::RectangleShape resetButton;
    sf::Text resetButtonText;
    
    // Win screen elements
    std::string winnerName;
    sf::RectangleShape winOverlay;
    sf::Text winTitle;
    sf::Text winSubtitle;
    sf::RectangleShape newGameButton;
    sf::Text newGameButtonText;
    sf::RectangleShape exitButton;
    sf::Text exitButtonText;

    // Helper functions
    // void handleEvents();
    void update(); // Update the GUI
    void render(); // Render the GUI
    void initializeSpecialActions(); // Initialize special actions
    void initializeBasicActions();   // Initialize basic actions
    void initializeResetButton();    // Initialize reset button
    void initializeWinScreen();      // Initialize win screen
    void initializeSetupScreen();    // Initialize setup screen
    // void initializeRealGame();       // Initialize game with default players
    void initializeGui();            // Common function for GUI initialization

    // Drawing components
    void drawPlayers(); // Draw players
    void drawGameLog(); // Draw game log
    void drawActionPanel(); // Draw action panel
    void drawNotifications(); // Draw notifications
    void drawDebugInfo(); // Draw debug info
    void drawSpecialActions(); // Draw special action buttons
    void drawPlayerButtons();  // Draw player selection buttons
    void drawResetButton();    // Draw reset button
    void drawWinScreen();      // Draw win screen
    void drawSetupScreen();    // Draw setup screen

    // Utility functions
    sf::Color getRoleColor(coup::Role role); // Get role color
    std::string getRoleName(coup::Role role); // Get role name
    void addLogMessage(const std::string &message); // Add log message
    void toggleDebugMode(); // Toggle debug mode
    void handleSpecialActionClick(int actionIndex); // Handle click on special action
    void handleBasicActionClick(int actionIndex);   // Handle click on basic action
    void handleResetClick();                        // Handle click on reset button
    void handleWinScreenClick(sf::Vector2i mousePos); // Handle clicks on win screen
    void handleSetupScreenClick(sf::Vector2i mousePos); // Handle clicks on setup screen
    void handleTextInput(sf::Uint32 unicode);       // Handle text input
    
    // Game management functions
    void resetGame();          // Reset the game
    void checkForWinner();     // Check if there's a winner
    void startNewGame();       // Start a new game
    void createGameFromSetup(); // Create game from setup configuration
    
    // Functions for actions that require target selection
    void executeArrest(std::shared_ptr<coup::Player> targetPlayer);
    void executeSanction(std::shared_ptr<coup::Player> targetPlayer);
    void executeCoup(std::shared_ptr<coup::Player> targetPlayer);
    
    // Helper functions for player buttons
    void createPlayerButtons(); // Create player buttons
    void clearPlayerButtons(); // Clear player buttons
    
    // Functions for two-stage action mechanism
    void startSpecialActionSelection(const std::string& actionName, coup::Role requiredRole); // Start special action selection
    void createPerformerButtons(coup::Role requiredRole); // Create performer buttons
    void createTargetButtons(std::shared_ptr<coup::Player> performer); // Create target buttons
    void executeSpecialAction(std::shared_ptr<coup::Player> performer, std::shared_ptr<coup::Player> target = nullptr); // Execute special action
    void cancelSpecialActionSelection(); // Cancel special action selection

public:
    /**
     * Default constructor
     * Initializes the GUI starting with the setup screen
     */
    CoupGUI();
    
    /**
     * Alternative constructor
     * Takes existing game and players (currently not used)
     */
    CoupGUI(std::shared_ptr<coup::Game> existingGame, std::vector<std::shared_ptr<coup::Player>> existingPlayers);
    
    /**
     * Destructor
     */
    ~CoupGUI();

    /**
     * Main game loop
     * Handles events, updates, and rendering
     */
    void run();
    
    /**
     * Adds a notification message to the queue
     * @param message The notification text to display
     */
    void addNotification(const std::string &message);
};