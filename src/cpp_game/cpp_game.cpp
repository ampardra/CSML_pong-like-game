#include <iostream>
#include <raylib.h>
#include <cmath>
#include <queue>
#include <chrono>
#include <string>
#include <fstream>
using namespace std;
using namespace std::chrono;

// Enums
enum MovementMode { LINE, SIN, CURVE };

// Constants and vals
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;
const int DEFAULT_FPS = 60;


// Class Declarations
class Ball;
class Paddle;
class AI_Paddle;

// Function Prototypes
void InitGame(Ball& ball, Paddle& player, AI_Paddle& ai);
void UpdateGame(Ball& ball, Paddle& player, AI_Paddle& ai, int& playerScore, int& aiScore, float deltaTime, float* maxFPS);
void DrawGame(Ball& ball, Paddle& player, AI_Paddle& ai, float deltaTime, int playerScore, int aiScore, float maxFPS);
void ResetBall(Ball& ball);


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
    string GetMode();
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
    x += x_speed;
    y += y_speed;

    if (x + radius > SCREEN_WIDTH || x - radius < 0) {
        if (x_speed >= 0)
            spin -= 2;
        else
            spin += 2;
        x_speed *= -1;
    }

    if (y + radius >= SCREEN_HEIGHT || y - radius <= 0) {
        if (x_speed >= 0)
            spin -= 2;
        else
            spin += 2;
        y_speed *= -1;
        if (y + radius >= SCREEN_HEIGHT)
            y = SCREEN_HEIGHT - radius;
        else
            y = radius;
    }

    if (mode == SIN) {
        y += range * cos(time);
        time += 5 / range;
    }
    if (mode == CURVE)
        y_speed += gravity;
    angle += spin;

    if (IsKeyPressed(KEY_S))
        mode = (mode != SIN) ? SIN : LINE;
    if (IsKeyPressed(KEY_L))
        mode = LINE;
    if (IsKeyPressed(KEY_C))
        mode = (mode != CURVE) ? CURVE : LINE;
    if (IsKeyPressed(KEY_E))
        range += 2;
    if (IsKeyPressed(KEY_Q) && range != 5)
        range -= 2;
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
};

Paddle::Paddle(float x, float y)
    : x(x), y(y), width(25), height(120), speed(5), velocity(0), acceleration(0.3) {}

void Paddle::Draw() const {
    DrawRectangleRounded(Rectangle{x, y, width, height}, 0.5, 0, WHITE);
}

void Paddle::Update() {
    if (IsKeyDown(KEY_UP)) {
        velocity -= speed;
        y += velocity;
        speed += acceleration;
        acceleration += 0.05;
    } else if (IsKeyDown(KEY_DOWN)) {
        velocity += speed;
        y += velocity;
        speed += acceleration;
        acceleration += 0.05;
    } else {
        speed = 5;
        acceleration = 0.3;
        velocity = 0;
    }
    LimitMovements();
}

void Paddle::CollisionWithBall(Ball& ball) {
    ball.x_speed *= -1;
    if (velocity > 0) {
        ball.y_speed += acceleration - 0.3;
        ball.spin -= acceleration * 10;
    } else if (velocity < 0) {
        ball.y_speed -= acceleration - 0.3;
        ball.spin += acceleration * 10;
    }
}

void Paddle::LimitMovements() {
    if (y < 0) {
        y = 0;
        speed = 5;
    } else if (y + 120 > SCREEN_HEIGHT) {
        y = SCREEN_HEIGHT - 120;
        speed = 5;
    }
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
    float targetY = ball.y - height / 2;  // AI aims for the center of the ball
    float diff = targetY - y;

    float threshold = 5;  // Dead zone to prevent shaking

    if (fabs(diff) > threshold) {  // Move only if outside the dead zone
        if (diff > 0)
            y += speed;
        else
            y -= speed;
    }
    LimitMovements();
}

// Reset the ball to the center with initial speed
void ResetBall(Ball& ball) {
    ball.x = SCREEN_WIDTH / 2;
    ball.y = SCREEN_HEIGHT / 2;
    // Reset speeds (you can add some randomness if desired)
    ball.x_speed = (ball.x_speed > 0) ? 5 : -5; // Change direction for variety
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
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong-CPP");
    int fps = 0;
    SetTargetFPS(fps);

    auto lastTime = high_resolution_clock::now(), currentTime = high_resolution_clock::now();
    float deltaTime = 0.0f;
    float maxFPS = 0;
    while (!WindowShouldClose()) {
        
        currentTime = high_resolution_clock::now();
        deltaTime = duration_cast<duration<float>>(currentTime - lastTime).count();
        lastTime = currentTime;

        // Change fps mode
        if (IsKeyPressed(KEY_I)) {
            fps = DEFAULT_FPS - fps;
            SetTargetFPS(fps);
        }
        

        // Update game objects and check scoring
        UpdateGame(ball, player, ai, playerScore, aiScore, deltaTime, &maxFPS);
        DrawGame(ball, player, ai, deltaTime, playerScore, aiScore, maxFPS);
    }

    CloseWindow();
    ofstream file("../../data/cpp.log", std::ios::app);
    if (file.is_open())
    {
        file << maxFPS << '\n';
        file.close();
    }
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
    DrawText(TextFormat("Mode: %s", ball.GetMode().c_str()), SCREEN_WIDTH / 4, 20, 20, WHITE);
    // Draw scores at the top center
    DrawText(TextFormat("Player: %d", playerScore), SCREEN_WIDTH / 2 - 400, SCREEN_HEIGHT - 50, 40, BLUE);
    DrawText(TextFormat("AI: %d", aiScore), SCREEN_WIDTH / 2 + 300, SCREEN_HEIGHT - 50, 40, GREEN);
    // Draw max fps that has been observed
    DrawText(TextFormat("Max FPS: %f", maxFPS), SCREEN_WIDTH / 2 + 80, 20, 30, RED);
    EndDrawing();
}
