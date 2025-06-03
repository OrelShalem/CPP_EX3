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

// מצב משחק יחיד
enum class GuiState
{
    SETUP,        // מסך הגדרות ראשוני
    PLAYING,      // מצב משחק אמיתי
    WIN_SCREEN    // מסך ניצחון
};

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
    sf::RectangleShape specialActionsPanel; // פאנל לפעולות מיוחדות

    // GUI state
    GuiState currentState;
    sf::Clock globalClock;
    std::vector<std::string> logMessages;
    std::queue<std::string> notificationQueue;
    bool debugMode;

    // Setup screen elements
    struct PlayerSetup {
        std::string name;
        coup::Role role;
        sf::Text nameText;
        sf::RectangleShape nameInput;
        sf::RectangleShape roleButtons[6]; // כפתורים לכל 6 התפקידים
        sf::Text roleButtonTexts[6];
        bool nameCompleted;
        bool roleSelected;
    };
    
    std::vector<PlayerSetup> setupPlayers;
    int numPlayersToSetup;
    int currentSetupStep; // 0 = בחירת מספר שחקנים, 1+ = הגדרת שחקנים
    sf::RectangleShape playerCountButtons[5]; // כפתורים לבחירת 2-6 שחקנים
    sf::Text playerCountTexts[5];
    sf::RectangleShape startGameButton;
    sf::Text startGameButtonText;
    std::string currentInputText;
    int activeInputPlayer; // איזה שחקן כרגע מזין שם
    
    // משחק אמיתי
    std::shared_ptr<coup::Game> game;
    std::vector<std::shared_ptr<coup::Player>> realPlayers;
    std::string currentPlayerName;

    // מצב בחירת מטרה - פשוט יותר!
    bool waitingForTarget;
    std::string pendingActionName; // שם הפעולה שממתינה לביצוע
    
    // מנגנון דו-שלבי לפעולות מיוחדות
    enum class SpecialActionState
    {
        NONE,                   // אין פעולה מיוחדת
        SELECTING_PERFORMER,    // בוחר מי מבצע את הפעולה
        SELECTING_TARGET        // בוחר על מי מבצעים את הפעולה
    };
    SpecialActionState specialActionState;
    std::string pendingSpecialAction;        // שם הפעולה המיוחדת
    coup::Role pendingSpecialActionRole;     // התפקיד הנדרש
    std::shared_ptr<coup::Player> selectedPerformer; // השחקן שנבחר לבצע את הפעולה

    // כפתורי בחירת שחקנים
    struct PlayerButton
    {
        std::string playerName;
        sf::RectangleShape button;
        sf::Text buttonText;
    };
    std::vector<PlayerButton> playerButtons;

    // נתונים לפעולות מיוחדות מחוץ לתור
    struct SpecialAction
    {
        std::string name;
        coup::Role requiredRole;
        sf::RectangleShape button;
        sf::Text buttonText;
    };
    std::vector<SpecialAction> specialActions;

    // נתונים לפעולות בסיסיות בתור
    struct BasicAction
    {
        std::string name;
        sf::RectangleShape button;
        sf::Text buttonText;
        sf::Color buttonColor;
    };
    std::vector<BasicAction> basicActions;

    // כפתור ריסט
    sf::RectangleShape resetButton;
    sf::Text resetButtonText;
    
    // מסך ניצחון
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
    void update();
    void render();
    void initializeSpecialActions(); // פונקציה לאתחול פעולות מיוחדות
    void initializeBasicActions();   // פונקציה לאתחול פעולות בסיסיות
    void initializeResetButton();    // פונקציה לאתחול כפתור ריסט
    void initializeWinScreen();      // פונקציה לאתחול מסך ניצחון
    void initializeSetupScreen();    // פונקציה לאתחול מסך הגדרות
    // void initializeRealGame();       // פונקציה לאתחול משחק אמיתי עם שחקנים ברירת מחדל
    void initializeGui();            // פונקציה משותפת לאתחול הממשק הגרפי

    // Drawing components
    void drawPlayers();
    void drawGameLog();
    void drawActionPanel();
    void drawNotifications();
    void drawDebugInfo();
    void drawSpecialActions(); // פונקציה לציור כפתורי פעולות מיוחדות
    void drawPlayerButtons();  // פונקציה לציור כפתורי בחירת שחקנים
    void drawResetButton();    // פונקציה לציור כפתור ריסט
    void drawWinScreen();      // פונקציה לציור מסך ניצחון
    void drawSetupScreen();    // פונקציה לציור מסך הגדרות

    // Utility functions
    sf::Color getRoleColor(coup::Role role);
    std::string getRoleName(coup::Role role);
    void addLogMessage(const std::string &message);
    void toggleDebugMode();
    void handleSpecialActionClick(int actionIndex); // טיפול בלחיצה על פעולה מיוחדת
    void handleBasicActionClick(int actionIndex);   // טיפול בלחיצה על פעולה בסיסית
    void handleResetClick();                        // טיפול בלחיצה על כפתור ריסט
    void handleWinScreenClick(sf::Vector2i mousePos); // טיפול בלחיצות במסך ניצחון
    void handleSetupScreenClick(sf::Vector2i mousePos); // טיפול בלחיצות במסך הגדרות
    void handleTextInput(sf::Uint32 unicode);       // טיפול בהזנת טקסט
    
    // Game management functions
    void resetGame();          // איפוס המשחק מחדש
    void checkForWinner();     // בדיקה אם יש מנצח
    void startNewGame();       // התחלת משחק חדש
    void createGameFromSetup(); // יצירת משחק מהגדרות
    
    // פונקציות לביצוע פעולות עם בחירת מטרה
    void executeArrest(std::shared_ptr<coup::Player> targetPlayer);
    void executeSanction(std::shared_ptr<coup::Player> targetPlayer);
    void executeCoup(std::shared_ptr<coup::Player> targetPlayer);
    
    // פונקציות עזר לכפתורי שחקנים
    void createPlayerButtons();
    void clearPlayerButtons();
    
    // פונקציות חדשות למנגנון דו-שלבי
    void startSpecialActionSelection(const std::string& actionName, coup::Role requiredRole);
    void createPerformerButtons(coup::Role requiredRole);
    void createTargetButtons(std::shared_ptr<coup::Player> performer);
    void executeSpecialAction(std::shared_ptr<coup::Player> performer, std::shared_ptr<coup::Player> target = nullptr);
    void cancelSpecialActionSelection();

public:
    CoupGUI();  // קונסטרקטור שמתחיל במסך הגדרות
    
    // קונסטרקטור שמקבל משחק ושחקנים קיימים (לא בשימוש כרגע)
    CoupGUI(std::shared_ptr<coup::Game> existingGame, std::vector<std::shared_ptr<coup::Player>> existingPlayers);
    
    ~CoupGUI();

    void run();
    void addNotification(const std::string &message);
};