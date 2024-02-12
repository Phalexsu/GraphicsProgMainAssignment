#include "chessApplication.h"
#include "GeometricTools.h"
#include "BufferLayout.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Shaders.h"
#include "RenderCommands.h"
#include "PerspectiveCamera.h"
#include "TextureManager.h"



chessApp::chessApp(const std::string& name, const std::string& version){

}
chessApp::~chessApp(){
}

//Argument parsing
unsigned int chessApp::ParseArguments(int argc, char** argv){
	GLFWApplication::ParseArguments(argc, argv);
	return 0;
} 

// Initialization 
unsigned int chessApp::Init(){
	GLFWApplication::Init();
	
	return 0;
} 
/**
* @brief checks if all keys that you are not supposed to be able to hold down are released
* 
* @param GLFWwindow* window - the window that we are working with
* @param bool &pressed - if a key is pressed right now
*/
void checkReleased(GLFWwindow* window,bool &pressed) {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE && glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) {
		pressed = false;
	}
}
/**
* @brief enebles/disables the textures
* 
* @param GLFWwindow* window - the window that we are working with
* @param bool &texture - if textures is enable or not
* @param bool &pressed - if a key is pressed right now
* 
* @see checkReleased(...)
*/
void textureKey(GLFWwindow* window,bool &texture, bool& pressed) {
	if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && !pressed) {
		texture = !texture;
		pressed = true;
	}
	checkReleased(window, pressed);
}
/**
* @brief rotates and zooms the camera with keypresses
*
* @param GLFWwindow* window - the window that we are working with
* @param PerspectiveCamera* cam - the camera that we are working with
*/
void camKeys(GLFWwindow* window,PerspectiveCamera* cam) {
	glm::vec4 pos = glm::vec4(cam->GetPosition(),1.0f);
	//zoom in
	if ((glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) && pos[1] > 0.5f) {
		glm::vec3 scale(0.995f, 0.995f, 0.995f);
		pos = glm::scale(glm::mat4(1.0f), scale)*pos;
		glm::vec3 newPos(pos[0], pos[1], pos[2]);
		cam->SetPosition(newPos);
	}

	//zoom out
	if ((glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) && pos[1] < 9.0f) {
		glm::vec3 scale(1.005f, 1.005f, 1.005f);
		pos = glm::scale(glm::mat4(1.0f), scale) * pos;
		glm::vec3 newPos(pos[0], pos[1], pos[2]);
		cam->SetPosition(newPos);
	}

	//rotate the camera clockwise
	if ((glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)) {
		glm::vec3 rotate(0.0f, 1.0f, 0.0f);
		pos = glm::rotate(glm::mat4(1.0f), glm::radians(-0.5f), rotate) * pos;
		glm::vec3 newPos(pos[0], pos[1], pos[2]);
		cam->SetPosition(newPos);
	}

	//rotate the camera anti-clockwise
	if ((glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)) {
		glm::vec3 rotate(0.0f, 1.0f, 0.0f);
		pos = glm::rotate(glm::mat4(1.0f), glm::radians(0.5f), rotate) * pos;
		glm::vec3 newPos(pos[0], pos[1], pos[2]);
		cam->SetPosition(newPos);
	}
}

/**
* @brief movement keys
* 
* @param GLFWwindow* window - the window that we are working with
* @param glm::vec2& selPos - selector position
* @param bool &pressed - if a key is pressed right now
* 
* @see checkReleased(...)
*/
void keyInput(GLFWwindow* window, glm::vec2& selPos, bool& pressed) {
	//move the selector up
	if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && !pressed && selPos.y<7.0f) {
		selPos.y += 1.0f;
		pressed = true;
	}

	//move the selector down
	if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && !pressed && selPos.y > 0.0f) {
		selPos.y -= 1.0f;
		pressed = true;
	}

	//move the selector left
	if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && !pressed && selPos.x > 0.0f) {
		selPos.x -= 1.0f;
		pressed = true;
	}

	//move the selector right
	if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && !pressed && selPos.x < 7.0f) {
		selPos.x += 1.0f;
		pressed = true;
	}
	
	checkReleased(window, pressed);

}

