//C++ Standard Template
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>

//Third Party
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <SDL/SDL.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
//#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

//GLOBAL VARIABLES
int gScreenWidth = 1000;
int gScreenHeight = 1000;
SDL_Window*		gGraphicsApplicationWindow = nullptr;
SDL_GLContext	gOpenGLContext = nullptr;

bool gQuit = false; //If true, quit


//VAO (stores attributes)
GLuint gVertexArrayObject = 0;

//VBO (stores data) - accessed by VAO
GLuint gVertexBufferObject = 0;

//Index Buffer Object (IBO)
GLuint gIndexBufferObject = 0;

//Program object for shaders
GLuint gGraphicsPipelineShaderProgram = 0;

//Movement variables for Quads
float g_uOffset = 0.0f;
float g_uOffsetQuad1 = -0.7f;
float g_uOffsetQuad5 = 0.0f;
float g_uOffsetQuad6 = 0.0f;
float g_uOffsetQuad7 = 0.0f;

//Gravity Variables
float g_vOffsetQuad1 = -0.75f;
float g_vOffsetQuad5 = 0.0f;
float g_vOffsetQuad6 = 0.0f;
float g_vOffsetQuad7 = 0.0f;
float gGravity = -0.00009999f;
bool isCollide = false;
bool isDivide = false;
bool canJump = false;

//Collision Struct
struct Collider {
	glm::vec2 position;
	glm::vec2 size;
};

//Vector to store Colliders
std::vector<Collider> colliders;


//Defining Colliders
Collider quad1Collider = { glm::vec2(0.09f, -0.09f), glm::vec2(0.09f, -0.11f) }; //Character
Collider quad2Collider = { glm::vec2(-0.9f, -0.9f), glm::vec2(1.8f, 0.19f) }; // Floor
Collider quad3Collider = { glm::vec2(-0.9f, -0.8f), glm::vec2(0.19f, 1.8f) }; // Left Wall
Collider quad4Collider = { glm::vec2(0.8f, -0.8f), glm::vec2(0.09f, 1.8f) };  // Right Wall
Collider quad5Collider = { glm::vec2(-0.02f, 0.2f), glm::vec2(0.02f, 1.5f) };  // Middle Divider
Collider quad6Collider;
Collider quad7Collider;




std::string LoadShaderAsString(const std::string& filename) {

	//shader program loaded as single string. Dont want to compile everytime
	std::string result = ""; 

	std::string line = "";
	std::ifstream myFile(filename.c_str());

	if (myFile.is_open()) {
		while (std::getline(myFile, line)) {
			result += line + '\n';
		}
		myFile.close();
	}

	return result;
}

GLuint CompileShader(GLuint type, const std::string& source) {
	GLuint shaderObject{};

	//for error checking
	if (type == GL_VERTEX_SHADER) {
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (type == GL_FRAGMENT_SHADER) {
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}

	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	return shaderObject;
}

GLuint CreateShaderProgram(const std::string& vertexshadersource, const std::string& fragmentshadersource) {
	GLuint programObject = glCreateProgram();

	GLuint myVertexShader = CompileShader(GL_VERTEX_SHADER, vertexshadersource);
	GLuint myFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentshadersource);

	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	//validate program
	glValidateProgram(programObject);

	return programObject;
}

void CreateGraphicsPipeline() {

	std::string vertexShaderSource = LoadShaderAsString("./vert.glsl");
	std::string fragmentShaderSource = LoadShaderAsString("./frag.glsl");

	gGraphicsPipelineShaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

}

