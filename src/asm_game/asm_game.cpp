#include <iostream>
#include <raylib.h>
#include <cmath>
#include <queue>
#include <chrono>
#include<unordered_map>
using namespace std;
using namespace std::chrono;

// External assembly functions
extern "C" void update_ball(float* x, float* y, float* x_speed, float* y_speed, int radius, float* spin);
extern "C" void update_ball_movement(float* y, float* time, int range, int mode, float gravity, float* y_speed, float* angle, float spin);
extern "C" void update_key_s(int* mode, float* time);
extern "C" void update_key_l(int* mode, float* time);
extern "C" void update_key_c(int* mode, float* time);
extern "C" void update_key_range(float* range);
extern "C" void limit_movements(float* y, float* speed);
extern "C" void update_paddle_movement(float* velocity, float* y, float* speed, float* acceleration);
extern "C" void collision_with_ball(float* x_speed, float* y_speed, float* spin, float velocity, float acceleration);
extern "C" void update_ai_paddle(float* y, float speed, float ball_y);

// Enums
enum MovementMode { LINE, SIN, CURVE };
enum GameState {Login, GameMenu, Game};


// Constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;
const int DEFAULT_FPS = 60;
// Login menu vals
string usernameInput = "";
string passwordInput = "";
bool showPassword = false;
bool usernameActive = false;
bool passwordActive = false;
bool loginButtonPressed = false;
// General Game state
GameState state = Login;


// Class Declarations
class Ball;
class Paddle;
class AI_Paddle;
class Menu;

// Function Prototypes
void InitGame(Ball& ball, Paddle& player, AI_Paddle& ai);
void UpdateGame(Ball& ball, Paddle& player, AI_Paddle& ai, int& playerScore, int& aiScore, float deltaTime, float* maxFPS);
void DrawGame(Ball& ball, Paddle& player, AI_Paddle& ai, float deltaTime, int playerScore, int aiScore, float maxFPS);
void ResetBall(Ball& ball);
void DrawLoginScreen();
void HandleLoginInput();
typedef void (*MenuHandler)();

void ShowLoginScreen();
void ShowGameMenu();
void ShowGame();

// Ball Class
class Ball {
public:
    float x, y;
    int radius;
    float x_speed, y_speed;
    MovementMode mode;
    float spin, angle, time, gravity, range;
    deque<Vector2> trail;

    Ball(float x, float y);
    void Draw() const;
    void Update();
    void UpdateTrail();
    char* GetMode();
};

Ball::Ball(float x, float y)
    : x(x), y(y), x_speed(-5), y_speed(-5), radius(15), mode(LINE),
      spin(0), angle(0), time(0), gravity(2), range(5) {}

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

char* Ball::GetMode() {
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
    limit_movements(&y, &speed);
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

// Reset the ball to the center with initial speed
void ResetBall(Ball& ball) {
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    // Reset speeds and angles (you can add some randomness if desired)
    ball.x_speed = (ball.x_speed > 0) ? -5 : 5; // Change direction for variety
    ball.y_speed = -5;
    ball.spin = 0;
    ball.angle = 0;
    ball.trail.clear();
}

// Main Game Loop
int main() {
    Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Paddle player(10, SCREEN_HEIGHT / 2 - 60);
    AI_Paddle ai(SCREEN_WIDTH - 35, SCREEN_HEIGHT / 2 - 60);

    // Scoring variables
    int playerScore = 0;
    int aiScore = 0;

    // Max FPS variable for comparing asm and cpp
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong-ASM");
    int fps = DEFAULT_FPS;
    SetTargetFPS(fps);

    auto lastTime = high_resolution_clock::now(), currentTime = high_resolution_clock::now();
    float deltaTime = 0.0f;
    float maxFPS = 0;
    while (!WindowShouldClose()) {
        if (state == Login || state == GameMenu)
        {
            BeginDrawing();
            ClearBackground(DARKGRAY);

            if (state == Login) {
                HandleLoginInput();
                DrawLoginScreen();
            } else if (state == GameMenu) {
                ClearBackground(BLACK);
                DrawText("Welcome to Pong!", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 40, WHITE);
                DrawText("Press ENTER to start", SCREEN_WIDTH / 2 - 160, SCREEN_HEIGHT / 2 + 50, 30, GREEN);
                if (IsKeyPressed(KEY_ENTER)) {
                    state = Game;// Move to game logic
                }
            }

            EndDrawing();
        }
        else if (state == Game)
        {
            currentTime = high_resolution_clock::now();
            deltaTime = duration_cast<duration<float>>(currentTime - lastTime).count();
            lastTime = currentTime;

            // Change fps mode
            if (IsKeyPressed(KEY_I)) {
                fps = 60 - fps;
                SetTargetFPS(fps);
            }

            // Update game objects and check scoring
            UpdateGame(ball, player, ai, playerScore, aiScore, deltaTime, &maxFPS);
            DrawGame(ball, player, ai, deltaTime, playerScore, aiScore, maxFPS);
        }
    }

    CloseWindow();
    return 0;
}

void UpdateGame(Ball& ball, Paddle& player, AI_Paddle& ai, int& playerScore, int& aiScore, float deltaTime, float* maxFPS) {
    ball.Update();
    player.Update();
    ai.Update(ball);

    // Updating max FPS Value
        if (*maxFPS < 1 / deltaTime && 1 / deltaTime < 1000)
            *maxFPS = 1 / deltaTime;

    // Check collisions between ball and paddles
    if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {player.x, player.y, player.width, player.height}))
        player.CollisionWithBall(ball);
    if (CheckCollisionCircleRec({ball.x, ball.y}, ball.radius, {ai.x, ai.y, ai.width, ai.height}))
        ai.CollisionWithBall(ball);

    // Check if the ball has left the screen to update scores
    if (ball.x - ball.radius < 0) {  // Ball went off the left side => AI scores
        aiScore++;
        ResetBall(ball);
    }
    else if (ball.x + ball.radius > SCREEN_WIDTH) {  // Ball went off the right side => Player scores
        playerScore++;
        ResetBall(ball);
    }
}

