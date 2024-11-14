/* Compilation on Linux: 
 g++ -std=c++17 ./src/*.cpp -o prog -I ./include/ -I./../common/thirdparty/ -lSDL2 -ldl
*/

// Third Party Libraries
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
GLuint vao = 0, vbo = 0, ebo = 0;
size_t vertexCount = 0;
size_t indexCount = 0;
Camera camera;

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float constant;
    float linear;
    float quadratic;
};

PointLight pointLight;

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

Material material;


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
        std::cerr << "Unable to open shader file: " << filename << std::endl;
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

    SDL_SetRelativeMouseMode(SDL_TRUE);

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
        std::cerr << "Failed to create SDL window.\n";
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

    pointLight.position = glm::vec3(0.0f, 0.0f, 2.0f);
    pointLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09f;
    pointLight.quadratic = 0.032f;

    material.ambient = glm::vec3(0.1f, 0.2f, 0.25f);
    material.diffuse = glm::vec3(0.6f, 0.6f, 0.6f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 35.0f;
}

void CreateGraphicsPipeline() {
    std::string vertexShaderSource = LoadShaderAsString("./shaders/vert.glsl");
    std::string fragmentShaderSource = LoadShaderAsString("./shaders/frag.glsl");

    gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
}

void GenerateBufferData(const ModelLoader& model) {
    const auto& vertices = model.getVertices();
    vertexCount = vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    const auto& indices = model.getIndices();
    indexCount = indices.size();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

}

void VertexSpecification(const ModelLoader& model) {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    GenerateBufferData(model);

    // Setup vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    // // Color
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)(sizeof(GL_FLOAT) * 3));
    // // Normal
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)(sizeof(GL_FLOAT) * 6));

    glBindVertexArray(0);

    // Disable vertex attributes
    glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);
    // glDisableVertexAttribArray(2);
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
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) {
                std::cout << "ESC: Goodbye! (Leaving MainApplicationLoop())" << std::endl;
                gQuit = true;
            }
            if (e.key.keysym.sym == SDLK_TAB) {
                wireframeMode = !wireframeMode;
                glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL);
            }
        }
    }

    const Uint8* state = SDL_GetKeyboardState(NULL);
    float cameraSpeed = 0.05f;

    if (state[SDL_SCANCODE_W]) {
        camera.MoveForward(cameraSpeed);
    }
    if (state[SDL_SCANCODE_S]) {
        camera.MoveBackward(cameraSpeed);
    }
    if (state[SDL_SCANCODE_A]) {
        camera.MoveLeft(cameraSpeed);
    }
    if (state[SDL_SCANCODE_D]) {
        camera.MoveRight(cameraSpeed);
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
    glm::mat4 view = camera.GetViewMatrix();
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

    // float time = SDL_GetTicks() / 1000.0f;
    // float radius = 5.0f;
    // pointLight.position.x = radius * cos(time);
    // pointLight.position.z = radius * sin(time);
    // pointLight.position.y = 2.0f;

    // GLint viewPosLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "viewPos");
    // glUniform3fv(viewPosLocation, 1, glm::value_ptr(camera.GetPosition()));

    // GLint lightPosLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "lightPos");
    // glUniform3fv(lightPosLocation, 1, glm::value_ptr(pointLight.position));

    // GLint lightColorLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "lightColor");
    // glUniform3fv(lightColorLocation, 1, glm::value_ptr(pointLight.color));

    // glUniform1f(glGetUniformLocation(gGraphicsPipelineShaderProgram, "lightConstant"), pointLight.constant);
    // glUniform1f(glGetUniformLocation(gGraphicsPipelineShaderProgram, "lightLinear"), pointLight.linear);
    // glUniform1f(glGetUniformLocation(gGraphicsPipelineShaderProgram, "lightQuadratic"), pointLight.quadratic);

    // GLint materialAmbientLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "material.ambient");
    // GLint materialDiffuseLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "material.diffuse");
    // GLint materialSpecularLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "material.specular");
    // GLint materialShininessLoc = glGetUniformLocation(gGraphicsPipelineShaderProgram, "material.shininess");

    // glUniform3fv(materialAmbientLoc, 1, glm::value_ptr(material.ambient));
    // glUniform3fv(materialDiffuseLoc, 1, glm::value_ptr(material.diffuse));
    // glUniform3fv(materialSpecularLoc, 1, glm::value_ptr(material.specular));
    // glUniform1f(materialShininessLoc, material.shininess);
}

void Draw() {
    glBindVertexArray(vao);
    //glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
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
    glDeleteBuffers(1, &ebo);

    // Delete our Graphics pipeline
    glDeleteProgram(gGraphicsPipelineShaderProgram);

    // Destroy our SDL2 Window
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    window = nullptr;

    // Quit SDL subsystems
    SDL_Quit();
}
