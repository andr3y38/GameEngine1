#include "GameEngine.h"
#include <iostream>
#include <assert.h>

namespace GE {
	GameEngine::GameEngine() {
		w = 1280;
		h = 720;
		windowflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	}
	GameEngine::GameEngine(int _w, int _h) {
		w = _w;
		h = _h;
		windowflags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	}
	GameEngine::~GameEngine() {

	}
	bool GameEngine::init(bool vsync) {
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "Unable to initialise SDL! SDL error: " << SDL_GetError() << std::endl;
			return false;
		}
		// Set the OpenGL version for the program
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // OpenGL 3+
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1); // OpenGL 3.1
		// Set the type of profile which is core meaning modern OpenGL
		// means no legacy features for backwards compatibility
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		// Create the window frame and features
		// Fixed size window created for OpenGL to render to
		window = SDL_CreateWindow("3D Game Engines", 50, 50, w, h, windowflags);
		
		assert(window);





		// Create the OpenGL context and link to the window object
		// Context represents OpenGL for program such as objects and drawing
		glContext = SDL_GL_CreateContext(window);
		// Check the context object was created
		assert(glContext);
		// Check we are setup with the correct GL version
		std::cout << "glGetString (GL_VERSION) returns: " << glGetString(GL_VERSION) << std::endl;

		// Initialise GLEW. GLEW solves a problem with OpenGL on windows
		// GLEW binds latest extensions that can be used
		GLenum status = glewInit();
		// Check GLEW initialised
		if (status != GLEW_OK) {
			std::cerr << "Error initialising GLEW! Error: " << glewGetErrorString(status) << std::endl;
			return false;
		}

		// Try to turn on VSync, if requested
		if (vsync) {
			if (SDL_GL_SetSwapInterval(1) != 0) {
				std::cerr << "Warning: unable to set VSync! Error: " << SDL_GetError() << std::endl;
				return false;
			}
		}

		// Create camera object
		cam = new Camera(glm::vec3(0.0f, 0.0f, 5.0f),	// cam position
			glm::vec3(0.0f, 0.0f, 0.0f),				// cam look at
			glm::vec3(0.0f, 1.0f, 0.0f),				// cam up direction
			120, w / h, 0.1f, 800.0f);					// fov, aspect ratio, near and far clip planes
		cam->setTarget(glm::vec3(0.5f, 0.0f, 0.5f));

		// Initialise the object renderers
		m = new Model();
		
		m->loadFromFile(".\\model.obj");

		if (m->getVertices() == nullptr) {
			std::cerr << "Failed to load model" << std::endl;
		}
		
		mat = new Texture(".\\space_frigate_6_color.png");

		mr = new ModelRenderer(m);
		mr->init();
		mr->setPos(0.0f, 0.0f, -20.0f);
		mr->setMaterial(mat);


		/* 
		mr = new ModelRenderer(m);
		mr->init();
		*/

		skybox = new SkyboxRenderer("front.jpg", "back.jpg",
			"right.jpg", "left.jpg",
			"top.jpg", "bottom.jpg");

		return true;
	}

	// Check if the window was closed and SDL_QUIT message was resultant
	bool GameEngine::keep_running() {
		// Need to do this to keep the event queue up to date
		SDL_PumpEvents();

		SDL_Event evt;

		// Check for quit event
		if (SDL_PeepEvents(&evt, 1, SDL_GETEVENT, SDL_QUIT, SDL_QUIT)) {
			// If user quit the program, then return false
			return false;
		}

		// If no SDL_QUIT event in event queue then keep running
		return true;
	}

	// Update method which updates the game logic
	// Used to invoke GE object update methods
	void GameEngine::update() {
		mr->setRotation(0.0f, mr->getRotY() + 2.5f, 0.0f);
	}

	void GameEngine::processInput() {
		const float camSpeed = 0.2f;
		const float	mouseSens = 0.1f;
		int mouse_x, mouse_y;
		SDL_GetMouseState(&mouse_x, &mouse_y);
		float diffx = mouse_x - cam->getOldMouseX();
		float diffy = cam->getOldMouseY() - mouse_y;

		cam->setYaw((cam->getYaw() + diffx) * mouseSens);
		cam->setPitch((cam->getPitch() + diffy) * mouseSens);

		glm::vec3 direction;
		direction.x = cos(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		direction.y = sin(glm::radians(cam->getPitch()));
		direction.z = sin(glm::radians(cam->getYaw())) * cos(glm::radians(cam->getPitch()));
		cam->setTarget(glm::normalize(direction));

		bool keyStates[4] = { 0, 0, 0, 0 };
		int UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3;

		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
						keyStates[UP] = true;
						break;
				case SDL_SCANCODE_DOWN:
						keyStates[DOWN] = true;
						break;
				case SDL_SCANCODE_LEFT:
						keyStates[LEFT] = true;
						break;
				case SDL_SCANCODE_RIGHT:
						keyStates[RIGHT] = true;
						break;
				}
			}

			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_UP:
					keyStates[UP] = false;
					break;
				case SDL_SCANCODE_DOWN:
					keyStates[DOWN] = false;
					break;
				case SDL_SCANCODE_LEFT:
					keyStates[LEFT] = false;
					break;
				case SDL_SCANCODE_RIGHT:
					keyStates[RIGHT] = false;
					break;
				}

			}

		}

		if (keyStates[UP]) {
			cam->setPos(cam->getPos() + cam->getTarget() * camSpeed);
		}

		if (keyStates[DOWN]) {
			cam->setPos(cam->getPos() - cam->getTarget() * camSpeed);
		}

		if (keyStates[LEFT]) {
			cam->setPos(cam->getPos() - glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed);
		}

		if (keyStates[RIGHT]) {
			cam->setPos(cam->getPos() + glm::normalize(glm::cross(cam->getTarget(), cam->getUpDir())) * camSpeed);
		}

		cam->updateCamMatrices();

		cam->setOldMouseX(w / 2);
		cam->setOldMouseY(h / 2);

	}
	
	// Draw method. Used to render scenes to the window frame
	// For now, just clears the background
	void GameEngine::draw() {
		glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		skybox->draw(cam);


		mr->draw(cam);

		SDL_GL_SwapWindow(window);
	}

	// Shutdown method closes OpenGL and SDL
	// When additional objects are added, ensure these are also freed safely
	void GameEngine::shutdown() {
		// Release object renderers
		mr->destroy();
		skybox->destroy();

		// Release memory associate with camera and primitive renderers
		delete skybox;
		delete mr;
		delete m;
		delete cam;
		

		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		window = nullptr;
		SDL_Quit();
	}

	// Helped to set text in the window title bar
	void GameEngine::setwindowtitle(const char* new_title) {
		SDL_SetWindowTitle(window, new_title);
	}

	// Helper function to display program information
	// Part of the namespace, but not the GameEngine class
	void display_info_message(const char* msg) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Basic Game Engine", msg, nullptr);
	}
}