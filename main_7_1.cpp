#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"

GLuint programColor;
GLuint programTexture;
glm::vec3 tabl[200];
glm::vec3 fishtab[200];
glm::vec3 tabl2[10];
glm::vec3 coord[9];
int counter;
int cont;
int acont;
int counter1;
int cont1;
int acont1;
glm::vec3 sharkcoor;
int tabb[300];
float timee[9];
Core::Shader_Loader shaderLoader;

obj::Model shipModel;
obj::Model sharkModel;
obj::Model fishModel;
obj::Model groundModel;
obj::Model seaweedModel;

glm::vec3 cameraPos = glm::vec3(0, 0, 5);
glm::vec3 cameraDir; // Wektor "do przodu" kamery
glm::vec3 cameraSide; // Wektor "w bok" kamery
glm::vec3 cameraVer; // Wektor "w góre" kamery
float cameraAngle = 0;
float cameraAngle1 = 0;
float poprzedniaPozycjaMyszki[2] = { 0,0 };
float roznicaMyszki[2] = { 0,0 };
glm::mat4 cameraMatrix, perspectiveMatrix;

glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, -0.9f, -1.0f));
glm::vec3 lightDir1 = glm::normalize(glm::vec3(-1.0f, -0.9f, 1.0f));
glm::vec3 sharkLoc;
glm::quat rotation = glm::quat(1, 0, 0, 0);

GLuint textureAsteroid;
GLuint textureGround;
GLuint textureSeaweed;
GLuint textureShark;
float timeq;
float timefish;
float timeshark;
int pomoc;
void keyboard(unsigned char key, int x, int y)
{
	
	float angleSpeed = 0.1f;
	float angleSpeed1 = 0.5f;
	float moveSpeed = 0.7f;
	switch(key)
	{
		//na boki
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
		//góra dół
	case 'q': cameraPos -= cameraVer * moveSpeed; break;
	case 'e': cameraPos += cameraVer * moveSpeed; break;
	// lewo prawo
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += cameraSide * moveSpeed; break;
	case 'a': cameraPos -= cameraSide * moveSpeed; break;
	}
}

void mouse(int x, int y)
{
	float nowaPozycja[2] = { x, y };
	roznicaMyszki[0] = poprzedniaPozycjaMyszki[0] - nowaPozycja[0];
	roznicaMyszki[1] = poprzedniaPozycjaMyszki[1] - nowaPozycja[1];
	poprzedniaPozycjaMyszki[0] = nowaPozycja[0];
	poprzedniaPozycjaMyszki[1] = nowaPozycja[1];
}

