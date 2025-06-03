//orel8155@gmail.com

/**
 * @file CoupGUI.cpp
 * @brief Implementation of the graphical user interface for the Coup game
 * 
 * This file contains the complete implementation of the CoupGUI class which handles
 * all graphical rendering, user input, and game state management for the Coup card game.
 * 
 * Key Features:
 * - Complete game setup flow (player count, names, roles)
 * - Real-time game visualization with player cards and status
 * - Interactive buttons for all game actions (basic and special)
 * - Game log and notification system
 * - Win screen with game restart options
 *
 * 
 * @author Orel
 * @date 2025
 */

#include "CoupGUI.hpp"
#include <iostream>
#include <sstream>
#include <cmath>
/**
 * @brief Initializes all graphical user interface components for the Coup game
 * 
 * Sets up the main game window, fonts, background elements, game board, panels,
 * role colors, and initializes all interactive components. This method must be
 * called before any rendering operations.
 * 
 * Creates:
 * - Main game window (1280x720) with 60 FPS limit
 * - Game board panel for player displays
 * - Action panel for basic game actions
 * - Special actions panel for role-specific actions
 * - Log panel for game history
 * - Color scheme for different player roles
 * 
 * @throws std::runtime_error if font loading fails
 */
void CoupGUI::initializeGui()
{
    // Create main window with title "Coup - Game" and specific styles
    window.create(sf::VideoMode(1280, 720), "Coup - Game", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // Load the main font from file - used for all text elements
    if (!mainFont.loadFromFile("assets/fonts/Heebo-Regular.ttf"))
    {
        std::cerr << "Failed to load main font" << std::endl;
    }
    titleFont = mainFont;

    // Initialize simple background
    background.setSize(sf::Vector2f(1280, 720));
    background.setFillColor(sf::Color(20, 30, 48)); // Elegant dark blue

    // Initialize game board - the main area where player cards are displayed
    gameBoard.setSize(sf::Vector2f(900, 500));
    gameBoard.setPosition(30, 100);
    gameBoard.setFillColor(sf::Color(30, 40, 60));
    gameBoard.setOutlineThickness(2);
    gameBoard.setOutlineColor(sf::Color(100, 120, 180));

    // Initialize log panel - displays game history and events
    logPanel.setSize(sf::Vector2f(280, 280));
    logPanel.setPosition(970, 100);
    logPanel.setFillColor(sf::Color(25, 35, 55));
    logPanel.setOutlineThickness(2);
    logPanel.setOutlineColor(sf::Color(100, 120, 180));

    // Initialize action panel - contains buttons for basic game actions
    actionPanel.setSize(sf::Vector2f(1220, 80));
    actionPanel.setPosition(30, 620);
    actionPanel.setFillColor(sf::Color(25, 35, 55));
    actionPanel.setOutlineThickness(2);
    actionPanel.setOutlineColor(sf::Color(100, 120, 180));

    // Initialize special actions panel - contains buttons for role-specific actions
    specialActionsPanel.setSize(sf::Vector2f(280, 180));
    specialActionsPanel.setPosition(970, 400);
    specialActionsPanel.setFillColor(sf::Color(25, 35, 55));
    specialActionsPanel.setOutlineThickness(2);
    specialActionsPanel.setOutlineColor(sf::Color(100, 120, 180));

    // Create colors for different player roles
    roleColors[coup::Role::GENERAL] = sf::Color(180, 40, 40);  // Red
    roleColors[coup::Role::GOVERNOR] = sf::Color(40, 80, 180); // Blue
    roleColors[coup::Role::SPY] = sf::Color(100, 100, 100);    // Gray
    roleColors[coup::Role::BARON] = sf::Color(180, 140, 40);   // Gold
    roleColors[coup::Role::JUDGE] = sf::Color(80, 40, 100);    // Purple
    roleColors[coup::Role::MERCHANT] = sf::Color(40, 140, 40); // Green

    // Initialize special action buttons
    initializeSpecialActions();

    // Initialize basic action buttons
    initializeBasicActions();
    
    // Initialize reset button
    initializeResetButton();
    
    // Initialize win screen
    initializeWinScreen();
}
/**
 * @brief Default constructor - starts with game setup screen
 * 
 * Initializes the CoupGUI in setup mode with all default values:
 * - Sets the initial state to SETUP (first game screen)
 * - Initializes all game state variables to their defaults
 * - Prepares for player configuration
 * 
 * This constructor is used when starting a new game from scratch,
 * allowing the players to configure the game before playing.
 */
CoupGUI::CoupGUI()
    : currentState(GuiState::SETUP),      // Start in setup screen
      globalClock(),                       // Initialize timer
      debugMode(false),                    // Debug mode off by default
      numPlayersToSetup(0),                // No players configured yet
      currentSetupStep(0),                 // First step of setup (selecting player count)
      activeInputPlayer(-1),               // No player input active
      game(nullptr),                       // No game instance yet
      waitingForTarget(false),             // Not waiting for target selection
      pendingActionName(""),               // No action pending
      specialActionState(SpecialActionState::NONE),  // No special action in progress
      pendingSpecialAction(""),            // No special action pending
      pendingSpecialActionRole(coup::Role::GENERAL), // Default role (placeholder)
      selectedPerformer(nullptr)           // No performer selected
{
    std::cout << "Initializing Coup Game Setup Screen" << std::endl;
    
    // Initialize the graphical interface
    initializeGui();
    
    // Initialize the setup screen components
    initializeSetupScreen();
}



/**
 * @brief Constructor for starting with existing game and players
 * 
 * @param existingGame Shared pointer to an already initialized Game object
 * @param existingPlayers Vector of Player objects that will participate in the game
 * 
 * This constructor initializes the GUI directly with an existing game state.
 * It sets the current state to PLAYING, bypassing the setup screen, and uses
 * the provided game and player objects to populate the game state.
 * 
 * The constructor:
 * - Sets the initial state to PLAYING mode
 * - Initializes the GUI components
 * - Sets the current player from the game state
 * - Adds initial log messages to inform the player about the game start
 * 
 * This is useful for testing or when loading a saved game state.
 */
CoupGUI::CoupGUI(std::shared_ptr<coup::Game> existingGame, std::vector<std::shared_ptr<coup::Player>> existingPlayers)
    : currentState(GuiState::PLAYING),
      globalClock(),
      debugMode(false),
      game(existingGame),
      realPlayers(existingPlayers),
      waitingForTarget(false),
      pendingActionName(""),
      specialActionState(SpecialActionState::NONE),
      pendingSpecialAction(""),
      pendingSpecialActionRole(coup::Role::GENERAL), // Default value
      selectedPerformer(nullptr)
{
    std::cout << "Initializing Coup Game with provided players" << std::endl;
    
    // Initialize the graphical interface
    initializeGui();
    
    // Set the current player
    if (!realPlayers.empty()) {
        auto players = game->players();
        if (!players.empty()) {
            currentPlayerName = players[0];
        } else if (realPlayers[0]) {
            currentPlayerName = realPlayers[0]->name();
        }
    }
    
    // Add opening messages
    addLogMessage("Game started with " + std::to_string(realPlayers.size()) + " players");
    addNotification("Game started! " + currentPlayerName + "'s turn");
}

/**
 * @brief Destructor - cleans up resources
 * 
 * Currently no dynamic resources need explicit cleanup as all resources are managed
 * by smart pointers or SFML's automatic resource management. The SFML window
 * will be automatically closed and fonts will be properly deallocated.
 * 
 * Future considerations:
 * - If file I/O or network connections are added, cleanup would be needed here
 * - If custom memory allocation is used, deallocation should be added
 */
CoupGUI::~CoupGUI()
{
    // No dynamic resources to clean up
}


/**
 * @brief Initializes special action buttons for role-specific abilities
 * 
 * Creates interactive buttons for each role's unique special abilities that can be
 * performed out of turn. Each action requires a specific role to perform and may
 * require target selection.
 * 
 * Special Actions Created:
 * - Governor: "Undo Tax" - Reverses a tax action
 * - Spy: "see and block arrest" - Views coins and blocks arrest actions  
 * - Judge: "Undo Bribe" - Reverses a bribe action
 * - General: "Undo Coup" - Reverses a coup action
 * - Baron: "Invest" - Generates additional income
 * 
 * Button Layout:
 * - All buttons positioned vertically in the special actions panel
 * - Light blue color scheme for consistency
 * - 35 pixel spacing between buttons
 * - Text centered within each button
 * 
 * @note Buttons are only displayed when a player with the required role is active
 */
void CoupGUI::initializeSpecialActions()
{
    // Create list of special actions
    specialActions.clear();

    // Position and size of the buttons
    float buttonWidth = 150;
    float buttonHeight = 30;
    float startX = 985;
    float startY = 450;
    float spacing = 35;

    // Create buttons for each role's special abilities

    // Governor - Undo Tax
    SpecialAction governorAction;
    governorAction.name = "Undo Tax";
    governorAction.requiredRole = coup::Role::GOVERNOR;

    governorAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    governorAction.button.setPosition(startX + 95, startY);
    governorAction.button.setFillColor(sf::Color(86, 116, 255)); // כחול בהיר

    governorAction.buttonText.setFont(mainFont);
    governorAction.buttonText.setString(governorAction.name);
    governorAction.buttonText.setCharacterSize(16);
    governorAction.buttonText.setFillColor(sf::Color::White);
    governorAction.buttonText.setPosition(startX + 100 + (buttonWidth - governorAction.buttonText.getLocalBounds().width) / 2 - 5,
                                          startY + 5);

    specialActions.push_back(governorAction);

    // Spy - Spy Peek
    SpecialAction spyAction;
    spyAction.name = "see and block arrest";
    spyAction.requiredRole = coup::Role::SPY;

    spyAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    spyAction.button.setPosition(startX + 95, startY + spacing);
    spyAction.button.setFillColor(sf::Color(86, 116, 255)); // כחול בהיר

    spyAction.buttonText.setFont(mainFont);
    spyAction.buttonText.setString(spyAction.name);
    spyAction.buttonText.setCharacterSize(16);
    spyAction.buttonText.setFillColor(sf::Color::White);
    spyAction.buttonText.setPosition(startX + 100 + (buttonWidth - spyAction.buttonText.getLocalBounds().width) / 2 - 5,
                                     startY + spacing + 5);

    specialActions.push_back(spyAction);

    // Judge - Undo Bribe
    SpecialAction judgeAction;
    judgeAction.name = "Undo Bribe";
    judgeAction.requiredRole = coup::Role::JUDGE;

    judgeAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    judgeAction.button.setPosition(startX + 95, startY + spacing * 2);
    judgeAction.button.setFillColor(sf::Color(86, 116, 255)); // כחול בהיר

    judgeAction.buttonText.setFont(mainFont);
    judgeAction.buttonText.setString(judgeAction.name);
    judgeAction.buttonText.setCharacterSize(16);
    judgeAction.buttonText.setFillColor(sf::Color::White);
    judgeAction.buttonText.setPosition(startX + 100 + (buttonWidth - judgeAction.buttonText.getLocalBounds().width) / 2 - 5,
                                       startY + spacing * 2 + 5);

    specialActions.push_back(judgeAction);
    // General - Undo Coup
    SpecialAction generalAction;
    generalAction.name = "Undo Coup";
    generalAction.requiredRole = coup::Role::GENERAL;

    generalAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    generalAction.button.setPosition(startX + 95, startY + spacing * 4);
    generalAction.button.setFillColor(sf::Color(86, 116, 255)); // כחול בהיר

    generalAction.buttonText.setFont(mainFont);
    generalAction.buttonText.setString(generalAction.name);
    generalAction.buttonText.setCharacterSize(16);
    generalAction.buttonText.setFillColor(sf::Color::White);
    generalAction.buttonText.setPosition(startX + 100 + (buttonWidth - generalAction.buttonText.getLocalBounds().width) / 2 - 5,
                                         startY + spacing * 4 + 5);

    specialActions.push_back(generalAction);

    // Baron - Invest
    SpecialAction baronAction;
    baronAction.name = "Invest";
    baronAction.requiredRole = coup::Role::BARON;

    baronAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    baronAction.button.setPosition(startX + 95, startY + spacing * 3);
    baronAction.button.setFillColor(sf::Color(86, 116, 255)); // כחול בהיר

    baronAction.buttonText.setFont(mainFont);
    baronAction.buttonText.setString(baronAction.name);
    baronAction.buttonText.setCharacterSize(16);
    baronAction.buttonText.setFillColor(sf::Color::White);
    baronAction.buttonText.setPosition(startX + 100 + (buttonWidth - baronAction.buttonText.getLocalBounds().width) / 2 - 5,
                                      startY + spacing * 3 + 5);

    specialActions.push_back(baronAction);

    
}

/**
 * @brief Initializes basic action buttons available to all players
 * 
 * Creates six main action buttons that form the core gameplay mechanics.
 * Actions are divided into two categories with distinct visual styling:
 * economic actions (blue) and attack actions (orange).
 * 
 * Economic Actions (Blue):
 * - Gather: Collect 1 coin (always available)
 * - Tax: Collect 2 coins (role-based ability, challengeable)
 * - Bribe: Influence other players (role-based ability)
 * 
 * Attack Actions (Orange):
 * - Arrest: Eliminate a player's role (blockable by Spy)
 * - Sanction: Apply economic penalty to target player
 * - Coup: Expensive but powerful elimination (costs 7 coins)
 * 
 * Button Layout:
 * - Horizontally centered in the action panel
 * - 120px width, 40px height per button
 * - 10px spacing between buttons
 * - Color-coded by action type
 * 
 * @note Attack actions require target selection after clicking
 */
void CoupGUI::initializeBasicActions()
{
    // Create list of basic actions
    basicActions.clear();

    // Position and size of the buttons - all buttons in one row
    float buttonWidth = 120;
    float buttonHeight = 40;
    float totalWidth = 6 * buttonWidth + 5 * 10;          // 6 buttons + 5 spaces of 10 pixels
    float startX = (window.getSize().x - totalWidth) / 2; // Center of the buttons
    float startY = 640;                                   // Position of the buttons in the action panel
    float spacing = buttonWidth + 10;                     // Space between the buttons

    // Colors for the buttons
    sf::Color blueButtonColor(70, 130, 180);  // Blue for economic actions
    sf::Color orangeButtonColor(180, 95, 60); // Orange for attack actions

    // Economic actions

    // 1. Gather
    BasicAction gatherAction;
    gatherAction.name = "Gather";
    gatherAction.buttonColor = blueButtonColor;

    gatherAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    gatherAction.button.setPosition(startX, startY);
    gatherAction.button.setFillColor(gatherAction.buttonColor);

    gatherAction.buttonText.setFont(mainFont);
    gatherAction.buttonText.setString(gatherAction.name);
    gatherAction.buttonText.setCharacterSize(18);
    gatherAction.buttonText.setFillColor(sf::Color::White);
    gatherAction.buttonText.setPosition(
        startX + (buttonWidth - gatherAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - gatherAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(gatherAction);

    // 2. Tax
    BasicAction taxAction;
    taxAction.name = "Tax";
    taxAction.buttonColor = blueButtonColor;

    taxAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    taxAction.button.setPosition(startX + spacing, startY);
    taxAction.button.setFillColor(taxAction.buttonColor);

    taxAction.buttonText.setFont(mainFont);
    taxAction.buttonText.setString(taxAction.name);
    taxAction.buttonText.setCharacterSize(18);
    taxAction.buttonText.setFillColor(sf::Color::White);
    taxAction.buttonText.setPosition(
        startX + spacing + (buttonWidth - taxAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - taxAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(taxAction);

    // 3. Bribe
    BasicAction bribeAction;
    bribeAction.name = "Bribe";
    bribeAction.buttonColor = blueButtonColor;

    bribeAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    bribeAction.button.setPosition(startX + spacing * 2, startY);
    bribeAction.button.setFillColor(bribeAction.buttonColor);

    bribeAction.buttonText.setFont(mainFont);
    bribeAction.buttonText.setString(bribeAction.name);
    bribeAction.buttonText.setCharacterSize(18);
    bribeAction.buttonText.setFillColor(sf::Color::White);
    bribeAction.buttonText.setPosition(
        startX + spacing * 2 + (buttonWidth - bribeAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - bribeAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(bribeAction);

    // Attack actions

    // 4. Arrest
    BasicAction arrestAction;
    arrestAction.name = "Arrest";
    arrestAction.buttonColor = orangeButtonColor;

    arrestAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    arrestAction.button.setPosition(startX + spacing * 3, startY);
    arrestAction.button.setFillColor(arrestAction.buttonColor);

    arrestAction.buttonText.setFont(mainFont);
    arrestAction.buttonText.setString(arrestAction.name);
    arrestAction.buttonText.setCharacterSize(18);
    arrestAction.buttonText.setFillColor(sf::Color::White);
    arrestAction.buttonText.setPosition(
        startX + spacing * 3 + (buttonWidth - arrestAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - arrestAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(arrestAction);

    // 5. Sanction
    BasicAction sanctionAction;
    sanctionAction.name = "Sanction";
    sanctionAction.buttonColor = orangeButtonColor;

    sanctionAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    sanctionAction.button.setPosition(startX + spacing * 4, startY);
    sanctionAction.button.setFillColor(sanctionAction.buttonColor);

    sanctionAction.buttonText.setFont(mainFont);
    sanctionAction.buttonText.setString(sanctionAction.name);
    sanctionAction.buttonText.setCharacterSize(18);
    sanctionAction.buttonText.setFillColor(sf::Color::White);
    sanctionAction.buttonText.setPosition(
        startX + spacing * 4 + (buttonWidth - sanctionAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - sanctionAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(sanctionAction);

    // 6. Coup
    BasicAction coupAction;
    coupAction.name = "Coup";
    coupAction.buttonColor = orangeButtonColor;

    coupAction.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    coupAction.button.setPosition(startX + spacing * 5, startY);
    coupAction.button.setFillColor(coupAction.buttonColor);

    coupAction.buttonText.setFont(mainFont);
    coupAction.buttonText.setString(coupAction.name);
    coupAction.buttonText.setCharacterSize(18);
    coupAction.buttonText.setFillColor(sf::Color::White);
    coupAction.buttonText.setPosition(
        startX + spacing * 5 + (buttonWidth - coupAction.buttonText.getLocalBounds().width) / 2,
        startY + (buttonHeight - coupAction.buttonText.getLocalBounds().height) / 2 - 5);

    basicActions.push_back(coupAction);
}

/**
 * @brief Initializes the reset game button
 * 
 * Creates a red reset button positioned in the top-right corner of the screen.
 * This button allows players to restart the current game with the same player
 * configuration (names and roles) but reset all game state.
 * 
 * Button Properties:
 * - Red color (200, 60, 60) to indicate destructive action
 * - 100x30 pixel size for visibility without taking too much space
 * - White text and border for contrast
 * - Positioned at (1150, 20) - top-right corner
 * 
 * Functionality:
 * - Preserves player names and roles
 * - Resets all coin counts to starting values
 * - Clears game log and action history
 * - Returns to first player's turn
 * 
 * @see resetGame() for the actual reset implementation
 */
void CoupGUI::initializeResetButton()
{
    // Reset button
    float buttonWidth = 100;
    float buttonHeight = 30;
    float buttonX = 1150;  // Top-right corner
    float buttonY = 20;

    resetButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    resetButton.setPosition(buttonX, buttonY);
    resetButton.setFillColor(sf::Color(200, 60, 60)); // Red
    resetButton.setOutlineThickness(2);
    resetButton.setOutlineColor(sf::Color::White);

    resetButtonText.setFont(mainFont);
    resetButtonText.setString("Reset Game");
    resetButtonText.setCharacterSize(14);
    resetButtonText.setFillColor(sf::Color::White);
    resetButtonText.setPosition(
        buttonX + (buttonWidth - resetButtonText.getLocalBounds().width) / 2,
        buttonY + (buttonHeight - resetButtonText.getLocalBounds().height) / 2 - 5
    );
}

void CoupGUI::initializeWinScreen()
{   
    // Half-transparent black background
    winOverlay.setSize(sf::Vector2f(1280, 720));
    winOverlay.setPosition(0, 0);
    winOverlay.setFillColor(sf::Color(0, 0, 0, 180)); // Half-transparent black

    // Win title
    winTitle.setFont(mainFont);
    winTitle.setCharacterSize(48);
    winTitle.setFillColor(sf::Color::Yellow);
    winTitle.setStyle(sf::Text::Bold);

    // Win subtitle
    winSubtitle.setFont(mainFont);
    winSubtitle.setCharacterSize(24);
    winSubtitle.setFillColor(sf::Color::White);

    // New game button
    float buttonWidth = 150;
    float buttonHeight = 50;
    float centerX = 640; // Center of the screen
    float centerY = 400;

    newGameButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    newGameButton.setPosition(centerX - buttonWidth - 20, centerY);
    newGameButton.setFillColor(sf::Color(70, 130, 180)); // Blue
    newGameButton.setOutlineThickness(3);
    newGameButton.setOutlineColor(sf::Color::White);

    newGameButtonText.setFont(mainFont);
    newGameButtonText.setString("New Game");
    newGameButtonText.setCharacterSize(20);
    newGameButtonText.setFillColor(sf::Color::White);

    // Exit button
    exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    exitButton.setPosition(centerX + 20, centerY);
    exitButton.setFillColor(sf::Color(180, 60, 60)); // Red
    exitButton.setOutlineThickness(3);
    exitButton.setOutlineColor(sf::Color::White);

    exitButtonText.setFont(mainFont);
    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(20);
    exitButtonText.setFillColor(sf::Color::White);
}

void CoupGUI::initializeSetupScreen()
{
    // Initialize player count selection buttons (2-6)
    float buttonWidth = 80;
    float buttonHeight = 60;
    float startX = 350;
    float startY = 250;
    float spacingX = 120;
    
    for (int i = 0; i < 5; i++) {
        int playerCount = i + 2; // 2-6 players
        
        playerCountButtons[i].setSize(sf::Vector2f(buttonWidth, buttonHeight));
        playerCountButtons[i].setPosition(startX + i * spacingX, startY);
        playerCountButtons[i].setFillColor(sf::Color(70, 130, 180));
        playerCountButtons[i].setOutlineThickness(3);
        playerCountButtons[i].setOutlineColor(sf::Color::White);
        
        playerCountTexts[i].setFont(mainFont);
        playerCountTexts[i].setString(std::to_string(playerCount));
        playerCountTexts[i].setCharacterSize(32);
        playerCountTexts[i].setFillColor(sf::Color::White);
        playerCountTexts[i].setPosition(
            startX + i * spacingX + (buttonWidth - playerCountTexts[i].getLocalBounds().width) / 2,
            startY + (buttonHeight - playerCountTexts[i].getLocalBounds().height) / 2 - 5
        );
    }
    
    // Start game button
    startGameButton.setSize(sf::Vector2f(200, 60));
    startGameButton.setPosition(540, 600);
    startGameButton.setFillColor(sf::Color(60, 180, 60)); // Green
    startGameButton.setOutlineThickness(3);
    startGameButton.setOutlineColor(sf::Color::White);
    
    startGameButtonText.setFont(mainFont);
    startGameButtonText.setString("Start Game");
    startGameButtonText.setCharacterSize(24);
    startGameButtonText.setFillColor(sf::Color::White);
    startGameButtonText.setPosition(
        540 + (200 - startGameButtonText.getLocalBounds().width) / 2,
        600 + (60 - startGameButtonText.getLocalBounds().height) / 2 - 5
    );
}

/**
 * Main game loop function that runs the entire game
 * 
 * This function:
 * - Handles all event processing
 * - Updates game state
 * - Renders each frame
 * - Manages error handling through try-catch blocks
 */
void CoupGUI::run()
{
    std::cout << "Starting Coup game" << std::endl;
    sf::Clock frameClock;

    while (window.isOpen())
    {
        try {
            frameClock.restart();

            // Process all window events
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                    break;
                }

                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::F3)
                    {
                        // Toggle debug mode
                        toggleDebugMode();
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                    {
                        if (currentState == GuiState::SETUP && activeInputPlayer >= 0)
                        {
                            // Cancel name input
                            activeInputPlayer = -1;
                            currentInputText = "";
                            addNotification("הזנת השם בוטלה");
                        }
                        else if (waitingForTarget)
                        {
                            // Cancel target selection for basic actions
                            waitingForTarget = false;
                            pendingActionName = "";
                            clearPlayerButtons();
                            addNotification("בחירת מטרה בוטלה");
                        }
                        else if (specialActionState != SpecialActionState::NONE)
                        {
                            // Cancel special action
                            cancelSpecialActionSelection();
                            addNotification("Special action cancelled");
                        }
                    }
                }

                // Handle text input for name entry
                if (event.type == sf::Event::TextEntered && currentState == GuiState::SETUP)
                {
                    handleTextInput(event.text.unicode);
                }

                // Handle mouse click events
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // === Setup screen interaction handling ===
                    if (currentState == GuiState::SETUP)
                    {
                        handleSetupScreenClick(mousePos);
                        continue; // Skip other button processing in setup mode
                    }

                    // === Win screen interaction handling ===
                    if (currentState == GuiState::WIN_SCREEN)
                    {
                        handleWinScreenClick(mousePos);
                        continue; // Skip other button processing in win screen
                    }

                    // === Reset button handling ===
                    sf::FloatRect resetBounds = resetButton.getGlobalBounds();
                    if (resetBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        handleResetClick();
                        continue;
                    }

                    // === Two-stage special action handling ===
                    if (specialActionState == SpecialActionState::SELECTING_PERFORMER)
                    {
                        // Performer selection stage
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // Find selected player
                                for (const auto &player : realPlayers)
                                {
                                    if (player->name() == playerButtons[i].playerName)
                                    {
                                        selectedPerformer = player;
                                        
                                        // Check if action requires a target
                                        if (pendingSpecialAction == "see and block arrest")
                                        {
                                            // Move to target selection stage
                                            specialActionState = SpecialActionState::SELECTING_TARGET;
                                            createTargetButtons(selectedPerformer);
                                            addNotification("Select target for " + pendingSpecialAction);
                                        }
                                        else
                                        {
                                            // Action doesn't need a target - execute directly
                                            executeSpecialAction(selectedPerformer);
                                        }
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    else if (specialActionState == SpecialActionState::SELECTING_TARGET)
                    {
                        // Target selection stage
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // Find selected target
                                for (const auto &player : realPlayers)
                                {
                                    if (player->name() == playerButtons[i].playerName)
                                    {
                                        executeSpecialAction(selectedPerformer, player);
                                        break;
                                    }
                                }
                                break;
                            }
                        }
                    }
                    // === Basic action target selection handling ===
                    else if (waitingForTarget && !pendingActionName.empty())
                    {
                        // Check if player button was clicked
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // Found selected player
                                std::string selectedPlayerName = playerButtons[i].playerName;
                                
                                // Find the actual player in the players list
                                for (const auto &player : realPlayers)
                                {
                                    if (player->name() == selectedPlayerName && player->isActive())
                                    {
                                        try {
                                            // Execute appropriate action based on pending action name
                                            if (pendingActionName == "Arrest")
                                            {
                                                executeArrest(player);
                                            }
                                            else if (pendingActionName == "Sanction")
                                            {
                                                executeSanction(player);
                                            }
                                            else if (pendingActionName == "Coup")
                                            {
                                                executeCoup(player);
                                            }
                                            
                                            // Reset target selection state
                                            waitingForTarget = false;
                                            pendingActionName = "";
                                            clearPlayerButtons();
                                            
                                            break; // Exit inner loop only
                                        }
                                        catch (const std::exception &e) {
                                            addNotification("Error: " + std::string(e.what()));
                                            
                                            // Clean up even in case of error
                                            waitingForTarget = false;
                                            pendingActionName = "";
                                            clearPlayerButtons();
                                            break;
                                        }
                                        catch (...) {
                                            addNotification("Unknown error occurred!");
                                            
                                            // Clean up even in case of error
                                            waitingForTarget = false;
                                            pendingActionName = "";
                                            clearPlayerButtons();
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Check if special action button was clicked
                    for (size_t i = 0; i < specialActions.size(); i++)
                    {
                        sf::FloatRect buttonBounds = specialActions[i].button.getGlobalBounds();
                        if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                        {
                            handleSpecialActionClick(i);
                            break;
                        }
                    }

                    // Check if basic action button was clicked
                    for (size_t i = 0; i < basicActions.size(); i++)
                    {
                        sf::FloatRect buttonBounds = basicActions[i].button.getGlobalBounds();
                        if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                        {
                            handleBasicActionClick(i);
                            break;
                        }
                    }
                }
            }

            
            // Update game state
            try {
                update();
                
            } catch (const std::exception &e) {
                cout << "ERROR in update(): " << e.what() << endl;
                addNotification("שגיאה ב-update: " + std::string(e.what()));
            }

            // Render the frame
            try {
                render();
                
            } catch (const std::exception &e) {
                cout << "ERROR in render(): " << e.what() << endl;
                addNotification("שגיאה ב-render: " + std::string(e.what()));
            }

            // Sleep to maintain consistent frame rate
            sf::sleep(sf::milliseconds(33)); // ~30 FPS
        }
        catch (const std::exception &e) {
            cout << "CRITICAL ERROR in main loop: " << e.what() << endl;
            addNotification("שגיאה קריטית: " + std::string(e.what()));
            // Don't close window, continue running
        }
        catch (...) {
            cout << "UNKNOWN CRITICAL ERROR in main loop!" << endl;
            addNotification("שגיאה לא ידועה!");
            // Don't close window, continue running
        }
    }
}

/**
 * Updates the game state and UI elements each frame.
 * This method is responsible for:
 * 1. Checking for a winner when in playing state
 * 2. Updating the current player information when the turn changes
 * 3. Monitoring changes in game state (like coin counts)
 * 4. Handling periodic visual refreshes
 */
void CoupGUI::update()
{
    // Check if there is a winner (only in playing state)
    if (currentState == GuiState::PLAYING)
    {
        checkForWinner();
    }

    // Update current player name if it changed during the game
    if (game && !realPlayers.empty() && currentState == GuiState::PLAYING) {
        try {
            shared_ptr<coup::Player> currentPlayer = game->getPlayer();
            if (currentPlayer && currentPlayer->name() != currentPlayerName) {
                currentPlayerName = currentPlayer->name();
                // Force visual update when player changes
                addNotification("Turn changed to: " + currentPlayerName);
            }
            
            // Check if there are changes in game state and update visuals if needed
            static int lastCoinsCount = -1;
            int currentCoinsCount = currentPlayer->coins();
            if (lastCoinsCount != currentCoinsCount) {
                lastCoinsCount = currentCoinsCount;
            }
            
        } catch (const std::exception &e) {
            // Ignore errors - for example if the game is over
        }
    }
    
    // Periodic visual refresh every X frames
    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter >= 30) {  // Refresh approximately every second (at 30 FPS)
        frameCounter = 0;
        
    }
}

void CoupGUI::toggleDebugMode()
{
    debugMode = !debugMode;
    addNotification(debugMode ? "Debug mode enabled" : "Debug mode disabled");
}

/**
 * Renders the entire game interface based on the current state.
 * This function is responsible for drawing all visual elements of the game
 * including backgrounds, panels, buttons, and game information.
 */
void CoupGUI::render()
{
    window.clear();

    // Draw appropriate screen based on current game state
    if (currentState == GuiState::SETUP)
    {
        // Draw the simple background for setup screen
        window.draw(background);
        
        // Draw the setup interface with player configuration options
        drawSetupScreen();
    }
    else if (currentState == GuiState::WIN_SCREEN)
    {
        // Draw simple background for win screen
        window.draw(background);
        
        // Draw win screen showing the winner and game results
        drawWinScreen();
    }
    else // GuiState::PLAYING - Main gameplay state
    {
        // Draw the game background
        window.draw(background);

        // Draw the game title at the top of the screen
        sf::Text titleText;
        titleText.setFont(titleFont);
        titleText.setString("Coup - Game");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(520, 30);
        window.draw(titleText);

        // Draw reset button to allow starting a new game
        drawResetButton();

        // Draw the main game board and all player cards
        window.draw(gameBoard);
        drawPlayers();

        // Draw the game log panel showing history of moves
        window.draw(logPanel);
        drawGameLog();

        // Draw special actions panel (for out-of-turn abilities)
        window.draw(specialActionsPanel);
        drawSpecialActions();

        // Draw the action panel showing available moves for current player
        window.draw(actionPanel);
        drawActionPanel();

        // Draw any active notifications or alerts
        drawNotifications();

        // Draw player selection buttons when needed
        // (when targeting a player or selecting a performer for special actions)
        if ((waitingForTarget && !playerButtons.empty()) || 
            (specialActionState != SpecialActionState::NONE && !playerButtons.empty()))
        {
            drawPlayerButtons();
        }

        // Draw debug information when debug mode is enabled
        if (debugMode)
        {
            drawDebugInfo();
        }
    }

    // Display everything on screen
    window.display();
}

void CoupGUI::drawPlayers()
{
    // Try-catch block to handle any exceptions during player rendering
    try {
        // Check if game or players exist
        if (game == nullptr || realPlayers.empty()) {
            return;  // No game or players to display
        }

        size_t numPlayers = realPlayers.size();

        // Card dimensions and positioning variables
        float cardWidth = 140;
        float cardHeight = 200;
        float startX = 60;
        float startY = 150;
        float spacingX = 180;
        int playersPerRow = 4;

        // Loop through all players to draw their cards
        for (size_t i = 0; i < numPlayers; i++)
        {
            // Check if player pointer is valid
            if (!realPlayers[i]) {
                cout << "Warning: null player at index " << i << endl;
                continue;
            }
            
            // Retrieve player information
            std::string playerName = realPlayers[i]->name();
            coup::Role playerRole = realPlayers[i]->role();
            int coins = realPlayers[i]->coins();
            bool isCurrentPlayer = (playerName == currentPlayerName);

            // Calculate position based on grid layout
            int row = static_cast<int>(i) / playersPerRow;
            int col = static_cast<int>(i) % playersPerRow;
            float x = startX + col * spacingX;
            float y = startY + row * (cardHeight + 30);

            // Create and configure player card rectangle
            sf::RectangleShape playerCard;
            playerCard.setSize(sf::Vector2f(cardWidth, cardHeight));
            playerCard.setPosition(x, y);
            playerCard.setFillColor(getRoleColor(playerRole));

            // Highlight current player with yellow border
            if (isCurrentPlayer)
            {
                playerCard.setOutlineThickness(4);
                playerCard.setOutlineColor(sf::Color::Yellow);
            }
            // Highlight potential targets with green border
            else if (waitingForTarget && playerName != currentPlayerName && realPlayers[i]->isActive())
            {
                playerCard.setOutlineThickness(3);
                playerCard.setOutlineColor(sf::Color::Green);
            }
            // Default border for other players
            else
            {
                playerCard.setOutlineThickness(1);
                playerCard.setOutlineColor(sf::Color(150, 150, 150));
            }

            // Draw the player card
            window.draw(playerCard);

            // Draw player name
            sf::Text nameText;
            nameText.setFont(mainFont);
            nameText.setString(playerName);
            nameText.setCharacterSize(18);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 10, y + 10);
            window.draw(nameText);

            // Draw player role
            sf::Text roleText;
            roleText.setFont(mainFont);
            roleText.setString(getRoleName(playerRole));
            roleText.setCharacterSize(16);
            roleText.setFillColor(sf::Color::White);
            roleText.setPosition(x + 10, y + 40);
            window.draw(roleText);

            // Draw coin count
            sf::Text coinsText;
            coinsText.setFont(mainFont);
            coinsText.setString("Coins: " + std::to_string(coins));
            coinsText.setCharacterSize(16);
            coinsText.setFillColor(sf::Color::Yellow);
            coinsText.setPosition(x + 10, y + 70);
            window.draw(coinsText);
        }
    } catch (const std::exception &e) {
        // Handle and display any errors that occur during rendering
        cout << "ERROR in drawPlayers(): " << e.what() << endl;
        // Show error message on screen instead of crashing
        sf::Text errorText;
        errorText.setFont(mainFont);
        errorText.setString("Error drawing players: " + std::string(e.what()));
        errorText.setCharacterSize(16);
        errorText.setFillColor(sf::Color::Red);
        errorText.setPosition(100, 200);
        window.draw(errorText);
    }
}

void CoupGUI::drawGameLog()
{
    sf::Text logTitle;
    logTitle.setFont(mainFont);
    logTitle.setString("Game Log");
    logTitle.setCharacterSize(18);
    logTitle.setFillColor(sf::Color::White);
    logTitle.setPosition(990, 110);
    window.draw(logTitle);

    // הצגת הודעות יומן (אחרונות בתחתית)
    // חישוב כמה הודעות אפשר להציג בגבולות הפאנל החדש
    size_t maxMessagesToShow = 12;  // פחות הודעות בגלל הפאנל המקוצר
    size_t numMessagesToShow = std::min(logMessages.size(), maxMessagesToShow);
    float yPos = 360; // תחתית הפאנל החדש (100+280-20)

    for (size_t i = 0; i < numMessagesToShow; i++)
    {
        size_t msgIndex = logMessages.size() - 1 - i;
        sf::Text msgText;
        msgText.setFont(mainFont);
        
        // קיצוץ הודעות ארוכות מדי
        std::string message = logMessages[msgIndex];
        if (message.length() > 35) {  // מגביל לפי רוחב הפאנל
            message = message.substr(0, 32) + "...";
        }
        
        msgText.setString(message);
        msgText.setCharacterSize(12);
        msgText.setFillColor(sf::Color(200, 200, 200));
        msgText.setPosition(980, yPos);
        window.draw(msgText);
        yPos -= 20; // קפיצה למעלה לשורה הבאה
        
        // עצירה אם הגענו לגבול העליון של הפאנל
        if (yPos < 130) break;  // מתחת לכותרת
    }
}

/**
 * Draws the action panel where players can select game actions
 * 
 * This method is responsible for rendering the action panel interface which displays
 * all available actions a player can take during their turn. It includes:
 * - Setting up and displaying the main action panel title
 * - Iterating through and drawing all basic action buttons
 * - Positioning and rendering text for each action button
 * - Creating a centered title for the basic actions section
 */
void CoupGUI::drawActionPanel()
{
    // Initialize the main action panel title
    sf::Text actionTitle;
    actionTitle.setFont(mainFont);
    // Set visual properties for the title
    actionTitle.setCharacterSize(18);
    actionTitle.setFillColor(sf::Color::White);
    actionTitle.setPosition(50, 625);
    window.draw(actionTitle);

    // Draw all basic action buttons and their labels
    for (size_t i = 0; i < basicActions.size(); i++)
    {
        // Draw the button background
        window.draw(basicActions[i].button);

        // Draw the button label text
        window.draw(basicActions[i].buttonText);
    }

    // Create and configure the basic actions section title
    sf::Text basicTitle;
    basicTitle.setFont(mainFont);
    basicTitle.setCharacterSize(18);
    basicTitle.setFillColor(sf::Color::White);

    // Center the title above the action buttons
    float titleX = actionPanel.getPosition().x + (actionPanel.getSize().x / 2);
    float titleY = 610;
    basicTitle.setOrigin(basicTitle.getLocalBounds().width / 2, 0);
    basicTitle.setPosition(titleX, titleY);

    // Render the title
    window.draw(basicTitle);
}

/**
 * Draws in-game notifications to the screen
 * 
 * This method is responsible for:
 * - Displaying notification messages at the top of the screen
 * - Automatically removing notifications after a timed period
 * - Centering each notification text for better visibility
 * 
 * The notification system works as a queue where messages are displayed
 * for 3 seconds before being removed to show the next notification.
 */
void CoupGUI::drawNotifications()
{
    // If there are no notifications to display, exit early
    if (notificationQueue.empty())
        return;

    // Create and configure the notification text
    sf::Text notifText;
    notifText.setFont(mainFont);
    notifText.setString(notificationQueue.front());  // Display the first notification in queue
    notifText.setCharacterSize(20);
    notifText.setFillColor(sf::Color::Yellow);      // Yellow color for visibility

    // Center the notification at the top of the screen
    sf::FloatRect textBounds = notifText.getLocalBounds();
    notifText.setPosition((window.getSize().x - textBounds.width) / 2, 60);

    // Render the notification
    window.draw(notifText);

    // Remove old notifications after 3 seconds
    static sf::Clock notifClock;
    if (notifClock.getElapsedTime().asSeconds() > 3.0f)
    {
        notificationQueue.pop();     // Remove the current notification
        notifClock.restart();        // Reset the timer for the next notification
    }
}

/**
 * Draws debug information on the screen when debug mode is enabled.
 * 
 * This method displays technical information including:
 * - Current FPS (Frames Per Second) calculated from globalClock
 * - Current game mode
 * - Size of the game board element
 * - Number of players in the game
 * 
 * The information is displayed in a small green text at the top-left corner
 * of the screen, providing developers with real-time performance metrics
 * and game state data for debugging purposes.
 */
void CoupGUI::drawDebugInfo()
{
    // Create a string stream to build the debug text
    std::stringstream debugText;
    
    // Calculate FPS by taking the reciprocal of the time elapsed since last frame
    // and add various debug information to the text stream
    debugText << "FPS: " << std::round(1.0f / globalClock.restart().asSeconds())
              << " | Mode: GAME"  // Current game mode
              << " | Elements: Board(" << gameBoard.getSize().x << "x" << gameBoard.getSize().y << ")"  // Board dimensions
              << " | Players: " << realPlayers.size();  // Number of active players

    // Create and configure the text object for displaying debug info
    sf::Text debugInfoText;
    debugInfoText.setFont(mainFont);
    debugInfoText.setString(debugText.str());
    debugInfoText.setCharacterSize(14);  // Small text size for debug info
    debugInfoText.setFillColor(sf::Color(100, 255, 100));  // Light green color
    debugInfoText.setPosition(10, 5);  // Position at top-left corner
    
    // Draw the debug text to the window
    window.draw(debugInfoText);
}

/**
 * Adds a new message to the game log history.
 * 
 * This method adds the provided message to the game's log history,
 * which keeps track of all game actions and events. It also maintains
 * a maximum log size by removing the oldest messages when the limit is reached.
 * 
 * @param message The message to add to the game log
 */
void CoupGUI::addLogMessage(const std::string &message)
{
    // Add the new message to the end of the log
    logMessages.push_back(message);

    // Limit the history to 100 messages by removing the oldest ones
    if (logMessages.size() > 100)
    {
        logMessages.erase(logMessages.begin());
    }
}

/**
 * Adds a notification message to the queue for display on screen.
 * 
 * This method queues a notification message that will be displayed to the player
 * during gameplay. Notifications are typically used to inform players about
 * game events, errors, or status updates.
 * 
 * @param message The notification text to be displayed
 */
void CoupGUI::addNotification(const std::string &message)
{
    notificationQueue.push(message);
}

/**
 * Determines the color associated with a specific game role.
 * 
 * This method returns the appropriate color for a given role from the
 * predefined color map. Each role in the game has a distinctive color
 * used for visual identification on player cards and UI elements.
 * 
 * @param role The game role to get a color for
 * @return The color associated with the role, or gray if not found
 */
sf::Color CoupGUI::getRoleColor(coup::Role role)
{
    if (roleColors.find(role) != roleColors.end())
    {
        return roleColors[role];
    }

    // Default color (gray) if role not found in map
    return sf::Color(100, 100, 100);
}

/**
 * Converts a role enumeration value to its display name.
 * 
 * This method translates the internal role enumeration to a human-readable
 * string representation for display in the UI. Each role in the game has
 * a specific name that identifies its character type.
 * 
 * @param role The game role to convert to string
 * @return The string name of the role
 */
std::string CoupGUI::getRoleName(coup::Role role)
{
    switch (role)
    {
    case coup::Role::GENERAL:
        return "General";
    case coup::Role::GOVERNOR:
        return "Governor";
    case coup::Role::SPY:
        return "Spy";
    case coup::Role::BARON:
        return "Baron";
    case coup::Role::JUDGE:
        return "Judge";
    case coup::Role::MERCHANT:
        return "Merchant";
    default:
        return "Unknown";
    }
}

/**
 * Handles click events on special action buttons.
 * 
 * This method is triggered when a player clicks on one of the special action buttons
 * that can be performed out of turn. It initiates the two-step selection process for
 * the special action.
 * 
 * @param actionIndex The index of the clicked special action in the specialActions array
 */
void CoupGUI::handleSpecialActionClick(int actionIndex)
{
    if (actionIndex < 0 || actionIndex >= static_cast<int>(specialActions.size()))
        return;

    const SpecialAction &action = specialActions[actionIndex];
    
    // Start the two-step selection mechanism
    startSpecialActionSelection(action.name, action.requiredRole);
}

/**
 * Draws all available special actions on the special actions panel.
 * 
 * This method renders the special actions panel with all out-of-turn actions
 * that players can perform. It displays the action name, the role required
 * for each action, and a representative player who has that role.
 * Special filtering is applied for Baron's Invest action to only show it
 * when the current player is a Baron.
 */
void CoupGUI::drawSpecialActions()
{
    // Draw title for the special actions panel
    sf::Text specialTitle;
    specialTitle.setFont(mainFont);
    specialTitle.setString("Out Of Turn Actions");
    specialTitle.setCharacterSize(18);
    specialTitle.setFillColor(sf::Color::White);
    specialTitle.setPosition(1030, 410);
    window.draw(specialTitle);

    // Draw player names and roles before the buttons
    float currentY = 450; // Initial Y position
    float spacing = 35;   // Spacing between buttons

    for (size_t i = 0; i < specialActions.size(); i++)
    {
        // Check whether to display the Baron's Invest button if current player is not a Baron
        bool showButton = true;
        if (specialActions[i].requiredRole == coup::Role::BARON) {
            // Search for the current player in the real players list
            bool isCurrentPlayerBaron = false;
            for (const auto &player : realPlayers) {
                if (player->name() == currentPlayerName && player->role() == coup::Role::BARON) {
                    isCurrentPlayerBaron = true;
                    break;
                }
            }
            // If current player is not a Baron, don't show the button
            if (!isCurrentPlayerBaron) {
                showButton = false;
            }
        }
        
        if (!showButton) {
            continue;  // Skip displaying this button
        }

        // Find a player example with the appropriate role
        std::string playerExample = "";
        for (const auto &player : realPlayers)
        {
            if (player->role() == specialActions[i].requiredRole)
            {
                playerExample = player->name();
                break;
            }
        }

        // Abbreviate to first letter
        std::string shortName = "";
        if (!playerExample.empty()) {
            shortName = playerExample.substr(0, 1);
        }

        // Update current button position
        specialActions[i].button.setPosition(1080, currentY);
        specialActions[i].buttonText.setPosition(
            1080 + (specialActions[i].button.getSize().x - specialActions[i].buttonText.getLocalBounds().width) / 2,
            currentY + 5);

        // Draw player name and role
        sf::Text playerText;
        playerText.setFont(mainFont);
        playerText.setString(shortName + " (" + getRoleName(specialActions[i].requiredRole) + ")");
        playerText.setCharacterSize(16);
        playerText.setFillColor(sf::Color::White);
        playerText.setPosition(980, currentY + 5);
        window.draw(playerText);

        // Draw the button and its text
        window.draw(specialActions[i].button);
        window.draw(specialActions[i].buttonText);

        // Advance the vertical position for the next button
        currentY += spacing;
    }
}

/**
 * Handles the click event on a basic action button.
 * This function processes different game actions based on the selected button.
 * 
 * @param actionIndex The index of the clicked action button
 */
void CoupGUI::handleBasicActionClick(int actionIndex)
{
    // Validate the action index to ensure it's within range
    if (actionIndex < 0 || actionIndex >= static_cast<int>(basicActions.size()))
        return;

    // Get the selected action details
    const BasicAction &action = basicActions[actionIndex];
    std::string actionName = action.name;
    
    try
    {
        // Get the current player
        shared_ptr<coup::Player> currentPlayer = game->getPlayer();

        // Perform the appropriate action based on the button clicked
        if (actionName == "Gather")
        {
            // Income action - collect 1 coin
            currentPlayer->gather();
            addLogMessage(currentPlayerName + " gathered 1 coin");
            
            // Update current player name after the action
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }

        else if (actionName == "Tax")
        {
            // Tax action - collect 2 coins (Duke's special ability)
            currentPlayer->tax();
            addLogMessage(currentPlayerName + " taxed and received 2 coins");
            
            // Update current player name after the action
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }
        else if (actionName == "Bribe")
        {
            // Bribe action
            currentPlayer->bribe();
            addLogMessage(currentPlayerName + " bribed another player");
            
            // Update current player name after the action
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }
        else if (actionName == "Arrest")
        {
            // Arrest action requires selecting a target player
            pendingActionName = "Arrest";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform an arrest");
        }
        else if (actionName == "Sanction")
        {
            // Sanction action requires selecting a target player
            pendingActionName = "Sanction";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform a sanction");
        }
        else if (actionName == "Coup")
        {
            // Coup action requires selecting a target player
            pendingActionName = "Coup";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform a coup");
        }
        
        // Notify the user of successful action and next player's turn
        addNotification(action.name + " action succeeded. Next player: " + currentPlayerName);
    }
    catch (const std::exception &e)
    {
        // Handle any errors that occur during action execution
        addNotification("Error: " + std::string(e.what()));
        addLogMessage("Failed action: " + std::string(e.what()));
    }
}

/**
 * Helper functions for player selection buttons
 */

/**
 * Creates player selection buttons for targeting actions.
 * This function generates buttons for each active player except the current player.
 * These buttons are used when a player needs to select another player as a target
 * for actions such as coup, arrest, or sanction.
 */
void CoupGUI::createPlayerButtons()
{
    // Clear any existing player buttons
    playerButtons.clear();
    
    // Define button dimensions and positioning
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;
    float startY = 300;
    float spacing = 45;
    
    int buttonIndex = 0;
    
    // Create a button for each active player that isn't the current player
    for (const auto &player : realPlayers)
    {
        if (player->name() != currentPlayerName && player->isActive())
        {
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            // Configure the button rectangle
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(sf::Color(70, 130, 180)); // Blue color
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::White);
            
            // Configure the button text
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name());
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            // Add the button to our collection
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

/**
 * Clears all player selection buttons.
 * Called when player selection is complete or canceled.
 */
void CoupGUI::clearPlayerButtons()
{
    playerButtons.clear();
}

/**
 * Draws the player selection interface with all player buttons.
 * This displays a modal dialog with buttons for each targetable player
 * and instructions based on the current action context.
 */
void CoupGUI::drawPlayerButtons()
{
    // Create a semi-transparent background panel
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(300, 400));
    background.setPosition(350, 250);
    background.setFillColor(sf::Color(30, 30, 30, 200)); // Semi-transparent black
    background.setOutlineThickness(3);
    background.setOutlineColor(sf::Color::Yellow);
    window.draw(background);
    
    // Create a dynamic title based on the current selection state
    sf::Text title;
    title.setFont(mainFont);
    title.setCharacterSize(20);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(420, 260);
    
    // Set appropriate title text based on the current selection context
    if (specialActionState == SpecialActionState::SELECTING_PERFORMER)
    {
        title.setString("Select " + getRoleName(pendingSpecialActionRole) + ":");
    }
    else if (specialActionState == SpecialActionState::SELECTING_TARGET)
    {
        title.setString("Select target:");
    }
    else if (waitingForTarget)
    {
        title.setString("Choose a target:");
    }
    else
    {
        title.setString("Select player:");
    }
    
    window.draw(title);
    
    // Draw all player buttons
    for (const auto &playerButton : playerButtons)
    {
        window.draw(playerButton.button);
        window.draw(playerButton.buttonText);
    }
    
    // Draw cancel instruction
    sf::Text cancelText;
    cancelText.setFont(mainFont);
    cancelText.setString("ESC to cancel");
    cancelText.setCharacterSize(14);
    cancelText.setFillColor(sf::Color(200, 200, 200));
    cancelText.setPosition(450, 620);
    window.draw(cancelText);
}

/**
 * Executes the arrest action against a target player
 * This function handles the process of arresting another player
 * and updates the game state accordingly
 *
 * @param targetPlayer The player who will be arrested
 */
void CoupGUI::executeArrest(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        // Get current player from the game
        auto currentPlayer = game->getPlayer();
        
        // Execute the arrest action
        currentPlayer->arrest(targetPlayer);
        addLogMessage(currentPlayerName + " arrested " + targetPlayer->name());
        
        // Update current player name after the action
        // This is needed because turns change after actions
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // If there's an error updating, continue with existing name
        }
        
        addNotification("Arrest completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in arrest: " + std::string(e.what()));
    }
}

/**
 * Executes the sanction action against a target player
 * This function handles applying sanctions to another player
 * and updates the game state accordingly
 *
 * @param targetPlayer The player who will be sanctioned
 */
void CoupGUI::executeSanction(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        // Get current player from the game
        auto currentPlayer = game->getPlayer();
        
        // Execute the sanction action
        currentPlayer->sanction(*targetPlayer);
        addLogMessage(currentPlayerName + " sanctioned " + targetPlayer->name());
        
        // Update current player name after the action
        // This is needed because turns change after actions
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // If there's an error updating, continue with existing name
        }
        
        addNotification("Sanction completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in sanction: " + std::string(e.what()));
    }
}

/**
 * Executes the coup action against a target player
 * This function handles the process of eliminating another player
 * from the game and updates the game state accordingly
 *
 * @param targetPlayer The player who will be eliminated with a coup
 */
void CoupGUI::executeCoup(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        // Get current player from the game
        auto currentPlayer = game->getPlayer();
        
        // Execute the coup action
        currentPlayer->coup(targetPlayer);
        addLogMessage(currentPlayerName + " performed coup on " + targetPlayer->name());
        
        // Update current player name after the action
        // This is needed because turns change after actions
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // If there's an error updating, continue with existing name
        }
        
        addNotification("Coup completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in coup: " + std::string(e.what()));
    }
}

/**
 * Starts the selection process for a special action.
 * This function initializes the selection process for actions that require
 * a performer (e.g., Undo Tax, Undo Bribe, Undo Coup) or a target (e.g., see and block arrest).
 * 
 * @param actionName The name of the special action to perform
 * @param requiredRole The role required to perform the action
 */
/**
 * Starts the selection process for a special action.
 * This function initializes the selection process for actions that require
 * a performer (e.g., Undo Tax, Undo Bribe, Undo Coup) or a target (e.g., see and block arrest).
 * It identifies eligible players for performing the action and manages the flow based on
 * how many eligible players are available.
 * 
 * @param actionName The name of the special action to perform
 * @param requiredRole The role required to perform the action
 */
void CoupGUI::startSpecialActionSelection(const std::string& actionName, coup::Role requiredRole)
{
    // Count how many players have the required role
    std::vector<std::shared_ptr<coup::Player>> availablePerformers;
    for (const auto &player : realPlayers)
    {
        if (player->role() == requiredRole && player->isActive())
        {
            availablePerformers.push_back(player);
        }
    }
    
    if (availablePerformers.empty())
    {
        addNotification("No active " + getRoleName(requiredRole) + " players found!");
        return;
    }
    
    // Define variables before checking player count
    pendingSpecialAction = actionName;
    pendingSpecialActionRole = requiredRole;
    
    // Check if the action requires a target
    bool requiresTarget = (actionName == "see and block arrest");
    
    if (availablePerformers.size() == 1)
    {
        // Only one player has this role - automatic selection
        selectedPerformer = availablePerformers[0];
        
        if (requiresTarget)
        {
            // Action requires a target - proceed to target selection phase
            specialActionState = SpecialActionState::SELECTING_TARGET;
            createTargetButtons(selectedPerformer);
            addNotification("Select target for " + actionName);
        }
        else
        {
            // Action doesn't require a target - execute directly
            executeSpecialAction(selectedPerformer);
        }
        return;
    }
    
    // Multiple eligible players - start selection mechanism
    specialActionState = SpecialActionState::SELECTING_PERFORMER;
    
    createPerformerButtons(requiredRole);
    addNotification("Select which " + getRoleName(requiredRole) + " performs " + actionName);
}

/**
 * Creates interactive buttons for players with a specific role
 * 
 * This method generates a set of clickable buttons for all active players who have
 * the specified role. These buttons are used to select which player will perform
 * a special action associated with their role.
 * 
 * @param requiredRole The role that players must have to be included in the selection
 */
void CoupGUI::createPerformerButtons(coup::Role requiredRole)
{
    // Clear any existing buttons from previous selections
    playerButtons.clear();
    
    // Define button dimensions and positioning
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;     // X coordinate for the left edge of all buttons
    float startY = 250;     // Y coordinate for the top of the first button
    float spacing = 45;     // Vertical space between buttons
    
    int buttonIndex = 0;
    
    // Iterate through all players in the game
    for (const auto &player : realPlayers)
    {
        // Only create buttons for active players with the required role
        if (player->role() == requiredRole && player->isActive())
        {
            // Create a new button for this player
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            // Configure the button's appearance
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(getRoleColor(requiredRole));  // Color based on role
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::White);
            
            // Configure the text on the button
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name() + " (" + getRoleName(requiredRole) + ")");
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            
            // Center the text on the button
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            // Add the configured button to the collection
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

/**
 * Creates buttons for selecting target players
 * @param performer The player who will perform an action on the target
 */
void CoupGUI::createTargetButtons(std::shared_ptr<coup::Player> performer)
{
    // Clear any existing player buttons
    playerButtons.clear();
    
    // Define button dimensions and positioning
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;
    float startY = 250;
    float spacing = 45;
    
    int buttonIndex = 0;
    
    for (const auto &player : realPlayers)
    {
        // Only active players that are not the performer themselves
        if (player->isActive() && player->name() != performer->name())
        {
            // Create a new button for this player
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            // Configure the button's appearance
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(sf::Color(70, 130, 180)); // Steel blue color
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::Green);
            
            // Configure the text on the button
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name());
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            
            // Center the text on the button
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            // Add the configured button to the collection
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

/**
 * Executes a special action by a player, possibly targeting another player
 * @param performer The player performing the special action
 * @param target The optional target player of the action
 */
void CoupGUI::executeSpecialAction(std::shared_ptr<coup::Player> performer, std::shared_ptr<coup::Player> target)
{
    cout << "DEBUG: Executing special action " << pendingSpecialAction << " by " << performer->name();
    if (target) {
        cout << " on " << target->name();
    }
    cout << endl;
    
    try
    {
        if (pendingSpecialAction == "Undo Tax")
        {
            performer->undo(coup::UndoableAction::TAX);
            addLogMessage(performer->name() + " (Governor) undid tax action");
            addNotification("Tax action undone by " + performer->name() + "!");
        }
        else if (pendingSpecialAction == "Undo Bribe")
        {
            cout << "DEBUG: Undoing bribe action by " << performer->name() << endl;
            performer->undo(coup::UndoableAction::BRIBE);
            addLogMessage(performer->name() + " (Judge) undid bribe action");
            addNotification("Bribe action undone by " + performer->name() + "!");
        }
        else if (pendingSpecialAction == "Undo Coup")
        {
            performer->undo(coup::UndoableAction::COUP);
            addLogMessage(performer->name() + " (General) undid coup action");
            addNotification("Coup undone by " + performer->name() + "!");
        }
        else if (pendingSpecialAction == "see and block arrest")
        {
            if (target)
            {
                performer->view_coins(*target);
                addLogMessage(performer->name() + " (Spy) viewed " + target->name() + "'s coins: " + std::to_string(target->coins()));
                addNotification(performer->name() + " viewed coins and blocked arrest!");
            }
            else
            {
                addNotification("Spy action requires a target!");
                return;
            }
        }
        else if (pendingSpecialAction == "Invest")
        {
            performer->invest();
            addLogMessage(performer->name() + " (Baron) invested");
            addNotification(performer->name() + " invested successfully!");
        }
    }
    catch (const std::exception &e)
    {
        addNotification("Special action failed: " + std::string(e.what()));
        addLogMessage("Failed special action by " + performer->name() + ": " + std::string(e.what()));
    }
    
    // Reset the special action state
    cancelSpecialActionSelection();
}

/**
 * Cancels any pending special action selection and resets related state variables
 */
void CoupGUI::cancelSpecialActionSelection()
{
    specialActionState = SpecialActionState::NONE;
    pendingSpecialAction = "";
    selectedPerformer = nullptr;
    clearPlayerButtons();
}

/**
 * Draws the reset button on the game screen
 */
void CoupGUI::drawResetButton()
{
    window.draw(resetButton);
    window.draw(resetButtonText);
}

/**
 * Draws the win screen showing the winner and game over options
 */
void CoupGUI::drawWinScreen()
{
    // Semi-transparent background overlay
    window.draw(winOverlay);

    // Update victory text
    winTitle.setString(winnerName + " won!");
    sf::FloatRect titleBounds = winTitle.getLocalBounds();
    winTitle.setPosition((1280 - titleBounds.width) / 2, 250);

    winSubtitle.setString("Game Over!");
    sf::FloatRect subtitleBounds = winSubtitle.getLocalBounds();
    winSubtitle.setPosition((1280 - subtitleBounds.width) / 2, 320);

    // Draw the text elements
    window.draw(winTitle);
    window.draw(winSubtitle);

    // Update button text positioning
    sf::FloatRect newGameBounds = newGameButtonText.getLocalBounds();
    newGameButtonText.setPosition(
        newGameButton.getPosition().x + (newGameButton.getSize().x - newGameBounds.width) / 2,
        newGameButton.getPosition().y + (newGameButton.getSize().y - newGameBounds.height) / 2 - 5
    );

    sf::FloatRect exitBounds = exitButtonText.getLocalBounds();
    exitButtonText.setPosition(
        exitButton.getPosition().x + (exitButton.getSize().x - exitBounds.width) / 2,
        exitButton.getPosition().y + (exitButton.getSize().y - exitBounds.height) / 2 - 5
    );

    // Draw the buttons
    window.draw(newGameButton);
    window.draw(newGameButtonText);
    window.draw(exitButton);
    window.draw(exitButtonText);
}

/**
 * Resets the current game while keeping the same players and roles
 */
void CoupGUI::resetGame()
{
    try {
        if (!game || realPlayers.empty()) {
            addNotification("Cannot reset - no game initialized!");
            return;
        }

        // Store existing player data
        std::vector<std::string> playerNames;
        std::vector<coup::Role> playerRoles;
        
        for (const auto& player : realPlayers) {
            if (player) {
                playerNames.push_back(player->name());
                playerRoles.push_back(player->role());
            }
        }
        
        // Create a new game instance
        game = std::make_shared<coup::Game>();
        
        // Clear the old players list
        realPlayers.clear();
        
        // Create new players with the same names and roles
        for (size_t i = 0; i < playerNames.size(); i++) {
            auto newPlayer = game->createPlayer(playerNames[i], playerRoles[i]);
            realPlayers.push_back(newPlayer);
        }

        // Return to normal play state
        currentState = GuiState::PLAYING;

        // Reset current player to the first one
        if (!realPlayers.empty() && realPlayers[0]) {
            currentPlayerName = realPlayers[0]->name();
        }

        // Clear temporary states
        waitingForTarget = false;
        pendingActionName = "";
        specialActionState = SpecialActionState::NONE;
        clearPlayerButtons();

        // Clear logs and notifications
        logMessages.clear();
        while (!notificationQueue.empty()) {
            notificationQueue.pop();
        }

        addLogMessage("Game reset successfully!");
        addNotification("Game has been reset! " + currentPlayerName + "'s turn");

    } catch (const std::exception& e) {
        addNotification("Error resetting game: " + std::string(e.what()));
    }
}

/**
 * Checks if there is a winner in the game and updates the game state accordingly
 */
void CoupGUI::checkForWinner()
{
    if (!game || realPlayers.empty()) {
        return;
    }

    // Count active players
    int activePlayers = 0;
    std::shared_ptr<coup::Player> lastActivePlayer = nullptr;

    for (const auto& player : realPlayers) {
        if (player && player->isActive()) {
            activePlayers++;
            lastActivePlayer = player;
        }
    }

    // If only one active player remains, we have a winner
    if (activePlayers == 1 && lastActivePlayer) {
        winnerName = lastActivePlayer->name();
        currentState = GuiState::WIN_SCREEN;
        addLogMessage(winnerName + " won the game!");
        addNotification("Game Over! " + winnerName + " is the winner!");
    }
}

/**
 * Starts a new game by returning to the setup screen
 */
void CoupGUI::startNewGame()
{
    // Return to setup screen
    currentState = GuiState::SETUP;
    currentSetupStep = 0;
    numPlayersToSetup = 0;
    activeInputPlayer = -1;
    currentInputText = "";
    
    // Clear game data
    game = nullptr;
    realPlayers.clear();
    setupPlayers.clear();
    
    // Clear temporary states
    waitingForTarget = false;
    pendingActionName = "";
    specialActionState = SpecialActionState::NONE;
    clearPlayerButtons();
    
    // Clear logs and notifications
    logMessages.clear();
    while (!notificationQueue.empty()) {
        notificationQueue.pop();
    }
    
    addNotification("Back to setup screen");
}

/**
 * Handles the reset button click event
 */
void CoupGUI::handleResetClick()
{
    addNotification("Resetting game...");
    resetGame();
}

/**
 * Handles mouse clicks on the win screen
 * @param mousePos The position of the mouse click
 */
void CoupGUI::handleWinScreenClick(sf::Vector2i mousePos)
{
    sf::FloatRect newGameBounds = newGameButton.getGlobalBounds();
    sf::FloatRect exitBounds = exitButton.getGlobalBounds();

    if (newGameBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        // New Game button
        startNewGame();
    }
    else if (exitBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        // Exit button
        window.close();
    }
}

/**
 * Draws the setup screen where players configure the game
 * This handles both player count selection and player configuration
 */
void CoupGUI::drawSetupScreen()
{
    // Main title
    sf::Text titleText;
    titleText.setFont(mainFont);
    titleText.setString("Coup - Setup");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition((1280 - titleBounds.width) / 2, 100);
    window.draw(titleText);
    
    if (currentSetupStep == 0) {
        // Player count selection step
        sf::Text instructionText;
        instructionText.setFont(mainFont);
        instructionText.setString("Choose number of players for the game:");
        instructionText.setCharacterSize(24);
        instructionText.setFillColor(sf::Color::White);
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setPosition((1280 - instrBounds.width) / 2, 200);
        window.draw(instructionText);
        
        // Draw player count buttons (2-6 players)
        for (int i = 0; i < 5; i++) {
            window.draw(playerCountButtons[i]);
            window.draw(playerCountTexts[i]);
        }
    }
    else {
        // Player setup step
        sf::Text instructionText;
        instructionText.setFont(mainFont);
        instructionText.setString("Setup players:");
        instructionText.setCharacterSize(24);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(50, 170);
        window.draw(instructionText);
        
        // Draw player configuration inputs
        float yPos = 220;
        for (int i = 0; i < numPlayersToSetup; i++) {
            // Player number label
            sf::Text playerNumText;
            playerNumText.setFont(mainFont);
            playerNumText.setString("Player " + std::to_string(i + 1) + ":");
            playerNumText.setCharacterSize(20);
            playerNumText.setFillColor(sf::Color::White);
            playerNumText.setPosition(50, yPos);
            window.draw(playerNumText);
            
            // Name input field
            if (i < static_cast<int>(setupPlayers.size())) {
                setupPlayers[i].nameInput.setPosition(200, yPos - 5);
                setupPlayers[i].nameInput.setSize(sf::Vector2f(150, 30));
                setupPlayers[i].nameInput.setFillColor(activeInputPlayer == i ? sf::Color(100, 100, 255) : sf::Color(60, 60, 60));
                setupPlayers[i].nameInput.setOutlineThickness(2);
                setupPlayers[i].nameInput.setOutlineColor(sf::Color::White);
                window.draw(setupPlayers[i].nameInput);
                
                setupPlayers[i].nameText.setFont(mainFont);
                setupPlayers[i].nameText.setString(setupPlayers[i].name);
                setupPlayers[i].nameText.setCharacterSize(16);
                setupPlayers[i].nameText.setFillColor(sf::Color::White);
                setupPlayers[i].nameText.setPosition(210, yPos);
                window.draw(setupPlayers[i].nameText);
                
                // Role selection buttons
                std::vector<std::string> roleNames = {"General", "Governor", "Spy", "Baron", "Judge", "Merchant"};
                std::vector<coup::Role> roles = {coup::Role::GENERAL, coup::Role::GOVERNOR, coup::Role::SPY, 
                                               coup::Role::BARON, coup::Role::JUDGE, coup::Role::MERCHANT};
                
                for (int j = 0; j < 6; j++) {
                    setupPlayers[i].roleButtons[j].setPosition(370 + j * 85, yPos - 5);
                    setupPlayers[i].roleButtons[j].setSize(sf::Vector2f(80, 30));
                    
                    // Highlight selected role in green
                    if (setupPlayers[i].roleSelected && setupPlayers[i].role == roles[j]) {
                        setupPlayers[i].roleButtons[j].setFillColor(sf::Color(60, 180, 60)); // Green
                    } else {
                        setupPlayers[i].roleButtons[j].setFillColor(getRoleColor(roles[j]));
                    }
                    
                    setupPlayers[i].roleButtons[j].setOutlineThickness(1);
                    setupPlayers[i].roleButtons[j].setOutlineColor(sf::Color::White);
                    window.draw(setupPlayers[i].roleButtons[j]);
                    
                    setupPlayers[i].roleButtonTexts[j].setFont(mainFont);
                    setupPlayers[i].roleButtonTexts[j].setString(roleNames[j]);
                    setupPlayers[i].roleButtonTexts[j].setCharacterSize(12);
                    setupPlayers[i].roleButtonTexts[j].setFillColor(sf::Color::White);
                    sf::FloatRect textBounds = setupPlayers[i].roleButtonTexts[j].getLocalBounds();
                    setupPlayers[i].roleButtonTexts[j].setPosition(
                        370 + j * 85 + (80 - textBounds.width) / 2,
                        yPos + (30 - textBounds.height) / 2 - 5
                    );
                    window.draw(setupPlayers[i].roleButtonTexts[j]);
                }
            }
            
            yPos += 60;
        }
        
        // Check if all players are ready to start
        bool allReady = true;
        for (int i = 0; i < numPlayersToSetup; i++) {
            if (i >= static_cast<int>(setupPlayers.size()) || 
                !setupPlayers[i].nameCompleted || !setupPlayers[i].roleSelected) {
                allReady = false;
                break;
            }
        }
        
        // Draw start game button if all players are configured
        if (allReady) {
            window.draw(startGameButton);
            window.draw(startGameButtonText);
        }
        
        // Instructions for players
        sf::Text helpText;
        helpText.setFont(mainFont);
        helpText.setString("Click on the name field to enter a name, and select a role from the buttons");
        helpText.setCharacterSize(16);
        helpText.setFillColor(sf::Color(200, 200, 200));
        helpText.setPosition(50, 680);
        window.draw(helpText);
    }
}

/**
 * Handles click events on the setup screen
 * @param mousePos The position of the mouse click
 */
void CoupGUI::handleSetupScreenClick(sf::Vector2i mousePos)
{
    if (currentSetupStep == 0) {
        // Player count selection step
        for (int i = 0; i < 5; i++) {
            sf::FloatRect buttonBounds = playerCountButtons[i].getGlobalBounds();
            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                numPlayersToSetup = i + 2; // 2-6 players
                currentSetupStep = 1;
                
                // Initialize player setup structures
                setupPlayers.clear();
                setupPlayers.resize(numPlayersToSetup);
                
                for (int j = 0; j < numPlayersToSetup; j++) {
                    setupPlayers[j].name = "";
                    setupPlayers[j].role = coup::Role::GENERAL;
                    setupPlayers[j].nameCompleted = false;
                    setupPlayers[j].roleSelected = false;
                }
                
                activeInputPlayer = -1;
                addNotification(std::to_string(numPlayersToSetup) + " players selected");
                break;
            }
        }
    }
    else {
        // Player configuration step
        float yPos = 220;
        
        for (int i = 0; i < numPlayersToSetup && i < static_cast<int>(setupPlayers.size()); i++) {
            // Check for clicks on name input fields
            sf::FloatRect nameInputBounds(200, yPos - 5, 150, 30);
            if (nameInputBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                activeInputPlayer = i;
                currentInputText = setupPlayers[i].name;
                break;
            }
            
            // Check for clicks on role buttons
            std::vector<coup::Role> roles = {coup::Role::GENERAL, coup::Role::GOVERNOR, coup::Role::SPY, 
                                           coup::Role::BARON, coup::Role::JUDGE, coup::Role::MERCHANT};
            
            for (int j = 0; j < 6; j++) {
                sf::FloatRect roleButtonBounds(370 + j * 85, yPos - 5, 80, 30);
                if (roleButtonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                    setupPlayers[i].role = roles[j];
                    setupPlayers[i].roleSelected = true;
                    addNotification("Role " + getRoleName(roles[j]) + " for player " + std::to_string(i + 1) + " selected");
                    break;
                }
            }
            
            yPos += 60;
        }
        
        // Check for clicks on the start game button
        sf::FloatRect startBounds = startGameButton.getGlobalBounds();
        if (startBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            // Verify all players are ready
            bool allReady = true;
            for (int i = 0; i < numPlayersToSetup; i++) {
                if (!setupPlayers[i].nameCompleted || !setupPlayers[i].roleSelected) {
                    allReady = false;
                    break;
                }
            }
            
            if (allReady) {
                createGameFromSetup();
            } else {
                addNotification("Please fill in the details of all players!");
            }
        }
    }
}

/**
 * Handles text input for player name fields
 * @param unicode The Unicode value of the character input
 */
void CoupGUI::handleTextInput(sf::Uint32 unicode)
{
    if (currentState != GuiState::SETUP || activeInputPlayer < 0 || 
        activeInputPlayer >= static_cast<int>(setupPlayers.size())) {
        return;
    }
    
    if (unicode == 8) { // Backspace
        if (!currentInputText.empty()) {
            currentInputText.pop_back();
        }
    }
    else if (unicode == 13) { // Enter key
        if (!currentInputText.empty()) {
            setupPlayers[activeInputPlayer].name = currentInputText;
            setupPlayers[activeInputPlayer].nameCompleted = true;
            activeInputPlayer = -1;
            currentInputText = "";
            addNotification("Player name saved!");
        }
    }
    else if (unicode >= 32 && unicode < 128) { // Standard ASCII characters
        if (currentInputText.length() < 15) { // Limit name length
            currentInputText += static_cast<char>(unicode);
        }
    }
    
    // Update displayed text
    if (activeInputPlayer >= 0 && activeInputPlayer < static_cast<int>(setupPlayers.size())) {
        setupPlayers[activeInputPlayer].name = currentInputText;
    }
}

/**
 * Creates a new game based on the setup configuration
 * Initializes the game with players and their roles
 */
void CoupGUI::createGameFromSetup()
{
    try {
        // Create new game instance
        game = std::make_shared<coup::Game>();
        realPlayers.clear();
        
        // Create players based on configuration
        for (int i = 0; i < numPlayersToSetup; i++) {
            auto player = game->createPlayer(setupPlayers[i].name, setupPlayers[i].role);
            realPlayers.push_back(player);
        }
        
        // Switch to playing state
        currentState = GuiState::PLAYING;
        
        // Set current player
        if (!realPlayers.empty()) {
            currentPlayerName = realPlayers[0]->name();
        }
        
        // Reset temporary states
        waitingForTarget = false;
        pendingActionName = "";
        specialActionState = SpecialActionState::NONE;
        clearPlayerButtons();
        
        // Clear logs and notifications
        logMessages.clear();
        while (!notificationQueue.empty()) {
            notificationQueue.pop();
        }
        
        addLogMessage("Game started with " + std::to_string(realPlayers.size()) + " players");
        addNotification("Game started! Turn of " + currentPlayerName);
        
        std::cout << "Game created successfully with " << realPlayers.size() << " players" << std::endl;
        
    } catch (const std::exception& e) {
        addNotification("Error creating game: " + std::string(e.what()));
        std::cerr << "Error creating game: " << e.what() << std::endl;
    }
}
