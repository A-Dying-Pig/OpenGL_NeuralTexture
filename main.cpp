// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>

#include <glad/glad.h>

// Include GLEW
//#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "common/glad_shader.hpp"
#include "common/objloader.hpp"
#include "common/stb_image.h"
#include "common/stb_image_write.h"

//npy
//#include "common/cnpy.h"
#include <cnpy.h>


int window_width = 1024;
int window_height = 768;
int total_frame = 2000;


void saveImageViewNormal(const char * filename){
	GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
	int x = viewport[0];
  int y = viewport[1];
  int width = viewport[2];
  int height = viewport[3];
	//std::cout<<"x: "<<x<<" ,y: "<<y<<std::endl;

	GLfloat * image_view_normal_data = new GLfloat[height * width * 3];
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glReadPixels(x, y, width, height, GL_RGB, GL_FLOAT, image_view_normal_data);

	// invert pixels
	for (int j = 0; j * 2 < height; ++j) {
    int x = j * width * 3;
    int y = (height - 1 - j) * width * 3;
    for (int i = width * 3; i > 0; --i) {
        uint8_t tmp = image_view_normal_data[x];
        image_view_normal_data[x] = image_view_normal_data[y];
        image_view_normal_data[y] = tmp;
      	++x;
      	++y;
  	}
	}

	//std::ofstream out("view_normal.txt");
	std::vector<float> data;
	//write into files
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			int t = j * width * 3 + i * 3;
			data.push_back(image_view_normal_data[t]);
			data.push_back(image_view_normal_data[t+1]);
			data.push_back(image_view_normal_data[t+2]);
			//out << image_view_normal_data[t]<<"/"<<image_view_normal_data[t+1]<<"/"<<image_view_normal_data[t+2]<<" ";
		}
		//out<<std::endl;
	}
	//out.close();
	cnpy::npy_save(filename,&data[0],{(unsigned long)height,(unsigned long)width,3},"w");
	delete[] image_view_normal_data;
}

void saveImageUV(const char * filename){
	GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
	int x = viewport[0];
  int y = viewport[1];
  int width = viewport[2];
  int height = viewport[3];
	//std::cout<<"x: "<<x<<" ,y: "<<y<<std::endl;

	GLfloat * image_uv_data = new GLfloat[height * width * 3];

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(x, y, width, height, GL_RGB, GL_FLOAT, image_uv_data);

	// invert pixels
	for (int j = 0; j * 2 < height; ++j) {
    int x = j * width * 3;
    int y = (height - 1 - j) * width * 3;
    for (int i = width * 3; i > 0; --i) {
        uint8_t tmp = image_uv_data[x];
        image_uv_data[x] = image_uv_data[y];
        image_uv_data[y] = tmp;
      	++x;
      	++y;
  	}
	}

	//std::ofstream out("uv.txt");
	std::vector<float> data;
	//write into files
	for (int j = 0; j < height; j++){
		for (int i = 0; i < width; i++){
			int t = j * width * 3 + i * 3;
			data.push_back(image_uv_data[t]);
			data.push_back(image_uv_data[t+1]);
			//out << image_uv_data[t]<<"/"<<image_uv_data[t+1]<<" ";
		}
		//out<<std::endl;
	}
	//out.close();
	cnpy::npy_save(filename,&data[0],{(unsigned long)height,(unsigned long)width,2},"w");
	delete[] image_uv_data;
}



