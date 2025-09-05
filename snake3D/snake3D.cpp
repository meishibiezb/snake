#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <windows.h>
#include <random>

#include "Shader.h"
// #include "GLFW/glfw3.h"
// #include "glad/glad.h"
// #include "glm.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

int init();

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
Direction dir = Direction::right;

GLFWwindow *window;
Shader *s;
unsigned int VBO;
unsigned int VAO;
double lastTime;

float fov = 60;
unsigned int screenWidth = 1200;
unsigned int screenHeight = 1200;
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  20.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);



int main(){

    init();

    while (!gameMode.gameEnd)
    {
        input();
        update();
        output();
    }

    cout<<"得分:"<<score<<endl;
    system("pause");

    glfwTerminate();
    return 0;
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
    double deltaTime = glfwGetTime() - lastTime;
    if (deltaTime > 0.5)
    {
        snake.dir = dir;
        snake.forward();
        if (score % 11 == 0)
        {
            generateFood();
        }
        lastTime = glfwGetTime();
        score++;
    }

    input();

    if(gameMode.gameEnd)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

GameMode::GameMode()
{
    gameEnd = false;
    system("chcp 65001");
    
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
        cameraPos = glm::vec3(cameraPos.y, cameraPos.y, cameraPos.z * 1.25f);
        cout<<"选择中等模式"<<endl;
        break;
    case 3:
        x = 32;
        y = 32;
        cameraPos = glm::vec3(cameraPos.y, cameraPos.y, cameraPos.z * 1.6f);
        cout<<"选择困难模式"<<endl;
        break;
    default:
        x = 64;
        y = 32;
        cameraPos = glm::vec3(cameraPos.y, cameraPos.y, cameraPos.z * 3.0f);
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

    cameraPos = glm::vec3((float)(x / 2), (float)(y / 2), cameraPos.z);
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
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindVertexArray(VAO);

    s->use();
    s->setInt("type",1);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm:: mat4 projection = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(fov), (float)screenWidth / (float)screenHeight, 0.1f, 500.0f);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float interval = 1.0f;

    for (int i = 0; i < gameMode.y; i++) {
        for (int j = 0; j < gameMode.x; j++) {
            Block* block = &gameMode.map[i * gameMode.x + j];
            model = glm::mat4(1.0f);
            switch (block->type)
            {
            case BlockType::wall:
                model = glm::translate(model, glm::vec3((float)j * interval, (float)(gameMode.y - i) * interval, 1.0f * interval));
                s->setInt("type",2);
                break;
            case BlockType::air:
                model = glm::translate(model, glm::vec3((float)j * interval, (float)(gameMode.y - i) * interval, 0.0f * interval));
                s->setInt("type",1);
                break;
            case BlockType::food:
                model = glm::translate(model, glm::vec3((float)j * interval, (float)(gameMode.y - i) * interval, 1.0f * interval));
                s->setInt("type",3);
                break;
            case BlockType::body:
                model = glm::translate(model, glm::vec3((float)j * interval, (float)(gameMode.y - i) * interval, 1.0f * interval));
                s->setInt("type",4);
                switch (block->dir)
                {
                case Direction::up:
                    model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                case Direction::down:
                    break;
                case Direction::left:
                    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                case Direction::right:
                    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, -1.0f));
                    break;
                
                default:
                    break;
                }
                break;
            
            default:
                break;
            }
            s->setMatrix4("model", model);
            s->setMatrix4("view", view);
            s->setMatrix4("projection", projection);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }

    glfwPollEvents();
    glfwSwapBuffers(window);
}

void input()
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && snake.dir != down)
    {
        dir = Direction::up;
        // cout<<"W"<<"    ";
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && snake.dir != up)
    {
        dir = Direction::down;
        // cout<<"S"<<"    ";
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && snake.dir != Direction::right)
    {
        dir = Direction::left;
        // cout<<"A"<<"    ";
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && snake.dir != Direction::left)
    {
        dir = Direction::right;
        // cout<<"D"<<"    ";
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    screenWidth = width;
    screenHeight = height;
}

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

int init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "snake3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    s = new Shader("./assets/shaders/VertexShader.glsl", "./assets/shaders/FragmentShader.glsl");

    glViewport(0, 0, screenWidth, screenHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int texture1;
    glGenTextures(1,&texture1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,texture1);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("./assets/textures/stone_bricks.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"failed to load texture"<<std::endl;
    }
    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1,&texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("./assets/textures/mossy_stone_bricks.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"failed to load texture"<<std::endl;
    }
    stbi_image_free(data);

    unsigned int texture3;
    glGenTextures(1,&texture3);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,texture3);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("./assets/textures/red_mushroom_block.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"failed to load texture"<<std::endl;
    }
    stbi_image_free(data);

    unsigned int texture4;
    glGenTextures(1,&texture4);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,texture4);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    data = stbi_load("./assets/textures/green_concrete.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout<<"failed to load texture"<<std::endl;
    }
    stbi_image_free(data);
    
    s->use();
    s->setInt("texture1",0);
    s->setInt("texture2",1);
    s->setInt("texture3",2);
    s->setInt("texture4",3);

    return 0;
}