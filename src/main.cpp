#include <iostream>
#include <raylib.h>
#include <cmath>
#include <queue>
using namespace std;


const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int TRAIL_LENGTH = 20;

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

        void Update() {
            x += x_speed;
            y += y_speed;
            if(x + radius >= SCREEN_WIDTH || x - radius <= 0) {
                if (x_speed >= 0)
                    spin -= 2;
                else spin += 2;
                x_speed *= -1;
            }    

            if(y + radius >= SCREEN_HEIGHT || y - radius <= 0) {
                if (x_speed >= 0)
                    spin -= 2;
                else spin += 2;
                y_speed *= -1;
                if (y + radius >= SCREEN_HEIGHT)
                    y = SCREEN_HEIGHT - radius;
                else y = radius;    
            }
            
            if (mode == SIN)
            {
                y += 5 * cos(time);
                time++;
            }
            if (mode == CURVE)
                y_speed += gravity;
            angle += spin;
            
            if (IsKeyDown(KEY_S))
                if (mode != SIN)
                {
                    mode = SIN;
                    time = 0;
                } else {
                    mode = LINE;
                    time = 0;
                }
            if (IsKeyDown(KEY_L))
                if (mode != LINE)
                {
                    mode = LINE;
                    time = 0;
                }
            if (IsKeyDown(KEY_C))
                if (mode != CURVE)
                {
                    mode = CURVE;
                    time = 0;
                } else {
                    x_speed = -5;
                    y_speed = -5;
                    mode = LINE;
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