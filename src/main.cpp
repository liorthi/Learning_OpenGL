/*
TODO:
1. Make the shaders as files and load them at runtime instead of hardcoding them in the source code.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

GLFWwindow* initWindow(int width, int height, const char* title);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
std::string loadShaderSource(std::string filePath);
int createShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath);
void setUnionValues(int shaderProgram);

// Functions for generating vertices and indices for a circle
std::vector<float> generateVertices(int numSegments, float radius = 0.5f);
std::vector<unsigned int> generateIndices(int numSegments);

#define VERTEX_SHADER_PATH "vertexShader.GLSL"
#define FRAGMENT_SHADER_PATH "fragmentShader.GLSL"

int main() 
{


    int numberOfPoints = 3; 

    GLFWwindow* window = initWindow(800, 600, "Planatery System 2D");
    if (window == nullptr)
        return -1;

    int shaderProgram = createShaderProgram(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    if (shaderProgram == -1)
        return -1;
    
    std::vector<float> vertices = generateVertices(numberOfPoints, 0.5f);
    std::vector<unsigned int> indices = generateIndices(numberOfPoints);
    
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); // VAO - Vertex Array Object: stores the vertex attribute configuration and which VBO to use
    glGenBuffers(1, &VBO);      // VBO - Vertex Buffer Object: stores the actual vertex data in GPU memory
    glGenBuffers(1, &EBO);      // EBO - Element Buffer Object: stores the indices for indexed drawing, allowing reuse of vertex data and reducing redundancy
    
    glBindVertexArray(VAO);  // bind VAO 
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind VBO
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // bind EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); 

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // unbind when done

    // render loop
    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // use the shader program
        glUseProgram(shaderProgram);

        // update the uniform color
        setUnionValues(shaderProgram);
        
        // draw call
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    
    glfwTerminate();
    return 0;
}

/*
Initializes GLFW, creates a window, and sets up OpenGL context. 
It also loads OpenGL function pointers using GLAD. If any step fails, it prints an error message and returns nullptr.
@param width The width of the window to create.
@param height The height of the window to create.
@param title The title of the window to create.
@return A pointer to the created GLFWwindow, or nullptr if initialization fails.
*/
GLFWwindow *initWindow(int width, int height, const char *title)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}

/*
The callback function that gets called whenever the window is resized. 
It adjusts the OpenGL viewport to match the new window dimensions, ensuring that the rendered content scales correctly with the window size.
@param window The GLFWwindow that was resized.
@param width The new width of the window.
@param height The new height of the window.
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*
Processes user input (e.g., keyboard input).
@param window The GLFWwindow to check for input events. 
*/
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/*
Loads the shader source code from a file and returns it as a null-terminated string.
@param filePath The path to the shader source file.
@return A pointer to a null-terminated string containing the shader source code, or nullptr if loading fails. The caller is responsible for freeing the allocated memory.
*/
std::string loadShaderSource(std::string filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cout << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string shaderSourceStr = buffer.str();
    file.close();
    return shaderSourceStr;
}

/*
Creates a shader program from vertex and fragment shader sources.
@param vertexShaderSource The source code for the vertex shader.
@param fragmentShaderSource The source code for the fragment shader.
@return The ID of the created shader program, or -1 if creation fails.
*/
int createShaderProgram(std::string vertexShaderPath, std::string fragmentShaderPath)
{
    // load shader sources from files
    std::string vertexShaderCode = loadShaderSource(vertexShaderPath);
    std::string fragmentShaderCode = loadShaderSource(fragmentShaderPath);

    if (vertexShaderCode.empty() || fragmentShaderCode.empty())
    {
        std::cout << "Failed to load shader sources." << std::endl;
        return -1;
    }
    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* fragmentShaderSource = fragmentShaderCode.c_str();

    // ===================
    // == vertex shader ==
    // ===================
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    // =====================
    // == fragment shader ==
    // =====================
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    // ====================
    // == shader program ==
    // ====================
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/*
Sets the uniform values for the shader program. 
This function can be used to set any uniform variables defined in the shader, such as colors, transformation matrices, etc.
@param shaderProgram The ID of the shader program for which to set the uniform values.
*/
void setUnionValues(int shaderProgram)
{
    // for now, we only have one uniform variable (ourColor) in the fragment shader
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
}


/*
Generates the vertices for a circle with the specified number of segments and radius.
@param numSegments The number of segments to approximate the circle (more segments = smoother circle).
@param radius The radius of the circle.
@return A vector of floats containing the vertex positions for the circle. The first three values are the center vertex (0, 0, 0), followed by the vertices around the circumference.
*/
std::vector<float> generateVertices(int numSegments, float radius)
{
    // std::vector<float> vertices;
    // float angleStep = 2.0f * M_PI / numSegments;
    // vertices.push_back(0.0f); // center x
    // vertices.push_back(0.0f); // center y
    // vertices.push_back(0.0f); // center z

    // for (int i = 0; i < numSegments; i++)
    // {
    //     vertices.push_back(radius * cos(angleStep * i)); // x
    //     vertices.push_back(radius * sin(angleStep * i) * 1.25); // y
    //     vertices.push_back(0.0f); // z
    // }
    // return vertices;

    return {
    // positions          // colors
    0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   // bottom left
    0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f    // top 
    };
}

/*
Generates the indices for drawing a circle using triangle fan mode.
@param numSegments The number of segments used to approximate the circle.
@return A vector of unsigned integers containing the indices for drawing the circle.
*/
std::vector<unsigned int> generateIndices(int numSegments)
{
    // std::vector<unsigned int> indices;
    
    // for (int i = 1; i <= numSegments; i++)
    // {
    //     indices.push_back(0); // center vertex
    //     indices.push_back(i); // current vertex
    //     indices.push_back(i % numSegments + 1); // next vertex (wrap around)
    // }
    // return indices;

    return {0, 1, 2};
}