glm::mat4 createCameraMatrix()
{
	/*glm::quat obrotY = glm::angleAxis(roznicaMyszki[0] * 0.005f, glm::vec3(0, 1, 0));
	glm::quat obrotX = glm::angleAxis(roznicaMyszki[1] * 0.005f, glm::vec3(1, 0, 0));

	glm::quat rotationChange = obrotX * obrotY;

	rotation = rotationChange * rotation;
	rotation = glm::normalize(rotation);

	roznicaMyszki[0] = 0;
	roznicaMyszki[1] = 0;

	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	cameraSide = glm::inverse(rotation) * glm::vec3(1, 0, 0);

	return Core::createViewMatrixQuat(cameraPos, rotation);*/
	
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
	glClearColor(0.2021f, 0.20f, 0.0128f, 1.0f);

	//glm::mat4 shipRotation = glm::mat4_cast(glm::inverse(rotation));

	glm::mat4 shipInitialTransformation = glm::translate(glm::vec3(0,-2.25f,-5)) * glm::rotate(glm::radians(180.0f), glm::vec3(0,1,-0.1)) * glm::scale(glm::vec3(0.25f));
	glm::mat4 shipModelMatrix = glm::translate(cameraPos  +cameraDir * 0.5f) * glm::rotate(-cameraAngle, glm::vec3(0,1,0)) * shipInitialTransformation;
	//glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f) * shipRotation * shipInitialTransformation;
	
	timeq = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	if (timefish > 30)
		timefish = (glutGet(GLUT_ELAPSED_TIME) / 1000.0f - 4) - glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	else
		timefish = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	drawObjectTexture(&sharkModel, shipModelMatrix, textureShark);
	drawObjectTexture(&groundModel, glm::translate(glm::vec3(0, -100, 0)), textureGround);
	//pierwsza partia ryb
	for (int i = 0; i < 100; i++)
	{
		tabl[i].z = tabl[i].z +(timefish*0.01f);
		if (counter < 20)
		{
			tabl[i].x = tabl[i].x - (sinf(timefish * 0.001f));
			tabl[i].y = tabl[i].y - (sinf(timefish * 0.001f));
		}
		else
		{
			tabl[i].x = tabl[i].x + (sinf(timefish * 0.001f));
			tabl[i].y = tabl[i].y + (sinf(timefish * 0.001f));
		}
		if (counter == 40)
			counter = 0;
		if (acont % 2000 == 0)
		{
			tabl[i].z = -110 - tabl[i].z;
			acont = 0;
		}
		drawObjectTexture(&fishModel, glm::translate(tabl[i]), textureAsteroid);
	}
	for (int i = 100; i < 200; i++)
	{
		tabl[i].z = tabl[i].z - (timefish*0.01f);
		if (counter < 20)
		{
			tabl[i].x = tabl[i].x + (sinf(timefish * 0.001f));
			tabl[i].y = tabl[i].y + (sinf(timefish * 0.001f));
		}
		else
		{
			tabl[i].x = tabl[i].x - (sinf(timefish * 0.001f));
			tabl[i].y = tabl[i].y - (sinf(timefish * 0.001f));
		}
		if (counter == 40)
			counter = 0;
		if (cont % 2000 == 0)
		{
			tabl[i].z = 70 - tabl[i].z;
			cont = 0;
		}
		drawObjectTexture(&fishModel, glm::translate(tabl[i]) * glm::rotate(glm::radians(180.0f), glm::vec3(0, 1, 0)), textureAsteroid);
	}
	counter++;
	cont++;
	acont++;
	
	//druga partia ryb
	for (int i = 0; i < 100; i++)
	{
		fishtab[i].x = fishtab[i].x + (timefish*0.005f);
		if (counter1 < 20)
		{
			fishtab[i].y = fishtab[i].y - (sinf(timefish * 0.001f));
			fishtab[i].z = fishtab[i].z - (sinf(timefish * 0.001f));
		}
		else
		{
			fishtab[i].y = fishtab[i].y + (sinf(timefish * 0.001f));
			fishtab[i].z = fishtab[i].z + (sinf(timefish * 0.001f));
		}
		if (counter1 == 40)
			counter1 = 0;
		if (acont1 % 2500 == 0)
		{
			fishtab[i].x = -120 - fishtab[i].x;
			acont1 = 0;
		}
		drawObjectTexture(&fishModel, glm::translate(fishtab[i]) * glm::rotate(glm::radians(90.0f), glm::vec3(0, 1, 0)), textureAsteroid);
	}
	for (int i = 100; i < 200; i++)
	{
		fishtab[i].x = fishtab[i].x - (timefish*0.005f);
		if (counter1 < 20)
		{
			fishtab[i].y = fishtab[i].y + (sinf(timefish * 0.001f));
			fishtab[i].z = fishtab[i].z + (sinf(timefish * 0.001f));
		}
		else
		{
			fishtab[i].y = fishtab[i].y - (sinf(timefish * 0.001f));
			fishtab[i].z = fishtab[i].z - (sinf(timefish * 0.001f));
		}
		if (counter1 == 40)
			counter1 = 0;
		if (cont1 % 2500 == 0)
		{
			fishtab[i].x = 90 - fishtab[i].x;
			cont1 = 0;
		}
		drawObjectTexture(&fishModel, glm::translate(fishtab[i]) * glm::rotate(glm::radians(270.0f), glm::vec3(0, 1, 0)), textureAsteroid);
	}
	counter1++;
	cont1++;
	acont1++;
	timeshark = timeq / 4;
	
	for (int j = 0; j < 5; j++)
	{
		sharkLoc.x = 0;
		sharkLoc.y = tabl2[j].y;
		sharkLoc.z = tabl2[j].z + 15;
		if (sharkLoc.z < 0)
			drawObjectTexture(&sharkModel,
				glm::translate(glm::vec3(tabl2[j])) *
				glm::rotate(glm::radians(timeshark * 360.0f), glm::vec3(0, 1, 0)) *
				glm::translate(sharkLoc) *
				glm::rotate(glm::radians(270.f), glm::vec3(0, 1, 0)),
				textureShark);
		else
			drawObjectTexture(&sharkModel, 
				glm::translate(glm::vec3(tabl2[j])) * 
				glm::rotate(glm::radians(timeshark * 360.0f), glm::vec3(0, 1, 0)) * 
				glm::translate(sharkLoc) *
				glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0)),
				textureShark);
		sharkLoc = glm::vec3(0, 0, 0 );
		
	}
	//sekcja wodorostow
	for (int s = 1; s < 150; s++)
	{
				//zmienic tutaj na wodorosty
		drawObjectTexture(&seaweedModel, glm::translate(glm::vec3(tabb[s] * pow(-1,s), -100, tabb[s - 1] * pow(-1, s))), textureSeaweed);
	}
	
	for (int e = 150; e < 225; e++)
	{
		//zmienic tutaj na wodorosty
		drawObjectTexture(&seaweedModel, glm::translate(glm::vec3(tabb[e-1], -100, tabb[e] * pow(-1, e))), textureSeaweed);

	}
	for (int e = 225; e < 300; e++)
	{
		//zmienic tutaj na wodorosty
		drawObjectTexture(&seaweedModel, glm::translate(glm::vec3(tabb[e - 1] * pow(-1, e), -100, tabb[e] )), textureSeaweed);
	}
		//poruszajace sie


	//sekcja pościgu rekina za rybami
	for (int w = 0; w < 9; w++)
	{
		if (timee[w] < 1000)
			timee[w] = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
		else
			coord[w] = glm::vec3(0, -80, 0);
	}

	
	sharkcoor = glm::vec3(-2 * sinf(timeq * 0.7), 1 * cosf(timeq * 0.8), -100 + timeq * 5);


	coord[0] = glm::vec3(2 * sinf(timeq * 0.2), 1 * sinf(timeq), -42 + timee[0] * 3);
	coord[1] = glm::vec3(-2, 1 * cosf(timeq*0.4f), -42 + timee[1] * 3);
	coord[2] = glm::vec3(-3 * cosf(timeq * 2), 2 * sinf(timeq*0.9), -49 + timee[2] * 4);
	coord[3] = glm::vec3(0, 0, -45 + timee[3] * 5);
	coord[4] = glm::vec3(6 * sinf(timeq*0.2), -3 * cosf(timeq * 2), -48 + timee[4] * 4);
	coord[5] = glm::vec3(4, 7 * cosf(timeq * 1.8), -39 + timee[5] * 2);
	coord[6] = glm::vec3(-4, 5 * sinf(timeq * 0.5), -44 + timee[6] * 3);
	coord[7] = glm::vec3(-7 * cosf(timeq * 0.8), 4, -44 + timee[7] * 2);
	coord[8] = glm::vec3(-5 * sinf(timeq * 0.1), 7 * cosf(timeq * 0.8), -44 + timee[8] * 2);

	drawObjectTexture(&fishModel, glm::translate(coord[0]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[1]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[2]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[3]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[4]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[5]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[6]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[7]), textureAsteroid);
	drawObjectTexture(&fishModel, glm::translate(coord[8]), textureAsteroid);
	drawObjectTexture(&sharkModel, glm::translate(sharkcoor),textureShark);

	for (int t = 0; t < 9; t++)
	{
	if (abs(sharkcoor.x - coord[t].x)  < 2 && abs(sharkcoor.y - coord[t].y) < 4 && abs(sharkcoor.z - coord[t].z)  < 0.4)
		timee[t] = 10000;
	}
	
	glutSwapBuffers();
}

