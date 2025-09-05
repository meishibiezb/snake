#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <windows.h>
#include <random>

using namespace std;

void input();
void update();
void output();

void generateFood();

enum Direction{
    up,
    down,
    left,
    right
};
enum BlockType{
    body,
    wall,
    air,
    food
};
class Block{
public:
    int x;
    int y;
    BlockType type;
    Direction dir;
    Block* getBlock(Direction dir, vector<Block>* map);
    Block(){}
};
class Snake{
public:
    Direction dir;
    Block* head;
    Block* tail;
    void forward();
    Snake();
};
class GameMode{
public:
    bool gameEnd;
    vector<Block> map;
    int x;
    int y;
    GameMode();
};

GameMode gameMode;
Snake snake;
unsigned int score = 0;

int main(){

    while (!gameMode.gameEnd)
    {
        input();
        update();
        output();
    }

    cout<<"得分:"<<score<<endl;
    system("pause");
}

Block* Block::getBlock(Direction dir, vector<Block>* map)
{
    int index = y * gameMode.x + x;
    if (dir == Direction::left && x > 0)
    {
        return &gameMode.map[index - 1];
    }
    else if (dir == Direction::right && x < gameMode.x - 1)
    {
        return &gameMode.map[index + 1];
    }
    else if (dir == Direction::up && x > 0)
    {
        return &gameMode.map[index - gameMode.y];
    }
    else if (dir == Direction::down && y < gameMode.y - 1)
    {
        return &gameMode.map[index + gameMode.x];   
    }
    return this;
}

void Snake::forward()
{
    Block* frontBlock = head->getBlock(dir, &gameMode.map);
    if (frontBlock->type == BlockType::body || frontBlock->type == BlockType::wall)
    {
        gameMode.gameEnd = true;
        return;
    }
    head->dir = dir;
    head = head->getBlock(head->dir, &gameMode.map);
    head->dir = dir;
    score += 11;
    if (frontBlock->type != BlockType::food)
    {
        score -= 11;
        tail->type = air;
        tail = tail->getBlock(tail->dir, &gameMode.map);
    }
    head->type = body;
}

void update(){
    snake.forward();
    input();
    if (score % 11 == 0)
    {
        generateFood();
    }
    score++;
}

GameMode::GameMode()
{
    gameEnd = false;
    // system("chcp 65001");
    
    cout<<"使用WASD键操作"<<endl;
    cout<<"请输入数字选择游戏难度：1.简单 2.中等 3.困难"<<endl;
    int difficulty;
    cin>>difficulty;
    switch (difficulty)
    {
    case 1:
        x = 16;
        y = 16;
        cout<<"选择简单模式"<<endl;
        break;
    case 2:
        x = 24;
        y = 24;
        cout<<"选择中等模式"<<endl;
        break;
    case 3:
        x = 32;
        y = 32;
        cout<<"选择困难模式"<<endl;
        break;
    default:
        x = 64;
        y = 32;
        cout<<"选择特别困难模式"<<endl;
        break;
    }
    map = vector<Block>(x * y);
    for (int i = 0; i < x * y; i++)
    {
        map[i].type = air;
        map[i].x = i % x;
        map[i].y = i / x;
    }
    for (int i = 0; i < x; i++)
    {
        map[i].type = wall;
        map[(y - 1) * x + i].type = wall;
    }
    for (int i = 1; i < y; i++)
    {
        map[i * x].type = wall;
        map[i * x + (x - 1)].type = wall;
    }
    this_thread::sleep_for(chrono::milliseconds(1000));
    system("cls");
}

Snake::Snake()
{
    dir = Direction::right;
    tail = &gameMode.map[gameMode.x + 1];
    tail->type = body;
    tail->dir = dir;
    head = tail->getBlock(tail->dir, &gameMode.map);
    head->type = body;
    head->dir = dir;
}

void output(){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hConsole, pos);
    for (int i = 0; i < gameMode.y; i++) {
        for (int j = 0; j < gameMode.x; j++) {
            Block* block = &gameMode.map[i * gameMode.x + j];
            const char* str = (block->type == wall) ? "墙" :
                              (block->type == food) ? "食" :
                              (block->type == body) ? 
                                (block->dir == Direction::up)    ? "宀" :
                                (block->dir == Direction::down)  ? "灬" :
                                (block->dir == Direction::left)  ? "《" :
                                (block->dir == Direction::right) ? "》" :
                                "蛇" : "十";
            WriteConsoleA(hConsole, str, strlen(str), NULL, NULL);
        }
        WriteConsoleA(hConsole, "\n", 1, NULL, NULL);
    }
    // cout << "\033[" + to_string(gameMode.y) + "A";
    // cout << "\033[" + to_string(gameMode.x) + "D";
}

void input()
{
    for (int i = 0; i < 25; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(10));
        if (GetAsyncKeyState('W') & 0x8000 && snake.dir != down)
        {
            snake.dir = Direction::up;
        }
        if (GetAsyncKeyState('S') & 0x8000 && snake.dir != up)
        {
            snake.dir = Direction::down;
        }
        if (GetAsyncKeyState('A') & 0x8000 && snake.dir != Direction::right)
        {
            snake.dir = Direction::left;
        }
        if (GetAsyncKeyState('D') & 0x8000 && snake.dir != Direction::left)
        {
            snake.dir = Direction::right;
        }
    }
}

void generateFood()
{
    static mt19937 generator(chrono::system_clock::now().time_since_epoch().count());
    auto map = &gameMode.map;
    uniform_int_distribution<> int_dist_x(1, gameMode.x - 2);
    uniform_int_distribution<> int_dist_y(1, gameMode.y - 2);
    bool generated = false;
    while (!generated)
    {
        int random_x = int_dist_x(generator);
        int random_y = int_dist_y(generator);
        auto block = &(*map)[random_y * gameMode.x + random_x];
        if (block->type == air)
        {
            block->type = food;
            generated = true;
        }
    }
}