void GetOpenGLVersionInfo() {
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void VertexSpecification(std::vector<GLfloat> &vertexData) {

	//Lives on the CPU
	vertexData = 
	{
		 //x      y      z
		 // Quad 1 (Character)
		 //0 - Vertex
		-0.09f,-0.09f, 0.0f,    //Left vertex
		 1.0f, 0.0f, 0.0f,     //Color
		 //1 - Vertex
		 0.09f, -0.09f, 0.0f,    //Right vertex 
		 0.0f, 1.0f, 0.0f,     //Color
		 //2 - Vertex
		-0.09f, 0.09f, 0.0f,     //Top left vertex 
		 0.0f, 0.0f, 1.0f,     //Color
		 //3 - Vertex
		 0.09f, 0.09f, 0.0f,	//Top right vertex
		 0.0f, 0.0f, 1.0f,     //Color

		 //Quad 2 (Floor)
		//0 - Vertex
		-0.9f, -0.9f, 0.0f,    // Left vertex
		 0.0f, 0.0f, 0.0f,     // Color
		 //1 - Vertex
		 0.9f, -0.9f, 0.0f,    // Right vertex 
		 0.0f, 0.0f, 0.0f,     // Color
		 //2 - Vertex
		-0.9f,  -0.8f, 0.0f,    // Top left vertex 
		 0.0f, 0.0f, 0.0f,     // Color
		 //3 - Vertex
		 0.9f,  -0.8f, 0.0f,    // Top right vertex 
		 0.0f, 0.0f, 0.0f,     // Color

		 //Quad 3 (Left Wall)
		//0 - Vertex
		-0.9f, -0.8f, 0.0f,    // Left vertex
		 0.0f, 0.0f, 0.0f,     // Color
		 //1 - Vertex
		-0.8f, -0.8f, 0.0f,    // Right vertex 
		 0.0f, 0.0f, 0.0f,     // Color
		 //2 - Vertex
		-0.9f,  1.0f, 0.0f,    // Top left vertex 
		 0.0f, 0.0f, 0.0f,     // Color
		 //3 - Vertex
		 -0.8f,  1.0f, 0.0f,    // Top right vertex 
		 0.0f, 0.0f, 0.0f,     // Color

		//Quad 4 (Right Wall)
	   //0 - Vertex
		0.8f, -0.8f, 0.0f,    // Left vertex
		0.0f, 0.0f, 0.0f,     // Color
		//1 - Vertex
	    0.9f, -0.8f, 0.0f,    // Right vertex 
		0.0f, 0.0f, 0.0f,     // Color
		//2 - Vertex
	    0.8f,  1.0f, 0.0f,    // Top left vertex 
		0.0f, 0.0f, 0.0f,     // Color
		//3 - Vertex
		0.9f,  1.0f, 0.0f,    // Top right vertex 
		0.0f, 0.0f, 0.0f,     // Color

		//Quad 5 (Middle Divider)
	   //0 - Vertex
		-0.02f, 0.2f, 0.0f,    // Left vertex
		0.0f, 0.0f, 0.0f,     // Color
		//1 - Vertex
		0.02f, 0.2f, 0.0f,    // Right vertex 
		0.0f, 0.0f, 0.0f,     // Color
		//2 - Vertex
		-0.02f,  1.5f, 0.0f,    // Top left vertex 
		0.0f, 0.0f, 0.0f,     // Color
		//3 - Vertex
		0.02f,  1.5f, 0.0f,    // Top right vertex 
		0.0f, 0.0f, 0.0f,     // Color

		//Quad 6 (Left Player)
	   //0 - Vertex
		0.0f, 0.0f, 0.0f,    // Left vertex 20 21
		1.0f, 0.0f, 0.0f,     // Color
		//1 - Vertex
		0.0f, 0.0f, 0.0f,    // Right vertex 26 27
		0.0f, 1.0f, 0.0f,     // Color
		//2 - Vertex
		0.0f,  0.0f, 0.0f,    // Top left vertex  32 33
		0.0f, 0.0f, 1.0f,     // Color
		//3 - Vertex
		0.0f,  0.0f, 0.0f,    // Top right vertex  38 39
		0.0f, 0.0f, 1.0f,    // Color

		//Quad 7 (Right Player)
	   //0 - Vertex
		0.0f, 0.0f, 0.0f,    // Left vertex 144 145
		1.0f, 0.0f, 0.0f,     // Color
		//1 - Vertex
		0.0f, 0.0f, 0.0f,    // Right vertex 150 151
		0.0f, 1.0f, 0.0f,     // Color
		//2 - Vertex
		0.0f,  0.0f, 0.0f,    // Top left vertex  156 157
		0.0f, 0.0f, 1.0f,     // Color
		//3 - Vertex
		0.0f,  0.0f, 0.0f,    // Top right vertex 162 163
		0.0f, 0.0f, 1.0f    // Color


			 		
	};

	//Start setting things up on the GPU
	glGenVertexArrays(1, &gVertexArrayObject);
	//select the array
	glBindVertexArray(gVertexArrayObject);

	//Start generating VBO
	glGenBuffers(1, &gVertexBufferObject);
	//select the buffer
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(GLfloat),
		vertexData.data(),
		GL_DYNAMIC_DRAW); //Using dynamic draw because data will change 


	const std::vector <GLuint> indexBufferData{ 
		//Quad 1       Highest Indicie: 27
		2, 0, 1, 3, 2, 1,
		//Quad 2
		4, 5, 6, 6, 5, 7,
		//Quad 3
		10, 8, 9, 11, 10, 9,
		//Quad 4
		14, 12, 13, 15, 14, 13,
		//Quad 5
		18, 16, 17, 19, 18, 17,
		//Quad 6
		22, 20, 21, 23, 22, 21,
		//Quad 7
		26, 24, 25, 27, 26, 25
		
	};

	//Set up the Index Buffer Object (IBO i.e. EBO)
	glGenBuffers(1, &gIndexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIndexBufferObject);
	//Populate our Index Buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indexBufferData.size() * sizeof(GLuint),
		indexBufferData.data(), GL_DYNAMIC_DRAW);
				

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GL_FLOAT)*6,
		(void*)0
		);

	
    //Now link to VAO
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
		3, //R,G,B
		GL_FLOAT,
		GL_FALSE,
		sizeof(GL_FLOAT)*6,
		(GLvoid*)(sizeof(GL_FLOAT)*3)
		);
	 
  //Clean up
  glBindVertexArray(0);
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}





