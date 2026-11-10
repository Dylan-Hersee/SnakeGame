#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

Color green = {173, 204, 96, 255};
Color darkGreen = {43, 41, 24, 255};

int cellsize = 30;
int cellCount = 20;
int offset = 45;


double updateTime = 0;



bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for(unsigned int i = 0; i < deque.size(); i++){
        if(Vector2Equals(element, deque[i])){
            return true;
        }
    }
    return false;
}

bool eventTriggered(double interval){
    double currentTime = GetTime();
    if(currentTime - updateTime >= interval){
        updateTime = currentTime;
        return true;
    }
    return false;
}

class Food {

    public:
    Vector2 position;
    Texture2D texture;


    Food(deque<Vector2> snakebody){
        Image image = LoadImage("graphics/food.png");
        texture = LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPosition(snakebody);
    }

    ~Food(){
        UnloadTexture(texture);

    }

    void Draw(){
        DrawTexture(texture, offset + position.x * cellsize, offset + position.y * cellsize, WHITE);
    }

    Vector2 GenerateRandomCell(){
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);
        return Vector2{x, y};
    }

    Vector2 GenerateRandomPosition(deque<Vector2> snakeBody){
        float x = GetRandomValue(0, cellCount -1);
        float y = GetRandomValue(0, cellCount -1);
        Vector2 randomPosition = Vector2{x, y};
        while(ElementInDeque(randomPosition, snakeBody)){
            randomPosition = GenerateRandomCell();
        }
       
        return randomPosition;
    }

};

class Snake {
    public:
        deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Vector2 direction = {1, 0};
        bool addSegment = false;

        void Draw(){

            for(unsigned int i = 0; i< body.size(); i++){
                float x = body[i].x;
                float y = body[i].y;
                Rectangle rec = Rectangle {offset + x * cellsize, offset + y * cellsize, (float)cellsize, (float)cellsize};
                DrawRectangleRounded(rec, 0.5, 6, darkGreen);
            }

        }

        void Move(){
            body.push_front(Vector2Add(body[0], direction));
            if(addSegment == true){
                addSegment = false;
                
            }
            else{
                body.pop_back();
            }
            
        }

        void Reset(){
            body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
            direction = {1, 0};
        }

};

class Game{
    public:
    Snake snake;
    Food food = Food(snake.body);
    int score = 0;
    Sound eatSound;
    Sound wallHit;
    Sound move;
    double speed = 0.40;
    int update;
    Game(){
        InitAudioDevice();
        eatSound = LoadSound("sound/music_food.mp3");
        wallHit = LoadSound("sound/music_gameover.mp3");
        

    }
    ~Game(){
        UnloadSound(eatSound);
        UnloadSound(wallHit);

        CloseAudioDevice();
    }

    bool running = true;

    void Draw(){
        food.Draw();
        snake.Draw();
    }
        void Move(){

            if(running){
            snake.Move();
            CheckCollides();
            CheckCollidesEdge();
            CollideswithBody();
            }
        }   

        void CheckCollides(){
            if(Vector2Equals(snake.body[0], food.position)){
                food.position = food.GenerateRandomPosition(snake.body);
                snake.addSegment = true;
                update++;

                if(update == 4){
                    speed = max(0.02, speed - 0.06);
                    update = 0;
                }
                score++;
                PlaySound(eatSound);
            }
            
        }

        void CheckCollidesEdge(){
            if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                GameOver();
                PlaySound(wallHit);
            }
            if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                GameOver();
            }
        }

        void CollideswithBody(){
            deque<Vector2> headlessBody = snake.body;
            headlessBody.pop_front();
            if(ElementInDeque(snake.body[0],headlessBody)){
                GameOver();
               
            }
        }

        void GameOver(){
            snake.Reset();
            food.position = food.GenerateRandomPosition(snake.body);
            running = false;
            score = 0;
            update = 0;
            speed = 0.40;
             PlaySound(wallHit);
        }
    };

int main() {
    cout << "Starting Game" << endl;

    InitWindow(2 * offset + cellsize * cellCount, 2 * offset + cellsize * cellCount, "Snake Game");
    SetTargetFPS(60);

    Game game = Game();

    while (!WindowShouldClose()) {
        BeginDrawing();


        if(eventTriggered(game.speed)){
            game.Move();
        }

        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
            game.running = true;
        }

        if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1){
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1){
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
            game.running = true;
        }
        
        //Drawing
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset -5, (float)cellsize * cellCount+10, (float)cellsize * cellCount + 10}, 5, darkGreen);
        DrawText("Snake Game", offset -5, 5, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset -5, offset + cellsize * cellCount + 10, 40, darkGreen);
        game.Draw();
        EndDrawing();
        
    }

    CloseWindow();
    return 0;
}