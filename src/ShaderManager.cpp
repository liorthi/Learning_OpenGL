#include "ShaderManager.h"

/*
Loads the shader source code from a file and returns it as a string.
@param shaderPath The path to the shader source file.
@return A string containing the shader source code, or an empty string if loading fails.
*/
std::string ShaderManager::loadShader(const char *shaderPath)
{
    std::ifstream file(shaderPath);
    if (!file.is_open())
        throw std::runtime_error("Failed to open shader file: " + std::string(shaderPath));
        
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string shaderSourceStr = buffer.str();
    file.close();
    return shaderSourceStr;
}

/*
Compiles the shader source code and checks for compilation errors.
@param shaderCode The source code of the shader to compile.
@param shaderType The type of shader (e.g., GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
@return The ID of the compiled shader, or -1 if compilation fails.
*/
unsigned int ShaderManager::compileShader(const char *shaderCode, unsigned int shaderType)
{
    unsigned int shaderId;
    int success;
    char infoLog[512];
    
    shaderId = glCreateShader(shaderType);
    glShaderSource(shaderId, 1, &shaderCode, NULL);
    glCompileShader(shaderId);

    // print compile errors if any
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    };
    return shaderId;
}


ShaderManager::ShaderManager(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode = "";
    std::string fragmentCode = "";
    int success;
    char infoLog[512];

    try 
    {
        // 1. retrieve the vertex/fragment source code from filePath
        vertexCode = loadShader(vertexPath);
        fragmentCode = loadShader(fragmentPath);
    }
    catch(std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    vertex = compileShader(vShaderCode, GL_VERTEX_SHADER);
        if (vertex == -1)
            return;

    fragment = compileShader(fShaderCode, GL_FRAGMENT_SHADER);
        if (fragment == -1)
            return;
    
    // shader Program
    this->ID = glCreateProgram();
    glAttachShader(this->ID, vertex);
    glAttachShader(this->ID, fragment);
    glLinkProgram(this->ID);

    // print linking errors if any
    glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(this->ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

ShaderManager::~ShaderManager()
{
    glDeleteProgram(this->ID);
}

/*
Uses/activates the shader program.
*/
void ShaderManager::use()
{
    glUseProgram(this->ID);
}

// ===============================
// == utility uniform functions ==
// ===============================

void ShaderManager::setBool(const std::string &name, bool value) const      
{         
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int)value); 
}

void ShaderManager::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), value); 
}

void ShaderManager::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(this->ID, name.c_str()), value); 
} 