/**
* @brief handles the code for what happens if you try to select a cube or place it
* 
* @param GLFWwindow* window - the window that we are working with
* @param bool &pressed - if a key is pressed right now
* @param bool &select - if a cube can be selected or not
* @param glm::vec2& selPos - selector position
* @param std::vector<glm::mat4> &cubeModelMatricies - vector of all the cube model matricies
* @param glm::mat4& cubeScale - the scale of the cubes
* @param glm::mat4& cubeRotation - the rotation of the cubes
* @param std::vector<glm::mat4> &cubeTranslation - vector of all cube translations
* @param int k - the index of the cube that is checking this function
* @param bool &empty - if the tile where the selector is, is empty or not
* 
* @see checkReleased(...)
*/
void selectCube(GLFWwindow* window, bool& pressed, bool& select,
	glm::vec2& selPos, std::vector<glm::mat4> &cubeModelMatricies,
	glm::mat4& cubeScale, glm::mat4& cubeRotation,
	std::vector<glm::mat4> &cubeTranslations, int k, bool &empty) {

	glm::vec3 trans((1.0f / 16.0f), (1.0f / 16.0f), 0.05f); //starting square(bottom left at the start)
	//place a square
	if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && !pressed && !select && empty) {
		int i = 0;
		while (i<selPos[0]) {
			i++;
		}
		trans[0] *= 2 * i + 1;
		trans[0] += -0.5f;
		i = 0;
		while (i < selPos[1]) {
			i++;
		}
		trans[1] *= 2 * i + 1;
		trans[1] += -0.5f;
		cubeTranslations[k] = glm::translate(glm::mat4(1.0f), trans);
		cubeModelMatricies[k] = cubeScale * cubeRotation * cubeTranslations[k];
		select = true;
		pressed = true;
	}
	//select a cube
	else if ((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && !pressed && select) {
		select = false;
		pressed = true;
	}
	//trying to place a cube on a occupied tile
	else if((glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) && !pressed && !empty) {
		select = true;
		pressed = true;
	}

	checkReleased(window, pressed);
}