void InitializeProgram() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize video subsystem" << std::endl;
		exit(1);
	}

	//ATTRIBUTES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//Create window
	gGraphicsApplicationWindow = SDL_CreateWindow("Game Window",
		4, 4, 
		gScreenWidth, gScreenHeight,
	    SDL_WINDOW_OPENGL);

	if (gGraphicsApplicationWindow == nullptr) {
		std::cout << "SDL_Window was not able to be created" << std::endl;
		exit(1);
	}

	//create OpenGL context
	gOpenGLContext = SDL_GL_CreateContext(gGraphicsApplicationWindow);

	if (gOpenGLContext == nullptr) {
		std::cout << "OpenGL context not available\n";
		exit(1);
	}

	// Initialize the Glad Library(loading up all the OpenGL functions and getting their address) 
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress)) {
		std::cout << "GLAD was not initialized" << std::endl;
		exit(1);
	}

	GetOpenGLVersionInfo();
}

void Input(std::vector<GLfloat> vertexData) {
	SDL_Event e;

while (SDL_PollEvent(&e) != 0) {
	if (e.type == SDL_QUIT) {
		std::cout << "Goodbye!" << std::endl;
		gQuit = true;

	}

}

//Retrieve keyboard state
const Uint8* state = SDL_GetKeyboardState(NULL);
float stepSize = 0.0003f; //speed
float jumpSize = 0.2f; //jump height
float jumpSize2 = 1.0f;
float maxVerticalVelocity = 1.0f; // Maximum upward velocity
//const float maxJumpHeight = 0.01f;
Uint64 currentTime = SDL_GetTicks();
Uint64 lastTime = currentTime;
//bool canJump = false;

int i = 0;
//g_vOffsetQuad5 += gGravity;

// Temporary variables to store the new position of the moving object
float new_uOffsetQuad1 = g_uOffsetQuad1;
float new_vOffsetQuad1 = g_vOffsetQuad1;
float new_uOffsetQuad5 = g_uOffsetQuad5;
float new_vOffsetQuad5 = g_vOffsetQuad5;
float new_uOffsetQuad6 = g_uOffsetQuad6;
float new_vOffsetQuad6 = g_vOffsetQuad6;
float new_uOffsetQuad7 = g_uOffsetQuad7;
float new_vOffsetQuad7 = g_vOffsetQuad7;



if (state[SDL_SCANCODE_RIGHT]) {
	new_uOffsetQuad1 += stepSize;
	//std::cout << "g_uOffset: " << new_uOffsetQuad1 << std::endl;
}


if (state[SDL_SCANCODE_LEFT]) {
	new_uOffsetQuad1 -= stepSize;
	//std::cout << "g_uOffset: " << new_uOffsetQuad1 << std::endl;
}
if (state[SDL_SCANCODE_UP] && isCollide) {

	//new_vOffsetQuad1 -= gGravity;
	new_vOffsetQuad1 += jumpSize;
	new_vOffsetQuad1 += gGravity;
	isCollide = false;


	//std::cout << "g_vOffset: " << new_vOffsetQuad1 << std::endl;
}

if ((state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_LEFT]) && state[SDL_SCANCODE_UP] && isCollide) {
	// Assuming horizontal velocity is half of the horizontal step size
	float horizontalVelocity = (state[SDL_SCANCODE_RIGHT] ? stepSize : -stepSize) * 0.5f;
	new_vOffsetQuad1 += horizontalVelocity;
	isCollide = false;
}



