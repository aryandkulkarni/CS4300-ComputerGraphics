#include "SDLGraphicsProgram.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "Sphere.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                w,
                                h,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();


    // Setup our Renderer
    m_renderer = new Renderer(w,h);    
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    if(m_renderer!=nullptr){
        delete m_renderer;
    }


    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = nullptr;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

	return success;
}

// ====================== Create the planets =============
// NOTE: I will admit it is a little lazy to have these as globals,
//       we could build on our ObjectManager structure to manage the
//       each object that is inserted and automatically create a 'SceneNode'
//       for it for example. Or otherwise, build a 'SceneTree' class that
//       manages creating nodes and setting their type.
//       (e.g. A good use of the Factory Design Pattern if you'd like to
//             invetigate further--but it is beyond the scope of this assignment).

// Create the Moon
Object* sphere3;
SceneNode* Moon;
// Create the Earth
Object* sphere2;
SceneNode* Earth;
// Create the Sun
Object* sphere;
SceneNode* Sun;
// ====================== Create the planets =============

//Loops forever!
void SDLGraphicsProgram::Loop(){
    // Initialize rotation angle
    static float rotate = 0.0f;

    // ================== Initialize the planets ===============

    // Create the Sun
    Object* sunSphere = new Sphere();
    sunSphere->LoadTexture("./../../common/textures/sun.ppm");
    SceneNode* Sun = new SceneNode(sunSphere);

    // Create Planet1
    Object* planet1Sphere = new Sphere();
    planet1Sphere->LoadTexture("./../../common/textures/earth.ppm");
    SceneNode* Planet1 = new SceneNode(planet1Sphere);

    // Create Planet1 moons
    Object* planet1Moon1Sphere = new Sphere();
    planet1Moon1Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet1Moon1 = new SceneNode(planet1Moon1Sphere);

    Object* planet1Moon2Sphere = new Sphere();
    planet1Moon2Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet1Moon2 = new SceneNode(planet1Moon2Sphere);

    // Create Planet2
    Object* planet2Sphere = new Sphere();
    planet2Sphere->LoadTexture("./../../common/textures/mercury.ppm");
    SceneNode* Planet2 = new SceneNode(planet2Sphere);

    // Create Planet2 moons
    Object* planet2Moon1Sphere = new Sphere();
    planet2Moon1Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet2Moon1 = new SceneNode(planet2Moon1Sphere);

    Object* planet2Moon2Sphere = new Sphere();
    planet2Moon2Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet2Moon2 = new SceneNode(planet2Moon2Sphere);

    // Create Planet3
    Object* planet3Sphere = new Sphere();
    planet3Sphere->LoadTexture("./../../common/textures/earth.ppm");
    SceneNode* Planet3 = new SceneNode(planet3Sphere);

    // Create Planet3 moons
    Object* planet3Moon1Sphere = new Sphere();
    planet3Moon1Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet3Moon1 = new SceneNode(planet3Moon1Sphere);

    Object* planet3Moon2Sphere = new Sphere();
    planet3Moon2Sphere->LoadTexture("./../../common/textures/rock.ppm");
    SceneNode* Planet3Moon2 = new SceneNode(planet3Moon2Sphere);

    // ================== Build the scene graph hierarchy ===============

    // Render our scene starting from the sun.
    m_renderer->setRoot(Sun);

    // Make the planets children of the Sun
    Sun->AddChild(Planet1);
    Sun->AddChild(Planet2);
    Sun->AddChild(Planet3);

    // Make the moons children of the planets
    Planet1->AddChild(Planet1Moon1);
    Planet1->AddChild(Planet1Moon2);

    Planet2->AddChild(Planet2Moon1);
    Planet2->AddChild(Planet2Moon2);

    Planet3->AddChild(Planet3Moon1);
    Planet3->AddChild(Planet3Moon2);

    // Set a default position for our camera
    m_renderer->GetCamera(0)->SetCameraEyePosition(0.0f, 0.0f, 70.0f); // Moved camera back to see all planets

    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
    
    // Set the camera speed for how fast we move.
    float cameraSpeed = 5.0f;

    // While application is running
    while (!quit) {

        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User posts an event to quit
            // An example is hitting the "x" in the corner of the window.
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Handle keyboard input for the camera class
            switch (e.type) {
                // Handle keyboard presses
                case SDL_KEYDOWN:
                    switch (e.key.keysym.sym) {
                        case SDLK_LEFT:
                            m_renderer->GetCamera(0)->MoveLeft(cameraSpeed);
                            break;
                        case SDLK_RIGHT:
                            m_renderer->GetCamera(0)->MoveRight(cameraSpeed);
                            break;
                        case SDLK_UP:
                            m_renderer->GetCamera(0)->MoveForward(cameraSpeed);
                            break;
                        case SDLK_DOWN:
                            m_renderer->GetCamera(0)->MoveBackward(cameraSpeed);
                            break;
                        case SDLK_RSHIFT:
                            m_renderer->GetCamera(0)->MoveUp(cameraSpeed);
                            break;
                        case SDLK_RCTRL:
                            m_renderer->GetCamera(0)->MoveDown(cameraSpeed);
                            break;
                    }
                    break;
            }
        } // End SDL_PollEvent loop.
        // ================== Use the planets ===============
        // TODO:
        //      After the planets have been created, and the hierarchy
        //      has been made, you can transform them.
        //      Note, you'll need to setup the scene graph world/local
        //      transformation correctly first before the planets are shown.
        //      
        //      The 'Sun' for example will be the only object shown initially
        //      since the rest of the planets are children (or grandchildren)
        //      of the Sun.

        rotate += 0.005f;

        float rotateAroundSun = rotate * 0.2f;
        float rotateSunSpin = rotate * 0.1f;
        float rotatePlanet1Spin = rotate * 5.0f;
        float rotatePlanet1Moon1Orbit = rotate * 2.5f;
        float rotatePlanet1Moon2Orbit = rotate * 1.5f;

        float rotatePlanet2Orbit = rotate * 0.3f;
        float rotatePlanet2Spin = rotate * 4.0f;
        float rotatePlanet2Moon1Orbit = rotate * 2.0f;
        float rotatePlanet2Moon2Orbit = rotate * 3.0f;

        float rotatePlanet3Orbit = rotate * 0.15f;
        float rotatePlanet3Spin = rotate * 2.5f;
        float rotatePlanet3Moon1Orbit = rotate * 1.0f;
        float rotatePlanet3Moon2Orbit = rotate * 1.5f;

        Sun->GetLocalTransform().LoadIdentity();
        Sun->GetLocalTransform().Scale(3.0f, 3.0f, 3.0f);
        Sun->GetLocalTransform().Rotate(rotateSunSpin, 0.0f, 1.0f, 0.0f);

        Planet1->GetLocalTransform().LoadIdentity();
        Planet1->GetLocalTransform().Rotate(rotateAroundSun, 0.0f, 1.0f, 0.0f);
        Planet1->GetLocalTransform().Translate(12.0f, 0.0f, 0.0f);
        Planet1->GetLocalTransform().Rotate(rotatePlanet1Spin, 0.0f, 1.0f, 0.0f);
        Planet1->GetLocalTransform().Scale(1.0f, 1.0f, 1.0f);

        Planet1Moon1->GetLocalTransform().LoadIdentity();
        Planet1Moon1->GetLocalTransform().Rotate(rotatePlanet1Moon1Orbit, 0.0f, 1.0f, 0.0f);
        Planet1Moon1->GetLocalTransform().Translate(3.5f, 0.0f, 0.0f);
        Planet1Moon1->GetLocalTransform().Scale(0.3f, 0.3f, 0.3f);

        Planet1Moon2->GetLocalTransform().LoadIdentity();
        Planet1Moon2->GetLocalTransform().Rotate(rotatePlanet1Moon2Orbit, 0.0f, 1.0f, 0.0f);
        Planet1Moon2->GetLocalTransform().Translate(-4.0f, 0.0f, 0.0f);
        Planet1Moon2->GetLocalTransform().Scale(0.25f, 0.25f, 0.25f);

        Planet2->GetLocalTransform().LoadIdentity();
        Planet2->GetLocalTransform().Rotate(rotatePlanet2Orbit, 0.0f, 1.0f, 0.0f);
        Planet2->GetLocalTransform().Translate(18.0f, 0.0f, 0.0f);
        Planet2->GetLocalTransform().Rotate(rotatePlanet2Spin, 0.0f, 1.0f, 0.0f);
        Planet2->GetLocalTransform().Scale(0.8f, 0.8f, 0.8f);

        Planet2Moon1->GetLocalTransform().LoadIdentity();
        Planet2Moon1->GetLocalTransform().Rotate(rotatePlanet2Moon1Orbit, 0.0f, 1.0f, 0.0f); 
        Planet2Moon1->GetLocalTransform().Translate(2.5f, 0.0f, 0.0f);
        Planet2Moon1->GetLocalTransform().Scale(0.2f, 0.2f, 0.2f);

        Planet2Moon2->GetLocalTransform().LoadIdentity();
        Planet2Moon2->GetLocalTransform().Rotate(rotatePlanet2Moon2Orbit, 0.0f, 1.0f, 0.0f);
        Planet2Moon2->GetLocalTransform().Translate(-3.0f, 0.0f, 0.0f);
        Planet2Moon2->GetLocalTransform().Scale(0.22f, 0.22f, 0.22f);

        Planet3->GetLocalTransform().LoadIdentity();
        Planet3->GetLocalTransform().Rotate(rotatePlanet3Orbit, 0.0f, 1.0f, 0.0f);
        Planet3->GetLocalTransform().Translate(26.0f, 0.0f, 0.0f);
        Planet3->GetLocalTransform().Rotate(rotatePlanet3Spin, 0.0f, 1.0f, 0.0f);
        Planet3->GetLocalTransform().Scale(1.2f, 1.2f, 1.2f);

        Planet3Moon1->GetLocalTransform().LoadIdentity();
        Planet3Moon1->GetLocalTransform().Rotate(rotatePlanet3Moon1Orbit, 0.0f, 1.0f, 0.0f);
        Planet3Moon1->GetLocalTransform().Translate(5.0f, 0.0f, 0.0f);
        Planet3Moon1->GetLocalTransform().Scale(0.3f, 0.3f, 0.3f);

        Planet3Moon2->GetLocalTransform().LoadIdentity();
        Planet3Moon2->GetLocalTransform().Rotate(rotatePlanet3Moon2Orbit, 0.0f, 1.0f, 0.0f);
        Planet3Moon2->GetLocalTransform().Translate(-6.0f, 0.0f, 0.0f);
        Planet3Moon2->GetLocalTransform().Scale(0.35f, 0.35f, 0.35f);

        // ================== Update and render the scene ==================

        // Update our scene through our renderer
        m_renderer->Update();
        // Render our scene using our selected renderer
        m_renderer->Render();
        // Delay to slow things down just a bit!
        SDL_Delay(25); // TODO: You can change this or implement a frame
                    // independent movement method if you like.
        //Update screen of our specified window
        SDL_GL_SwapWindow(GetSDLWindow());
    }
    // Disable text input
    SDL_StopTextInput();
}




// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