void saveScreenshot(const char *filename,int originID)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

		GLubyte * data = new GLubyte[height * width * 3];

		// glBindTexture(GL_TEXTURE_2D, originID);
    // glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glReadBuffer(GL_COLOR_ATTACHMENT0);
    // glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		
		// invert pixels
		for (int j = 0; j * 2 < height; ++j) {
    	int x = j * width * 3;
    	int y = (height - 1 - j) * width * 3;
    	for (int i = width * 3; i > 0; --i) {
        	uint8_t tmp = data[x];
        	data[x] = data[y];
        	data[y] = tmp;
        	++x;
        	++y;
    	}
		}
  int saved = stbi_write_png(filename, width, height, 3, data, 0);

  delete[] data;

}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( window_width, window_height, "Vase Display", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	// glewExperimental = true; // Needed for core profile
	// if (glewInit() != GLEW_OK) {
	// 	fprintf(stderr, "Failed to initialize GLEW\n");
	// 	return -1;
	// }

	//Initialize GLAD
	int gladInitRes = gladLoadGL();
      if (!gladInitRes) {
         fprintf(stderr, "Unable to initialize glad\n");
         glfwDestroyWindow(window);
         glfwTerminate();
         return -1;
      }

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the mouse at the center of the screen
  //glfwPollEvents();
  //glfwSetCursorPos(window, 1024/2, 768/2);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	Shader ourShader("TransformVertexShader.vertexshader","TextureFragmentShader.fragmentshader");

	// TEXTURE
	// Load the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("Vase/Vase-obj_0.jpg", &width, &height, &nrChannels, 0); 
	printf("texture width: %d, height: %d, channels: %d\n",width,height,nrChannels);
	unsigned int texture;
	glGenTextures(1, &texture);  
	glBindTexture(GL_TEXTURE_2D, texture);  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	stbi_image_free(data);
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(ourShader.ID, "myTextureSampler");

	//LOAD MODEL
	//load vertex,uv,normals from file
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.
	bool res = loadOBJ("Vase/Vase-obj2.obj", vertices, uvs, normals);
	int vertices_size = vertices.size();
	printf("vertices: %lu , uvs: %lu , normals: %lu \n",vertices.size(),uvs.size(),normals.size());

	//vertex/uv/normal BUFFER
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);


	GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  int vp_width = viewport[2];
  int vp_height = viewport[3];
	//std::cout<<"view port width: "<<vp_width<<", height: "<<vp_height<<std::endl;
	
	// FRAME OUTPUT
	//frame buffer
	GLuint fbo;
	glGenFramebuffers(1,&fbo);
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	//std::cout<<"max buffer number: " << GL_MAX_DRAW_BUFFERS<<std::endl;

	//original color output
	GLuint origin_color;
	glGenTextures(1,&origin_color);
	glBindTexture(GL_TEXTURE_2D,origin_color);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB8, vp_width, vp_height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,origin_color, 0);

	//render buffer
	GLuint rbo;
	glGenRenderbuffers(1,&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER,rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, vp_width, vp_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);  
	
	//image_uv output
	GLuint  image_uv;
	glGenTextures(1,&image_uv);
	glBindTexture(GL_TEXTURE_2D, image_uv);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, vp_width, vp_height, 0,GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,image_uv, 0);

	//view direction output
	GLuint view_normal;
	glGenTextures(1,&view_normal);
	glBindTexture(GL_TEXTURE_2D,view_normal);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,vp_width,vp_height,0,GL_RGB,GL_FLOAT,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,view_normal,0);

	GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   

	// MATRIX
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 2000.0f);
	// Camera matrix
	glm::mat4 View       = glm::lookAt(
								glm::vec3(1200,0,0), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
	// Model matrix : an identity matrix (model will be at the origin)
	glm::vec3 myRotationAxis( 1, 0, 0);
	glm::mat4 translateMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, -400.0f, 0.0f));
	glm::mat4 rotateMatrix = glm::rotate( -1.570796f, myRotationAxis );
	//glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::vec3 yAxis(0,1,0);
	//glm::mat4 Model      = 	translateMatrix * rotateMatrix;

	// std::cout << glm::to_string(Projection) << std::endl;
	// std::cout << glm::to_string(View) << std::endl;
	// std::cout << glm::to_string(translateMatrix) << std::endl;
	// std::cout << glm::to_string(rotateMatrix) << std::endl;


	// Enable depth test
	glEnable(GL_DEPTH_TEST);

  //LOOP
	char file_name[10];
	for(int z = 0; z < total_frame; z++){
		glm::mat4 yrotateMatrix = glm::rotate(6.283184f/total_frame * z,yAxis);
		glm::mat4 vMatrix = View * translateMatrix * yrotateMatrix * rotateMatrix;
		glm::mat4 viewMat = glm::transpose(glm::inverse(vMatrix));
		glm::mat4 MVP        = Projection * vMatrix; // Remember, matrix multiplication is the other way around

		
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);
		glDrawBuffers(3, DrawBuffers); // "3" is the size of DrawBuffers

		// Clear the screen
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		ourShader.use();

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		ourShader.setMat4("MVP",MVP);
		ourShader.setMat4("viewMat",viewMat);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 382881); // 12*3 indices starting at 0 -> 12 triangles		

		//screen shots
		sprintf(file_name,"output/frame/%04d.png",z);
		saveScreenshot(file_name,origin_color);

		//image uv
		sprintf(file_name,"output/uv/%04d.npy",z);
		saveImageUV(file_name);

		//view normal
		sprintf(file_name,"output/view_normal/%04d.npy",z);
		saveImageViewNormal(file_name);


		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Check if the ESC key was pressed or the window was closed
		if(glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS || glfwWindowShouldClose(window) != 0){
			break;
		}
	} 

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1,&normalbuffer);
	glDeleteRenderbuffers(1,&rbo);
	glDeleteFramebuffers(1,&fbo);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1,&image_uv);
	glDeleteTextures(1,&origin_color);
	glDeleteTextures(1,&view_normal);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();


	return 0;
}

