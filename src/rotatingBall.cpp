#include <raylib.h>
#include <cmath> // For sin, cos

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_RADIUS = 50;
const int LINE_LENGTH = 50; // Length of the rotating indicator line

float ball_x = SCREEN_WIDTH / 2;
float ball_y = SCREEN_HEIGHT / 2;
float ball_speed_x = 4.0f;
float ball_speed_y = 3.0f;
float angle = 0.0f; // Rotation angle in degrees

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rotating Ball with Indicator Line");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Move ball
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;

        // Bounce off walls
        if (ball_x + BALL_RADIUS >= SCREEN_WIDTH || ball_x - BALL_RADIUS <= 0) {
            ball_speed_x *= -1;
        }
        if (ball_y + BALL_RADIUS >= SCREEN_HEIGHT || ball_y - BALL_RADIUS <= 0) {
            ball_speed_y *= -1;
        }

        // Rotate the indicator line based on movement
        angle += 4.0f; // Increase rotation speed
        if (angle >= 360.0f) angle -= 360.0f;

        // Calculate line end position using rotation
        float angleRad = angle * (PI / 180.0f);
        float line_x = ball_x + cos(angleRad) * LINE_LENGTH;
        float line_y = ball_y + sin(angleRad) * LINE_LENGTH;

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the ball as a filled circle
        DrawCircle(ball_x, ball_y, BALL_RADIUS, BLUE);

        // Draw the rotating indicator line
        DrawLineEx((Vector2){ball_x, ball_y}, (Vector2){line_x, line_y}, 4, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
