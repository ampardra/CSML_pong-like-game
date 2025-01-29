#include <raylib.h>
#include <cmath> // For sin, cos

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_RADIUS = 40;

float ball_x = SCREEN_WIDTH / 2;
float ball_y = SCREEN_HEIGHT / 2;
float ball_speed = 5.0f;
float angle = 0.0f; // Rotation angle of ball

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textured Rotating Ball");

    // Load ball texture (should be a circular image with transparency)
    Texture2D ballTexture = LoadTexture("ball.png");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Rotate the ball continuously
        angle += 2.0f;
        if (angle >= 360.0f) angle -= 360.0f;

        // Move the ball
        float angleRad = angle * (PI / 180.0f);
        ball_x += ball_speed * cos(angleRad);
        ball_y += ball_speed * sin(angleRad);

        // Bounce off walls
        if (ball_x + BALL_RADIUS >= SCREEN_WIDTH || ball_x - BALL_RADIUS <= 0) {
            angle = 180.0f - angle;
        }
        if (ball_y + BALL_RADIUS >= SCREEN_HEIGHT || ball_y - BALL_RADIUS <= 0) {
            angle = -angle;
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(WHITE);

        // Draw a circular mask behind the texture (to ensure the shape looks circular)
        DrawCircle(ball_x, ball_y, BALL_RADIUS * 0.75, BLACK);

        // Draw the rotated ball texture with transparency
        Rectangle source = {0, 0, (float)ballTexture.width, (float)ballTexture.height}; // Full texture
        Rectangle dest = {ball_x, ball_y, BALL_RADIUS * 1.8, BALL_RADIUS * 1.5}; // Scale to ball size
        Vector2 origin = {(float)BALL_RADIUS, (float)BALL_RADIUS}; // Rotate around center
        DrawTexturePro(ballTexture, source, dest, origin, angle, WHITE);

        EndDrawing();
    }

    UnloadTexture(ballTexture); // Free texture memory
    CloseWindow();
    return 0;
}
