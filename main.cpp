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
#include <cnpy.h>


int window_width = 512;
int window_height = 384;
int viewport_width = 512;
int viewport_height = 384;
bool enable_output = false;
int total_frame = 2000;
glm::vec3 background_color = vec3(0.3f,0.3f,0.3f); 
int object_id = 0; // 0 for basketball; 1 for vase

char uv_output_dir[] = "/Volumes/Seagate Backup Plus Drive/512x384Basketball60/uv/%04d.npy";
char screenshot_output_dir[] = "/Volumes/Seagate Backup Plus Drive/512x384Basketball60/frame/%04d.png";
char view_normal_output_dir[] = "/Volumes/Seagate Backup Plus Drive/512x384Basketball60/view_normal/%04d.npy";

//Phone light
struct Light {
    vec3 direction;	//normalized; direction adjusted light direction
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


void initBackground(GLuint &bgVAO, GLuint &bgVBO){
	// put the vase on the ground
	// coordinates - normal - uv
	float vertices_basketball[] = {
    // bottom face
    -2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 0.0f, 0.0f, // top-right
    2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 0.08f, 0.0f,// top-left
    2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 0.04f, 0.08f,// bottom-left
    2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 0.04f, 0.08f,// bottom-left
  	-2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 0.08f, 0.0f,// bottom-right
    -2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 0.0f,0.0f// top-right    
    };
	float vertices_vase[] = {
    // bottom face
    -2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 1.0f, 0.0f, // top-right
    2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 0.8f, 0.0f,// top-left
    2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 0.8f, 0.15f,// bottom-left
    2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 0.8f, 0.15f,// bottom-left
  	-2500.0f, 0.0f,  2500.0f,  0.0f,2500.0f,0.0f, 1.0f, 0.15f,// bottom-right
    -2500.0f, 0.0f, -2500.0f,  0.0f,2500.0f,0.0f, 1.0f,0.0f// top-right    
    };

	glGenVertexArrays(1, &bgVAO);
	glGenBuffers(1,&bgVBO);
	glBindVertexArray(bgVAO);
	//fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
	if (object_id == 0){
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_basketball), vertices_basketball, GL_STATIC_DRAW);
	}else if (object_id == 1){
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_vase), vertices_vase, GL_STATIC_DRAW);
	}
	// link vertex attributes
	//coordinates
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	// uv
	glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
	// normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void saveImageViewNormal(const char * filename){
	GLfloat * image_view_normal_data = new GLfloat[viewport_height * viewport_width * 3];
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	glReadPixels(0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, image_view_normal_data);

	// invert pixels
	for (int j = 0; j * 2 < viewport_height; ++j) {
    int x = j * viewport_width * 3;
    int y = (viewport_height - 1 - j) * viewport_width * 3;
    for (int i = viewport_width * 3; i > 0; --i) {
        GLfloat tmp = image_view_normal_data[x];
        image_view_normal_data[x] = image_view_normal_data[y];
        image_view_normal_data[y] = tmp;
      	++x;
      	++y;
  	}
	}

	//std::ofstream fout("normal.txt");
	std::vector<float> data;
	//write into files
	for (int j = 0; j < viewport_height; j++){
		for (int i = 0; i < viewport_width; i++){
			int t = j * viewport_width * 3 + i * 3;
			data.push_back(image_view_normal_data[t]);
			data.push_back(image_view_normal_data[t+1]);
			data.push_back(image_view_normal_data[t+2]);
			//fout << image_view_normal_data[t] << "/"<<image_view_normal_data[t+1] <<"/"<<image_view_normal_data[t+2]<<" ";
		}
		//fout<<std::endl;
	}
	//fout.close();
	cnpy::npy_save(filename,&data[0],{(unsigned long)viewport_height,(unsigned long)viewport_width,3},"w");
	delete[] image_view_normal_data;
}

void saveImageUV(const char * filename){
	GLfloat * image_uv_data = new GLfloat[viewport_height * viewport_width * 3];

	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glReadPixels(0, 0, viewport_width, viewport_height, GL_RGB, GL_FLOAT, image_uv_data);

	// invert pixels
	for (int j = 0; j * 2 < viewport_height; ++j) {
    int x = j * viewport_width * 3;
    int y = (viewport_height - 1 - j) * viewport_width * 3;
    for (int i = viewport_width * 3; i > 0; --i) {
        GLfloat tmp = image_uv_data[x];
        image_uv_data[x] = image_uv_data[y];
        image_uv_data[y] = tmp;
      	++x;
      	++y;
  	}
	}
	
	//std::ofstream fout("uv.txt");
	std::vector<float> data;
	//write into files
	for (int j = 0; j < viewport_height; j++){
		for (int i = 0; i < viewport_width; i++){
			int t = j * viewport_width * 3 + i * 3;
			data.push_back(image_uv_data[t]);
			data.push_back(image_uv_data[t+1]);
			//fout << image_uv_data[t] << "/"<<image_uv_data[t+1] <<" ";
		}
		//fout<<std::endl;
	}
	//fout.close();
	cnpy::npy_save(filename,&data[0],{(unsigned long)viewport_height,(unsigned long)viewport_width,2},"w");
	delete[] image_uv_data;
}



