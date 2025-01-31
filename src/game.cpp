#include <iostream>
#include <raylib.h>
#include <cmath>
#include <queue>
using namespace std;

//extern assembly functions for update
extern "C" void update_ball(float* x, float* y, float* x_speed, float* y_speed, int radius, float* spin);
extern "C" void update_ball_movement(float* y, float* time, float range, int mode, float gravity, float* y_speed, float spin, float* angle);
extern "C" void update_mode_controls(int* mode, float* time, float* range, float* x_speed, float* y_speed);


//constants 
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;

//classes and enums
enum MovementMode {
    LINE,
    SIN,
    CURVE
};

class Ball {
    public:
        float x,y;
        int radius;
        float x_speed, y_speed;
        MovementMode mode;
        float spin, angle;
        float time;
        float gravity = 2;
        float range = 5;
        deque<Vector2> trail;  // Stores past ball positions
        Ball(float x, float y) : x(x), y(y) {
            x_speed = -5;
            y_speed = -5;
            radius = 15;
            mode = LINE;
            time = 0;
            spin = 0;
            angle = 0;
        }
        
        void Draw(){
            float angleRad = angle * (PI / 180.0f);
            float line_x = x + cos(angleRad) * radius;
            float line_y = y + sin(angleRad) * radius;
            for (int i = 0; i < trail.size(); i++) {
                float alpha = (float)((TRAIL_LENGTH - i - 1) * (TRAIL_LENGTH - i - 1)) / (TRAIL_LENGTH * TRAIL_LENGTH);  // Gradual transparency
                Color fadeColor = {0, 200, 255, (unsigned char)(alpha * 255)};
                DrawCircle(trail[i].x, trail[i].y, radius / 4, fadeColor);
            }
            DrawCircle(x, y, radius, WHITE);
            DrawLineEx({x, y}, {line_x, line_y}, 4, RED);
            cout << spin << endl;
        }
        //update function which will replace with assembly code 
        void Update() {
            update_ball(&x, &y, &x_speed, &y_speed, radius, &spin);
            int MODE = mode;
            update_ball_movement(&y, &time, range, MODE, gravity, &y_speed, spin, &angle);
            update_mode_controls(&MODE, &time, &range, &x_speed, &y_speed);
            switch (MODE)
            {
                case 0:
                    mode = LINE;
                    break;
                case 1:
                    mode = SIN;
                    break;
                default:
                    mode = CURVE;
                    break;
            }
            
            UpdateTrail();
        }  

        void UpdateTrail() {
            trail.push_front({x, y});
            if (trail.size() > TRAIL_LENGTH) 
                trail.pop_back();
        }  
};

class Paddle {
    protected:
        void LimitMovements() {
            if (y < 0) {
                y = 0;
                speed = 5;
            }
            else if(y + 120 > SCREEN_HEIGHT) {
                y = SCREEN_HEIGHT - 120;
                speed = 5;
            }
        }
    public:
        float x,y;
        float width, height;
        float speed, acceleration;
        float volacity;
        Paddle(float x, float y) : x(x), y(y) {
            speed = 5;
            width = 25;
            height = 120;
            volacity = 0;
            acceleration = 0.3;
        }

        void Draw() {
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.5, 0, WHITE);
        }

        void Update() {
            if (IsKeyDown(KEY_UP))
            {
                volacity -= speed;
                y += volacity;
                speed += acceleration;
                acceleration += 0.05;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                volacity += speed;
                y += volacity;
                speed += acceleration;
                acceleration += 0.05;
            }
            else {
                speed = 5;
                acceleration = 0.3;
                volacity = 0;
            }
            LimitMovements();
        }

        void CollisionWithBall(Ball &ball) {
            ball.x_speed *= -1;
            if (volacity > 0) {
                ball.y_speed += acceleration - 0.3;
                ball.spin -= acceleration * 10;
            }
            else if(volacity < 0) {
                ball.y_speed -= acceleration - 0.3;
                ball.spin += acceleration * 10;
            }
        }
};

class AI_Paddle : public Paddle {
    public:
        AI_Paddle(float x, float y) : Paddle(x,y) {}
        
        void CollisionWithBall(Ball &ball) {
            ball.x_speed *= -1;
        }

        void Update(Ball ball) {
            if (ball.y - 5 <= y)
            {
                y -= speed;
            } else if(ball.y + 110 >= y) {
                y += speed;
            }
            LimitMovements();
        }
};

int main () {
    Ball ball = Ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    Paddle player = Paddle(10, SCREEN_HEIGHT / 2 - 60);
    AI_Paddle ai = AI_Paddle(SCREEN_WIDTH - 35, SCREEN_HEIGHT / 2 - 60);

    cout << "Game is starting!" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        ball.Update();
        player.Update();
        ai.Update(ball);
        BeginDrawing();
            ClearBackground(BLACK);
            ball.Draw();
            player.Draw();
            ai.Draw();

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
            
        EndDrawing();
    }

    CloseWindow();
}