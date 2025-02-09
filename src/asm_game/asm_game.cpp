#include <iostream>
#include <raylib.h>
#include <cmath>
#include <queue>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <algorithm>
using namespace std;
using namespace std::chrono;

// External assembly functions
extern "C" void update_ball(float* x, float* y, float* x_speed, float* y_speed, int radius, float* spin);
extern "C" void update_ball_movement(float* y, float* time, int range, int mode, float gravity, float* y_speed, float* angle, float spin);
extern "C" void update_key_s(int* mode, float* time);
extern "C" void update_key_l(int* mode, float* time);
extern "C" void update_key_c(int* mode, float* time);
extern "C" void update_key_range(float* range);
extern "C" void limit_movements(float* y);
extern "C" void update_paddle_movement(float* velocity, float* y, float* speed, float* acceleration);
extern "C" void collision_with_ball(float* x_speed, float* y_speed, float* spin, float velocity, float acceleration);
extern "C" void update_ai_paddle(float* y, float speed, float ball_y);

// Enums
enum MovementMode { LINE, SIN, CURVE };
enum GameState { Login, Register, GameMenu, GameSettings, Game, GameOverLose, GameOverWin, Ranking, Manual };


// Constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;
const int DEFAULT_FPS = 60;



// Class Declarations
class Ball;
class Paddle;
class AI_Paddle;
class Menu;

// Account management structures
struct UserAccount {
    std::string username;
    std::string password;
    int wins = 0;
    int losses = 0;
    int points = 0;
};

std::unordered_map<std::string, UserAccount> accounts;

void LoadAccounts() {
    ifstream file("../../data/accounts.dat");
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            size_t sep1 = line.find('|');
            size_t sep2 = line.find('|', sep1+1);
            size_t sep3 = line.find('|', sep2+1);
            size_t sep4 = line.find('|', sep3+1);
            if (sep1 != std::string::npos && sep2 != std::string::npos && 
                sep3 != std::string::npos && sep4 != std::string::npos) {
                UserAccount acc;
                acc.username = line.substr(0, sep1);
                acc.password = line.substr(sep1+1, sep2-sep1-1);
                acc.wins = stoi(line.substr(sep2+1, sep3-sep2-1));
                acc.losses = stoi(line.substr(sep3+1, sep4-sep3-1));
                acc.points = stoi(line.substr(sep4+1));
                accounts[acc.username] = acc;
            }
        }
        file.close();
    }
}

void SaveAccounts() {
    ofstream file("../../data/accounts.dat");
    if (file.is_open()) {
        for (const auto& pair : accounts) {
            file << pair.second.username << "|" 
                 << pair.second.password << "|"
                 << pair.second.wins << "|"
                 << pair.second.losses << "|"
                 << pair.second.points << "\n";
        }
        file.close();
    }
}
    
    

// Ball Class
class Ball {
public:
    float x, y;
    int radius;
    float x_speed, y_speed;
    float speed;
    MovementMode mode;
    float spin, angle, time, gravity, range;
    deque<Vector2> trail;

    Ball(float x, float y);
    void Draw() const;
    void Update();
    void UpdateTrail();
    string GetMode();
    void SetSpeed(float s);
    void SetMode(int m);
};

Ball::Ball(float x, float y)
    : x(x), y(y), x_speed(-5), y_speed(-5), radius(15), mode(LINE),
      spin(0), angle(0), time(0), gravity(2), range(5), speed(5) {}

void Ball::SetSpeed(float s) {
    speed = s;
    x_speed = -s;
    y_speed = -s;
}   

void Ball::SetMode(int m) {
    mode = (m == 0) ? LINE : (m == 1) ? SIN : CURVE;
}

void Ball::Draw() const {
    float angleRad = angle * (PI / 180.0f);
    // Draw ball trail
    for (int i = 0; i < trail.size(); i++) {
        float alpha = (float)((TRAIL_LENGTH - i - 1) * (TRAIL_LENGTH - i - 1)) / (TRAIL_LENGTH * TRAIL_LENGTH);
        DrawCircle(trail[i].x, trail[i].y, radius / 4, {0, 200, 255, (unsigned char)(alpha * 255)});
    }
    // Draw ball and direction line
    DrawCircle(x, y, radius, WHITE);
    DrawLineEx({x, y}, {x + cos(angleRad) * radius, y + sin(angleRad) * radius}, 4, RED);
}

