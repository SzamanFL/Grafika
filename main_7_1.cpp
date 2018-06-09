#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programTexture;
glm::vec3 tabl[10];
glm::vec3 tabl1[50];
glm::vec3 tabl2[10];
int tabb[500];
Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sharkModel;
obj::Model sphereModel;
obj::Model groundModel;
obj::Model seaweedModel;

glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
glm::vec3 cameraVer; // Wektor "w góre" kamery
float cameraAngle = 0;
float cameraAngle1 = 0;

glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
glm::vec3 lightDir1 = glm::normalize(glm::vec3(-1.0f, -0.9f, 1.0f));

glm::quat rotation = glm::quat(1, 0, 0, 0);

GLuint textureAsteroid;
GLuint textureGround;
GLuint textureSeaweed;

void keyboard(unsigned char key, int x, int y)
{
	
	float angleSpeed = 0.1f;
	float angleSpeed1 = 0.2f;
	float moveSpeed = 0.1f;
	switch(key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'q': cameraPos -= cameraVer * moveSpeed; break;
	case 'e': cameraPos += cameraVer * moveSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
}

glm::mat4 createCameraMatrix()
{
	cameraDir = glm::vec3(cosf(cameraAngle - glm::radians(90.0f)), 0.0f, sinf(cameraAngle - glm::radians(90.0f)));
	glm::vec3 up = glm::vec3(0, 1, 0);
	cameraVer = glm::vec3(0.0f, sinf(cameraAngle1 - glm::radians(90.0f)), 0.0f);
	cameraSide = glm::cross(cameraDir, up);

	return Core::createViewMatrix(cameraPos, cameraDir, -cameraVer);
}

void drawObjectColor(obj::Model * model, glm::mat4 modelMatrix, glm::vec3 color)
{
	GLuint program = programColor;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightDir1"), lightDir1.x, lightDir1.y, lightDir1.z);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}

void drawObjectTexture(obj::Model * model, glm::mat4 modelMatrix, GLuint textureId)
{
	GLuint program = programTexture;

	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightDir1"), lightDir1.x, lightDir1.y, lightDir1.z);
	Core::SetActiveTexture(textureId, "textureSampler", program, 0);

	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelViewProjectionMatrix"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	Core::DrawModel(model);

	glUseProgram(0);
}
void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0,-0.25f,0)) * glm::rotate(glm::radians(180.0f), glm::vec3(0,1,0)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos  +cameraDir * 0.5f) * glm::rotate(-cameraAngle, glm::vec3(0,1,0)) * shipInitialTransformation;
	
	drawObjectColor(&shipModel, shipModelMatrix, glm::vec3(0.6f));
	drawObjectColor(&groundModel, glm::translate(glm::vec3(0, -40, 0)), glm::vec3(0.2f, 0.05f, 0.2f));

	//drawObjectTexture(&sphereModel, glm::translate(glm::vec3(0,0,0)), textureAsteroid);
	for (int i = 0; i < 10; i++)
	{
		drawObjectTexture(&sphereModel, glm::translate(tabl[i]), textureAsteroid);
		//drawObjectColor(&sharkModel, glm::translate(tabl[i]), glm::vec3(0.2f, 1.0f, 0.2f));
		//drawObjectTexture(&groundModel, glm::translate(tabl1[i]), textureGround);
	}
	for (int j = 0; j < 5; j++)
	{
		drawObjectColor(&sharkModel, glm::translate(tabl2[j]), glm::vec3(0.2f, 0.20f, 0.2f));
	}
	for (int s = 1; s < 500; s++)
		 {
				//zmienic tutaj na wodorosty
			drawObjectColor(&seaweedModel, glm::translate(glm::vec3(tabb[s], -20, tabb[s - 1])), glm::vec3(0.2f,1.0f,0.2f));
			//drawObjectTexture(&seaweedModel, glm::translate(glm::vec3(tabb[i], -20, tabb[i - 1])), textureSeaweed);
		}
		//poruszajace sie
		float timee = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	int czas = -40 + timee * 2;
	if (-40 + timee * 2 <= 0) {
		czas = 10000;
		
	}
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(0, 0, czas)), textureAsteroid);
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(2, 0, -42 + timee * 2)), textureAsteroid);
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(-2, 0, -42 + timee * 2)), textureAsteroid);
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(-3, 0, -44 + timee * 2)), textureAsteroid);
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(0, 0, -44 + timee * 2)), textureAsteroid);
	drawObjectTexture(&sphereModel, glm::translate(glm::vec3(3, 0, -44 + timee * 2)), textureAsteroid);
	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	for (int i = 0; i < 10 ;i++)
	{
		tabl[i] = glm::ballRand(20.0f);
	}
	for (int j = 0; j < 10; j++)
	{
		tabl1[j] = glm::ballRand(20.0f);
	}
	for (int s = 0; s < 5; s++)
	{
		tabl2[s] = glm::ballRand(60.0f);
	}
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	groundModel = obj::loadModelFromFile("models/Rockwall.obj");
	textureGround = Core::LoadTexture("textures/droptomaterial,color,bump,displacement.jpg");

	seaweedModel = obj::loadModelFromFile("models/Seaweed.obj");
	textureSeaweed = Core::LoadTexture("textures/seaweed.png");

	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	sharkModel = obj::loadModelFromFile("models/GreatWhite.obj");

	sphereModel = obj::loadModelFromFile("models/fish.obj");
	textureAsteroid = Core::LoadTexture("textures/fish_texture.png");
}

void shutdown()
{
	shaderLoader.DeleteProgram(programColor);
	shaderLoader.DeleteProgram(programTexture);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char ** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
