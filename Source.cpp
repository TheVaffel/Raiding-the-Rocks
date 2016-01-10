#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <stdio.h> //system call
#include <cmath>
#include <limits>

#include "vertexshader.h"
#include "fragmentshader.h"
#include "geometryShader.h"

#include "colorVertexShader.h"
#include "colorFragmentShader.h"
#include "colorGeometryShader.h"

#include "shadowVertexShader.h"
#include "shadowFragmentShader.h"

#include <SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RockModels.hpp"


using namespace std;
using namespace glm;

GLuint createShader(GLenum type, const GLchar* src) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	return shader;
}

void printShaderCompilerError(GLuint shader) {
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_FALSE)
		return;
	cout << "Shader compiled unsuccessfully" << endl;

	char buffer[512];
	glGetShaderInfoLog(shader, 512, NULL, buffer);

	cout << buffer << endl;
	cout << "Problem was with shader num " << shader << endl;
	glfwTerminate();
}

double movingforward = 0.0;
double movingright = 0.0;
double movingup = 0.0;

double movingSpeed = 0.03;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if((key == GLFW_KEY_W&&action == GLFW_PRESS)||(key == GLFW_KEY_S && action == GLFW_RELEASE))
		movingforward += movingSpeed;
	if((key == GLFW_KEY_W&&action == GLFW_RELEASE)||(key == GLFW_KEY_S && action == GLFW_PRESS))
		movingforward -= movingSpeed;

	if((key == GLFW_KEY_D&&action == GLFW_PRESS)||(key == GLFW_KEY_A && action == GLFW_RELEASE))
		movingright += movingSpeed;
	if((key == GLFW_KEY_D&&action == GLFW_RELEASE)||(key == GLFW_KEY_A && action == GLFW_PRESS))
		movingright -= movingSpeed;

		if((key == GLFW_KEY_SPACE&&action == GLFW_PRESS)||(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE))
			movingup += movingSpeed;
		if((key == GLFW_KEY_SPACE&&action == GLFW_RELEASE)||(key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS))
			movingup -= movingSpeed;


}

GLuint makeShaderProgram(const char* vsSrc, const char* gsSrc, const char* fsSrc){
	GLuint vs = createShader(GL_VERTEX_SHADER, vsSrc);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSrc);
	GLuint gs = createShader(GL_GEOMETRY_SHADER, gsSrc);
	printShaderCompilerError(vs);
	printShaderCompilerError(fs);
	printShaderCompilerError(gs);

	GLuint sp = glCreateProgram();

	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glAttachShader(sp, gs);
	glBindFragDataLocation(sp, 0, "outColor");
	glLinkProgram(sp);

	GLint isLinked = 0;
	glGetProgramiv(sp, GL_LINK_STATUS, &isLinked);
	if(isLinked == GL_FALSE){
		cout<<"Håkon, you f****"<<endl;
		char bytes[512];
		GLsizei l;
		glGetProgramInfoLog(sp, 512, &l, bytes);
		cout<<bytes<<endl;
	}

	return sp;
}

GLuint makeShaderProgram(const char* vsSrc, const char* fsSrc){
	GLuint vs = createShader(GL_VERTEX_SHADER, vsSrc);
	GLuint fs = createShader(GL_FRAGMENT_SHADER, fsSrc);
	printShaderCompilerError(vs);
	printShaderCompilerError(fs);

	GLuint sp = glCreateProgram();

	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glBindFragDataLocation(sp, 0, "outColor");
	glLinkProgram(sp);

	return sp;
}