void Ball::Update() {
    update_ball(&x, &y, &x_speed, &y_speed, radius, &spin);
    int MODE = mode;
    update_ball_movement(&y, &time, range, MODE, gravity, &y_speed, &angle, spin);
    update_key_s(&MODE, &time);
    update_key_l(&MODE, &time);
    update_key_c(&MODE, &time);
    update_key_range(&range);
    mode = (MODE == 0) ? LINE : (MODE == 1) ? SIN : CURVE;
    UpdateTrail();
}

void Ball::UpdateTrail() {
    trail.push_front({x, y});
    if (trail.size() > TRAIL_LENGTH)
        trail.pop_back();
}

string Ball::GetMode() {
    switch (mode)
    {
    case 0:
        return "Line";
        break;
    case 1:
        return "Sin";
    default:
        return "Curve";
        break;
    }
}

// Paddle Class
class Paddle {
protected:
    void LimitMovements();

public:
    float x, y, width, height, speed, acceleration, velocity;

    Paddle(float x, float y);
    void Draw() const;
    void Update();
    void CollisionWithBall(Ball& ball);
    void SetSpeed(int s);
};

Paddle::Paddle(float x, float y)
    : x(x), y(y), width(25), height(120), speed(5), velocity(0), acceleration(0.3) {}

void Paddle::Draw() const {
    DrawRectangleRounded(Rectangle{x, y, width, height}, 0.5, 0, WHITE);
}

void Paddle::Update() {
    update_paddle_movement(&velocity, &y, &speed, &acceleration);
    LimitMovements();
}

void Paddle::CollisionWithBall(Ball& ball) {
    collision_with_ball(&ball.x_speed, &ball.y_speed, &ball.spin, velocity, acceleration);
}

void Paddle::LimitMovements() {
    limit_movements(&y);
}

void Paddle::SetSpeed(int s) {
    speed = s;
}

// AI Paddle Class
class AI_Paddle : public Paddle {
public:
    AI_Paddle(float x, float y);
    void CollisionWithBall(Ball& ball);
    void Update(const Ball& ball);
};

AI_Paddle::AI_Paddle(float x, float y)
    : Paddle(x, y) {}

void AI_Paddle::CollisionWithBall(Ball& ball) {
    ball.x_speed *= -1;
}

void AI_Paddle::Update(const Ball& ball) {
    update_ai_paddle(&y, speed, ball.y);
    LimitMovements();
}




// Menu Class
class Menu {
    private:
        GameState& state;
        std::string usernameInput;
        std::string passwordInput;
        bool showPassword = false;
        bool usernameActive = false;
        bool passwordActive = false;
        Ball& ball;
        Paddle& player;
        AI_Paddle& ai;
        int& playerScore;
        int& aiScore;
        float deltaTime = 0.0f;
        float maxFPS = 0;
        high_resolution_clock::time_point lastTime;
        std::string usernameRegInput;
        std::string passwordRegInput;
        std::string confirmPasswordInput;
        bool regUsernameActive = false;
        bool regPasswordActive = false;
        bool regConfirmActive = false;
        std::string errorMessage;
        // GameSetting
        int selectedOption = 0;
        int difficulty = 1; // 0: Easy, 1: Medium, 2: Hard
        int ballMode = 0;   // 0: Line, 1: Sin, 2: Curve
        bool musicOn = true;
        int fps = 60;
        Music bgMusic;

    public:
        Menu(GameState& gameState, Ball& b, Paddle& p, AI_Paddle& a, int& pScore, int& aScore)
            : state(gameState), ball(b), player(p), ai(a), playerScore(pScore), aiScore(aScore) {
            lastTime = high_resolution_clock::now();
        }
    
