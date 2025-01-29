#include <raylib.h>
#include <cmath>  // For sin, cos
#include <deque>  // For storing past positions

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int BALL_RADIUS = 15;
const int LINE_LENGTH = BALL_RADIUS;  // Length of the rotating indicator line

float ball_x = SCREEN_WIDTH / 2;
float ball_y = SCREEN_HEIGHT / 2;
float ball_speed_x = 4.0f;
float ball_speed_y = 3.0f;
float angle = 0.0f;  // Rotation angle in degrees

std::deque<Vector2> trail;  // Stores past ball positions
const int TRAIL_LENGTH = 20;  // Max number of trail points

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Ball with Trail Effect");

    SetTargetFPS(60);
    float time = 0;
    while (!WindowShouldClose()) {
        // Store the current position in the trail
        trail.push_front({ball_x, ball_y});
        if (trail.size() > TRAIL_LENGTH) {
            trail.pop_back();  // Remove the oldest point
        }

        // Move ball
        ball_x += ball_speed_x;
        ball_y += ball_speed_y;
        ball_y += 5 * cos(time);
        time++;
        // Bounce off walls
        if (ball_x + BALL_RADIUS >= SCREEN_WIDTH || ball_x - BALL_RADIUS <= 0) {
            ball_speed_x *= -1;
        }
        if (ball_y + BALL_RADIUS >= SCREEN_HEIGHT || ball_y - BALL_RADIUS <= 0) {
            ball_speed_y *= -1;
        }

        // Rotate the indicator line
        angle += 4.0f;
        if (angle >= 360.0f) angle -= 360.0f;

        // Calculate line end position
        float angleRad = angle * (PI / 180.0f);
        float line_x = ball_x + cos(angleRad) * LINE_LENGTH;
        float line_y = ball_y + sin(angleRad) * LINE_LENGTH;

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw the fading trail (oldest positions get more transparent)
        for (int i = 0; i < trail.size(); i++) {
            float alpha = (float)((TRAIL_LENGTH - i - 1) * (TRAIL_LENGTH - i - 1)) / (TRAIL_LENGTH * TRAIL_LENGTH);  // Gradual transparency
            Color fadeColor = {0, 200, 255, (unsigned char)(alpha * 255)};
            DrawCircle(trail[i].x, trail[i].y, BALL_RADIUS / 4, fadeColor);
        }

        // Draw the ball
        DrawCircle(ball_x, ball_y, BALL_RADIUS, BLUE);

        // Draw rotating indicator line
        DrawLineEx({ball_x, ball_y}, {line_x, line_y}, 4, RED);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