void init()
{
	srand(time(0));
	glEnable(GL_DEPTH_TEST);
	counter = 0;
	cont = 0;
	acont = 0;
	counter1 = 0;
	cont1 = 0;
	acont1 = 0;
	//Ryby
	for (int i = 0; i < 200 ;i++)
	{
		tabl[i] = glm::ballRand(110.0f);
	}
	for (int i = 0; i < 200; i++)
	{
		fishtab[i] = glm::ballRand(110.0f);
	}
	/*for (int j = 0; j < 10; j++)
	{
		tabl1[j] = rand() % 10;
	}*/
	//Rekiny
	for (int s = 0; s < 5; s++) //rekiny
	{	
		tabl2[s] = glm::ballRand(80.0f);
	}
	//Wodorosty
	for (int q = 0; q < 300; q++)
	{
		tabb[q] = rand() % 100 + 1;
	}
	//Czas ryb
	for (int q = 0; q < 9; q++)
	{
		timee[q] = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	}
	programColor = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programTexture = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	groundModel = obj::loadModelFromFile("models/Rockwall.obj");
	textureGround = Core::LoadTexture("textures/ground.png");

	seaweedModel = obj::loadModelFromFile("models/Grass.obj");
	textureSeaweed = Core::LoadTexture("textures/Grass.png");

	shipModel = obj::loadModelFromFile("models/spaceship.obj");
	sharkModel = obj::loadModelFromFile("models/GreatWhite.obj");
	textureShark = Core::LoadTexture("textures/shark.png");

	fishModel = obj::loadModelFromFile("models/fish.obj");
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
	glutInitWindowSize(800, 800);
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