//std::cout << "g_vOffsetQuad1: " << new_vOffsetQuad1 << std::endl;

// Update collider position to match the new object position
quad1Collider.position.x = new_uOffsetQuad1;
quad1Collider.position.y = new_vOffsetQuad1;
quad5Collider.position.x = new_uOffsetQuad5;
quad5Collider.position.y = new_vOffsetQuad5;
//quad6Collider.position.x = new_uOffsetQuad6;
//quad6Collider.position.y = new_vOffsetQuad6;
//quad7Collider.position.x = new_uOffsetQuad7;
//quad7Collider.position.y = new_vOffsetQuad7;

//std::cout << "quad5 Collider: Position(" << quad5Collider.position.x << ", " << quad5Collider.position.y << "), Size(" << quad5Collider.size.x << ", " << quad5Collider.size.y << ")" << std::endl;
//std::cout << "quad2 Collider: Position(" << quad2Collider.position.x << ", " << quad2Collider.position.y << "), Size(" << quad2Collider.size.x << ", " << quad2Collider.size.y << ")" << std::endl;

// Check for collisions with quad2 (Floor)
if (quad1Collider.position.y < quad2Collider.position.y + quad2Collider.size.y &&
	quad1Collider.position.y + quad1Collider.size.y > quad2Collider.position.y &&
	quad1Collider.position.x + quad1Collider.size.x > quad2Collider.position.x &&
	quad1Collider.position.x < quad2Collider.position.x + quad2Collider.size.x) {
	// Collision detected with quad2 (Floor), prevent movement in the y-direction
	new_vOffsetQuad1 = std::max(new_vOffsetQuad1, quad2Collider.position.y + quad2Collider.size.y);
	isCollide = true;
	//std::cout << "Collision detected with floor!" << std::endl;
}

// Check for collisions with quad3 (Left Wall)
if (quad1Collider.position.x + quad1Collider.size.x > quad3Collider.position.x &&
	quad1Collider.position.x < quad3Collider.position.x + quad3Collider.size.x) {
	// Collision detected with quad3 (Left Wall), prevent movement past left wall
	new_uOffsetQuad1 = std::max(new_uOffsetQuad1, quad3Collider.position.x + quad3Collider.size.x);
	//std::cout << "Collision detected with Left Wall!" << std::endl;
}

