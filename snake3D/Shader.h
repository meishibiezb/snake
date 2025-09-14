#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h" // 包含glad来获取所有的必须OpenGL头文件
#include "GLFW/glfw3.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class Shader
{
public:
    // 程序ID
    unsigned int ID;

    // 构造器读取并构建着色器
    Shader();
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用/激活程序
    void use()const;
    // uniform工具函数
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void setMatrix4(const std::string &name, glm::mat4) const;
};

#endif