void DrawGame(Ball& ball, Paddle& player, AI_Paddle& ai, float deltaTime, int playerScore, int aiScore, float maxFPS) {
    BeginDrawing();
    ClearBackground(BLACK);

    ball.Draw();
    player.Draw();
    ai.Draw();

    // Draw Ball mode information
    DrawText(TextFormat("Mode: %s", ball.GetMode()), SCREEN_WIDTH / 4, 20, 20, WHITE);
    // Draw scores at the top center
    DrawText(TextFormat("Player: %d", playerScore), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT - 50, 40, BLUE);
    DrawText(TextFormat("AI: %d", aiScore), SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT - 50, 40, GREEN);
    // Draw max fps that has been observed
    DrawText(TextFormat("Max FPS: %f", maxFPS), SCREEN_WIDTH / 2 + 80, 20, 30, RED);
    EndDrawing();
}


void DrawLoginScreen() {
    DrawText("Login to Pong", SCREEN_WIDTH / 2 - 100, 200, 40, WHITE);
    
    // Username Field
    DrawText("Username:", SCREEN_WIDTH / 2 - 150, 300, 30, LIGHTGRAY);
    DrawRectangle(SCREEN_WIDTH / 2 - 150, 340, 300, 40, usernameActive ? LIGHTGRAY : RAYWHITE);
    DrawText(usernameInput.c_str(), SCREEN_WIDTH / 2 - 140, 350, 30, BLACK);
    
    // Password Field
    DrawText("Password:", SCREEN_WIDTH / 2 - 150, 400, 30, LIGHTGRAY);
    DrawRectangle(SCREEN_WIDTH / 2 - 150, 440, 300, 40, passwordActive ? LIGHTGRAY : RAYWHITE);
    string displayedPassword = showPassword ? passwordInput : string(passwordInput.length(), '*');
    DrawText(displayedPassword.c_str(), SCREEN_WIDTH / 2 - 140, 450, 30, BLACK);
    
    // Toggle Password Visibility
    DrawText("Press TAB to toggle password visibility", SCREEN_WIDTH / 2 - 200, 500, 20, LIGHTGRAY);
    
    // Login Button
    DrawRectangle(SCREEN_WIDTH / 2 - 80, 550, 160, 50, GREEN);
    DrawText("LOGIN", SCREEN_WIDTH / 2 - 30, 565, 30, BLACK);
    
    // Register Button
    DrawRectangle(SCREEN_WIDTH / 2 - 80, 620, 160, 50, BLUE);
    DrawText("REGISTER", SCREEN_WIDTH / 2 - 50, 635, 30, WHITE);
}

void HandleLoginInput() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePoint = GetMousePosition();
        usernameActive = CheckCollisionPointRec(mousePoint, { SCREEN_WIDTH / 2 - 150, 340, 300, 40 });
        passwordActive = CheckCollisionPointRec(mousePoint, { SCREEN_WIDTH / 2 - 150, 440, 300, 40 });
        loginButtonPressed = CheckCollisionPointRec(mousePoint, {SCREEN_WIDTH / 2 - 80, 550, 160, 50});
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

    if (loginButtonPressed && !usernameInput.empty() && !passwordInput.empty())
    {
       state = GameMenu;        
    }
    
    
    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (usernameActive && !usernameInput.empty()) usernameInput.pop_back();
        else if (passwordActive && !passwordInput.empty()) passwordInput.pop_back();
    }
    if (IsKeyPressed(KEY_TAB)) showPassword = !showPassword;
    if (IsKeyPressed(KEY_ENTER) && !usernameInput.empty() && !passwordInput.empty()) {
        state = GameMenu;
    }
}
