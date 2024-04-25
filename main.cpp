#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

using namespace std;

//Global Variables

Color blue = {0, 153, 204, 255};
Color red = {153, 0, 0, 255};
Color black = {0, 0, 0, 255};

int cellSize = 30;
int cellCount = 25;
int offset = 75;


double lastUpdateTime = 0;

bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if(currentTime - lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> deque)
{
    for(unsigned int i = 0; i < deque.size(); i++)
    {
        if(Vector2Equals(deque[i], element))
        {
            return true;
        }
    }
    return false;
}

class Snake
{
    public:

        deque<Vector2> body = {Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9}};
        Vector2 direction = {1, 0};
        bool addSegment = false;

        void Draw()
        {
            //The compiler was complaining about i being a int
            for(int i = 0; (float)i < body.size(); i++)
            {
                float x = body[i].x;
                float y = body[i].y;
                Rectangle segment = Rectangle{offset + x*cellSize, offset + y*cellSize, (float)cellSize, (float)cellSize};
                DrawRectangleRounded(segment, 0.5, 6, black);
            }
        }

        void Update()
        {

            body.push_front(Vector2Add(body[0], direction));
            if(addSegment == true)
            {
                addSegment = false;
            }
            else
            {
                body.pop_back();
            }       

        }

        void Reset()
        {
            body = {Vector2{6, 9},Vector2{5, 9},Vector2{4, 9}};
            direction = {1, 0};
        }
};

//Food Class
class Food
{
    public:
        Vector2 position;
        Texture2D texture;

        //Constructor
        Food(deque<Vector2> snakeBody)
        {

            Image image = LoadImage("Graphics/BetterFood.png");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GenerateRandomPos(snakeBody);

        }
        
        //Decontructor
        ~Food()
        {
           UnloadTexture(texture); 
        }

        void Draw()
        {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize,WHITE);
        }

        Vector2 GenerateRandomCell()
        {
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);

            return Vector2{x, y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
        {
         
           Vector2 position = GenerateRandomCell();
           while(ElementInDeque(position, snakeBody))
           {
                GenerateRandomCell();
           }
           return position;
        }

};

class Game
{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;
        int score = 0;

        void Draw()
        {
            food.Draw();
            snake.Draw();
        }

        void Update()
        {
            if(running)
            {
                snake.Update();
                CheckCollisionWithFood();
                CheckCollisionWithEdges();
                CheckCollisionWithTail();
            }
        }

        void CheckCollisionWithFood()
        {
            if(Vector2Equals(snake.body[0], food.position))
            {
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score =  score + 10;
            }

        }

        void CheckCollisionWithEdges()
        {
           if(snake.body[0].x == cellCount || snake.body[0].x == -1)
           {
                GameOver();   
           }
           if(snake.body[0].y == cellCount || snake.body[0].y == -1)
           {
                GameOver();   
           }
        }

        void CheckCollisionWithTail()
        {
            deque <Vector2> headlessBody =  snake.body;
            headlessBody.pop_front();
            if(ElementInDeque(snake.body[0], headlessBody))
            {
                GameOver();
            }
        }

        void GameOver()
        {
            snake.Reset();
            food.position = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
            cout << "GAME OVER!" << endl;
        }
};

int main () 
{
    cout << "Starting Game..." << endl;
    InitWindow(2*offset + cellSize*cellCount, 2*offset + cellSize*cellCount, "SnakeGame");
    SetTargetFPS(60);

    Game game = Game();

    while(WindowShouldClose() == false)
    {
        BeginDrawing();

        if(eventTriggered(0.25))
        {
            game.Update();
        }

        //Movement
        if(IsKeyPressed(KEY_UP) && game.snake.direction.y != 1)
        {
            game.snake.direction = {0, -1};
            game.running = true;
        }
         if(IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1)
        {
            game.snake.direction = {0, 1};
            game.running = true;
        }
         if(IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1)
        {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
         if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1)
        {
            game.snake.direction = {1, 0};
            game.running = true;
        }
        

        ClearBackground(blue);
        DrawRectangleLinesEx(Rectangle{(float)offset-5, (float)offset-5, (float)cellSize*cellCount+10, (float)cellSize*cellCount+10}, 5, black);
        DrawText("~ Retro Snake ~", offset-5, 20, 40, black);
        DrawText(TextFormat("Score: %i", game.score), offset-5, offset + cellSize*cellCount + 10, 40, black);
        game.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}