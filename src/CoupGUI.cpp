#include "CoupGUI.hpp"
#include <iostream>
#include <sstream>
#include <cmath>

void CoupGUI::initializeGui()
{
    window.create(sf::VideoMode(1280, 720), "Coup - Game", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    if (!mainFont.loadFromFile("assets/fonts/Heebo-Regular.ttf"))
    {
        std::cerr << "Failed to load main font" << std::endl;
    }
    titleFont = mainFont;

    // אתחול רקע פשוט
    background.setSize(sf::Vector2f(1280, 720));
    background.setFillColor(sf::Color(20, 30, 48)); // כחול כהה אלגנטי

    // אתחול לוח המשחק
    gameBoard.setSize(sf::Vector2f(900, 500));
    gameBoard.setPosition(30, 100);
    gameBoard.setFillColor(sf::Color(30, 40, 60));
    gameBoard.setOutlineThickness(2);
    gameBoard.setOutlineColor(sf::Color(100, 120, 180));

    // אתחול לוח יומן
    logPanel.setSize(sf::Vector2f(280, 280));
    logPanel.setPosition(970, 100);
    logPanel.setFillColor(sf::Color(25, 35, 55));
    logPanel.setOutlineThickness(2);
    logPanel.setOutlineColor(sf::Color(100, 120, 180));

    // אתחול לוח פעולות
    actionPanel.setSize(sf::Vector2f(1220, 80));
    actionPanel.setPosition(30, 620);
    actionPanel.setFillColor(sf::Color(25, 35, 55));
    actionPanel.setOutlineThickness(2);
    actionPanel.setOutlineColor(sf::Color(100, 120, 180));

    // אתחול פאנל פעולות מיוחדות
    specialActionsPanel.setSize(sf::Vector2f(280, 180));
    specialActionsPanel.setPosition(970, 400);
    specialActionsPanel.setFillColor(sf::Color(25, 35, 55));
    specialActionsPanel.setOutlineThickness(2);
    specialActionsPanel.setOutlineColor(sf::Color(100, 120, 180));

    // יצירת צבעים לפי תפקידים
    roleColors[coup::Role::GENERAL] = sf::Color(180, 40, 40);  // אדום
    roleColors[coup::Role::GOVERNOR] = sf::Color(40, 80, 180); // כחול
    roleColors[coup::Role::SPY] = sf::Color(100, 100, 100);    // אפור
    roleColors[coup::Role::BARON] = sf::Color(180, 140, 40);   // זהב
    roleColors[coup::Role::JUDGE] = sf::Color(80, 40, 100);    // סגול
    roleColors[coup::Role::MERCHANT] = sf::Color(40, 140, 40); // ירוק

    // אתחול כפתורי פעולות מיוחדות
    initializeSpecialActions();

    // אתחול כפתורי פעולות בסיסיות
    initializeBasicActions();
    
    // אתחול כפתור ריסט
    initializeResetButton();
    
    // אתחול מסך ניצחון
    initializeWinScreen();
}

// קונסטרקטור רגיל - מתחיל במסך הגדרות
CoupGUI::CoupGUI()
    : currentState(GuiState::SETUP),
      globalClock(),
      debugMode(false),
      numPlayersToSetup(0),
      currentSetupStep(0),
      activeInputPlayer(-1),
      game(nullptr),
      waitingForTarget(false),
      pendingActionName(""),
      specialActionState(SpecialActionState::NONE),
      pendingSpecialAction(""),
      pendingSpecialActionRole(coup::Role::GENERAL),
      selectedPerformer(nullptr)
{
    std::cout << "Initializing Coup Game Setup Screen" << std::endl;
    
    // אתחול הממשק הגרפי
    initializeGui();
    
    // אתחול מסך הגדרות
    initializeSetupScreen();
}



// קונסטרקטור שמקבל משחק ושחקנים קיימים
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
      pendingSpecialActionRole(coup::Role::GENERAL), // ערך ברירת מחדל
      selectedPerformer(nullptr)
{
    std::cout << "Initializing Coup Game with provided players" << std::endl;
    
    // אתחול הממשק הגרפי
    initializeGui();
    
    // הגדרת השחקן הנוכחי
    if (!realPlayers.empty()) {
        auto players = game->players();
        if (!players.empty()) {
            currentPlayerName = players[0];
        } else if (realPlayers[0]) {
            currentPlayerName = realPlayers[0]->name();
        }
    }
    
    // הוספת הודעת פתיחה
    addLogMessage("Game started with " + std::to_string(realPlayers.size()) + " players");
    addNotification("Game started! " + currentPlayerName + "'s turn");
}

CoupGUI::~CoupGUI()
{
    // אין משאבים דינמיים לנקות
}


void CoupGUI::initializeSpecialActions()
{
    // יצירת רשימת פעולות מיוחדות
    specialActions.clear();

    // מיקום ומידות הכפתורים
    float buttonWidth = 150;
    float buttonHeight = 30;
    float startX = 985;
    float startY = 450;
    float spacing = 35;

    // יצירת הכפתורים לפי התפקידים

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

void CoupGUI::initializeBasicActions()
{
    // יצירת רשימת פעולות בסיסיות
    basicActions.clear();

    // מיקום ומידות הכפתורים - כל הכפתורים בשורה אחת
    float buttonWidth = 120;
    float buttonHeight = 40;
    float totalWidth = 6 * buttonWidth + 5 * 10;          // 6 כפתורים + 5 רווחים של 10 פיקסלים
    float startX = (window.getSize().x - totalWidth) / 2; // מרכוז הכפתורים
    float startY = 640;                                   // מיקום הכפתורים בפאנל הפעולות
    float spacing = buttonWidth + 10;                     // מרווח בין הכפתורים

    // צבעים לכפתורים
    sf::Color blueButtonColor(70, 130, 180);  // כחול לפעולות כלכליות
    sf::Color orangeButtonColor(180, 95, 60); // כתום לפעולות תקיפה

    // פעולות כלכליות

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

    // פעולות תקיפה

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

void CoupGUI::initializeResetButton()
{
    // כפתור ריסט
    float buttonWidth = 100;
    float buttonHeight = 30;
    float buttonX = 1150;  // בפינה הימנית העליונה
    float buttonY = 20;

    resetButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    resetButton.setPosition(buttonX, buttonY);
    resetButton.setFillColor(sf::Color(200, 60, 60)); // אדום
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
    // רקע חצי שקוף
    winOverlay.setSize(sf::Vector2f(1280, 720));
    winOverlay.setPosition(0, 0);
    winOverlay.setFillColor(sf::Color(0, 0, 0, 180)); // שחור חצי שקוף

    // כותרת ניצחון
    winTitle.setFont(mainFont);
    winTitle.setCharacterSize(48);
    winTitle.setFillColor(sf::Color::Yellow);
    winTitle.setStyle(sf::Text::Bold);

    // כותרת משנה
    winSubtitle.setFont(mainFont);
    winSubtitle.setCharacterSize(24);
    winSubtitle.setFillColor(sf::Color::White);

    // כפתור משחק חדש
    float buttonWidth = 150;
    float buttonHeight = 50;
    float centerX = 640; // מרכז המסך
    float centerY = 400;

    newGameButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    newGameButton.setPosition(centerX - buttonWidth - 20, centerY);
    newGameButton.setFillColor(sf::Color(70, 130, 180)); // כחול
    newGameButton.setOutlineThickness(3);
    newGameButton.setOutlineColor(sf::Color::White);

    newGameButtonText.setFont(mainFont);
    newGameButtonText.setString("New Game");
    newGameButtonText.setCharacterSize(20);
    newGameButtonText.setFillColor(sf::Color::White);

    // כפתור יציאה
    exitButton.setSize(sf::Vector2f(buttonWidth, buttonHeight));
    exitButton.setPosition(centerX + 20, centerY);
    exitButton.setFillColor(sf::Color(180, 60, 60)); // אדום
    exitButton.setOutlineThickness(3);
    exitButton.setOutlineColor(sf::Color::White);

    exitButtonText.setFont(mainFont);
    exitButtonText.setString("Exit");
    exitButtonText.setCharacterSize(20);
    exitButtonText.setFillColor(sf::Color::White);
}

void CoupGUI::initializeSetupScreen()
{
    // אתחול כפתורי בחירת מספר שחקנים (2-6)
    float buttonWidth = 80;
    float buttonHeight = 60;
    float startX = 350;
    float startY = 250;
    float spacingX = 120;
    
    for (int i = 0; i < 5; i++) {
        int playerCount = i + 2; // 2-6 שחקנים
        
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
    
    // כפתור התחלת משחק
    startGameButton.setSize(sf::Vector2f(200, 60));
    startGameButton.setPosition(540, 600);
    startGameButton.setFillColor(sf::Color(60, 180, 60)); // ירוק
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

void CoupGUI::run()
{
    std::cout << "Starting Coup game" << std::endl;
    sf::Clock frameClock;

    while (window.isOpen())
    {
        try {
            frameClock.restart();

            // טיפול באירועים
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
                        // שינוי מצב דיבאג
                        toggleDebugMode();
                    }
                    else if (event.key.code == sf::Keyboard::Escape)
                    {
                        if (currentState == GuiState::SETUP && activeInputPlayer >= 0)
                        {
                            // ביטול הזנת שם
                            activeInputPlayer = -1;
                            currentInputText = "";
                            addNotification("הזנת השם בוטלה");
                        }
                        else if (waitingForTarget)
                        {
                            // ביטול בחירת מטרה לפעולות בסיסיות
                            waitingForTarget = false;
                            pendingActionName = "";
                            clearPlayerButtons();
                            addNotification("בחירת מטרה בוטלה");
                        }
                        else if (specialActionState != SpecialActionState::NONE)
                        {
                            // ביטול פעולה מיוחדת
                            cancelSpecialActionSelection();
                            addNotification("Special action cancelled");
                        }
                    }
                }

                // טיפול בהזנת טקסט
                if (event.type == sf::Event::TextEntered && currentState == GuiState::SETUP)
                {
                    handleTextInput(event.text.unicode);
                }

                // טיפול באירועי עכבר
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // === טיפול במסך הגדרות ===
                    if (currentState == GuiState::SETUP)
                    {
                        handleSetupScreenClick(mousePos);
                        continue; // לא מטפלים בכפתורים אחרים במסך הגדרות
                    }

                    // === טיפול במסך ניצחון ===
                    if (currentState == GuiState::WIN_SCREEN)
                    {
                        handleWinScreenClick(mousePos);
                        continue; // לא מטפלים בכפתורים אחרים במסך ניצחון
                    }

                    // === טיפול בכפתור ריסט ===
                    sf::FloatRect resetBounds = resetButton.getGlobalBounds();
                    if (resetBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                    {
                        handleResetClick();
                        continue;
                    }

                    // === טיפול בפעולות מיוחדות דו-שלביות ===
                    if (specialActionState == SpecialActionState::SELECTING_PERFORMER)
                    {
                        // בחירת מבצע הפעולה
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // מציאת השחקן שנבחר
                                for (const auto &player : realPlayers)
                                {
                                    if (player->name() == playerButtons[i].playerName)
                                    {
                                        selectedPerformer = player;
                                        
                                        // בדיקה אם הפעולה דורשת מטרה
                                        if (pendingSpecialAction == "see and block arrest")
                                        {
                                            // עבור לשלב בחירת מטרה
                                            specialActionState = SpecialActionState::SELECTING_TARGET;
                                            createTargetButtons(selectedPerformer);
                                            addNotification("Select target for " + pendingSpecialAction);
                                        }
                                        else
                                        {
                                            // הפעולה לא דורשת מטרה - מבצע ישירות
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
                        // בחירת מטרה לפעולה
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // מציאת המטרה שנבחרה
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
                    // === טיפול בפעולות בסיסיות (קיים) ===
                    else if (waitingForTarget && !pendingActionName.empty())
                    {
                        // בדיקה אם לחצו על אחד מכפתורי השחקנים
                        for (size_t i = 0; i < playerButtons.size(); i++)
                        {
                            sf::FloatRect buttonBounds = playerButtons[i].button.getGlobalBounds();
                            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                            {
                                // מצאנו את השחקן שנבחר
                                std::string selectedPlayerName = playerButtons[i].playerName;
                                
                                // מציאת השחקן ברשימת השחקנים האמיתיים
                                for (const auto &player : realPlayers)
                                {
                                    if (player->name() == selectedPlayerName && player->isActive())
                                    {
                                        try {
                                            // ביצוע הפעולה המתאימה
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
                                            
                                            // איפוס מצב בחירת המטרה
                                            waitingForTarget = false;
                                            pendingActionName = "";
                                            clearPlayerButtons();
                                            
                                            break; // יציאה מהלולאה הפנימית בלבד
                                        }
                                        catch (const std::exception &e) {
                                            addNotification("Error: " + std::string(e.what()));
                                            
                                            // נקה גם במקרה של שגיאה
                                            waitingForTarget = false;
                                            pendingActionName = "";
                                            clearPlayerButtons();
                                            break;
                                        }
                                        catch (...) {
                                            addNotification("Unknown error occurred!");
                                            
                                            // נקה גם במקרה של שגיאה
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

                    // בדיקה אם לחצו על אחד מכפתורי הפעולות המיוחדות
                    for (size_t i = 0; i < specialActions.size(); i++)
                    {
                        sf::FloatRect buttonBounds = specialActions[i].button.getGlobalBounds();
                        if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
                        {
                            handleSpecialActionClick(i);
                            break;
                        }
                    }

                    // בדיקה אם לחצו על אחד מכפתורי הפעולות הבסיסיות
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

            
            // עדכון המצב
            try {
                update();
                
            } catch (const std::exception &e) {
                cout << "ERROR in update(): " << e.what() << endl;
                addNotification("שגיאה ב-update: " + std::string(e.what()));
            }

            // ציור
            try {
                render();
                
            } catch (const std::exception &e) {
                cout << "ERROR in render(): " << e.what() << endl;
                addNotification("שגיאה ב-render: " + std::string(e.what()));
            }

            // שינה כדי לשמור על קצב פריימים עקבי
            sf::sleep(sf::milliseconds(33)); // ~30 FPS
        }
        catch (const std::exception &e) {
            cout << "CRITICAL ERROR in main loop: " << e.what() << endl;
            addNotification("שגיאה קריטית: " + std::string(e.what()));
            // לא סוגרים את החלון, ממשיכים
        }
        catch (...) {
            cout << "UNKNOWN CRITICAL ERROR in main loop!" << endl;
            addNotification("שגיאה לא ידועה!");
            // לא סוגרים את החלון, ממשיכים
        }
    }
}

void CoupGUI::update()
{
    // בדיקה אם יש מנצח (רק במצב משחק רגיל)
    if (currentState == GuiState::PLAYING)
    {
        checkForWinner();
    }

    // עדכון שם השחקן הנוכחי אם השתנה במהלך המשחק
    if (game && !realPlayers.empty() && currentState == GuiState::PLAYING) {
        try {
            shared_ptr<coup::Player> currentPlayer = game->getPlayer();
            if (currentPlayer && currentPlayer->name() != currentPlayerName) {
                currentPlayerName = currentPlayer->name();
                // כפיית עדכון חזותי כאשר השחקן משתנה
                addNotification("התור עבר ל: " + currentPlayerName);
            }
            
            // בדיקה אם יש שינויים במצב המשחק והפעלת עדכון חזותי במידת הצורך
            static int lastCoinsCount = -1;
            int currentCoinsCount = currentPlayer->coins();
            if (lastCoinsCount != currentCoinsCount) {
                lastCoinsCount = currentCoinsCount;
            }
            
        } catch (const std::exception &e) {
            // התעלם משגיאות - למשל אם המשחק נגמר
        }
    }
    
    // רענון חזותי יזום בכל X פריימים
    static int frameCounter = 0;
    frameCounter++;
    if (frameCounter >= 30) {  // רענון כל שניה בערך (ב-30 פריימים לשניה)
        frameCounter = 0;
        
    }
}

void CoupGUI::toggleDebugMode()
{
    debugMode = !debugMode;
    addNotification(debugMode ? "Debug mode enabled" : "Debug mode disabled");
}

void CoupGUI::render()
{
    window.clear();

    // ציור מסך לפי המצב הנוכחי
    if (currentState == GuiState::SETUP)
    {
        // ציור רקע פשוט
        window.draw(background);
        
        // ציור מסך הגדרות
        drawSetupScreen();
    }
    else if (currentState == GuiState::WIN_SCREEN)
    {
        // ציור רקע פשוט
        window.draw(background);
        
        // ציור מסך ניצחון
        drawWinScreen();
    }
    else // GuiState::PLAYING
    {
        // ציור רקע
        window.draw(background);

        // ציור כותרת
        sf::Text titleText;
        titleText.setFont(titleFont);
        titleText.setString("Coup - Game");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(520, 30);
        window.draw(titleText);

        // ציור כפתור ריסט
        drawResetButton();

        // ציור לוח המשחק
        window.draw(gameBoard);
        drawPlayers();

        // ציור לוח יומן
        window.draw(logPanel);
        drawGameLog();

        // ציור פאנל פעולות מיוחדות
        window.draw(specialActionsPanel);
        drawSpecialActions();

        // ציור לוח פעולות
        window.draw(actionPanel);
        drawActionPanel();

        // ציור התראות
        drawNotifications();

        // ציור כפתורי בחירת שחקנים אם צריך
        if ((waitingForTarget && !playerButtons.empty()) || 
            (specialActionState != SpecialActionState::NONE && !playerButtons.empty()))
        {
            drawPlayerButtons();
        }

        // ציור מידע דיבאג אם מופעל
        if (debugMode)
        {
            drawDebugInfo();
        }
    }

    window.display();
}

void CoupGUI::drawPlayers()
{
    try {
        if (game == nullptr || realPlayers.empty()) {
            return;  // אין משחק או שחקנים להציג
        }

        size_t numPlayers = realPlayers.size();

        float cardWidth = 140;
        float cardHeight = 200;
        float startX = 60;
        float startY = 150;
        float spacingX = 180;
        int playersPerRow = 4;

        for (size_t i = 0; i < numPlayers; i++)
        {
            // בדיקה שהשחקן תקין
            if (!realPlayers[i]) {
                cout << "Warning: null player at index " << i << endl;
                continue;
            }
            
            // שליפת מידע משחקן אמיתי
            std::string playerName = realPlayers[i]->name();
            coup::Role playerRole = realPlayers[i]->role();
            int coins = realPlayers[i]->coins();
            bool isCurrentPlayer = (playerName == currentPlayerName);

            int row = static_cast<int>(i) / playersPerRow;
            int col = static_cast<int>(i) % playersPerRow;
            float x = startX + col * spacingX;
            float y = startY + row * (cardHeight + 30);

            // ציור כרטיס שחקן
            sf::RectangleShape playerCard;
            playerCard.setSize(sf::Vector2f(cardWidth, cardHeight));
            playerCard.setPosition(x, y);
            playerCard.setFillColor(getRoleColor(playerRole));

            // הדגשת שחקן נוכחי
            if (isCurrentPlayer)
            {
                playerCard.setOutlineThickness(4);
                playerCard.setOutlineColor(sf::Color::Yellow);
            }
            else if (waitingForTarget && playerName != currentPlayerName && realPlayers[i]->isActive())
            {
                // הדגשת מטרות אפשריות
                playerCard.setOutlineThickness(3);
                playerCard.setOutlineColor(sf::Color::Green);
            }
            else
            {
                playerCard.setOutlineThickness(1);
                playerCard.setOutlineColor(sf::Color(150, 150, 150));
            }

            window.draw(playerCard);

            // ציור שם שחקן
            sf::Text nameText;
            nameText.setFont(mainFont);
            nameText.setString(playerName);
            nameText.setCharacterSize(18);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 10, y + 10);
            window.draw(nameText);

            // ציור תפקיד שחקן
            sf::Text roleText;
            roleText.setFont(mainFont);
            roleText.setString(getRoleName(playerRole));
            roleText.setCharacterSize(16);
            roleText.setFillColor(sf::Color::White);
            roleText.setPosition(x + 10, y + 40);
            window.draw(roleText);

            // ציור מטבעות
            sf::Text coinsText;
            coinsText.setFont(mainFont);
            coinsText.setString("Coins: " + std::to_string(coins));
            coinsText.setCharacterSize(16);
            coinsText.setFillColor(sf::Color::Yellow);
            coinsText.setPosition(x + 10, y + 70);
            window.draw(coinsText);
        }
    } catch (const std::exception &e) {
        cout << "ERROR in drawPlayers(): " << e.what() << endl;
        // נציג הודעת שגיאה במסך במקום לקרוס
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

void CoupGUI::drawActionPanel()
{
    sf::Text actionTitle;
    actionTitle.setFont(mainFont);

    actionTitle.setCharacterSize(18);
    actionTitle.setFillColor(sf::Color::White);
    actionTitle.setPosition(50, 625);
    window.draw(actionTitle);

    // ציור כפתורי הפעולות הבסיסיות
    for (size_t i = 0; i < basicActions.size(); i++)
    {
        // ציור הכפתור
        window.draw(basicActions[i].button);

        // ציור הטקסט
        window.draw(basicActions[i].buttonText);
    }

    // כותרת לפעולות
    sf::Text basicTitle;
    basicTitle.setFont(mainFont);
    basicTitle.setCharacterSize(18);
    basicTitle.setFillColor(sf::Color::White);

    // מיקום הכותרת במרכז
    float titleX = actionPanel.getPosition().x + (actionPanel.getSize().x / 2);
    float titleY = 610;
    basicTitle.setOrigin(basicTitle.getLocalBounds().width / 2, 0);
    basicTitle.setPosition(titleX, titleY);

    window.draw(basicTitle);
}

void CoupGUI::drawNotifications()
{
    if (notificationQueue.empty())
        return;

    // הצגת התראה ראשונה בתור
    sf::Text notifText;
    notifText.setFont(mainFont);
    notifText.setString(notificationQueue.front());
    notifText.setCharacterSize(20);
    notifText.setFillColor(sf::Color::Yellow);

    // מיקום במרכז מסך עליון
    sf::FloatRect textBounds = notifText.getLocalBounds();
    notifText.setPosition((window.getSize().x - textBounds.width) / 2, 60);

    window.draw(notifText);

    // הסרת התראות ישנות
    static sf::Clock notifClock;
    if (notifClock.getElapsedTime().asSeconds() > 3.0f)
    {
        notificationQueue.pop();
        notifClock.restart();
    }
}

void CoupGUI::drawDebugInfo()
{
    std::stringstream debugText;
    debugText << "FPS: " << std::round(1.0f / globalClock.restart().asSeconds())
              << " | Mode: GAME"
              << " | Elements: Board(" << gameBoard.getSize().x << "x" << gameBoard.getSize().y << ")"
              << " | Players: " << realPlayers.size();

    sf::Text debugInfoText;
    debugInfoText.setFont(mainFont);
    debugInfoText.setString(debugText.str());
    debugInfoText.setCharacterSize(14);
    debugInfoText.setFillColor(sf::Color(100, 255, 100));
    debugInfoText.setPosition(10, 5);
    window.draw(debugInfoText);
}

void CoupGUI::addLogMessage(const std::string &message)
{
    logMessages.push_back(message);

    // שמירה על היסטוריה מוגבלת
    if (logMessages.size() > 100)
    {
        logMessages.erase(logMessages.begin());
    }
}

void CoupGUI::addNotification(const std::string &message)
{
    notificationQueue.push(message);
}

sf::Color CoupGUI::getRoleColor(coup::Role role)
{
    if (roleColors.find(role) != roleColors.end())
    {
        return roleColors[role];
    }

    // צבע ברירת מחדל
    return sf::Color(100, 100, 100);
}

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

void CoupGUI::handleSpecialActionClick(int actionIndex)
{
    if (actionIndex < 0 || actionIndex >= static_cast<int>(specialActions.size()))
        return;

    const SpecialAction &action = specialActions[actionIndex];
    
    // התחל מנגנון בחירה דו-שלבי
    startSpecialActionSelection(action.name, action.requiredRole);
}

void CoupGUI::drawSpecialActions()
{
    // כותרת לפאנל פעולות מיוחדות
    sf::Text specialTitle;
    specialTitle.setFont(mainFont);
    specialTitle.setString("Out Of Turn Actions");
    specialTitle.setCharacterSize(18);
    specialTitle.setFillColor(sf::Color::White);
    specialTitle.setPosition(1030, 410);
    window.draw(specialTitle);

    // ציור שמות השחקנים ותפקידיהם לפני הכפתורים
    float currentY = 450; // מיקום התחלתי Y
    float spacing = 35;  // מרווח בין כפתורים

    for (size_t i = 0; i < specialActions.size(); i++)
    {
        // בדיקה אם לא להציג את הכפתור ה-Invest של ה-Baron אם השחקן הנוכחי אינו Baron
        bool showButton = true;
        if (specialActions[i].requiredRole == coup::Role::BARON) {
            // חיפוש השחקן הנוכחי ברשימת השחקנים האמיתיים
            bool isCurrentPlayerBaron = false;
            for (const auto &player : realPlayers) {
                if (player->name() == currentPlayerName && player->role() == coup::Role::BARON) {
                    isCurrentPlayerBaron = true;
                    break;
                }
            }
            // אם השחקן הנוכחי אינו Baron, לא נציג את הכפתור
            if (!isCurrentPlayerBaron) {
                showButton = false;
            }
        }
        
        if (!showButton) {
            continue;  // דילוג על הצגת כפתור
        }

        // מצא דוגמת שחקן עם התפקיד המתאים
        std::string playerExample = "";
        for (const auto &player : realPlayers)
        {
            if (player->role() == specialActions[i].requiredRole)
            {
                playerExample = player->name();
                break;
            }
        }

        // קיצור לשם הראשון
        std::string shortName = "";
        if (!playerExample.empty()) {
            shortName = playerExample.substr(0, 1);
        }

        // עדכון מיקום הכפתור הנוכחי
        specialActions[i].button.setPosition(1080, currentY);
        specialActions[i].buttonText.setPosition(
            1080 + (specialActions[i].button.getSize().x - specialActions[i].buttonText.getLocalBounds().width) / 2,
            currentY + 5);

        // ציור שם השחקן ותפקידו
        sf::Text playerText;
        playerText.setFont(mainFont);
        playerText.setString(shortName + " (" + getRoleName(specialActions[i].requiredRole) + ")");
        playerText.setCharacterSize(16);
        playerText.setFillColor(sf::Color::White);
        playerText.setPosition(980, currentY + 5);
        window.draw(playerText);

        // ציור הכפתור והטקסט שלו
        window.draw(specialActions[i].button);
        window.draw(specialActions[i].buttonText);

        // קידום המיקום האנכי לכפתור הבא
        currentY += spacing;
    }
}

void CoupGUI::handleBasicActionClick(int actionIndex)
{
    if (actionIndex < 0 || actionIndex >= static_cast<int>(basicActions.size()))
        return;

    const BasicAction &action = basicActions[actionIndex];
    std::string actionName = action.name;
    
    try
    {
        // קבלת השחקן הנוכחי
        shared_ptr<coup::Player> currentPlayer = game->getPlayer();

        // ביצוע הפעולה המתאימה
        if (actionName == "Gather")
        {
            currentPlayer->gather();
            addLogMessage(currentPlayerName + " gathered 1 coin");
            
            // עדכון שם השחקן הנוכחי אחרי הפעולה
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }

        else if (actionName == "Tax")
        {
            currentPlayer->tax();
            addLogMessage(currentPlayerName + " taxed and received 2 coins");
            
            // עדכון שם השחקן הנוכחי אחרי הפעולה
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }
        else if (actionName == "Bribe")
        {
            currentPlayer->bribe();
            addLogMessage(currentPlayerName + " bribed another player");
            
            // עדכון שם השחקן הנוכחי אחרי הפעולה
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        }
        else if (actionName == "Arrest")
        {
            // יצירת כפתורי בחירת שחקנים
            pendingActionName = "Arrest";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform an arrest");
        }
        else if (actionName == "Sanction")
        {
            // יצירת כפתורי בחירת שחקנים
            pendingActionName = "Sanction";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform a sanction");
        }
        else if (actionName == "Coup")
        {
            // יצירת כפתורי בחירת שחקנים
            pendingActionName = "Coup";
            waitingForTarget = true;
            createPlayerButtons();
            addNotification("click on a player to perform a coup");
        }
        
        addNotification(action.name + " action succeeded. Next player: " + currentPlayerName);
    }
    catch (const std::exception &e)
    {
        addNotification("Error: " + std::string(e.what()));
        addLogMessage("Failed action: " + std::string(e.what()));
    }
}

// פונקציות עזר לכפתורי שחקנים
void CoupGUI::createPlayerButtons()
{
    playerButtons.clear();
    
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;
    float startY = 300;
    float spacing = 45;
    
    int buttonIndex = 0;
    
    // יצירת כפתור לכל שחקן פעיל שאינו השחקן הנוכחי
    for (const auto &player : realPlayers)
    {
        if (player->name() != currentPlayerName && player->isActive())
        {
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            // יצירת הכפתור
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(sf::Color(70, 130, 180)); // כחול
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::White);
            
            // יצירת הטקסט
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name());
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

void CoupGUI::clearPlayerButtons()
{
    playerButtons.clear();
}

void CoupGUI::drawPlayerButtons()
{
    // רקע עבור כפתורי השחקנים
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(300, 400));
    background.setPosition(350, 250);
    background.setFillColor(sf::Color(30, 30, 30, 200)); // שקוף חלקית
    background.setOutlineThickness(3);
    background.setOutlineColor(sf::Color::Yellow);
    window.draw(background);
    
    // כותרת דינמית לפי המצב
    sf::Text title;
    title.setFont(mainFont);
    title.setCharacterSize(20);
    title.setFillColor(sf::Color::Yellow);
    title.setPosition(420, 260);
    
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
    
    // ציור כל הכפתורים
    for (const auto &playerButton : playerButtons)
    {
        window.draw(playerButton.button);
        window.draw(playerButton.buttonText);
    }
    
    // הוראה לביטול
    sf::Text cancelText;
    cancelText.setFont(mainFont);
    cancelText.setString("ESC to cancel");
    cancelText.setCharacterSize(14);
    cancelText.setFillColor(sf::Color(200, 200, 200));
    cancelText.setPosition(450, 620);
    window.draw(cancelText);
}

// פונקציות לביצוע פעולות עם בחירת מטרה
void CoupGUI::executeArrest(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        auto currentPlayer = game->getPlayer();
        
        currentPlayer->arrest(targetPlayer);
        addLogMessage(currentPlayerName + " arrested " + targetPlayer->name());
        
        // עדכון שם השחקן הנוכחי אחרי הפעולה
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // אם יש שגיאה בעדכון, נמשיך עם השם הקיים
        }
        
        addNotification("Arrest completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in arrest: " + std::string(e.what()));
    }
}

void CoupGUI::executeSanction(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        auto currentPlayer = game->getPlayer();
        
        currentPlayer->sanction(*targetPlayer);
        addLogMessage(currentPlayerName + " sanctioned " + targetPlayer->name());
        
        // עדכון שם השחקן הנוכחי אחרי הפעולה
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // אם יש שגיאה בעדכון, נמשיך עם השם הקיים
        }
        
        addNotification("Sanction completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in sanction: " + std::string(e.what()));
    }
}

void CoupGUI::executeCoup(std::shared_ptr<coup::Player> targetPlayer)
{
    try {
        auto currentPlayer = game->getPlayer();
        
        currentPlayer->coup(targetPlayer);
        addLogMessage(currentPlayerName + " performed coup on " + targetPlayer->name());
        
        // עדכון שם השחקן הנוכחי אחרי הפעולה
        try {
            currentPlayer = game->getPlayer();
            currentPlayerName = currentPlayer->name();
        } catch (const std::exception &e) {
            // אם יש שגיאה בעדכון, נמשיך עם השם הקיים
        }
        
        addNotification("Coup completed successfully! Next turn: " + currentPlayerName);
    }
    catch (const std::exception &e) {
        addNotification("Error in coup: " + std::string(e.what()));
    }
}

// ========== פונקציות חדשות למנגנון דו-שלבי ==========

void CoupGUI::startSpecialActionSelection(const std::string& actionName, coup::Role requiredRole)
{
    // ספירת כמה שחקנים יש מהתפקיד הנדרש
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
    
    // הגדרת המשתנים לפני בדיקת כמות השחקנים
    pendingSpecialAction = actionName;
    pendingSpecialActionRole = requiredRole;
    
    // בדיקה אם הפעולה דורשת מטרה
    bool requiresTarget = (actionName == "see and block arrest");
    
    if (availablePerformers.size() == 1)
    {
        // רק שחקן אחד מהתפקיד הזה
        selectedPerformer = availablePerformers[0];
        
        if (requiresTarget)
        {
            // הפעולה דורשת מטרה - עבור לשלב בחירת מטרה
            specialActionState = SpecialActionState::SELECTING_TARGET;
            createTargetButtons(selectedPerformer);
            addNotification("Select target for " + actionName);
        }
        else
        {
            // הפעולה לא דורשת מטרה - מבצע ישירות
            executeSpecialAction(selectedPerformer);
        }
        return;
    }
    
    // יש יותר משחקן אחד - מתחיל מנגנון בחירה
    specialActionState = SpecialActionState::SELECTING_PERFORMER;
    
    createPerformerButtons(requiredRole);
    addNotification("Select which " + getRoleName(requiredRole) + " performs " + actionName);
}

void CoupGUI::createPerformerButtons(coup::Role requiredRole)
{
    playerButtons.clear();
    
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;
    float startY = 250;
    float spacing = 45;
    
    int buttonIndex = 0;
    
    for (const auto &player : realPlayers)
    {
        if (player->role() == requiredRole && player->isActive())
        {
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(getRoleColor(requiredRole));
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::White);
            
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name() + " (" + getRoleName(requiredRole) + ")");
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

void CoupGUI::createTargetButtons(std::shared_ptr<coup::Player> performer)
{
    playerButtons.clear();
    
    float buttonWidth = 120;
    float buttonHeight = 35;
    float startX = 400;
    float startY = 250;
    float spacing = 45;
    
    int buttonIndex = 0;
    
    for (const auto &player : realPlayers)
    {
        // רק שחקנים פעילים שאינם המבצע עצמו
        if (player->isActive() && player->name() != performer->name())
        {
            PlayerButton playerButton;
            playerButton.playerName = player->name();
            
            playerButton.button.setSize(sf::Vector2f(buttonWidth, buttonHeight));
            playerButton.button.setPosition(startX, startY + buttonIndex * spacing);
            playerButton.button.setFillColor(sf::Color(70, 130, 180)); // כחול
            playerButton.button.setOutlineThickness(2);
            playerButton.button.setOutlineColor(sf::Color::Green);
            
            playerButton.buttonText.setFont(mainFont);
            playerButton.buttonText.setString(player->name());
            playerButton.buttonText.setCharacterSize(16);
            playerButton.buttonText.setFillColor(sf::Color::White);
            playerButton.buttonText.setPosition(
                startX + (buttonWidth - playerButton.buttonText.getLocalBounds().width) / 2,
                startY + buttonIndex * spacing + (buttonHeight - playerButton.buttonText.getLocalBounds().height) / 2 - 5
            );
            
            playerButtons.push_back(playerButton);
            buttonIndex++;
        }
    }
}

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
    
    // איפוס המצב
    cancelSpecialActionSelection();
}

void CoupGUI::cancelSpecialActionSelection()
{
    specialActionState = SpecialActionState::NONE;
    pendingSpecialAction = "";
    selectedPerformer = nullptr;
    clearPlayerButtons();
}

void CoupGUI::drawResetButton()
{
    window.draw(resetButton);
    window.draw(resetButtonText);
}

void CoupGUI::drawWinScreen()
{
    // רקע חצי שקוף
    window.draw(winOverlay);

    // Update victory text
    winTitle.setString(winnerName + " won!");
    sf::FloatRect titleBounds = winTitle.getLocalBounds();
    winTitle.setPosition((1280 - titleBounds.width) / 2, 250);

    winSubtitle.setString("Game Over!");
    sf::FloatRect subtitleBounds = winSubtitle.getLocalBounds();
    winSubtitle.setPosition((1280 - subtitleBounds.width) / 2, 320);

    // ציור הטקסטים
    window.draw(winTitle);
    window.draw(winSubtitle);

    // עדכון מיקום טקסט הכפתורים
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

    // ציור הכפתורים
    window.draw(newGameButton);
    window.draw(newGameButtonText);
    window.draw(exitButton);
    window.draw(exitButtonText);
}

void CoupGUI::resetGame()
{
    try {
        if (!game || realPlayers.empty()) {
            addNotification("Cannot reset - no game initialized!");
            return;
        }

        // שמירת נתוני השחקנים הקיימים
        std::vector<std::string> playerNames;
        std::vector<coup::Role> playerRoles;
        
        for (const auto& player : realPlayers) {
            if (player) {
                playerNames.push_back(player->name());
                playerRoles.push_back(player->role());
            }
        }
        
        // יצירת משחק חדש
        game = std::make_shared<coup::Game>();
        
        // ניקוי רשימת השחקנים הישנה
        realPlayers.clear();
        
        // יצירת שחקנים חדשים עם אותם שמות ותפקידים
        for (size_t i = 0; i < playerNames.size(); i++) {
            auto newPlayer = game->createPlayer(playerNames[i], playerRoles[i]);
            realPlayers.push_back(newPlayer);
        }

        // חזרה למצב משחק רגיל
        currentState = GuiState::PLAYING;

        // איפוס השחקן הנוכחי לראשון ברשימה
        if (!realPlayers.empty() && realPlayers[0]) {
            currentPlayerName = realPlayers[0]->name();
        }

        // ניקוי מצבים זמניים
        waitingForTarget = false;
        pendingActionName = "";
        specialActionState = SpecialActionState::NONE;
        clearPlayerButtons();

        // ניקוי יומן והתראות
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

void CoupGUI::checkForWinner()
{
    if (!game || realPlayers.empty()) {
        return;
    }

    // ספירת שחקנים פעילים
    int activePlayers = 0;
    std::shared_ptr<coup::Player> lastActivePlayer = nullptr;

    for (const auto& player : realPlayers) {
        if (player && player->isActive()) {
            activePlayers++;
            lastActivePlayer = player;
        }
    }

    // אם נשאר רק שחקן אחד פעיל - יש מנצח!
    if (activePlayers == 1 && lastActivePlayer) {
        winnerName = lastActivePlayer->name();
        currentState = GuiState::WIN_SCREEN;
        addLogMessage(winnerName + " won the game!");
        addNotification("Game Over! " + winnerName + " is the winner!");
    }
}

void CoupGUI::startNewGame()
{
    // חזרה למסך הגדרות
    currentState = GuiState::SETUP;
    currentSetupStep = 0;
    numPlayersToSetup = 0;
    activeInputPlayer = -1;
    currentInputText = "";
    
    // ניקוי נתוני משחק
    game = nullptr;
    realPlayers.clear();
    setupPlayers.clear();
    
    // ניקוי מצבים זמניים
    waitingForTarget = false;
    pendingActionName = "";
    specialActionState = SpecialActionState::NONE;
    clearPlayerButtons();
    
    // ניקוי יומן והתראות
    logMessages.clear();
    while (!notificationQueue.empty()) {
        notificationQueue.pop();
    }
    
    addNotification("Back to setup screen");
}

void CoupGUI::handleResetClick()
{
    addNotification("Resetting game...");
    resetGame();
}

void CoupGUI::handleWinScreenClick(sf::Vector2i mousePos)
{
    sf::FloatRect newGameBounds = newGameButton.getGlobalBounds();
    sf::FloatRect exitBounds = exitButton.getGlobalBounds();

    if (newGameBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        // כפתור משחק חדש
        startNewGame();
    }
    else if (exitBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))
    {
        // כפתור יציאה
        window.close();
    }
}

void CoupGUI::drawSetupScreen()
{
    // כותרת ראשית
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
        // שלב בחירת מספר שחקנים
        sf::Text instructionText;
        instructionText.setFont(mainFont);
        instructionText.setString("Choose number of players for the game:");
        instructionText.setCharacterSize(24);
        instructionText.setFillColor(sf::Color::White);
        sf::FloatRect instrBounds = instructionText.getLocalBounds();
        instructionText.setPosition((1280 - instrBounds.width) / 2, 200);
        window.draw(instructionText);
        
        // ציור כפתורי מספר שחקנים
        for (int i = 0; i < 5; i++) {
            window.draw(playerCountButtons[i]);
            window.draw(playerCountTexts[i]);
        }
    }
    else {
        // שלב הגדרת שחקנים
        sf::Text instructionText;
        instructionText.setFont(mainFont);
        instructionText.setString("Setup players:");
        instructionText.setCharacterSize(24);
        instructionText.setFillColor(sf::Color::White);
        instructionText.setPosition(50, 170);
        window.draw(instructionText);
        
        // ציור הגדרות שחקנים
        float yPos = 220;
        for (int i = 0; i < numPlayersToSetup; i++) {
            // מספר שחקן
            sf::Text playerNumText;
            playerNumText.setFont(mainFont);
            playerNumText.setString("Player " + std::to_string(i + 1) + ":");
            playerNumText.setCharacterSize(20);
            playerNumText.setFillColor(sf::Color::White);
            playerNumText.setPosition(50, yPos);
            window.draw(playerNumText);
            
            // שדה הזנת שם
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
                
                // כפתורי תפקידים
                std::vector<std::string> roleNames = {"General", "Governor", "Spy", "Baron", "Judge", "Merchant"};
                std::vector<coup::Role> roles = {coup::Role::GENERAL, coup::Role::GOVERNOR, coup::Role::SPY, 
                                               coup::Role::BARON, coup::Role::JUDGE, coup::Role::MERCHANT};
                
                for (int j = 0; j < 6; j++) {
                    setupPlayers[i].roleButtons[j].setPosition(370 + j * 85, yPos - 5);
                    setupPlayers[i].roleButtons[j].setSize(sf::Vector2f(80, 30));
                    
                    // צבע לפי אם נבחר
                    if (setupPlayers[i].roleSelected && setupPlayers[i].role == roles[j]) {
                        setupPlayers[i].roleButtons[j].setFillColor(sf::Color(60, 180, 60)); // ירוק
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
        
        // בדיקה אם כל השחקנים מוכנים
        bool allReady = true;
        for (int i = 0; i < numPlayersToSetup; i++) {
            if (i >= static_cast<int>(setupPlayers.size()) || 
                !setupPlayers[i].nameCompleted || !setupPlayers[i].roleSelected) {
                allReady = false;
                break;
            }
        }
        
        // כפתור התחלת משחק
        if (allReady) {
            window.draw(startGameButton);
            window.draw(startGameButtonText);
        }
        
        // הוראות
        sf::Text helpText;
        helpText.setFont(mainFont);
        helpText.setString("Click on the name field to enter a name, and select a role from the buttons");
        helpText.setCharacterSize(16);
        helpText.setFillColor(sf::Color(200, 200, 200));
        helpText.setPosition(50, 680);
        window.draw(helpText);
    }
}

void CoupGUI::handleSetupScreenClick(sf::Vector2i mousePos)
{
    if (currentSetupStep == 0) {
        // שלב בחירת מספר שחקנים
        for (int i = 0; i < 5; i++) {
            sf::FloatRect buttonBounds = playerCountButtons[i].getGlobalBounds();
            if (buttonBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                numPlayersToSetup = i + 2; // 2-6 שחקנים
                currentSetupStep = 1;
                
                // אתחול מבנה שחקנים
                setupPlayers.clear();
                setupPlayers.resize(numPlayersToSetup);
                
                for (int j = 0; j < numPlayersToSetup; j++) {
                    setupPlayers[j].name = "";
                    setupPlayers[j].role = coup::Role::GENERAL;
                    setupPlayers[j].nameCompleted = false;
                    setupPlayers[j].roleSelected = false;
                }
                
                activeInputPlayer = -1;
                addNotification("נבחרו " + std::to_string(numPlayersToSetup) + " שחקנים");
                break;
            }
        }
    }
    else {
        // שלב הגדרת שחקנים
        float yPos = 220;
        
        for (int i = 0; i < numPlayersToSetup && i < static_cast<int>(setupPlayers.size()); i++) {
            // בדיקת לחיצה על שדה שם
            sf::FloatRect nameInputBounds(200, yPos - 5, 150, 30);
            if (nameInputBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                activeInputPlayer = i;
                currentInputText = setupPlayers[i].name;
                break;
            }
            
            // בדיקת לחיצה על כפתורי תפקידים
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
        
        // בדיקת לחיצה על כפתור התחלת משחק
        sf::FloatRect startBounds = startGameButton.getGlobalBounds();
        if (startBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            // בדיקה שכל השחקנים מוכנים
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
    else if (unicode == 13) { // Enter
        if (!currentInputText.empty()) {
            setupPlayers[activeInputPlayer].name = currentInputText;
            setupPlayers[activeInputPlayer].nameCompleted = true;
            activeInputPlayer = -1;
            currentInputText = "";
            addNotification("Player name saved!");
        }
    }
    else if (unicode >= 32 && unicode < 128) { // תווים רגילים
        if (currentInputText.length() < 15) { // הגבלת אורך
            currentInputText += static_cast<char>(unicode);
        }
    }
    
    // עדכון הטקסט המוצג
    if (activeInputPlayer >= 0 && activeInputPlayer < static_cast<int>(setupPlayers.size())) {
        setupPlayers[activeInputPlayer].name = currentInputText;
    }
}

void CoupGUI::createGameFromSetup()
{
    try {
        // יצירת משחק חדש
        game = std::make_shared<coup::Game>();
        realPlayers.clear();
        
        // יצירת שחקנים על פי ההגדרות
        for (int i = 0; i < numPlayersToSetup; i++) {
            auto player = game->createPlayer(setupPlayers[i].name, setupPlayers[i].role);
            realPlayers.push_back(player);
        }
        
        // מעבר למצב משחק
        currentState = GuiState::PLAYING;
        
        // הגדרת השחקן הנוכחי
        if (!realPlayers.empty()) {
            currentPlayerName = realPlayers[0]->name();
        }
        
        // ניקוי מצבים זמניים
        waitingForTarget = false;
        pendingActionName = "";
        specialActionState = SpecialActionState::NONE;
        clearPlayerButtons();
        
        // ניקוי יומן והתראות
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
