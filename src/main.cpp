#include <iostream>
#include <raylib.h>
#include <cmath>
using namespace std;


const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;

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
        float time;
        Ball(float x, float y) : x(x), y(y) {
            x_speed = -5;
            y_speed = -5;
            radius = 15;
            mode = LINE;
            time = 0;
        }

        void Draw(){
            DrawCircle(x, y, radius, WHITE);
        }

        void Update() {
            x += x_speed;
            y += y_speed;
            if(x + radius >= SCREEN_WIDTH || x - radius <= 0)
                x_speed *= -1;

            if(y + radius >= SCREEN_HEIGHT || y - radius <= 0)
                y_speed *= -1;
            
            if (mode == SIN)
            {
                y += 5 * cos(time);
                time++;
            }
            if (IsKeyDown(KEY_S))
                if (mode != SIN)
                {
                    mode = SIN;
                    time = 0;
                } else mode = LINE;
            
        }    
};

class Paddle {
    protected:
        void LimitMovements() {
            if (y < 0) {
                y = 0;
                speed = 5;
            } else if(y + 120 > SCREEN_HEIGHT) {
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
                acceleration += 0.1;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                volacity += speed;
                y += volacity;
                speed += acceleration;
                acceleration += 0.1;
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
            if (volacity > 0)
                ball.y_speed += acceleration - 0.3;
            else if(volacity < 0) 
                ball.y_speed -= acceleration - 0.3;
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