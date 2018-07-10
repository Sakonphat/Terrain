// Include standard headers
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


// Include others OpenGL helper 
#include <common/shader.hpp>
#include <common/controls.hpp>

#include "terrain.h"


int main(void)
{
	// Initialise GLFW
	if (!glfwInit()){
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	//glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Terrain", NULL, NULL);
	if (window == NULL){
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.2 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark background
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID;
	GLuint program_wireframe = LoadShaders("simpleColor.vert", "simpleColor.frag");
	GLuint program_flatshade = LoadShaders("perpixelPhong.vert", "perpixelPhong.frag");
	programID = program_wireframe;

	//array of vec3 to store Geometry 
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;

	//Create VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	GLuint indexbuffer;
	glGenBuffers(1, &indexbuffer);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);

	//Create+set VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	// 2nd attribute buffer : normals
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	

	// size x size of the square terrain, size should be in [8, 1024]
	int size = 8;
	// to control the roughness of the terrain or H reduce scale, value between [0..1]
	float jagged = 1.0f;	
	// Create terrain
	GenerateTerrain(size, jagged, vertices, normals, indices);
	
	// randomized seed
	srand(time(NULL));

	// control one shot button pressed
	bool up_pressed = false;
	bool down_pressed = false;
	bool left_pressed = false;
	bool right_pressed = false;

	do{

		// press UP or Down arrow to change the size
		if ((glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) && (!up_pressed)){
			if (size < 1024) {
				size *= 2;
				printf("size> %i \n", size);
				// Create terrain
				GenerateTerrain(size, jagged, vertices, normals, indices);
			}
			up_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE){
			up_pressed = false;
		}
		if ((glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) && (!down_pressed)){
			if (size > 8) {
				size /= 2;
				printf("size> %i \n", size);
				// Create terrain
				GenerateTerrain(size, jagged, vertices, normals, indices);
			}
			down_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE){
			down_pressed = false;
		}

		// press UP or Down arrow to change the size
		if ((glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) && (!left_pressed)){
			if (jagged > 0.2f) {
				jagged -= 0.1f;
				printf("jagged> %f \n", jagged);
				// Create terrain
				GenerateTerrain(size, jagged, vertices, normals, indices);
			}
			left_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE){
			left_pressed = false;
		}
		if ((glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) && (!right_pressed)){
			if (jagged < 0.9f) {
				jagged += 0.1f;
				printf("jagged> %f \n", jagged);
				// Create terrain
				GenerateTerrain(size, jagged, vertices, normals, indices);
			}
			right_pressed = true;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE){
			right_pressed = false;
		}

		// switch shading mode Wireframe or Flatshade
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			programID = program_wireframe;
		}
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS){
			//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//programID = program_flatshade;
		}


		//Transfer data to VBO
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Set the shader program
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 ModelViewMatrix = ViewMatrix * ModelMatrix;
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// send uniform variable to shader
		glUniformMatrix4fv(glGetUniformLocation(programID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID, "ModelViewMatrix"), 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(programID, "ViewMatrix"), 1, GL_FALSE, &ViewMatrix[0][0]);

		glm::vec3 ObjColor = glm::vec3(1.0, 1.0, 0.0);
		glm::vec3 LightColor = glm::vec3(1.0, 1.0, 1.0);
		glm::vec3 lightDir = glm::vec3(1, -1, 1);
		float shiny = 100;
		glUniform1f(glGetUniformLocation(programID, "Shininess"), shiny);
		glUniform3f(glGetUniformLocation(programID, "Kd"), ObjColor.x, ObjColor.y, ObjColor.z);
		glUniform3f(glGetUniformLocation(programID, "Ld"), LightColor.x, LightColor.y, LightColor.z);
		glUniform3f(glGetUniformLocation(programID, "LightDirection"), lightDir.x, lightDir.y, lightDir.z);

		// Draw
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (char *)NULL);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);


	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &indexbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

