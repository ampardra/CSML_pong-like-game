#include <iostream>
#include <raylib.h>
#include <cmath>
#include <deque>
#include <chrono>
#include <string.h>

using namespace std;
using namespace std::chrono;

// Constants
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;

// Enums
enum MovementMode {
    LINE,
    SIN,
    CURVE
};

// Forward declarations (Prototyping classes)
class Ball;
class Paddle;
class AI_Paddle;

// Paddle Class Prototype
class Paddle {
protected:
    void LimitMovements();

public:
    float x, y;
    float width, height;
    float speed, acceleration;
    float velocity;
    int score;

    Paddle(float x, float y);
    void Draw();
    void Update();
    void IncrementScore();
    void CollisionWithBall(Ball &ball);
};

// AI Paddle (Inheriting from Paddle)
class AI_Paddle : public Paddle {
public:
    AI_Paddle(float x, float y);
    void CollisionWithBall(Ball &ball);
    void Update(Ball ball);
    void Ai_mode(float speedMode);
};

// Ball Class Prototype
class Ball {
public:
    float x, y;
    int radius;
    float x_speed, y_speed;
    bool waitingForClick;
    MovementMode mode;
    float spin, angle;
    float time;
    float gravity;
    float range;
    deque<Vector2> trail;

    Ball(float x, float y);
    void Draw();
    void Update(Paddle &player, AI_Paddle &ai);
    void UpdateTrail();
    char* GetMode();
};

// Paddle Implementation
Paddle::Paddle(float x, float y) : x(x), y(y) {
    speed = 5;
    width = 25;
    height = 120;
    velocity = 0;
    acceleration = 0.3;
    score = 0;
}

void Paddle::Draw() {
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

void Paddle::LimitMovements() {
    if (y < 0) {
        y = 0;
        speed = 5;
    } else if (y + 120 > SCREEN_HEIGHT) {
        y = SCREEN_HEIGHT - 120;
        speed = 5;
    }
}

void Paddle::IncrementScore() {
    score++;
}

void Paddle::CollisionWithBall(Ball &ball) {
    ball.x_speed *= -1;
    if (velocity > 0) {
        ball.y_speed += acceleration - 0.3;
        ball.spin -= acceleration * 10;
    } else if (velocity < 0) {
        ball.y_speed -= acceleration - 0.3;
        ball.spin += acceleration * 10;
    }
}

// AI Paddle Implementation
AI_Paddle::AI_Paddle(float x, float y) : Paddle(x, y) {}

void AI_Paddle::CollisionWithBall(Ball &ball) {
    ball.x_speed *= -1;
}

void AI_Paddle::Update(Ball ball) {
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

void AI_Paddle::Ai_mode(float speedMode) {
    speed = speedMode;
}

// Ball Implementation
Ball::Ball(float x, float y) : x(x), y(y) {
    x_speed = 0;
    y_speed = 0;
    radius = 15;
    mode = LINE;
    time = 0;
    spin = 0;
    angle = 0;
    gravity = 2;
    range = 5;
    waitingForClick = true;
}

void Ball::Draw() {
    float angleRad = angle * (PI / 180.0f);
    float line_x = x + cos(angleRad) * radius;
    float line_y = y + sin(angleRad) * radius;
    if (!waitingForClick)
        for (int i = 0; i < trail.size(); i++) {
            float alpha = (float)((TRAIL_LENGTH - i - 1) * (TRAIL_LENGTH - i - 1)) / (TRAIL_LENGTH * TRAIL_LENGTH);
            Color fadeColor = {0, 200, 255, (unsigned char)(alpha * 255)};
            DrawCircle(trail[i].x, trail[i].y, radius / 4, fadeColor);
        }

    DrawCircle(x, y, radius, WHITE);
    DrawLineEx({x, y}, {line_x, line_y}, 4, RED);
}

void Ball::Update(Paddle &player, AI_Paddle &ai) {
    if (waitingForClick) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {  // Check if the left mouse button is clicked
            Vector2 mousePos = GetMousePosition();
            float dx = mousePos.x - x;  // Direction vector
            float dy = mousePos.y - y;

            float length = sqrt(dx * dx + dy * dy);  // Normalize the vector
            if (length != 0) {
                x_speed = (dx / length) * 10;  // Scale speed
                y_speed = (dy / length) * 10;
            }

            waitingForClick = false;  // Ball is now moving
        }
        return;  // Do not move the ball until clicked
    }
    x += x_speed;
    y += y_speed;

    if (x + radius >= SCREEN_WIDTH || x - radius <= 0) {
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

    if (x + radius >= SCREEN_WIDTH || x - radius <= 0) {
        if (x + radius >= SCREEN_WIDTH) {
            player.IncrementScore();
        }    
        else
            ai.IncrementScore();
        x = SCREEN_WIDTH / 2;
        y = SCREEN_HEIGHT / 2;
        x_speed = 0;
        y_speed = 0;
        waitingForClick = true;  // Ball will wait for a new mouse click
        trail.clear();
    }
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

int main() {
    Ball ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Paddle player(10, SCREEN_HEIGHT / 2 - 60);
    AI_Paddle ai(SCREEN_WIDTH - 35, SCREEN_HEIGHT / 2 - 60);
    ai.Ai_mode(10);
    int fps = 60;
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong-CPP");
    SetTargetFPS(fps);

    auto lastTime = high_resolution_clock::now();
    auto currentTime = high_resolution_clock::now();
    float deltaTime = 0.0f;
    double maxFPS = 1;
    while (!WindowShouldClose()) {
        currentTime = high_resolution_clock::now();
        deltaTime = duration_cast<duration<float>>(currentTime - lastTime).count();
        lastTime = currentTime;
        if (maxFPS < 1 / deltaTime && 1 / deltaTime < 1000)
            maxFPS = 1 / deltaTime;
        
        if (IsKeyPressed(KEY_I)) {
            fps = 60 - fps;
            SetTargetFPS(fps);
        }

        ball.Update(player, ai);
        player.Update();
        ai.Update(ball);
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius,
             Rectangle{player.x, player.y, player.width, player.height}))
            {
                player.CollisionWithBall(ball);
            }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius,
             Rectangle{ai.x, ai.y, ai.width, ai.height}))
            {
                ai.CollisionWithBall(ball);
            }
        BeginDrawing();
        ClearBackground(BLACK);
        ball.Draw();
        player.Draw();
        ai.Draw();
        DrawText(TextFormat("Render time: %f", deltaTime), SCREEN_WIDTH / 8, 20, 20, WHITE);
        DrawText(TextFormat("MODE: %s", ball.GetMode()), 3 * SCREEN_WIDTH / 4, 30, 20, YELLOW);
        DrawText(TextFormat("max FPS: %lf", maxFPS), 3 * SCREEN_WIDTH / 8, 20, 30, RED);
        DrawText(TextFormat("player Score: %i", player.score), SCREEN_WIDTH / 4, SCREEN_HEIGHT - 30, 20, YELLOW);
        DrawText(TextFormat("cpu score: %i", ai.score), 3 * SCREEN_WIDTH / 4, SCREEN_HEIGHT - 30, 20, YELLOW);
        EndDrawing();
    }

    CloseWindow();
}