int main()
{
	//cout<<"HEI"<<endl;
	float  t_start = (float)glfwGetTime();;
	glfwInit();
	//glfwWindowHint(GLFW_SAMPLES, 4); //Commented for clarity, probably better quality if uncommented
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL); // Windowed


	//GLFWwindow* window = glfwCreateWindow(1920, 1080, "OpenGL", glfwGetPrimaryMonitor(), NULL); // Fullscreen

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	int e = glGetError();

	if(e == GL_INVALID_ENUM)
		cout<<"glewInit() made an GL_INVALID_ENUM error"<<endl;

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);
	GLuint colorShaderProgram = makeShaderProgram(colorVertexShaderSrc, colorGeometryShaderSrc, colorFragmentShaderSrc);
	//GLuint colorShaderProgram = makeShaderProgram(colorVertexShaderSrc, colorFragmentShaderSrc);
	int count = 0;

	//shaders for shadowing
	GLuint shadowShaderProgram = makeShaderProgram(shadowVertexShaderSrc, shadowFragmentShaderSrc);
	glUseProgram(shadowShaderProgram);

	GLuint unishadowmat = glGetUniformLocation(shadowShaderProgram, "transform");

	vec3 lightInvDir = vec3(0.0f, 1.0, 0.0f);
	mat4 depthProjectionMatrix = ortho<float>(-10, 10, -10, 10, -40, 40);
	//mat4 depthProjectionMatrix = ortho<float>(-1, 1, -1, 1, -1, 1);
	mat4 depthViewMatrix = lookAt(vec3(0.0, 0.0, 0.0),-lightInvDir,  vec3(0.0, 0.0, -1.0f));
	mat4 depthMVP = depthProjectionMatrix*depthViewMatrix;
	//glUniformMatrix4fv(unishadowmat, 1, GL_FALSE, value_ptr(depthMVP));

	//Framebuffer for shadowmap
	GLuint shadowFrameBuffer;
	glGenFramebuffers(1, &shadowFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);

	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	/*GLenum DrawBuffers[1] = {GL_DEPTH_ATTACHMENT};
	glDrawBuffers(1, DrawBuffers);*/


	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout<<"Something went wrong"<<endl;
	GLuint shadowVao;
	glGenVertexArrays(1, &shadowVao);
	glBindVertexArray(shadowVao);
	GLuint shadowPosAtt = glGetAttribLocation(shadowShaderProgram, "pos");
	glEnableVertexAttribArray(shadowPosAtt);
	glVertexAttribPointer(shadowPosAtt, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

	mat4 bias = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0);
	//mat4 bias = scale(translate(mat4(), vec3(0.5, 0.5, 0.5)), vec3(0.5, 0.5, 0.5));//This is equivalent to previous line.

	mat4 biastransform = bias*depthMVP;

	//cout<<glGetString(GL_VERSION)<<endl;
	e = glGetError();
	if(e != GL_NO_ERROR){
		cout<<"Encountered ann error"<<endl;
		if(e == GL_INVALID_OPERATION)
			cout<<"This is a new GL_INVALID_ENUM error"<<endl;
	}

	glUseProgram(colorShaderProgram);


	glBindAttribLocation(colorShaderProgram, 9, "pos");
	glBindAttribLocation(colorShaderProgram, 10, "color");

	glLinkProgram(colorShaderProgram);



	GLint uniColorDepthTexture = glGetUniformLocation(colorShaderProgram, "depthTex");
	GLint uniShaderShadowmat;
	uniShaderShadowmat = glGetUniformLocation(colorShaderProgram, "shadowMat");


	//glUniformMatrix4fv(uniShaderShadowmat, 1, GL_FALSE, value_ptr(biastransform));

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, keyCallback);
	glm::mat4 view;
	glm::mat4 proj;

	double mx, my;

	float turningSpeed = 2.0f;
	vec3 position(0.0, -1.0, -2.0);
	vec3 rightdir;
	double sx = 0, sy = 0;

	float theta, phi;

	glfwGetCursorPos(window, &sx, &sy);


	//ColoredQuad quad(vec3(-20.0f, -3.0f, 20.0f), vec3(40.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -40.0f), colorShaderProgram, shadowShaderProgram);
	//ColoredPyramid pyramid(1.0f, 1.0f, 1.0f, colorShaderProgram, shadowShaderProgram);
	//GroundModel ground(20, 20, colorShaderProgram, shadowShaderProgram);

	float heightdata[4] = {1.0, 1.5, 1.0, 1.0};
	Tile tile(3, 3, heightdata, 3, colorShaderProgram, shadowShaderProgram);


	vec3 direction;
	proj = glm::perspective(45.0f, 1.0f, 0.01f, 20.0f);
	glEnable(GL_DEPTH_TEST);

	const int worldSize = 50;
	float heightTable[worldSize+ 1][worldSize +1];
	for(int i = 0; i<worldSize+1; i++){
		for(int j = 0; j<worldSize+1; j++){
			heightTable[i][j] = ((((7*i + 6*j + 3)^ 19)%23) )*0.03*0.1;
			//cout<<heightTable[i][j]<<endl;
		}
	}


	Tile tiles[worldSize*worldSize];
	for(int i = 0; i<worldSize; i++){
		for(int j = 0;j<worldSize; j++){
			float hs2[4] = {heightTable[i][j], heightTable[i+1][j], heightTable[i][j+1], heightTable[i+1][j+1]};
			tiles[i*worldSize + j] = Tile(i-worldSize/2, j-worldSize/2, hs2, 5, colorShaderProgram, shadowShaderProgram);
		}
	}

	BreakingWall bw(colorShaderProgram, shadowShaderProgram);





	float t = (float)glfwGetTime();
	float nt, dt;
	const float moveFactor = 20.0;

	const int worldMidPointX = worldSize/2;
	const int worldMidPointY = worldSize/2;
	Wall* testWall = new Wall(&tiles[(worldMidPointX+1)*worldSize + worldMidPointY], 5, WALL_TYPE_SOFT_ROCK, colorShaderProgram, shadowShaderProgram);

	const int drawDistance = 10;
	//Currently, no shadow-vao is made for the tile map. We may do this later if necessary.



	Tile currtile;
	while (!glfwWindowShouldClose(window))
	{
		//cout<<"Begin anew"<<endl;
		if(sx == 0){
			glfwGetCursorPos(window, &sx, &sy);
		}
		glfwGetCursorPos(window, &mx, &my);

		nt = (float)glfwGetTime();
		dt = nt - t;
		if(nt - (int)nt < 1/60.0)
			cout<<1/dt<<endl;
		t = nt;
		theta = turningSpeed*(float)glm::radians(mx/50 * 0.5);
		phi = turningSpeed * (float)glm::radians(my/50 * 0.5);

		direction = vec3(-cos(phi)*sin(theta), sin(phi), cos(phi)*cos(theta));
		rightdir = normalize(cross(direction, vec3(0.0, 1.0, 0.0)));

		position += moveFactor*dt*direction*(float)movingforward;
		position += moveFactor*dt*(float)(movingright)*rightdir;
		position += moveFactor*dt*vec3(0.0f, -1.0f, 0.0f)*(float)movingup;

		currtile = tiles[(int)(floor(worldMidPointX - position.x + 0.5)*worldSize + floor(worldMidPointY - position.z+0.5))];
		float* data = currtile.getVertexData();
		/*for(int i = 0; i<pow(currtile.r+1, 2); i++){
			cout<<data[i]<<", ";
		}
		cout<<endl;*/
		float fheight = getHeight(data, currtile.r, -position.x -floor(-position.x), -position.z - floor(-position.z));
		//cout<<"Height: "<<fheight<<endl;
		//float fheight = 0;
		position.y = -fheight - 0.3;

		glm::mat4 rot1 = glm::rotate(glm::mat4(), phi, glm::vec3(1, 0.0, 0));
		glm::mat4 rot2 = glm::rotate(rot1, theta, glm::vec3(0, 1.0, 0));

		view = glm::translate(rot2, position);
		mat4 pyramidTrans = translate(rotate(mat4(), t,vec3(0.0, 1.0, 0.0)), vec3(1.5, 0.0, 0.0));
		mat4 floorTrans = translate(mat4(), vec3(0.0, 2.0f*sin(t), 0.0));
		mat4 groundTrans = translate(mat4(), vec3(0.0, -2.0f, 0.0));
		//pyramid.alterData(t);

		glBindFramebuffer(GL_FRAMEBUFFER, shadowFrameBuffer);
		glViewport(0, 0, 1024, 1024);
		glUseProgram(shadowShaderProgram);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//pyramid.drawToShadowMap(depthMVP, pyramidTrans);
		//quad.drawToShadowMap(depthMVP, floorTrans);
		//ground.drawToShadowMap(depthMVP, groundTrans);
		tile.drawToShadowMap(depthMVP, mat4());
		for(int i = std::max(0, -(int)position.x + worldMidPointX -drawDistance); i<std::min(worldSize, -(int)position.x + worldMidPointX + drawDistance); i++){
			for(int j = std::max(0, -(int)position.z + worldMidPointY -drawDistance); j<std::min(worldSize, -(int)position.z + worldMidPointY + drawDistance); j++){

				tiles[i*worldSize + j].drawToShadowMap(depthMVP, mat4());
			}
		}


		/*for(int i = 0; i<4; i++){
			for(int j = 0; j<4; j++){
				cout<<depthMVP[i][j]<<" ";
			}
			cout<<endl;
		}	*/
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, 800, 600);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glUseProgram(colorShaderProgram);
		glUniform1i(uniColorDepthTexture, 0);
		glUniformMatrix4fv(uniShaderShadowmat,1, GL_FALSE, value_ptr(biastransform));


		for(int i = std::max(0, -(int)position.x + worldMidPointX -drawDistance); i<std::min(worldSize, -(int)position.x + worldMidPointX + drawDistance); i++){
			for(int j = std::max(0, -(int)position.z + worldMidPointY -drawDistance); j<std::min(worldSize, -(int)position.z + worldMidPointY + drawDistance); j++){
				//cout<<i<<" "<<j<<", ";
				if(tiles[i*worldSize + j].wall != 0){
					//cout<<"H"<<endl;
					if(t > 3 && tiles[i*worldSize + j].gravel == 0){
						tiles[i*worldSize + j].wallIsBreaking();

					}
					tiles[i*worldSize +j].tick(t);
				}
				tiles[i*worldSize + j].issueDrawing(view, mat4());
			}
		}
		tile.draw(view, mat4());
		bw.update(t - 2);
		bw.draw(view, mat4());
		//cout<<endl<<endl;
		//testWall->draw(view, mat4());
		/*fillTileElementAndVertexBuffers( tiles, 10, tileElementBuffer, tileBufferData, 0, 0, 10, 10);
		glBindBuffer(GL_ARRAY_BUFFER, tileVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, MAX_TILES*MAX_RES*MAX_RES*7*sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, 10*10*sizeof(GLfloat)*(tiles[0].getResolution()+1)*(tiles[0].getResolution()+1)*tiles[0].getFloatsPerVertex(), tileBufferData);
		glBindVertexArray(tileVertexArrayObject);
		glDrawElements(GL_TRIANGLES, 10*10*2*3*res*res,//??
			GL_UNSIGNED_SHORT, tileElementBuffer);*/


		glfwSwapBuffers(window);

		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}


	//std::this_thread::sleep_for(std::chrono::seconds(3));
	glfwTerminate();
}
