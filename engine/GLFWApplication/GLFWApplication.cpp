#include "GLFWApplication.h"


GLFWApplication::GLFWApplication(const std::string& name, const std::string& version){};
GLFWApplication::~GLFWApplication(){};

//Argument parsing
unsigned int GLFWApplication::ParseArguments(int argc, char** argv){

	try {

		TCLAP::CmdLine cmd("Command description message", ' ', "0.9");

		TCLAP::ValueArg<int> widthArg("w", "width", "width", false, 800, "int");
		TCLAP::ValueArg<int> heightArg("g", "height", "height", false, 800, "int");
		cmd.add(widthArg);
		cmd.add(heightArg);

		cmd.parse(argc, argv);
		height = heightArg.getValue();
		width = widthArg.getValue();

	}
	catch (TCLAP::ArgException& e)
	{
		std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
	}
	return 0;
}; // Virtual function with default behavior.

//GLFWErrorCallback
void GLFWErrorCallback(int code, const char* description)
{
	std::cerr << "Error " << "0x" << std::hex << code << ':' << description << "\n";
}

// Initialization 
unsigned int GLFWApplication::Init(){ // Virtual function with defaut behavior
	glfwSetErrorCallback(GLFWErrorCallback);

	if (!glfwInit()) {
		std::cin.get();

		return EXIT_FAILURE;
	}
	glfwWindowHint(GLFW_RESIZABLE, false);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "window", NULL, NULL);
	if (window == nullptr)
	{

		glfwTerminate();

		std::cin.get();

		return EXIT_FAILURE;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << "\n";
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << "\n";
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
};