// Check for collisions with quad4 (Right Wall)
if (quad1Collider.position.x - quad1Collider.size.x < quad4Collider.position.x &&
	quad1Collider.position.x > quad4Collider.position.x - quad4Collider.size.x) {
	// Collision detected with quad4 (Right Wall), prevent movement past right wall
	new_uOffsetQuad1 = std::min(new_uOffsetQuad1, quad4Collider.position.x - quad4Collider.size.x);
	//std::cout << "Collision detected with Right Wall!" << std::endl;
}


if (quad5Collider.position.x + quad5Collider.size.x < quad1Collider.position.x + quad1Collider.size.x &&
	quad5Collider.position.x - quad5Collider.size.x > quad1Collider.position.x - quad1Collider.size.x)
	{
		g_vOffsetQuad5 += gGravity;
		}
	
	if(
		(quad5Collider.position.y < quad1Collider.position.y + quad1Collider.size.y &&
		 quad5Collider.position.y + quad5Collider.size.y > quad1Collider.position.y &&
		 quad5Collider.position.x + quad5Collider.size.x < quad1Collider.position.x + quad1Collider.size.x &&
		 quad5Collider.position.x - quad5Collider.size.x > quad1Collider.position.x - quad1Collider.size.x)
		) {
		
		//Variables for positions player and divider
		float leftSplitPoint = quad5Collider.position.x - quad5Collider.size.x;
		float rightSplitPoint = quad5Collider.position.x + quad5Collider.size.x;
		float leftOfPlayer = quad1Collider.position.x - quad1Collider.size.x;
		float rightOfPlayer = quad1Collider.position.x + quad1Collider.size.x;


		//Left player position after collision
		vertexData[120] = leftOfPlayer; //bottom left x
		vertexData[121] = -0.8f; //bottom left y
		vertexData[126] = leftSplitPoint; //bottom right x
		vertexData[127] = -0.8f; //bottom right y
		vertexData[132] = leftOfPlayer; //top left x
		vertexData[133] = -0.619f; //top left y
		vertexData[138] = leftSplitPoint; //top right x
		vertexData[139] = -0.619f; //top right y

		//Right player position after collsion
		vertexData[144] = rightOfPlayer;
		vertexData[145] = -0.8f;
		vertexData[150] = rightSplitPoint;
		vertexData[151] = -0.8f;
		vertexData[156] = rightOfPlayer;
		vertexData[157] = -0.619f;
		vertexData[162] = rightSplitPoint;
		vertexData[163] = -0.619f;


		//Make middle divider go to top of floor
		new_vOffsetQuad5 = -0.9999f;
		g_vOffsetQuad5 = new_vOffsetQuad5;

		//Make quad1 dissapear after collision
		new_vOffsetQuad1 = 0.0f;
		new_uOffsetQuad1 = 0.0f;

		g_vOffsetQuad1 = new_vOffsetQuad1;
		g_uOffsetQuad1 = new_uOffsetQuad1;

		vertexData[0] = 0.0f;
		vertexData[1] = 0.0f;
		vertexData[6] = 0.0f;
		vertexData[7] = 0.0f;
		vertexData[12] = 0.0f;
		vertexData[13] = 0.0f;
		vertexData[18] = 0.0f;
		vertexData[19] = 0.0f;
		

		quad1Collider.position = glm::vec2(0.0f, 0.0f);
		quad1Collider.size = glm::vec2(0.0f, 0.0f); // Set size to zero

		

		//Send to vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertexData.size() * sizeof(GLfloat), vertexData.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		
	
	}
	

	//std::cout << "g_vOffset: " << new_vOffsetQuad1 << std::endl;
	

		//Code to control left and right players
		if (vertexData[120] != 0.0f) {
			isDivide = true;
		}

		if (isDivide) {
			new_vOffsetQuad6 += gGravity;
			new_vOffsetQuad7 += gGravity;
		}

		if (isDivide && state[SDL_SCANCODE_RIGHT]) {
			new_uOffsetQuad6 += stepSize;
			new_uOffsetQuad7 += stepSize;
		}

		if (isDivide && state[SDL_SCANCODE_LEFT]) {
			new_uOffsetQuad6 -= stepSize;
			new_uOffsetQuad7 -= stepSize;
		}

		
		 if (isDivide && state[SDL_SCANCODE_UP] && canJump) {
			new_vOffsetQuad6 += jumpSize;
			new_vOffsetQuad7 += jumpSize;
			new_vOffsetQuad6 += gGravity;
			new_vOffsetQuad7 += gGravity;
			canJump = false;
			std::cout << "Can jump" << std::endl;
		}
		
		

		quad6Collider.position.x = new_uOffsetQuad6;
		quad6Collider.position.y = new_vOffsetQuad6;
		quad7Collider.position.x = new_uOffsetQuad7;
		quad7Collider.position.y = new_vOffsetQuad7;

		// Check for collisions with quad2 (Floor)
		if (quad6Collider.position.y - quad6Collider.size.y < 0
			) {
			// Collision detected with quad2 (Floor), prevent movement in the y-direction
			new_vOffsetQuad6 = quad6Collider.size.y;
			new_vOffsetQuad7 = quad7Collider.size.y;
			canJump = true;

			//std::cout << "Collision detected with floor!: " << new_vOffsetQuad6 << std::endl;
		}


		// Check for collisions with right side of divider
		if (quad6Collider.position.x + quad6Collider.size.x > quad5Collider.position.x - quad5Collider.size.x) {
			// Collision detected with quad4 (Right Wall), prevent movement past right wall
			new_uOffsetQuad6 = quad5Collider.position.x - quad5Collider.size.x;
			//std::cout << "Collision detected with Right Wall!" << std::endl;
		}

		// Check for collisions with right side of divider
		if (quad7Collider.position.x - quad7Collider.size.x < quad5Collider.position.x + quad5Collider.size.x) {
			// Collision detected with quad4 (Right Wall), prevent movement past right wall
			new_uOffsetQuad7 = quad5Collider.position.x - quad5Collider.size.x + .04f;
			//std::cout << "Collision detected with Right Wall!" << std::endl;
		}

		// Check for collisions with right side of divider
		if (quad6Collider.position.x - quad6Collider.size.x < quad3Collider.position.x + quad3Collider.size.x) {
			// Collision detected with quad4 (Right Wall), prevent movement past right wall
			new_uOffsetQuad6 = quad3Collider.position.x + quad5Collider.size.x + 0.17f;
			//std::cout << "Collision detected with Right Wall!" << std::endl;
		}

		// Check for collisions with right side of divider
		if (quad7Collider.position.x + quad7Collider.size.x > quad4Collider.position.x - quad4Collider.size.x) {
			// Collision detected with quad4 (Right Wall), prevent movement past right wall
			new_uOffsetQuad7 = quad4Collider.position.x - quad4Collider.size.x - 0.001f;
			//std::cout << "Collision detected with Right Wall!" << std::endl;
		}
	
	
	
	// Update the actual position if no collision occurred
	g_uOffsetQuad1 = new_uOffsetQuad1;
	g_vOffsetQuad1 = new_vOffsetQuad1;
	g_vOffsetQuad6 = new_vOffsetQuad6;
	g_vOffsetQuad7 = new_vOffsetQuad7;
	g_uOffsetQuad6 = new_uOffsetQuad6;
	g_uOffsetQuad7 = new_uOffsetQuad7;
	

	//Gravity for Quads
	g_vOffsetQuad1 += gGravity;
	//g_vOffsetQuad6 += gGravity;
	//g_vOffsetQuad7 += gGravity;
	
	//std::cout << "newg_vOffset6 " << new_vOffsetQuad6 << std::endl;
	//std::cout << "g_vOffset6: " << g_vOffsetQuad6 << std::endl;
	//std::cout << "position y " << quad6Collider.position.y - quad6Collider.size.y << std::endl;
	//std::cout << "g_uOffset6: " << new_uOffsetQuad6 << std::endl;
	

}

