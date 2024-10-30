/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// C++ Standard Template Library (STL)
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Our libraries
#include "ModelLoader.hpp"
#include "Camera.hpp"

// Screen Dimensions
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
SDL_Window* window = nullptr;
SDL_GLContext glContext;

bool gQuit = false;
bool wireframeMode = false;
bool modelLoaded = false;

GLuint gGraphicsPipelineShaderProgram = 0;
GLuint vao = 0, vbo = 0;
size_t vertexCount = 0;
Camera camera;

// vvvvvvvvvvvvvvvvvvv Error Handling Routines vvvvvvvvvvvvvvv
static void GLClearAllErrors() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

// Returns true if we have an error
static bool GLCheckErrorStatus(const char* function, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL Error:" << error
                  << "\tLine: " << line
                  << "\tfunction: " << function << std::endl;
        return true;
    }
    return false;
}

#define GLCheck(x) GLClearAllErrors(); x; GLCheckErrorStatus(#x, __LINE__);
// ^^^^^^^^^^^^^^^^^^^ Error Handling Routines ^^^^^^^^^^^^^^^


std::string LoadShaderAsString(const std::string& filename);
GLuint CompileShader(GLuint type, const std::string& source);
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
void InitializeProgram();
void CreateGraphicsPipeline();
void GenerateBufferData(const ModelLoader& model);
void VertexSpecification(const ModelLoader& model);
void Input();
void PreDraw();
void Draw();
void MainLoop();
void CleanUp();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./program <path_to_obj_file> ..." << std::endl;
        return -1;
    }

    std::string objPath = argv[1];
    ModelLoader model;
    if (!model.loadOBJ(objPath)) {
        std::cerr << "Failed to load model." << std::endl;
        return -1;
    }

    // 1. Setup the graphics program
    InitializeProgram();

    // 2. Setup our geometry
    VertexSpecification(model);

    // 3. Create our graphics pipeline
    CreateGraphicsPipeline();
    modelLoaded = true;

    // 4. Call the main application loop
    MainLoop();

    // 5. Call the cleanup function when our program terminates
    CleanUp();

    return 0;
}

/**
* LoadShaderAsString takes a filepath as an argument and will read line by line a file and return a string that is meant to be compiled at runtime for a vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*       LoadShaderAsString("./shaders/filepath");
*
* @param filename Path to the shader file
* @return Entire file stored as a single string 
*/
std::string LoadShaderAsString(const std::string& filename) {
    // Resulting shader program loaded as a single string
    std::string result = "";

    std::string line = "";
    std::ifstream myFile(filename.c_str());

    if (myFile.is_open()) {
        while (std::getline(myFile, line)) {
            result += line + '\n';
        }
        myFile.close();
    } else {
        std::cout << "Unable to open shader file: " << filename << std::endl;
        exit(-1);
    }

    return result;
}

/**
* CompileShader will compile any valid vertex, fragment, geometry, tesselation, or compute shader.
* e.g.
*	    Compile a vertex shader: 	CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
*       Compile a fragment shader: 	CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
*
* @param type We use the 'type' field to determine which shader we are going to compile.
* @param source : The shader source code.
* @return id of the shaderObject
*/
GLuint CompileShader(GLuint type, const std::string& source) {
    GLuint shaderObject = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shaderObject, 1, &src, nullptr);
    glCompileShader(shaderObject);

    int result;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        char* errorMessages = new char[length];
        glGetShaderInfoLog(shaderObject, length, &length, errorMessages);

        std::cout << "ERROR: Shader compilation failed!\n" << errorMessages << "\n";
        delete[] errorMessages;
        glDeleteShader(shaderObject);
        return 0;
    }

    return shaderObject;
}

/**
* Creates a graphics program object (i.e. graphics pipeline) with a Vertex Shader and a Fragment Shader
*
* @param vertexShaderSource Vertex source code as a string
* @param fragmentShaderSource Fragment shader source code as a string
* @return id of the program Object
*/
GLuint CreateShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
    GLuint programObject = glCreateProgram();

    GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    glAttachShader(programObject, myVertexShader);
    glAttachShader(programObject, myFragmentShader);
    glLinkProgram(programObject);

    glValidateProgram(programObject);

    glDetachShader(programObject, myVertexShader);
    glDetachShader(programObject, myFragmentShader);
    glDeleteShader(myVertexShader);
    glDeleteShader(myFragmentShader);

    return programObject;
}

void InitializeProgram() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL initialization failed! SDL Error: " << SDL_GetError() << "\n";
        exit(-1);
    }

    // Setup OpenGL Context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // We want to request a double buffer for smooth updating.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("OBJ Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cout << "Failed to create SDL window.\n";
        exit(-1);
    }

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context.\n";
        exit(-1);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD.\n";
        exit(-1);
    }
}

void CreateGraphicsPipeline() {
    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GenerateBufferData(const ModelLoader& model) {
    const auto& vertices = model.getVertexData();
    vertexCount = vertices.size() / 9;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void VertexSpecification(const ModelLoader& model) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);

    GenerateBufferData(model);

    // Setup vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (void*)0);
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 3));
    // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 9, (GLvoid*)(sizeof(GL_FLOAT) * 6));

    glBindVertexArray(0);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void Input() {
    static int mouseX = WINDOW_WIDTH / 2;
    static int mouseY = WINDOW_HEIGHT / 2;

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            std::cout << "Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            gQuit = true;
        }
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
            gQuit = true;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_TAB]) {
        SDL_Delay(250);
        wireframeMode = !wireframeMode;
        glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
    }
}

void PreDraw() {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!modelLoaded)
        return;

    glUseProgram(gGraphicsPipelineShaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
                                            0.1f, 100.0f);

    GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "model");
    if (u_ModelMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
    } else {
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    GLint u_ViewMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "view");
    if (u_ViewMatrixLocation >= 0) {
        glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
    } else {
        std::cout << "Could not find u_ModelMatrix, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }

    GLint u_ProjectionLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "projection");
    if (u_ProjectionLocation >= 0) {
        glUniformMatrix4fv(u_ProjectionLocation, 1, GL_FALSE, &projection[0][0]);
    } else {
        std::cout << "Could not find u_Perspective, maybe a mispelling?\n";
        exit(EXIT_FAILURE);
    }
}

void Draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);

    glUseProgram(0);
}

void MainLoop() {
    SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    while (!gQuit) {
        Input();
        PreDraw();
        Draw();
        SDL_GL_SwapWindow(window);
    }
}

void CleanUp() {
    // Delete our OpenGL Objects
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    // Delete our Graphics pipeline
    glDeleteProgram(gGraphicsPipelineShaderProgram);

    // Destroy our SDL2 Window
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}