        void HandleInput() {
            if (state == Login) {
                HandleLoginInput();
            } else if (state == Register) {
                HandleRegisterInput();
            } else if (state == GameMenu) {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mousePoint = GetMousePosition();
                    // Check if ranking button is clicked
                    if (CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 50})) {
                        state = Ranking;
                    }
                    // Check if manual button is clicked
                    else if (CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 120, 200, 50})) {
                        state = Manual;
                    }
                }
                if (IsKeyPressed(KEY_ENTER)) {
                    state = GameSettings;
                }
            } else if (state == GameOverWin || state == GameOverLose) {
                if (IsKeyPressed(KEY_SPACE)) {
                    state = GameMenu;
                } else if (IsKeyPressed(KEY_ENTER)) {
                    state = GameSettings;
                }
            } else if (state == Ranking || state == Manual) {
                if (IsKeyPressed(KEY_SPACE)) {
                    state = GameMenu;
                }
            } else if (state == GameSettings) {
                if (IsKeyPressed(KEY_DOWN)) selectedOption = (selectedOption + 1) % 4;
                if (IsKeyPressed(KEY_UP)) selectedOption = (selectedOption - 1 + 4) % 4;
                
                if (selectedOption == 0 && IsKeyPressed(KEY_RIGHT)) difficulty = (difficulty + 1) % 3;
                if (selectedOption == 0 && IsKeyPressed(KEY_LEFT)) difficulty = (difficulty - 1 + 3) % 3;
                
                if (selectedOption == 1 && IsKeyPressed(KEY_RIGHT)) ballMode = (ballMode + 1) % 3;
                if (selectedOption == 1 && IsKeyPressed(KEY_LEFT)) ballMode = (ballMode - 1 + 3) % 3;
                
                if (selectedOption == 2 && IsKeyPressed(KEY_ENTER)) musicOn = !musicOn;
                
                if (selectedOption == 3 && IsKeyPressed(KEY_ENTER)) { 
                    state = Game;
                    ball.SetMode(ballMode);
                    float speed = (difficulty == 0) ? 5 : (difficulty == 1) ? 10 : 15;
                    ball.SetSpeed(speed);
                    ai.SetSpeed(speed); 
                    if (musicOn)
                    {
                        InitAudioDevice();  // Initialize audio device
                        bgMusic = LoadMusicStream("../../music/stranger-things.mp3"); // Load background music
                        PlayMusicStream(bgMusic); // Play music
                    }
                      
                }
            }
            
        
            // Clear input fields when switching to Register or Login
            if (IsKeyPressed(KEY_SPACE)) {
                if (state == Register) {
                    state = Login;
                    usernameRegInput.clear();
                    passwordRegInput.clear();
                    confirmPasswordInput.clear();
                    errorMessage.clear();
                } else if (state == Login) {
                    state = Register;
                    usernameInput.clear();
                    passwordInput.clear();
                    errorMessage.clear();
                }
            }
        }
    
        void UpdateGame() {
            if (state == Game) {
                auto currentTime = high_resolution_clock::now();
                deltaTime = duration_cast<duration<float>>(currentTime - lastTime).count();
                lastTime = currentTime;
    
                if (IsKeyPressed(KEY_I)) {
                    fps = 60 - fps;
                    SetTargetFPS(fps);
                }
    
                ball.Update();
                player.Update();
                ai.Update(ball);
    
                if (maxFPS < 1 / deltaTime && 1 / deltaTime < 1000) {
                    maxFPS = 1 / deltaTime;    
                }
    
                if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height})) {
                    player.CollisionWithBall(ball);
                }
                if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {ai.x, ai.y, ai.width, ai.height})) {
                    ai.CollisionWithBall(ball);
                }
    
                if (ball.x - ball.radius < 0) {
                    aiScore++;
                    ResetBall(ball);
                } else if (ball.x + ball.radius > SCREEN_WIDTH) {
                    playerScore++;
                    ResetBall(ball);
                }

                if (abs(playerScore - aiScore) >= 10) {
                    if (playerScore > aiScore) {
                        state = GameOverWin;
                        // Update account stats
                        if (accounts.find(usernameInput) != accounts.end()) {
                            accounts[usernameInput].wins++;
                            accounts[usernameInput].points += 3;
                            SaveAccounts();
                        }
                    } else {
                        state = GameOverLose;
                        // Update account stats
                        if (accounts.find(usernameInput) != accounts.end()) {
                            accounts[usernameInput].losses++;
                            accounts[usernameInput].points -= 1;
                            SaveAccounts();
                        }
                    }
                    // Reset scores and fps and maxFPS for next game
                    playerScore = 0;
                    aiScore = 0;
                    fps = 60;
                    if (musicOn)
                    {
                        UnloadMusicStream(bgMusic); // Clean up music
                        CloseAudioDevice(); // Close audio
                    }
                    ofstream file("../../data/asm.log", std::ios::app);
                    if (file.is_open())
                    {
                        file << maxFPS << '\n';
                        file.close();
                    }
                    maxFPS = 0;
                }
            }
        }
    
        void Draw() {
            BeginDrawing();
            ClearBackground(state == Login || state == Register ? DARKBLUE : BLACK);
        
            if (state == Login) {
                DrawLoginScreen();
            } else if (state == Register) {
                DrawRegisterScreen();
            } else if (state == GameMenu) {
                DrawGameMenu();
            } else if (state == GameSettings) {
                DrawGameSettings();
            } else if (state == Game) {
                DrawGame();
            } else if (state == GameOverWin) {
                DrawGameOverScreen(true);
            } else if (state == GameOverLose) {
                DrawGameOverScreen(false);
            } else if (state == Ranking) {
                DrawRankingScreen();
            } else if (state == Manual) {
                DrawManualScreen();
            }
        
            EndDrawing();
        }
    
    private:
        void ResetBall(Ball& ball) {
            ball.x = SCREEN_WIDTH / 2;
            ball.y = SCREEN_HEIGHT / 2;
            // Reset speeds and angles (you can add some randomness if desired)
            ball.x_speed = (ball.x_speed > 0) ? -ball.speed : ball.speed; // Change direction for variety
            ball.y_speed = -ball.speed;
            ball.spin = 0;
            ball.angle = 0;
            ball.trail.clear();
        }

        void DrawGame() {
            if (musicOn)
            {
                UpdateMusicStream(bgMusic); // Update music stream
            }
            
            ball.Draw();
            player.Draw();
            ai.Draw();
    
            DrawText(TextFormat("Mode: %s", ball.GetMode().c_str()), SCREEN_WIDTH / 4, 20, 20, WHITE);
            DrawText(TextFormat("%s: %d", usernameInput.c_str(), playerScore), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT - 50, 40, BLUE);
            DrawText(TextFormat("AI: %d", aiScore), SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT - 50, 40, GREEN);
            DrawText(TextFormat("Max FPS: %f", maxFPS), SCREEN_WIDTH / 2 + 80, 20, 30, RED);
        }
    
        void DrawLoginScreen() {
            DrawText("Login to Pong", SCREEN_WIDTH / 2 - 100, 200, 40, WHITE);
            
            DrawText("Username:", SCREEN_WIDTH / 2 - 150, 300, 30, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH / 2 - 150, 340, 300, 40, usernameActive ? LIGHTGRAY : RAYWHITE);
            DrawText(usernameInput.c_str(), SCREEN_WIDTH / 2 - 140, 350, 30, BLACK);
            
            DrawText("Password:", SCREEN_WIDTH / 2 - 150, 400, 30, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH / 2 - 150, 440, 300, 40, passwordActive ? LIGHTGRAY : RAYWHITE);
            std::string displayedPassword = showPassword ? passwordInput : std::string(passwordInput.length(), '*');
            DrawText(displayedPassword.c_str(), SCREEN_WIDTH / 2 - 140, 450, 30, BLACK);
            
            DrawText("Press TAB to toggle password visibility", SCREEN_WIDTH / 2 - 200, 500, 20, LIGHTGRAY);
            
            DrawRectangle(SCREEN_WIDTH / 2 - 80, 550, 160, 50, GREEN);
            DrawText("LOGIN", SCREEN_WIDTH / 2 - 30, 565, 30, BLACK);
            DrawRectangle(SCREEN_WIDTH / 2 - 80, 620, 160, 50, BLUE);
            DrawText("REGISTER", SCREEN_WIDTH / 2 - 50, 635, 30, WHITE);
        }
    
        void HandleLoginInput() {
            Vector2 mousePoint = {0, 0};
            bool registerButtonPressed = false;
            bool loginButtonPressed = false;
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                mousePoint = GetMousePosition();
                registerButtonPressed = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH/2 - 80, 620, 160, 50});
                loginButtonPressed = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 80, 550, 160, 50});
                usernameActive = CheckCollisionPointRec(mousePoint, { SCREEN_WIDTH / 2 - 150, 340, 300, 40 });
                passwordActive = CheckCollisionPointRec(mousePoint, { SCREEN_WIDTH / 2 - 150, 440, 300, 40 });
            }
            
            int key = GetCharPressed();
            while (key > 0) {
                if (usernameActive && usernameInput.length() < 10) {
                    usernameInput += (char)key;
                } else if (passwordActive && passwordInput.length() < 10) {
                    passwordInput += (char)key;
                }
                key = GetCharPressed();
            }
    
            if (loginButtonPressed && !usernameInput.empty() && !passwordInput.empty()) {
                if (accounts.find(usernameInput) != accounts.end()) {
                    if (accounts[usernameInput].password == passwordInput) {
                        state = GameMenu;
                        errorMessage.clear();
                    } else {
                        errorMessage = "Invalid password!";
                    }
                } else {
                    errorMessage = "User not found!";
                }
            
            }
            
            // Add registration button check
            if (registerButtonPressed) {
                state = Register;
                usernameRegInput.clear();
                passwordRegInput.clear();
                confirmPasswordInput.clear();
                errorMessage.clear();
            }
            
            // Add error message drawing to DrawLoginScreen
            if (!errorMessage.empty()) {
                DrawText(errorMessage.c_str(), SCREEN_WIDTH/2 - 150, 670, 20, RED);
            }
            
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (usernameActive && !usernameInput.empty()) usernameInput.pop_back();
                else if (passwordActive && !passwordInput.empty()) passwordInput.pop_back();
            }
            if (IsKeyPressed(KEY_TAB)) showPassword = !showPassword;
            if (IsKeyPressed(KEY_ENTER) && !usernameInput.empty() && !passwordInput.empty()) {
                if (accounts.find(usernameInput) != accounts.end()) {
                    if (accounts[usernameInput].password == passwordInput) {
                        state = GameMenu;
                        errorMessage.clear();
                    } else {
                        errorMessage = "Invalid password!";
                    }
                } else {
                    errorMessage = "User not found!";
                }
            }
        }

        void DrawRegisterScreen() {
            DrawText("Create Account", SCREEN_WIDTH / 2 - 120, 200, 40, WHITE);
        
            // Username field
            DrawText("Username:", SCREEN_WIDTH / 2 - 150, 300, 30, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH / 2 - 150, 340, 300, 40, regUsernameActive ? LIGHTGRAY : RAYWHITE);
            DrawText(usernameRegInput.c_str(), SCREEN_WIDTH / 2 - 140, 350, 30, BLACK);
        
            // Password field
            DrawText("Password:", SCREEN_WIDTH / 2 - 150, 400, 30, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH / 2 - 150, 440, 300, 40, regPasswordActive ? LIGHTGRAY : RAYWHITE);
            DrawText(std::string(passwordRegInput.length(), '*').c_str(), SCREEN_WIDTH / 2 - 140, 450, 30, BLACK);
        
            // Confirm Password field
            DrawText("Confirm Password:", SCREEN_WIDTH / 2 - 150, 500, 30, LIGHTGRAY);
            DrawRectangle(SCREEN_WIDTH / 2 - 150, 540, 300, 40, regConfirmActive ? LIGHTGRAY : RAYWHITE);
            DrawText(std::string(confirmPasswordInput.length(), '*').c_str(), SCREEN_WIDTH / 2 - 140, 550, 30, BLACK);
        
            // Register button
            DrawRectangle(SCREEN_WIDTH / 2 - 80, 600, 160, 50, GREEN);
            DrawText("REGISTER", SCREEN_WIDTH / 2 - 50, 615, 30, BLACK);
        
            // Back to Login button
            DrawRectangle(SCREEN_WIDTH / 2 - 80, 670, 160, 50, BLUE);
            DrawText("BACK TO LOGIN", SCREEN_WIDTH / 2 - 70, 685, 20, WHITE);
        
            // Error message (if any)
            if (!errorMessage.empty()) {
                DrawText(errorMessage.c_str(), SCREEN_WIDTH / 2 - 150, 740, 20, RED);
            }
        }

        void HandleRegisterInput() {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePoint = GetMousePosition();

                // Check which field is active
                regUsernameActive = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 150, 340, 300, 40});
                regPasswordActive = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 150, 440, 300, 40});
                regConfirmActive = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 150, 540, 300, 40});

                // Check if Register button is clicked
                if (CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 80, 600, 160, 50})) {
                    if (passwordRegInput != confirmPasswordInput) {
                        errorMessage = "Passwords don't match!";
                    } else if (accounts.find(usernameRegInput) != accounts.end()) {
                        errorMessage = "Username already exists!";
                    } else if (usernameRegInput.empty() || passwordRegInput.empty()) {
                        errorMessage = "Fields cannot be empty!";
                    } else {
                        accounts[usernameRegInput] = {usernameRegInput, passwordRegInput};
                        SaveAccounts();
                        state = Login; // Go back to login after successful registration
                        errorMessage.clear();
                    }
                }

                // Check if Back to Login button is clicked
                if (CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 80, 670, 160, 50})) {
                    state = Login; // Switch to the login screen
                    errorMessage.clear(); // Clear any error messages
                }
            }

            // Handle text input
            int key = GetCharPressed();
            while (key > 0) {
                if (regUsernameActive && usernameRegInput.length() < 10) {
                    usernameRegInput += (char)key;
                } else if (regPasswordActive && passwordRegInput.length() < 10) {
                    passwordRegInput += (char)key;
                } else if (regConfirmActive && confirmPasswordInput.length() < 10) {
                    confirmPasswordInput += (char)key;
                }
                key = GetCharPressed();
            }

            // Handle backspace
            if (IsKeyPressed(KEY_BACKSPACE)) {
                if (regUsernameActive && !usernameRegInput.empty()) usernameRegInput.pop_back();
                else if (regPasswordActive && !passwordRegInput.empty()) passwordRegInput.pop_back();
                else if (regConfirmActive && !confirmPasswordInput.empty()) confirmPasswordInput.pop_back();
            }
        }

        void DrawGameOverScreen(bool won) {
            DrawText(won ? "You Win!" : "Game Over", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 50, won ? GREEN : RED);
            
            if (accounts.find(usernameInput) != accounts.end()) {
                auto& account = accounts[usernameInput];
                DrawText(TextFormat("Wins: %d", account.wins), SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2, 30, WHITE);
                DrawText(TextFormat("Losses: %d", account.losses), SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 40, 30, WHITE);
                DrawText(TextFormat("Total Points: %d", account.points), SCREEN_WIDTH/2 - 120, SCREEN_HEIGHT/2 + 80, 30, WHITE);
            }
            
            DrawText("Press SPACE to return to menu", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 100, 30, LIGHTGRAY);
            DrawText("Press ENTER to play again", SCREEN_WIDTH/2 - 180, SCREEN_HEIGHT - 60, 30, LIGHTGRAY);
        }

        void DrawGameMenu() {
            DrawText("Welcome to Pong!", SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 - 100, 40, WHITE);
            DrawText("Press ENTER to start", SCREEN_WIDTH/2 - 160, SCREEN_HEIGHT/2 - 50, 30, GREEN);
            
            // Ranking button
            DrawRectangle(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 50, 200, 50, BLUE);
            DrawText("RANKING", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 65, 30, WHITE);

            // Manual button
            DrawRectangle(SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 120, 200, 50, ORANGE);
            DrawText("MANUAL", SCREEN_WIDTH/2 - 60, SCREEN_HEIGHT/2 + 135, 30, WHITE);
        }

        void DrawRankingScreen() {
            DrawText("Player Rankings", SCREEN_WIDTH/2 - 150, 50, 40, WHITE);
            
            // Convert accounts to a vector for sorting
            std::vector<UserAccount> sortedAccounts;
            for (const auto& pair : accounts) {
                sortedAccounts.push_back(pair.second);
            }
            
            // Sort by points (descending)
            std::sort(sortedAccounts.begin(), sortedAccounts.end(), 
                [](const UserAccount& a, const UserAccount& b) {
                    return b.points < a.points;
                });
            
            // Display top 10 players
            int y = 150;
            for (size_t i = 0; i < sortedAccounts.size() && i < 10; i++) {
                const auto& account = sortedAccounts[i];
                DrawText(TextFormat("%d. %s: %d points", i+1, account.username.c_str(), account.points), 
                        SCREEN_WIDTH/2 - 200, y, 30, WHITE);
                y += 40;
            }
            
            DrawText("Press SPACE to return to menu", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 100, 30, LIGHTGRAY);
        }

        void DrawManualScreen() {
            DrawText("Pong Game Manual", SCREEN_WIDTH/2 - 150, 50, 40, WHITE);
            
            // Game controls
            DrawText("Controls:", 50, 120, 30, WHITE);
            DrawText("- W/S or Up/Down arrows to move paddle", 70, 160, 25, LIGHTGRAY);
            DrawText("- I to toggle FPS display (For reaching highest performance possible)", 70, 200, 25, LIGHTGRAY);
            DrawText("- ESC to exit the game", 70, 240, 25, LIGHTGRAY);
            DrawText("- S for Sin mode, C for Curve mode and L for Line mode", 70, 280, 25, LIGHTGRAY);
            DrawText("- E/Q to increase and decrease range in Sin mode", 70, 320, 25, LIGHTGRAY);


            
            // Game rules
            DrawText("Rules:", 50, 380, 30, WHITE);
            DrawText("- First to gain 10 points advantage wins", 70, 420, 25, LIGHTGRAY);
            DrawText("- Win gives +3 points to your XP", 70, 460, 25, LIGHTGRAY);
            DrawText("- Lose deducts 1 point from your XP", 70, 500, 25, LIGHTGRAY);
            DrawText("Press SPACE to return to menu", SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 100, 30, LIGHTGRAY);
        }

        void DrawGameSettings() {
            ClearBackground(DARKGRAY);
            
            DrawText("Game Settings", 320, 100, 40, WHITE);
            
            Color colorDifficulty = (selectedOption == 0) ? YELLOW : WHITE;
            Color colorBallMode = (selectedOption == 1) ? YELLOW : WHITE;
            Color colorMusic = (selectedOption == 2) ? YELLOW : WHITE;
            Color colorStart = (selectedOption == 3) ? YELLOW : WHITE;
            
            DrawText("Difficulty:", 300, 200, 30, WHITE);
            DrawText((difficulty == 0) ? "Easy" : (difficulty == 1) ? "Medium" : "Hard", 500, 200, 30, colorDifficulty);
            
            DrawText("Ball Mode:", 300, 250, 30, WHITE);
            DrawText((ballMode == 0) ? "Line" : (ballMode == 1) ? "Sin" : "Curve", 500, 250, 30, colorBallMode);
            
            DrawText("Music:", 300, 300, 30, WHITE);
            DrawText(musicOn ? "On" : "Off", 500, 300, 30, colorMusic);
            
            DrawText("Press ENTER to Start", 320, 400, 30, colorStart);
        }
};

int main() {
    // Initialize game objects
    Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Paddle player(10, SCREEN_HEIGHT / 2 - 60);
    AI_Paddle ai(SCREEN_WIDTH - 35, SCREEN_HEIGHT / 2 - 60);
    LoadAccounts();
    // Scoring variables
    int playerScore = 0;
    int aiScore = 0;

    // Initialize window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong-ASM");
    SetTargetFPS(DEFAULT_FPS);

    // Initialize menu
    GameState state = Login;
    Menu menu(state, ball, player, ai, playerScore, aiScore);

    while (!WindowShouldClose()) {
        menu.HandleInput();
        menu.UpdateGame();
        menu.Draw();
    }

    CloseWindow();
    return 0;
}