void saveScreenshot(const char *filename)
{
		GLubyte * data = new GLubyte[viewport_height * viewport_width * 3];

		glReadBuffer(GL_COLOR_ATTACHMENT0);
    // glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, viewport_width, viewport_height, GL_RGB, GL_UNSIGNED_BYTE, data);

		// invert pixels
		for (int j = 0; j * 2 < viewport_height; ++j) {
    	int x = j * viewport_width * 3;
    	int y = (viewport_height - 1 - j) * viewport_width * 3;
    	for (int i = viewport_width * 3; i > 0; --i) {
        	uint8_t tmp = data[x];
        	data[x] = data[y];
        	data[y] = tmp;
        	++x;
        	++y;
    	}
		}
  int saved = stbi_write_png(filename, viewport_width, viewport_height, 3, data, 0);

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
  //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set the mouse at the center of the screen
  //glfwPollEvents();
  //glfwSetCursorPos(window, 1024/2, 768/2);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);

	// Create and compile our GLSL program from the shaders
	Shader ourShader("TransformVertexShader.vertexshader","TextureFragmentShader.fragmentshader");

	// TEXTURE
	// Load the texture
	int width, height, nrChannels;
	unsigned char * data;
	if (object_id == 0){
		data = stbi_load("basketball/NBA BASKETBALL DIFFUSE.jpg", &width, &height, &nrChannels, 0); 
	}else if (object_id == 1){
		data = stbi_load("Vase/Vase-obj_0.jpg", &width, &height, &nrChannels, 0); 
	}
	printf("texture width: %d, height: %d, channels: %d\n",width,height,nrChannels);
	unsigned int texture;
	glGenTextures(1, &texture);  
	glBindTexture(GL_TEXTURE_2D, texture);  
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	stbi_image_free(data);

	//LOAD MODEL
	//load vertex,uv,normals from file
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; // Won't be used at the moment.
	if (object_id == 0){
		loadOBJ("basketball/basketball.obj", vertices, uvs, normals);
	}else if (object_id == 1){
		loadOBJ("Vase/Vase-obj2.obj", vertices, uvs, normals);
	}
	long vertices_size = vertices.size();
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

	glBindVertexArray(VertexArrayID);
	// 1st attribute
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0, (void*)0);
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0, (void*)0);
	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0, (void*)0);
	glBindVertexArray(0);

	
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
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB8, viewport_width, viewport_height, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,origin_color, 0);

	//render buffer
	GLuint rbo;
	glGenRenderbuffers(1,&rbo);
	glBindRenderbuffer(GL_RENDERBUFFER,rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, viewport_width, viewport_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);  
	
	//image_uv output
	GLuint  image_uv;
	glGenTextures(1,&image_uv);
	glBindTexture(GL_TEXTURE_2D, image_uv);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB32F, viewport_width, viewport_height, 0,GL_RGB, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D,image_uv, 0);

	//view direction output
	GLuint view_normal;
	glGenTextures(1,&view_normal);
	glBindTexture(GL_TEXTURE_2D,view_normal);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB32F,viewport_width,viewport_height,0,GL_RGB,GL_FLOAT,0);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,view_normal,0);

	GLenum DrawBuffers[3] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2};
	glBindFramebuffer(GL_FRAMEBUFFER, 0);   

	// BACKGROUND
	Shader backgroundShader("background.vertexshader","background.fragmentshader");
	GLuint bgVAO,bgVBO;
	initBackground(bgVAO,bgVBO);


	//PHONE LIGHT
	Light light;
	light.direction = normalize(vec3(-1.0f,-1.0f,0));
	light.ambient = vec3(0.1,0.1,0.1);
	light.diffuse = vec3(0.6,0.6,0.6);
	light.specular = vec3(0.3,0.3,0.3);

	//SHADOW
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//depth map
	GLuint depthMap;
	glGenTextures(1,&depthMap);
	glBindTexture(GL_TEXTURE_2D,depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewport_width, viewport_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); 

	
	Shader simpleDepthShader("simpleDepthShader.vertexshader","simpleDepthShader.fragmentshader");

	// MODEL MATRIX
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)viewport_width / (float)viewport_height, 1.0f, 4000.0f);
	// Camera matrix
	// glm::mat4 View       = glm::lookAt(
	// 							glm::vec3(1200,1200,0), // Camera is at (4,3,3), in World Space
	// 							glm::vec3(0,0,0), // and looks at the origin
	// 							glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
	// 					   );

	// Model matrix : an identity matrix (model will be at the origin)
	glm::vec3 myRotationAxis( 1, 0, 0);
	glm::mat4 translateMatrix;
	if (object_id == 0){
		translateMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, 230.0f, 0.0f));
	}else if (object_id == 1){
		translateMatrix = glm::mat4(1.0f);
	}
	glm::mat4 rotateMatrix = glm::rotate( -1.570796f, myRotationAxis );
	glm::vec3 yAxis(0,1,0);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  //LOOP
	char file_name[10];
	//while(1){
	for(int z = 0; z < total_frame; z++){
		//int z = 0;
		//model matrix
		glm::mat4 yrotateMatrix = glm::rotate(6.283184f/total_frame * z,yAxis);
		float dis = 1500 - z * 0.25;
		glm::vec4 camerapos = yrotateMatrix * glm::vec4(dis*0.5f,dis*0.866,0,1);
		//std::cout<<glm::to_string(camerapos)<<std::endl;

		glm::mat4 View       = glm::lookAt(
								glm::vec3(camerapos), // Camera is at (4,3,3), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );

		//LIGHT MATRIX
		float light_distance = 1200;
		glm::mat4 lightProjection = glm::ortho(-2000.0f, 2000.0f,-2000.0f, 2000.0f, 1.0f, 4000.0f); 
	  //glm::mat4 lightProjection =	glm::perspective(glm::radians(45.0f), (float)viewport_width / (float)viewport_height, 1.0f, 4000.0f); 
		glm::vec3 lightPosition = glm::vec3(1200,1200,0);
		//lightPosition = vec3(yrotateMatrix * vec4(lightPosition,1.0));
		glm::mat4 lightView = glm::lookAt(lightPosition, 
                                  		glm::vec3( 0.0f, 0.0f,  0.0f), 
                                  		glm::vec3( 0.0f, 1.0f,  0.0f));  

	  glm::mat4 lightSpaceMatrix = lightProjection * lightView; 

		//view port
		glViewport(0,0,viewport_width,viewport_height);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//PASS 1 - calculate depth map
		//in light's perspective 
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(  GL_DEPTH_BUFFER_BIT);


		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix",lightSpaceMatrix);

		// background
		//glm::mat4 BackdgroundModel = yrotateMatrix;
		glm::mat4 BackdgroundModel =  glm::mat4(1.0f);
		simpleDepthShader.setMat4("model",BackdgroundModel);
		glBindVertexArray(bgVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles	
		glBindVertexArray(0);

		glm::mat4 myScalingMatrix;
		if (object_id == 0){
			myScalingMatrix = glm::scale( mat4(1.0f),vec3(10.0f, 10.0f ,10.0f));		// basketball
		}else if (object_id == 1){
			myScalingMatrix = glm::scale( mat4(1.0f),vec3(0.9f, 0.9f ,0.9f));		// vase
		}

		glm::mat4 Model = translateMatrix * rotateMatrix * myScalingMatrix;
		//glm::mat4 Model = translateMatrix * rotateMatrix;
		simpleDepthShader.setMat4("model",Model);
		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, vertices_size); 
		glBindVertexArray(0);
		
		glBindFramebuffer(GL_FRAMEBUFFER,0);

		//PASS 2 - render image
		// in real camera's perspective
		//view port
		if(enable_output){
			glBindFramebuffer(GL_FRAMEBUFFER,fbo);
			glDrawBuffers(3, DrawBuffers); // "3" is the size of DrawBuffers
		}	
		glViewport(0,0,viewport_width,viewport_height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//DRAW BACKGROUND
		ourShader.use();
		ourShader.setMat4("view",View);
		ourShader.setMat4("projection",Projection);
		ourShader.setMat4("lightSpaceMatrix",lightSpaceMatrix);
		ourShader.setVec3f("light.direction",light.direction);
		ourShader.setVec3f("light.ambient",light.ambient);
		ourShader.setVec3f("light.diffuse",light.diffuse);
		ourShader.setVec3f("light.specular",light.specular);
		ourShader.setMat4("model",BackdgroundModel);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(ourShader.ID, "myTextureSampler"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(ourShader.ID, "shadowMap"), 1);

		ourShader.setInt("object_id",object_id);

		glBindVertexArray(bgVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 12*3 indices starting at 0 -> 12 triangles	
		glBindVertexArray(0);

		ourShader.setMat4("model",Model);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(glGetUniformLocation(ourShader.ID, "myTextureSampler"), 0);

		glBindVertexArray(VertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, vertices_size); 
		glBindVertexArray(0);
	

		if(enable_output){
			//screen shots
			sprintf(file_name,screenshot_output_dir,z);
			saveScreenshot(file_name);

			//image uv
			sprintf(file_name,uv_output_dir,z);
			saveImageUV(file_name);

			//view normal
			sprintf(file_name,view_normal_output_dir,z);
			saveImageViewNormal(file_name);
		}

		glBindFramebuffer(GL_FRAMEBUFFER,0);

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
	glDeleteFramebuffers(1,&depthMapFBO);
	glDeleteTextures(1,&depthMap);

	glDeleteBuffers(1, &bgVBO);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteVertexArrays(1,&bgVAO);

	ourShader.~Shader();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