void PreDraw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, gScreenWidth, gScreenHeight);
	glClearColor(1.f, 1.f, 0.f, 1.f);

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(gGraphicsPipelineShaderProgram);
	
	

	glm::mat4 translate5 = glm::translate(glm::mat4(1.0f), glm::vec3(g_uOffsetQuad5, g_vOffsetQuad5, 0.0f));
	glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(g_uOffsetQuad1, g_vOffsetQuad1, 0.0f));
	glm::mat4 translate6 = glm::translate(glm::mat4(1.0f), glm::vec3(g_uOffsetQuad6, g_vOffsetQuad6, 0.0f));
	glm::mat4 translate7 = glm::translate(glm::mat4(1.0f), glm::vec3(g_uOffsetQuad7, g_vOffsetQuad7, 0.0f));

	//Retrieve our location of our Model Matrix
	GLint u_ModelMatrixLocation = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix");
	GLint u_ModelMatrixLocation2 = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix2");
	GLint u_ModelMatrixLocation3 = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix3");
	GLuint u_ModelMatrixLocation4 = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix4");
	GLuint u_ModelMatrixLocation5 = glGetUniformLocation(gGraphicsPipelineShaderProgram, "u_ModelMatrix5");

	if (u_ModelMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &translate[0][0]);
	}
	else {
		std::cout << "Could not find u_ModelMatrix. \n";
		exit(EXIT_FAILURE);
	}

	// Pass the second model matrix to the shader program
	if (u_ModelMatrixLocation2 >= 0) {
		glm::mat4 identityMatrix = glm::mat4(1.0f); // Identity matrix for no transformation
		glUniformMatrix4fv(u_ModelMatrixLocation2, 1, GL_FALSE, &identityMatrix[0][0]);
	}
	else {
		std::cout << "Could not find u_ModelMatrix2. \n";
		exit(EXIT_FAILURE);
	}

	// Pass the third model matrix to the shader program
	if (u_ModelMatrixLocation3 >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation3, 1, GL_FALSE, &translate5[0][0]);
	}
	else {
		std::cout << "Could not find u_ModelMatrix3. \n";
		exit(EXIT_FAILURE);
	}

	// Pass the fourth model matrix to the shader program
	if (u_ModelMatrixLocation4 >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation4, 1, GL_FALSE, &translate6[0][0]);
	}
	else {
		std::cout << "Could not find u_ModelMatrix3. \n";
		exit(EXIT_FAILURE);
	}

	// Pass the fifth model matrix to the shader program
	if (u_ModelMatrixLocation5 >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation5, 1, GL_FALSE, &translate7[0][0]);
	}
	else {
		std::cout << "Could not find u_ModelMatrix3. \n";
		exit(EXIT_FAILURE);
	}
	



}

void Draw() {

	glBindVertexArray(gVertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, gVertexBufferObject);

	glDrawElements(GL_TRIANGLES,
		42, //Total number of indicies
		GL_UNSIGNED_INT,
		0);

	glUseProgram(0);

}

void MainLoop(std::vector<GLfloat> vertexData) {

	while (!gQuit) {
		Input(vertexData);

		PreDraw();

		Draw();

		//Update the screen
		SDL_GL_SwapWindow(gGraphicsApplicationWindow);

	}

}

void CleanUp() {
	//Make sure window isnt still allocated
	SDL_DestroyWindow(gGraphicsApplicationWindow);
	SDL_Quit();

}

int main(int argc,char* args[])
{
	std::vector<GLfloat> vertexData;
	//Sets up SDL window and OpenGL
	InitializeProgram();

	//Gets vertex data on to the GPU
	VertexSpecification(vertexData);

	//Creates pipline with vertex and fragment shader
	CreateGraphicsPipeline();

	//Handles input, PreDraw, and Draw. Updates every frame
	MainLoop(vertexData);

	//Call clean up function when program terminates
	CleanUp();

	return 0;
}