// Run function
unsigned int chessApp::Run() const{ // Pure virtual function, it must be redefined

	// Create buffers and arrays for chessboard
	auto gridGeometry = GeometricTools::UnitGridGeometry2DWTCoords(8, 8);
	auto gridTopology = GeometricTools::unitGridTopology(8, 8);
	auto gridVertexArray = std::make_shared<VertexArray>();
	auto gridIndexBuffer = std::make_shared<IndexBuffer>(gridTopology.data(), gridTopology.size());
	auto gridBufferLayout = BufferLayout({ {ShaderDataType::Float2, "position"},{ShaderDataType::Float2, "tcoords"} });
	auto gridVertexBuffer = std::make_shared<VertexBuffer>(gridGeometry.data(), gridGeometry.size() * sizeof(gridGeometry[0]));
	gridVertexBuffer->SetLayout(gridBufferLayout);
	gridVertexArray->AddVertexBuffer(gridVertexBuffer);
	gridVertexArray->SetIndexBuffer(gridIndexBuffer);

	// camera
	PerspectiveCamera* cam = new PerspectiveCamera(GLFWApplication::width, GLFWApplication::height);

	// applying the camera to the chessboard
	auto chessboardModelMatrix = glm::mat4(1.0f);
	auto chessboardRotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	auto chessboardTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	auto chessboardScale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	chessboardModelMatrix = chessboardScale * chessboardRotation * chessboardTranslation;
	auto chessboardViewProjectionMatrix = cam->GetViewProjectionMatrix();

	// shaders for chessboard
	auto chessboardShader = std::make_shared<Shader>(Shaders::vertexShader, Shaders::fragmentShader);
	chessboardShader->Bind();
	glm::vec2 gridPos = { 8,8 };
	chessboardShader->UploadUniformFloat2("u_divisions", gridPos);
	glm::vec2 selectorPos = { 0.0f ,0.0f };
	chessboardShader->UploadUniformMat4x4("u_chessModMat", chessboardModelMatrix);
	chessboardShader->UploadUniformMat4x4("u_chessViewProjMat", chessboardViewProjectionMatrix);

	// Create buffers and arrays for cube
	auto cube = GeometricTools::Cube3D(12);
	auto cubeTopology = GeometricTools::cubeTopology;
	auto cubeVertexArray = std::make_shared<VertexArray>();
	auto cubeIndexBuffer = std::make_shared<IndexBuffer>(cubeTopology.data(), cubeTopology.size());
	auto cubeBufferLayout = BufferLayout({ {ShaderDataType::Float3, "position"} });
	auto cubeVertexBuffer = std::make_shared<VertexBuffer>(cube.data(), cube.size() * sizeof(cube[0]));
	cubeVertexBuffer->SetLayout(cubeBufferLayout);
	cubeVertexArray->AddVertexBuffer(cubeVertexBuffer);
	cubeVertexArray->SetIndexBuffer(cubeIndexBuffer);


	//applying the camera to the cube
	auto cubeRotation = glm::rotate(glm::mat4(1.0f), glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	std::vector<glm::mat4> cubeTranslations;
	cubeTranslations.resize(32);
	int h = 0;
	for (int i = 0; i < cubeTranslations.size()/4; i++)
	{
		if (!(i > 1 && i < 6)) {
			for (int j = 0; j < cubeTranslations.size() / 4; j++)
			{
				cubeTranslations[h] = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f + (1 + j*2) * (1.0f / 16.0f), -0.5f + (1 + i*2) * (1.0f / 16.0f), 0.05f));
				h++;
				
			}
		}
	}
	auto cubeScale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f));
	std::vector <glm::mat4> cubeModelMatricies;
	cubeModelMatricies.resize(32);
	for (int i = 0; i < cubeModelMatricies.size(); i++)
	{
		cubeModelMatricies[i] = glm::mat4(1.0f);
		cubeModelMatricies[i] = cubeScale * cubeRotation * cubeTranslations[i];
	}
	auto cubeViewProjectionMatrix = cam->GetViewProjectionMatrix();

	// Shaders for cube
	auto cubeShader = std::make_shared<Shader>(Shaders::cubeVertexShader, Shaders::cubeFragmentShader);
	cubeShader->Bind();
	for (int i = 0; i < cubeModelMatricies.size(); i++)
	{
		cubeShader->UploadUniformMat4x4("u_cubeModMat", cubeModelMatricies[i]);
		cubeShader->UploadUniformMat4x4("u_cubeViewProjMat", cubeViewProjectionMatrix);

	}

	// Textures
	TextureManager* texMan = TextureManager::GetInstance();
	texMan->LoadTexture2DRGBA("floor", std::string(TEXTURES_DIR) + std::string("floor_texture.png"), 0);
	texMan->LoadCubeMapRGBA("cube", std::string(TEXTURES_DIR) + std::string("cube_texture.png"), 1);

	// Enables
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	//gameloop variables
	bool pressed = false; // if a button is pressed
	bool select = true; // if you can select something
	int xs = -1; // x position saved
	int ys = -1; // y postion saved
	bool texture = false; // if a texture is activated
	// value sent to the shader to tell it if a texture should be activated
	int textureInt = 0; 
	bool green = false; // if the cube should be green
	bool yellow = false; // if the cube should be yellow
	//throw away varible to fill a  function parameter that is referenced,
	// but does not involve the caller
	bool tmp = false; 

	while (!glfwWindowShouldClose(GLFWApplication::window))
	{
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//checks if any camerakeys is pressed
		camKeys(window,cam);
		
		//checks if the texture key is pressed
		textureKey(window, texture, pressed);
		if (!texture) {
			textureInt = 0;
			glBlendFunc(GL_ONE, GL_ZERO);
		}
		else {
			textureInt = 1;
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		
		//checks if any movement keys are pressed
		keyInput(GLFWApplication::window, selectorPos, pressed);

		//binds the chessboard shader and uploads the uniforms
		chessboardShader->Bind();
		chessboardShader->UploadUniformInt("u_texture", textureInt);
		chessboardShader->UploadUniformFloat2("u_selectorPosition", selectorPos);
		chessboardShader->UploadUniformMat4x4("u_chessViewProjMat", cam->GetViewProjectionMatrix());

		//binds the chessboard VA and draws the board
		gridVertexArray->Bind();	
		RenderCommands::DrawIndex(gridVertexArray, GL_TRIANGLES);

		//binds the cubeshader and uploads the uniforms
		cubeShader->Bind();
		cubeShader->UploadUniformInt("u_texture", textureInt);
		cubeShader->UploadUniformMat4x4("u_cubeViewProjMat", cam->GetViewProjectionMatrix());

		//binds the cube VA and draws the cubes
		cubeVertexArray->Bind();
		for (int i = 0; i < cubeModelMatricies.size(); i++)
		{
			green = false;
			yellow = false;

			//x and y postion of all the cubes changed from a coordinate system
			// that measures from -0.5 to 0.5 to one that measures from 0 to 7
			std::vector<float> xv;
			std::vector<float> yv;
			for (int j = 0; j < cubeModelMatricies.size(); j++)
			{
				xv.push_back((cubeTranslations[j][3][0] - 0.0625f) / 0.125f + 4.0f);
				yv.push_back((cubeTranslations[j][3][1] - 0.0625f) / 0.125f + 4.0f);
			}

			//if the selector is over this cube and a cube can be selected
			if( xv[i] == selectorPos[0] && yv[i] == selectorPos[1] && select) {
				selectCube(GLFWApplication::window, pressed, select, selectorPos,
					cubeModelMatricies, cubeScale, cubeRotation, cubeTranslations,
					i, tmp);
				xs = xv[i];
				ys = yv[i];
			}
			// if something has been selected
			else if (xs == xv[i] && ys == yv[i] && !select) {
				bool empty = true; //if the tile is empty
				yellow = true;
				//goes through all the cubes to check if the selctor is on top of any of them
				for (int j = 0; j < cubeModelMatricies.size(); j++)
				{
					if ( (xv[j] == selectorPos[0] && yv[j] == selectorPos[1])) {
						empty = false;
					}
				}
				// if the selector is on an empty tile and the user presses enter
				// it places it there. if it is not empty then the cube is unselcted
				selectCube(GLFWApplication::window, pressed, select, selectorPos,
					cubeModelMatricies, cubeScale, cubeRotation, cubeTranslations,
					i, empty);
			}
			// the cube becomes green if the selctor is on top of it
			if (xv[i] == selectorPos[0] && yv[i] == selectorPos[1]) {
				green = true;
			}
			// uploads the cube modelmatrix after all calculations are done
			cubeShader->UploadUniformMat4x4("u_cubeModMat", cubeModelMatricies[i]);
			//prioritizes yellow over green so that the selected cube does not
			// become green when hovered over
			if (yellow) {
				cubeShader->UploadUniformFloat4("cubeColor", glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
			}
			else if (green) {
				cubeShader->UploadUniformFloat4("cubeColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));		
			}
			//cubes 0-15 is blue and cubes 16-31 is red
			else if(i>15){
				cubeShader->UploadUniformFloat4("cubeColor", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
			}
			else{
				cubeShader->UploadUniformFloat4("cubeColor", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
			}
			//drawing the cubes
			RenderCommands::DrawIndex(cubeVertexArray, GL_TRIANGLES);

		}

		glfwSwapBuffers(GLFWApplication::window);
		// Exit the loop if escape is pressed
		if (glfwGetKey(GLFWApplication::window, GLFW_KEY_Q) == GLFW_PRESS) break;
	}

	glfwTerminate();
	return 0;
} 

