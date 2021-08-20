#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

// Font rendering
#include "Headers/FontTypeRendering.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// Include Shadowbox
#include "Headers/ShadowBox.h"

// OpenAL include
#include <AL/alut.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesFountain;
Shader shaderParticlesExplosion;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundiad
Shader shaderViewDepth;
//Shader pra dibujar el buffer de profundidad
Shader shaderDepth;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

ShadowBox* shadowBox;

Cylinder cylinderRay(10, 10, 0.05, 0.05);

// Lamps
Model modelLamp1;
Model modelLamp2;
Model modelLampPost2;
// Hierba
Model modelGrass_1;
Model modelGrass_2;
Model modelGrass_3;
// Fountain
Model modelFountain;

// Modelos de Oscar

// Cofre 
Model modelCofre;

//Tree
Model modelTree;

// Spyro
Model spyroModelAnimate;
// Diamante
Model diamanteModelAnimate;
// Enemigo
Model enemigoModelAnimate;
// Terrain model instance
Terrain terrain(-1, -1, 200, 20, "../Textures/Height_Map_OMT.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleExplosionID, textureParticleFireID, texId;
GLuint skyboxTextureID;

// Modelo para el redener de texto
FontTypeRendering::FontTypeRendering* modelText;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/grass_skybox/sh_ft.png",
		"../Textures/grass_skybox/sh_bk.png",
		"../Textures/grass_skybox/sh_up.png",
		"../Textures/grass_skybox/sh_dn.png",
		"../Textures/grass_skybox/sh_rt.png",
		"../Textures/grass_skybox/sh_lf.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixSpyro = glm::mat4(1.0f);
glm::mat4 modelMatrixActiveExplosion = glm::mat4(1.0f);
// Enemigos Definicion
std::vector<glm::mat4> modelMatrixEnemigoArray = { 
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)
};
std::vector<glm::vec3> EnemigoInitialPosition = { 
	glm::vec3( 40.0, 0, -40.0), glm::vec3( 40.0, 0,   0.0), glm::vec3( 40.0, 0,  40.0), glm::vec3(  0.0, 0, -40.0), 
	glm::vec3(-20.0, 0,  40.0), glm::vec3(-40.0, 0, -40.0), glm::vec3(-60.0, 0, -20.0), glm::vec3(-60.0, 0,  40.0),

	glm::vec3(80.0, 0, -40.0), glm::vec3( 80.0, 0,   0.0), glm::vec3( 60.0, 0,  40.0), glm::vec3(  0.0, 0, -60.0),
	glm::vec3(20.0, 0,  40.0), glm::vec3(-60.0, 0, -40.0), glm::vec3(-40.0, 0, -20.0), glm::vec3(-40.0, 0,  60.0),

	glm::vec3(40.0, 0, -80.0), glm::vec3( 60.0, 0, -20.0), glm::vec3( 40.0, 0, 60.0), glm::vec3( 20.0, 0, -60.0),
	glm::vec3( 0.0, 0,  80.0), glm::vec3(-40.0, 0, -60.0), glm::vec3(-80.0, 0,  0.0), glm::vec3(-60.0, 0,  60.0),

	glm::vec3(80.0, 0, -80.0), glm::vec3( 60.0, 0,  20.0), glm::vec3( 60.0, 0, 60.0), glm::vec3(-20.0, 0, -60.0),
	glm::vec3( 0.0, 0,  60.0), glm::vec3(-40.0, 0, -80.0), glm::vec3(-40.0, 0,  0.0), glm::vec3(-40.0, 0,  80.0)
};
std::vector<int> enemigo_anim_index_Array = { 
	2,2,2,2, 2,2,2,2,
	2,2,2,2, 2,2,2,2,
	2,2,2,2, 2,2,2,2,
	2,2,2,2, 2,2,2,2
};

std::vector<bool> enemigo_isVivo_Array = {
	true,true,true,true, true,true,true,true,
	true,true,true,true, true,true,true,true,
	true,true,true,true, true,true,true,true,
	true,true,true,true, true,true,true,true
};
// Cofres Definicion
std::vector<glm::mat4> modelMatrixCofresArray = {
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), 
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)
};
std::vector<glm::vec3> CofresInitialPosition = {
	glm::vec3(60.0, 0, -80.0), glm::vec3(60.0, 0,   0.0), glm::vec3(80.0, 0,  80.0), glm::vec3(0.0, 0, -80.0),
	glm::vec3(0.0, 0,  40.0), glm::vec3(-80.0, 0, -40.0), glm::vec3(-80.0, 0, -20.0), glm::vec3(-40.0, 0,  40.0)
};
std::vector<bool> cofres_isVivo_Array = {
	true,true,true,true,
	true,true,true,true
};

// Diamante positions
std::vector<glm::mat4> modelMatrixDiamantesArray = {
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)
};
std::vector<glm::vec3>  DiamantesInitialPosition = {
	glm::vec3(10.0f, 0.0f, -15.0f), glm::vec3(60.0f, 0.0f, 10.0f),glm::vec3(-10.0f, 0.0f, -84.0f), glm::vec3(-60.0f, 0.0f, 10.0f),
	glm::vec3(15.0f, 0.0f, -35.0f), glm::vec3(70.0f, 0.0f, 65.0f),glm::vec3(-20.0f, 0.0f, -90.0f), glm::vec3(-70.0f, 0.0f, 41.0f),
	glm::vec3(30.0f, 0.0f, -90.0f), glm::vec3(80.0f, 0.0f, 71.0f),glm::vec3(-30.0f, 0.0f, -65.0f), glm::vec3(-80.0f, 0.0f, 87.0f),
	glm::vec3(40.0f, 0.0f, -10.0f), glm::vec3(90.0f, 0.0f, 25.0f),glm::vec3(-40.0f, 0.0f, -35.0f), glm::vec3(-90.0f, 0.0f, 50.0f),
	glm::vec3(50.0f, 0.0f, -80.0f), glm::vec3(75.0f, 0.0f, 11.0f),glm::vec3(-50.0f, 0.0f, -10.0f), glm::vec3(-90.0f, 0.0f, 27.0f)
};
std::vector<bool> diamantes_isVivo_Array = {
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true,
	true,true,true,true
};

// Tree definition
std::vector<glm::mat4> modelMatrixTreesArray = {
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f),
	glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f)
};
std::vector<glm::vec3>  TreesInitialPosition = {
	glm::vec3(56.0f, 0.0f, -44.0f), glm::vec3(70.0f, 0.0f, 36.0f),glm::vec3(-53.0f, 0.0f, -44.0f), glm::vec3(-28.0f, 0.0f, 40.0f),
	glm::vec3(46.0f, 0.0f, -10.0f), glm::vec3(73.0f, 0.0f, 74.0f),glm::vec3(-43.0f, 0.0f, -22.0f), glm::vec3(-28.0f, 0.0f, 45.0f),
	glm::vec3(36.0f, 0.0f, -47.0f), glm::vec3(32.0f, 0.0f, 11.0f),glm::vec3(-33.0f, 0.0f, -22.0f), glm::vec3(-68.0f, 0.0f, 11.0f),
	glm::vec3(26.0f, 0.0f, -16.0f), glm::vec3(25.0f, 0.0f, 22.0f),glm::vec3(-23.0f, 0.0f, -45.0f), glm::vec3(-48.0f, 0.0f, 66.0f),
	glm::vec3(16.0f, 0.0f, -10.0f), glm::vec3(45.0f, 0.0f, 66.0f),glm::vec3(-13.0f, 0.0f, -20.0f), glm::vec3(-48.0f, 0.0f, 22.0f)
};
std::vector<float>  TreesInitialHeight = {
	1.0f, 0.9f, 0.4f, 0.6f,
	0.8f, 0.4f, 0.4f, 0.5f,
	0.6f, 0.9f, 1.0f, 0.6f,
	0.2f, 0.9f, 0.3f, 1.0f,
	0.4f, 1.0f, 0.5, 0.6f,
};
 // Hierba
std::vector<glm::vec3>  grassInitialPosition = {
	glm::vec3(56.0f, 0.0f, -44.0f), glm::vec3(70.0f, 0.0f, 36.0f),glm::vec3(-53.0f, 0.0f, -44.0f), glm::vec3(-28.0f, 0.0f, 40.0f),
	glm::vec3(46.0f, 0.0f, -10.0f), glm::vec3(73.0f, 0.0f, 74.0f),glm::vec3(-43.0f, 0.0f, -22.0f), glm::vec3(-28.0f, 0.0f, 45.0f),
	glm::vec3(36.0f, 0.0f, -47.0f), glm::vec3(32.0f, 0.0f, 11.0f),glm::vec3(-33.0f, 0.0f, -22.0f), glm::vec3(-68.0f, 0.0f, 11.0f),
	glm::vec3(26.0f, 0.0f, -16.0f), glm::vec3(25.0f, 0.0f, 22.0f),glm::vec3(-23.0f, 0.0f, -45.0f), glm::vec3(-48.0f, 0.0f, 66.0f),
	glm::vec3(16.0f, 0.0f, -10.0f), glm::vec3(45.0f, 0.0f, 66.0f),glm::vec3(-13.0f, 0.0f, -20.0f), glm::vec3(-48.0f, 0.0f, 22.0f),
	glm::vec3( 0.0f, 0.0f,   0.0f), glm::vec3(10.0f, 0.0f, 10.0f),glm::vec3(-20.0f, 0.0f, -20.0f), glm::vec3(-30.0, 0.0f, 30.0f),
	glm::vec3(40.0f, 0.0f,   40.0f), glm::vec3(50.0f, 0.0f, 40.0f),glm::vec3(-60.0f, 0.0f, -30.0f), glm::vec3(-50.0, 0.0f, 0.0f)
};

int animationIndex = 1;
int enemigo_anim_index = 2;
int modelSelected = 0;
bool enableCountSelected = true;

//Variables para el movimiento del enemigo
glm::vec3 vectorDirectorEnemigo;

//Variables para el reinicio de spyro
glm::vec3 vectorDirectorSpyro;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;


// Var animate helicopter
float rotHelHelY = 0.0;

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"explosion", glm::vec3(10.0, 2.0, 17.5)},
		{"fire", glm::vec3(0.0, 0.0, 7.0)}
};

double deltaTime;
double currTime, lastTime;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Diamante contador
int diamantesCount = 0;
int diamantesTotal = 400;

// Enemigos contador
int enemigosCount = 0;

// Life Spyro contador
float spyroLifeCount = 100.0f;
bool spyroAlive = true;
bool gameOver = true;
bool restarting = false;
float restartingCount = 0.0f;
bool ready2Play = true;

//Run Variables
bool isRun = false, isWalk = false;
float SpyroSpeed = 0.08f;
float SpyroWalkSpeed = 0.08f;
float SpyroRunSpeed = 0.25f;

// Jump Variables
bool isJump = false;
float gravedad = 1.81;
double tmv = 0;
double startTimeJump = 0;
// Shot Variables
bool isShot = false;
double tmv_shot = 0;
double startTimeShot = 0;

// Explosion variable
bool enableExplosion = false;
float contadorExplosion = 0.0f;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

 // OpenAL Defines
#define NUM_BUFFERS 8
#define NUM_SOURCES 46
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { 0.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 0.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source Ambient
ALfloat sourceAmbientPos[] = { 0.0, 0.0, 0.0 };
ALfloat sourceAmbientVel[] = { 0.0, 0.0, 0.0 };

// Source Enemigo 
ALfloat sourceCofreVel[] = { 0.0, 0.0, 0.0 };
ALfloat sourceEnemigoVel[] = { 0.0, 0.0, 0.0 };

// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = { 
	true, true, /* 0: billie 1: fire */
	true,true,true,true, /* Cofres */
	true,true,true,true,
	true, true, true, true,		true, true, true, true, /* Enemigos */
	true, true, true, true,		true, true, true, true,
	true, true, true, true,		true, true, true, true,
	true, true, true, true,		true, true, true, true
};

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
bool buscaColliderOBB(std::string nombre, std::string second_nombre);
bool buscaColliderSBB(std::string nombre, std::string second_nombre);
bool restartSpyroInitialPosition();
void restartAllPositions();

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for(unsigned int i = 0; i < nParticlesFire; i++ ) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine {};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesExplosion.initialize("../Shaders/particlesExplosion.vs", "../Shaders/particlesExplosion.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	cylinderRay.init();
	cylinderRay.setShader(&shader);
	cylinderRay.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Lamp models
	modelLamp1.loadModel("../models/Street-Lamp-Black/objLamp.obj");
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.loadModel("../models/Street_Light/Lamp.obj");
	modelLamp2.setShader(&shaderMulLighting);
	modelLampPost2.loadModel("../models/Street_Light/LampPost.obj");
	modelLampPost2.setShader(&shaderMulLighting);

	//Grass
	modelGrass_1.loadModel("../models/grass2/grass_1.obj");
	modelGrass_1.setShader(&shaderMulLighting);
	modelGrass_2.loadModel("../models/grass2/grass_2.obj");
	modelGrass_2.setShader(&shaderMulLighting);
	modelGrass_3.loadModel("../models/grass2/grass_3.obj");
	modelGrass_3.setShader(&shaderMulLighting);


	//Fountain
	modelFountain.loadModel("../models/fountain/fountain.obj");
	modelFountain.setShader(&shaderMulLighting);

	//Cofre
	modelCofre.loadModel("../models/cofre/cofre.fbx");
	modelCofre.setShader(&shaderMulLighting);

	//Tree
	modelTree.loadModel("../models/tree2/tree2.fbx");
	modelTree.setShader(&shaderMulLighting);

	//Spyro
	spyroModelAnimate.loadModel("../models/Spyro/spyro.fbx");
	spyroModelAnimate.setShader(&shaderMulLighting);

	//Diamante
	diamanteModelAnimate.loadModel("../models/Diamante/diamante.fbx");
	diamanteModelAnimate.setShader(&shaderMulLighting);

	//Enemigo
	enemigoModelAnimate.loadModel("../models/Enemigo/Enemigo.fbx");
	enemigoModelAnimate.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/OMT_grass.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/OMT_path.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/OMT_snow.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/OMT_cobblestone.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/Blend_Map_OMT.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	Texture textureParticlesFountain("../Textures/bluewater.png");
	bitmap = textureParticlesFountain.loadImage();
	data = textureParticlesFountain.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFountainID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesFountain.freeImage(bitmap);

	Texture textureParticlesExplosion("../Textures/fire.png");
	bitmap = textureParticlesExplosion.loadImage();
	data = textureParticlesExplosion.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleExplosionID);
	glBindTexture(GL_TEXTURE_2D, textureParticleExplosionID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticlesExplosion.freeImage(bitmap);

	Texture textureParticleFire("../Textures/fire.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0, 1, 0);
	n = glm::cross(glm::vec3(1, 0, 0), v);
	if (glm::length(n) < 0.00001f) {
		n = glm::cross(glm::vec3(0, 1, 0), v);
	}
	u = glm::cross(v, n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/BillieVsZombies.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/fire.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/Wind.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/spyroWalk.wav");
	buffer[4] = alutCreateBufferFromFile("../sounds/spyroRun.wav");
	buffer[5] = alutCreateBufferFromFile("../sounds/AmbientSound.wav");
	buffer[6] = alutCreateBufferFromFile("../sounds/Treasure.wav");
	buffer[7] = alutCreateBufferFromFile("../sounds/zombie_groan.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	// Billie Vs Zombies
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 0.15f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	// Fire
	source1Pos[0] = modelMatrixSpyro[3].x;
	source1Pos[1] = modelMatrixSpyro[3].y;
	source1Pos[2] = modelMatrixSpyro[3].z;
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 0.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	// Wind
	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.15f);
	alSourcefv(source[2], AL_POSITION, source0Pos);
	alSourcefv(source[2], AL_VELOCITY, source0Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 2000);

	// Walk
	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.15f);
	alSourcefv(source[3], AL_POSITION, source0Pos);
	alSourcefv(source[3], AL_VELOCITY, source0Vel);
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_TRUE);
	alSourcef(source[3], AL_MAX_DISTANCE, 2000);

	// Run
	alSourcef(source[4], AL_PITCH, 1.0f);
	alSourcef(source[4], AL_GAIN, 0.15f);
	alSourcefv(source[4], AL_POSITION, source0Pos);
	alSourcefv(source[4], AL_VELOCITY, source0Vel);
	alSourcei(source[4], AL_BUFFER, buffer[4]);
	alSourcei(source[4], AL_LOOPING, AL_TRUE);
	alSourcef(source[4], AL_MAX_DISTANCE, 2000);

	// Ambient
	alSourcef(source[5], AL_PITCH, 1.0f);
	alSourcef(source[5], AL_GAIN, 0.15f);
	alSourcefv(source[5], AL_POSITION, source0Pos);
	alSourcefv(source[5], AL_VELOCITY, source0Vel);
	alSourcei(source[5], AL_BUFFER, buffer[5]);
	alSourcei(source[5], AL_LOOPING, AL_TRUE);
	alSourcef(source[5], AL_MAX_DISTANCE, 2000);

	// Cofres
	for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
		ALfloat sourceCofrePos[] = { 0.0, 0.0, 0.0 };
		sourceCofrePos[0] = CofresInitialPosition[i][0];
		sourceCofrePos[1] = CofresInitialPosition[i][1];
		sourceCofrePos[2] = CofresInitialPosition[i][2];
		int indice = i + 6;
		alSourcef(source[indice], AL_PITCH, 1.0f);
		alSourcef(source[indice], AL_GAIN, 0.5f);
		alSourcefv(source[indice], AL_POSITION, sourceCofrePos);
		alSourcefv(source[indice], AL_VELOCITY, sourceEnemigoVel);
		alSourcei(source[indice], AL_BUFFER, buffer[6]);
		alSourcei(source[indice], AL_LOOPING, AL_TRUE);
		alSourcef(source[indice], AL_MAX_DISTANCE, 500);
	}

	// Enemigos
	for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
		ALfloat sourceEnemigoPos[] = { 0.0, 0.0, 0.0 };
		sourceEnemigoPos[0] = modelMatrixEnemigoArray[i][3][0];
		sourceEnemigoPos[1] = modelMatrixEnemigoArray[i][3][1];
		sourceEnemigoPos[2] = modelMatrixEnemigoArray[i][3][2];
		int indice = i + 6 + modelMatrixCofresArray.size();
		alSourcef(source[indice], AL_PITCH, 1.0f);
		alSourcef(source[indice], AL_GAIN, 1.0f);
		alSourcefv(source[indice], AL_POSITION, sourceEnemigoPos);
		alSourcefv(source[indice], AL_VELOCITY, sourceEnemigoVel);
		alSourcei(source[indice], AL_BUFFER, buffer[7]);
		alSourcei(source[indice], AL_LOOPING, AL_TRUE);
		alSourcef(source[indice], AL_MAX_DISTANCE, 50);
	}

	// Se inicializa el modelo de texeles.
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth,
		screenHeight);
	modelText->Initialize();
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderParticlesFountain.destroy();
	shaderParticlesExplosion.destroy();
	shaderParticlesFire.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelLamp1.destroy();
	modelLamp2.destroy();
	modelLampPost2.destroy();
	modelGrass_1.destroy();
	modelGrass_2.destroy();
	modelGrass_3.destroy();
	modelFountain.destroy();
	modelCofre.destroy();
	modelTree.destroy();

	// Custom objects animate
	spyroModelAnimate.destroy();
	diamanteModelAnimate.destroy();
	enemigoModelAnimate.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleExplosionID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	// Controlador xbox360 for pc
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GL_TRUE) {
		//std::cout << "Esta presente el joystick" << std::endl;
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		/*
		std::cout << "Número de ejes disponibles :=>" << axesCount << std::endl;
		std::cout << "Left Stick X axis: " << axes[0] << std::endl;
		std::cout << "Left Stick Y axis: " << axes[1] << std::endl;
		std::cout << "Left Trigger/L2: " << axes[2] << std::endl; // Right Stick X axis
		std::cout << "Right Stick X axis: " << axes[3] << std::endl; // Right Stick Y axis
		std::cout << "Right Stick Y axis: " << axes[4] << std::endl; // Left Trigger
		std::cout << "Right Trigger/R2: " << axes[5] << std::endl;
		*/

		if (fabs(axes[1]) > 0.2 && !gameOver) {
			isWalk = true;
			modelMatrixSpyro = glm::translate(modelMatrixSpyro, glm::vec3(0, 0, axes[1] * SpyroSpeed));
			animationIndex = 0;
		}
		else
			isWalk = false;
		if (fabs(axes[0]) > 0.2 && !gameOver) {
			modelMatrixSpyro = glm::rotate(modelMatrixSpyro, glm::radians(-axes[0] * 3.0f), glm::vec3(0, 1, 0));
			animationIndex = 0;
		}

		if (fabs(axes[2]) > 0.2) {
			camera->mouseMoveCamera(axes[2] * 3.0f, 0.0, deltaTime);
		}if (fabs(axes[3]) > 0.2) {
			camera->mouseMoveCamera(0.0, axes[3], deltaTime);
		}

		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		//std::cout << "Número de botones disponibles :=>" << buttonCount << std::endl;
		if (!isShot && buttons[1] == GLFW_PRESS && !gameOver) { // Boton B
			isShot = true;
			startTimeShot = currTime;
			tmv_shot = 0;
		}

		if (!isJump && buttons[0] == GLFW_PRESS && !gameOver) { // Boton A
			isJump = true;
			startTimeJump = currTime;
			tmv = 0;
		}

		if (!isRun && buttons[2] == GLFW_PRESS && !gameOver) { // Boton X
			isRun = true;
			SpyroSpeed = SpyroRunSpeed;
		}
		if (isRun && buttons[2] == GLFW_RELEASE && !gameOver) { // Boton X
			isRun = false;
			SpyroSpeed = SpyroWalkSpeed;
		}
		if (!restarting && buttons[3] == GLFW_PRESS && gameOver) { // Boton Y
			restarting = true;
		}
		if (ready2Play && buttons[7] == GLFW_PRESS && gameOver) { // Boton Start
			ready2Play = false;
			gameOver = false;
			restartingCount = 0.0;
			restarting = false;
		}
		/*
		4 LB
		5 RB
		6 Back
		7 Start
		8 L3
		9 R3
		10 Up
		11 Right
		12 Down
		13 Left
		*/
		if (restarting) {
			animationIndex = 2;
			isJump = false;
			isRun = false;
			isShot = false;
		}
	}

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	if (!enableExplosion && glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
		enableExplosion = true;
		contadorExplosion = 0.0f;
	}

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS){
		enableCountSelected = false;
		modelSelected++;
		if(modelSelected > 1)
			modelSelected = 0;
		//std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	// Guardar key frames
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = true;
		if(myfile.is_open())
			myfile.close();
		myfile.open(fileName);
	}
	if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE
			&& glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
		record = false;
		myfile.close();
	}
	if(availableSave && glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS){
		saveFrame = true;
		availableSave = false;
	}if(glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE)
		availableSave = true;

	// Spyro animate model movements
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !gameOver) {
		modelMatrixSpyro = glm::rotate(modelMatrixSpyro, glm::radians(1.0f), glm::vec3(0, 1, 0));
		animationIndex = 0;
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !gameOver) {
		modelMatrixSpyro = glm::rotate(modelMatrixSpyro, glm::radians(-1.0f), glm::vec3(0, 1, 0));
		animationIndex = 0;
	}if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !gameOver) {
		isWalk = true;
		modelMatrixSpyro = glm::translate(modelMatrixSpyro, glm::vec3(0, 0, SpyroSpeed));
		animationIndex = 0;
	}
	else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !gameOver) {
		isWalk = true;
		modelMatrixSpyro = glm::translate(modelMatrixSpyro, glm::vec3(0, 0, -SpyroSpeed));
		animationIndex = 0;
	}
	else if (!isJump && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !gameOver) {
		isJump = true;
		tmv = 0;
		startTimeJump = currTime;
	}
	else if (!isShot && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !gameOver) { // Boton B
		isShot = true;
		startTimeShot = currTime;
		tmv_shot = 0;
	}
	else if (!isRun && glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !gameOver) {
		isRun = true;
		SpyroSpeed = SpyroRunSpeed;
	}
	else if (!isRun && glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && !gameOver) {
		isRun = false;
		SpyroSpeed = SpyroWalkSpeed;
	}
	else if (!restarting && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS && gameOver) {
		restarting = true;
	}
	else if (ready2Play && glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS && gameOver) {
		ready2Play = false;
		gameOver = false;
		restartingCount = 0.0;
		restarting = false;
	}


	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
		modelMatrixEnemigoArray[i] = glm::translate(modelMatrixEnemigoArray[i], EnemigoInitialPosition[i]);
	}

	for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
		modelMatrixCofresArray[i] = glm::translate(modelMatrixCofresArray[i], CofresInitialPosition[i]);
	}

	for (int i = 0; i < modelMatrixDiamantesArray.size(); i++) {
		modelMatrixDiamantesArray[i] = glm::translate(modelMatrixDiamantesArray[i], DiamantesInitialPosition[i]);
	}

	for (int i = 0; i < modelMatrixTreesArray.size(); i++) {
		modelMatrixTreesArray[i] = glm::translate(modelMatrixTreesArray[i], TreesInitialPosition[i]);
	}

	restartAllPositions();

	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	shadowBox = new ShadowBox(-lightPos, camera.get(), 15.0f, 0.1f, 45.0f);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);

		
		axis = glm::axis(glm::quat_cast(modelMatrixSpyro));
		angleTarget = glm::angle(glm::quat_cast(modelMatrixSpyro));
		target = modelMatrixSpyro[3];

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;
		if(modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 centerBox = shadowBox->getCenter();

		// Matriz de proyección del shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix;
		//float near_plane = 0.1f, far_plane = 20.0f;
		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;

		//lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
		lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderParticlesExplosion.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderParticlesExplosion.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.7, 0.7, 0.9)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.7, 0.7, 0.9)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.7, 0.7, 0.9)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		glm::vec3 spotPosition = glm::vec3(glm::vec4(0.0, 3.0, 0.0, 1.0));
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", DiamantesInitialPosition.size());
		shaderTerrain.setInt("pointLightCount", DiamantesInitialPosition.size());
		for (int i = 0; i < DiamantesInitialPosition.size(); i++){
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, DiamantesInitialPosition[i]);
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.008, 0.008, 0.008));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0, 0.5, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.8, 0.4, 0.0)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.0)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.9, 0.1, 0.0)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.1);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.8, 0.4, 0.0)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.0)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.9, 0.1, 0.0)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.1);
		}

		/*******************************************
		 * 1. Render del buffer de profundidad desde el punto de vista de la luz
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Para visualizar la textura del mapa desde el punto de vista de la luz (Debug)
		 *******************************************/
		 // Reset viewport
		 /*
		 glViewport(0, 0, screenWidth, screenHeight);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 // Render de la escena desde la luz en un cuadrado
		 shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 shaderViewDepth.setFloat("near_plane", near_plane);
		 shaderViewDepth.setFloat("far_plane", far_plane);
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, depthMap);
		 boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		 boxViewDepth.render();*/

		 /*******************************************
		  * 3. Render de la escena normal
		  *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);

		/*******************************************
		* Ray in Spyro view direction
		*******************************************/
		glm::mat4 modelMatrixRay = glm::mat4(modelMatrixSpyro);
		modelMatrixRay = glm::translate(modelMatrixRay, glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 rayDirection = glm::normalize(glm::vec3(modelMatrixRay[2]));
		glm::vec3 ori = glm::vec3(modelMatrixRay[3]);
		glm::vec3 tar = ori + 5.0f * rayDirection;
		glm::vec3 tarm = ori + 2.5f * rayDirection;
		modelMatrixRay[3] = glm::vec4(tarm, 1.0f);
		modelMatrixRay = glm::rotate(modelMatrixRay, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
		modelMatrixRay = glm::scale(modelMatrixRay, glm::vec3(1.0, 5.0, 1.0));
		//cylinderRay.render(modelMatrixRay);

		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene(true);

		/*******************************************
		 * Creacion de colliders
		 * IMPORTANT do this before interpolations
		 *******************************************/

		/*************************************************************************************
		* Paredes
		*************************************************************************************/
		// Collider de Pared Norte
		glm::mat4 ModelMatrixColliderParedNorte = glm::mat4(1.0f);
		AbstractModel::OBB ParedNorteCollider;
		// Set the orientation of collider before doing the scale
		ParedNorteCollider.u = glm::quat_cast(ModelMatrixColliderParedNorte);
		ModelMatrixColliderParedNorte = glm::translate(
			ModelMatrixColliderParedNorte, glm::vec3(100.0, 10.0, 0.0));
		ModelMatrixColliderParedNorte = glm::scale(ModelMatrixColliderParedNorte,
			glm::vec3(1.0, 12.0, 98.8));
		ParedNorteCollider.c = glm::vec3(ModelMatrixColliderParedNorte[3]);
		ParedNorteCollider.e = glm::vec3(1.0, 12.0, 98.8);
		addOrUpdateColliders(collidersOBB, "paredNorte", ParedNorteCollider, ModelMatrixColliderParedNorte);

		// Collider de Pared Sur
		glm::mat4 ModelMatrixColliderParedSur = glm::mat4(1.0f);
		AbstractModel::OBB ParedSurCollider;
		// Set the orientation of collider before doing the scale
		ParedSurCollider.u = glm::quat_cast(ModelMatrixColliderParedSur);
		ModelMatrixColliderParedSur = glm::translate(
			ModelMatrixColliderParedSur, glm::vec3(-100.0, 10.0, 0.0));
		ModelMatrixColliderParedSur = glm::scale(ModelMatrixColliderParedSur,
			glm::vec3(1.0, 12.0, 98.8));
		ParedSurCollider.c = glm::vec3(ModelMatrixColliderParedSur[3]);
		ParedSurCollider.e = glm::vec3(1.0, 12.0, 98.8);
		addOrUpdateColliders(collidersOBB, "paredSur", ParedSurCollider, ModelMatrixColliderParedSur);

		// Collider de Pared Este
		glm::mat4 ModelMatrixColliderParedEste = glm::mat4(1.0f);
		AbstractModel::OBB ParedEsteCollider;
		// Set the orientation of collider before doing the scale
		ParedEsteCollider.u = glm::quat_cast(ModelMatrixColliderParedEste);
		ModelMatrixColliderParedEste = glm::translate(
			ModelMatrixColliderParedEste, glm::vec3(0.0, 10.0, 100.0));
		ModelMatrixColliderParedEste = glm::scale(ModelMatrixColliderParedEste,
			glm::vec3(98.8, 12.0, 1.0));
		ParedEsteCollider.c = glm::vec3(ModelMatrixColliderParedEste[3]);
		ParedEsteCollider.e = glm::vec3(98.8, 12.0, 1.0);
		addOrUpdateColliders(collidersOBB, "paredEste", ParedEsteCollider, ModelMatrixColliderParedEste);

		// Collider de Pared Oeste
		glm::mat4 ModelMatrixColliderParedOeste = glm::mat4(1.0f);
		AbstractModel::OBB ParedOesteCollider;
		// Set the orientation of collider before doing the scale
		ParedOesteCollider.u = glm::quat_cast(ModelMatrixColliderParedOeste);
		ModelMatrixColliderParedOeste = glm::translate(
			ModelMatrixColliderParedOeste, glm::vec3(0.0, 10.0, -100.0));
		ModelMatrixColliderParedOeste = glm::scale(ModelMatrixColliderParedOeste,
			glm::vec3(98.8, 12.0, 1.0));
		ParedOesteCollider.c = glm::vec3(ModelMatrixColliderParedOeste[3]);
		ParedOesteCollider.e = glm::vec3(98.8, 12.0, 1.0);
		addOrUpdateColliders(collidersOBB, "paredOeste", ParedOesteCollider, ModelMatrixColliderParedOeste);

		// Collider de Spyro
		AbstractModel::OBB spyroCollider;
		glm::mat4 modelMatrixColliderSpyro = glm::mat4(modelMatrixSpyro);
		spyroCollider.u = glm::quat_cast(modelMatrixColliderSpyro);
		modelMatrixColliderSpyro = glm::scale(modelMatrixColliderSpyro, glm::vec3(0.015, 0.015, 0.015));
		modelMatrixColliderSpyro = glm::translate(modelMatrixColliderSpyro, spyroModelAnimate.getObb().c);
		spyroCollider.e = spyroModelAnimate.getObb().e * glm::vec3(0.015, 0.015, 0.015);
		spyroCollider.c = glm::vec3(modelMatrixColliderSpyro[3]);
		addOrUpdateColliders(collidersOBB, "spyro", spyroCollider, modelMatrixSpyro);

		// Collider de Enemigo
		for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
			AbstractModel::OBB enemigoCollider;
			glm::mat4 modelMatrixColliderEnemigo = glm::mat4(modelMatrixEnemigoArray[i]);
			enemigoCollider.u = glm::quat_cast(modelMatrixColliderEnemigo);
			modelMatrixColliderEnemigo = glm::scale(modelMatrixColliderEnemigo, glm::vec3(0.005, 0.01, 0.01));
			modelMatrixColliderEnemigo = glm::translate(modelMatrixColliderEnemigo, enemigoModelAnimate.getObb().c);
			enemigoCollider.e = enemigoModelAnimate.getObb().e * glm::vec3(0.005, 0.01, 0.01);
			enemigoCollider.c = glm::vec3(modelMatrixColliderEnemigo[3]);
			addOrUpdateColliders(collidersOBB, "enemigo-" + std::to_string(i), enemigoCollider, modelMatrixColliderEnemigo);
		}

		// Collider de Cofres
		for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
			glm::mat4 modelMatrixColliderCofre = glm::mat4(modelMatrixCofresArray[i]);
			AbstractModel::OBB cofreCollider;
			// Set the orientation of collider before doing the scale
			cofreCollider.u = glm::quat_cast(modelMatrixColliderCofre);
			modelMatrixColliderCofre = glm::scale(modelMatrixColliderCofre, glm::vec3(0.01, 0.01, 0.01));
			modelMatrixColliderCofre = glm::translate(modelMatrixColliderCofre, modelCofre.getObb().c);
			cofreCollider.c = glm::vec3(modelMatrixColliderCofre[3]);
			cofreCollider.e = modelCofre.getObb().e * glm::vec3(0.01, 0.01, 0.01);
			addOrUpdateColliders(collidersOBB, "cofre-" + std::to_string(i), cofreCollider, modelMatrixColliderCofre);
		}

		// Collider de Diamantes
		for (int i = 0; i < DiamantesInitialPosition.size(); i++) {
			AbstractModel::SBB diamanteCollider;
			glm::mat4 modelMatrixColliderDiamante = glm::mat4(modelMatrixDiamantesArray[i]);
			modelMatrixColliderDiamante = glm::scale(modelMatrixColliderDiamante, glm::vec3(0.008, 0.008, 0.008));
			modelMatrixColliderDiamante = glm::translate(modelMatrixColliderDiamante, diamanteModelAnimate.getSbb().c);
			diamanteCollider.c = modelMatrixColliderDiamante[3];
			diamanteCollider.ratio = diamanteModelAnimate.getSbb().ratio * 0.008;
			addOrUpdateColliders(collidersSBB, "diamante-" + std::to_string(i), diamanteCollider, modelMatrixColliderDiamante);
		}

		// Collider de Tree
		for (int i = 0; i < TreesInitialPosition.size(); i++) {
			AbstractModel::OBB treeCollider;
			glm::mat4 modelMatrixColliderTree = glm::mat4(modelMatrixTreesArray[i]);
			treeCollider.u = glm::quat_cast(modelMatrixColliderTree);
			glm::vec3 treeHeight;
			treeHeight.x = TreesInitialHeight[i] / 100.0f;
			treeHeight.y = TreesInitialHeight[i] / 10.0f;
			treeHeight.z = TreesInitialHeight[i] / 100.0f;
			modelMatrixColliderTree = glm::scale(modelMatrixColliderTree, treeHeight);
			modelMatrixColliderTree = glm::translate(modelMatrixColliderTree, modelTree.getObb().c);
			treeCollider.e = modelTree.getObb().e * treeHeight;
			treeCollider.c = glm::vec3(modelMatrixColliderTree[3]);
			addOrUpdateColliders(collidersOBB, "tree-" + std::to_string(i), treeCollider, modelMatrixColliderTree);
		}
	
		/*******************************************
		 * Render de colliders
		 *******************************************/
		/*
		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			boxCollider.render(matrixCollider);
		}

		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			sphereCollider.render(matrixCollider);
		}
		*/

		/*******************************************
		 * Test Colisions
		 *******************************************/
		 // Cajas vs Cajas
		for (std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				if (it != jt
						&& testOBBOBB(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = buscaColliderOBB(it->first, jt->first);
					std::cout << "isCollision: " << isCollision << std::endl;
					//isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}
		// Esferas vs Esferas
		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string,
					std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				if (it != jt
						&& testSphereSphereIntersection(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = true;
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}
		// Cajas vs esferas
		for (std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			std::map<std::string,
					std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
					collidersOBB.begin();
			for (; jt != collidersOBB.end(); jt++) {
				if (testSphereOBox(std::get<0>(it->second),
								std::get<0>(jt->second))) {
					std::cout << "Colision " << it->first << " with "
							<< jt->first << std::endl;
					isCollision = buscaColliderSBB(it->first, jt->first);
					//isCollision = true;
					addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end(); colIt++) {
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it = collidersOBB.find(colIt->first);
			std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt = collidersSBB.find(colIt->first);
			if (it != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, it->first);
				else {
					if (it->first.compare("spyro") == 0 && !restarting)
						modelMatrixSpyro = std::get<1>(it->second);
				}
			}
			if (jt != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, jt->first);
			}
		}
		/*******************************************
		 * Prueba de colision de rayo
		 *******************************************/
		 // Rayo vs Esfera
		for (std::map<std::string,
			std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			float tray;
			if (raySphereIntersect(ori, tar, rayDirection, std::get<0>(it->second), tray))
				std::cout << "Colision " << it->first << " with " << "Ray" << std::endl;
		}

		// Rayo vs Caja
		for (std::map<std::string,
			std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			if (testIntersectRayOBB(ori, tar, rayDirection, std::get<0>(it->second))) {
				std::cout << "Colision " << it->first << " with " << "Ray" << std::endl;
				bool basura = buscaColliderOBB(it->first, "Ray");
			}

		}
		// Constantes de animaciones
		rotHelHelY += 0.5;
		animationIndex = 1;

		// Contador explosion
		if (enableExplosion) {
			if (contadorExplosion < 1.5f)
				contadorExplosion += 1.0f * deltaTime;
			else
				enableExplosion = false;
		}


		//Movimiento Enemgigo
		for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
			vectorDirectorEnemigo = glm::vec3(	modelMatrixSpyro[3][0] - modelMatrixEnemigoArray[i][3][0],
												modelMatrixSpyro[3][1] - modelMatrixEnemigoArray[i][3][1],
												modelMatrixSpyro[3][2] - modelMatrixEnemigoArray[i][3][2]);
			float magnitud = sqrtf(vectorDirectorEnemigo.x * vectorDirectorEnemigo.x +
				vectorDirectorEnemigo.y * vectorDirectorEnemigo.y +
				vectorDirectorEnemigo.z * vectorDirectorEnemigo.z);

			if (enemigo_isVivo_Array[i] && !gameOver) {
				if (magnitud <= 20.0f) {
					//glm::normalize(vectorDirectorEnemigo);
					vectorDirectorEnemigo = glm::vec3(vectorDirectorEnemigo.x / magnitud,
						vectorDirectorEnemigo.y / magnitud,
						vectorDirectorEnemigo.z / magnitud);
					modelMatrixEnemigoArray[i] = glm::translate(modelMatrixEnemigoArray[i], glm::vec3(vectorDirectorEnemigo.x * 0.1, 0.0, vectorDirectorEnemigo.z * 0.1));
					enemigo_anim_index_Array[i] = 1;
				}
				else
					enemigo_anim_index_Array[i] = 2;
			}
			
		}

		/*******************************************
		 * State machines
		 *******************************************/
		//glfwSwapBuffers(window);
		if (ready2Play) {
			modelText->render("Spyro vs Zombies", -0.1, 0.9);
			modelText->render("Pusla N / Start para Empezar", -0.2, -0.8);
		}
		else if(!restarting){
			modelText->render("Enemigos: " + std::to_string(enemigosCount) + " / " + std::to_string(enemigo_isVivo_Array.size()), -0.9, 0.9);
			modelText->render("Gemas recogidas: " + std::to_string(diamantesCount) + " / " + std::to_string(diamantesTotal), -0.3, 0.9);
			modelText->render("Vida: " + std::to_string(int(spyroLifeCount)) + "%", 0.3, 0.9);
		}
		else
			modelText->render("Cargando una nueva aventura", -0.2, 0.9);

		
		if (diamantesCount == diamantesTotal) {
			vectorDirectorSpyro = glm::vec3(modelMatrixSpyro[3][0], 30.0f - modelMatrixSpyro[3][1], modelMatrixSpyro[3][2]);
			gameOver = true;
		}
		if (gameOver && !ready2Play) {
			if (spyroAlive) {
				animationIndex = 2;
				restarting = restartSpyroInitialPosition();
			}
			else {
				animationIndex = 4;
				if (restarting) 
					restarting = restartSpyroInitialPosition();
				else
					modelText->render("Pulsa Y para reiniciar", -0.1, -0.8);
				for (int i = 0; i < enemigo_anim_index_Array.size(); i++)
					enemigo_anim_index_Array[i] = 2;
			}
			if(!restarting)
				modelText->render("Game Over", 0.0, -0.7);
		}
		
		glfwSwapBuffers(window);

		//restartSpyroInitialPosition();

		/****************************+
		 * Open AL sound data
		 */
		// Fuego
		source1Pos[0] = modelMatrixSpyro[3].x;
		source1Pos[1] = modelMatrixSpyro[3].y;
		source1Pos[2] = modelMatrixSpyro[3].z;
		alSourcefv(source[1], AL_POSITION, source1Pos);
		if(isShot)
			alSourcef(source[1], AL_GAIN, 1.0f);
		else
			alSourcef(source[1], AL_GAIN, 0.0f);

		//Walk
		if (isWalk) {
			alSourcef(source[3], AL_GAIN, 1.0f);
			alSourcef(source[4], AL_GAIN, 0.0f);
			alSourcef(source[2], AL_GAIN, 0.0f);
		}
		// Run
		if (isRun){
			alSourcef(source[3], AL_GAIN, 0.0f);
			alSourcef(source[4], AL_GAIN, 1.0f);
			alSourcef(source[2], AL_GAIN, 0.0f);
		}
		// Jump
		if (isJump){
			alSourcef(source[3], AL_GAIN, 0.0f);
			alSourcef(source[4], AL_GAIN, 0.0f);
			alSourcef(source[2], AL_GAIN, 1.0f);
		}
		if (!isWalk && !isRun && !isJump) {
			alSourcef(source[3], AL_GAIN, 0.0f);
			alSourcef(source[4], AL_GAIN, 0.0f);
			alSourcef(source[2], AL_GAIN, 0.0f);
		}
			

		//Diamantes
		for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
			ALfloat sourceCofrePos[] = { 0.0, 0.0, 0.0 };
			sourceCofrePos[0] = CofresInitialPosition[i][0];
			sourceCofrePos[1] = CofresInitialPosition[i][1];
			sourceCofrePos[2] = CofresInitialPosition[i][2];
			int indice = i + 6;
			if (cofres_isVivo_Array[i]) {
				alSourcef(source[indice], AL_GAIN, 1.0f);
			}
			else {
				alSourcef(source[indice], AL_GAIN, 0.0f);
			}
		}

		// Enemigos
		for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
			ALfloat sourceEnemigoPos[] = { 0.0, 0.0, 0.0 };
			sourceEnemigoPos[0] = modelMatrixEnemigoArray[i][3].x;
			sourceEnemigoPos[1] = modelMatrixEnemigoArray[i][3].y;
			sourceEnemigoPos[2] = modelMatrixEnemigoArray[i][3].z;
			int indice = i + 6 + modelMatrixCofresArray.size();
			alSourcefv(source[indice], AL_POSITION, sourceEnemigoPos);
			if (enemigo_isVivo_Array[i]) {
				alSourcef(source[indice], AL_GAIN, 1.0f);
			}
			else {
				alSourcef(source[indice], AL_GAIN, 0.0f);
			}
		}

		// Listener for the Thris person camera
		listenerPos[0] = modelMatrixSpyro[3].x;
		listenerPos[1] = modelMatrixSpyro[3].y;
		listenerPos[2] = modelMatrixSpyro[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(modelMatrixSpyro[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixSpyro[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;

		// Listener for the First person camera
		/*listenerPos[0] = camera->getPosition().x;
		listenerPos[1] = camera->getPosition().y;
		listenerPos[2] = camera->getPosition().z;
		alListenerfv(AL_POSITION, listenerPos);
		listenerOri[0] = camera->getFront().x;
		listenerOri[1] = camera->getFront().y;
		listenerOri[2] = camera->getFront().z;
		listenerOri[3] = camera->getUp().x;
		listenerOri[4] = camera->getUp().y;
		listenerOri[5] = camera->getUp().z;*/
		alListenerfv(AL_ORIENTATION, listenerOri);

		for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
			if (sourcesPlay[i]) {
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}
	}
}

void prepareScene() { // Importante agregar los modelos que se añaden a la escena

	skyboxSphere.setShader(&shaderSkybox);

	terrain.setShader(&shaderTerrain);

	//Lamp models
	modelLamp1.setShader(&shaderMulLighting);
	modelLamp2.setShader(&shaderMulLighting);
	modelLampPost2.setShader(&shaderMulLighting);

	//Grass
	modelGrass_1.setShader(&shaderMulLighting);
	modelGrass_2.setShader(&shaderMulLighting);
	modelGrass_3.setShader(&shaderMulLighting);

	//Spyro
	spyroModelAnimate.setShader(&shaderMulLighting);

	//Enemigo
	enemigoModelAnimate.setShader(&shaderMulLighting);
}

void prepareDepthScene() {

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);
		
	//Lamp models
	modelLamp1.setShader(&shaderDepth);
	modelLamp2.setShader(&shaderDepth);
	modelLampPost2.setShader(&shaderDepth);

	//Grass
	modelGrass_1.setShader(&shaderDepth);
	modelGrass_2.setShader(&shaderDepth);
	modelGrass_3.setShader(&shaderDepth);

	//Spyro
	spyroModelAnimate.setShader(&shaderDepth);

	//Enemigo
	enemigoModelAnimate.setShader(&shaderDepth);
}

void renderScene(bool renderParticles) { // Render de la Escena
	/*******************************************
		 * Terrain Cesped
		 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);
	/*
	// Render the lamps
	for (int i = 0; i < lamp1Position.size(); i++) {
		lamp1Position[i].y = terrain.getHeightTerrain(lamp1Position[i].x, lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}*/

	// Grass
	for (int i = 0; i < grassInitialPosition.size(); i++) {
		glDisable(GL_CULL_FACE);
		glm::vec3 grassPosition = grassInitialPosition[i];
		grassPosition.y = terrain.getHeightTerrain(grassPosition.x, grassPosition.z);
		modelGrass_1.setPosition(grassPosition);
		modelGrass_1.render();
		modelGrass_2.setPosition(grassPosition);
		modelGrass_2.render();
		modelGrass_3.setPosition(grassPosition);
		modelGrass_3.render();
		glEnable(GL_CULL_FACE);
	}
	// Cofres
	for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
		if (cofres_isVivo_Array[i]) {
			modelMatrixCofresArray[i][3][1] = terrain.getHeightTerrain(modelMatrixCofresArray[i][3][0], modelMatrixCofresArray[i][3][2]);
			glm::mat4 modelMatrixCofreBody = glm::mat4(modelMatrixCofresArray[i]);
			modelMatrixCofreBody = glm::scale(modelMatrixCofreBody, glm::vec3(0.01, 0.01, 0.01));
			modelCofre.render(modelMatrixCofreBody);
		}
	}

	// Diamantes
	for (int i = 0; i < modelMatrixDiamantesArray.size(); i++) {
		if (diamantes_isVivo_Array[i]) {
			modelMatrixDiamantesArray[i][3][1] = terrain.getHeightTerrain(modelMatrixDiamantesArray[i][3][0], modelMatrixDiamantesArray[i][3][2]);
			glm::mat4 modelMatrixDiamanteBody = glm::mat4(modelMatrixDiamantesArray[i]);
			modelMatrixDiamanteBody = glm::scale(modelMatrixDiamanteBody, glm::vec3(0.008, 0.008, 0.008));
			diamanteModelAnimate.setAnimationIndex(0);
			diamanteModelAnimate.render(modelMatrixDiamanteBody);
		}
	}
	glActiveTexture(GL_TEXTURE0);	

	// Tree
	for (int i = 0; i < modelMatrixTreesArray.size(); i++) {
		glm::vec3 treeHeight(TreesInitialHeight[i] / 10.0f);

		glDisable(GL_CULL_FACE);
		modelMatrixTreesArray[i][3][1] = terrain.getHeightTerrain(modelMatrixTreesArray[i][3][0], modelMatrixTreesArray[i][3][2]);
		glm::mat4 modelMatrixTreeBody = glm::mat4(modelMatrixTreesArray[i]);
		modelMatrixTreeBody = glm::scale(modelMatrixTreeBody, treeHeight);
		modelTree.render(modelMatrixTreeBody);
		glEnable(GL_CULL_FACE);
	}
	

	/*******************************************
	 * Custom Anim objects obj
	 *******************************************/

	 //Spyro
	glDisable(GL_CULL_FACE);
	if (!gameOver) {
		modelMatrixSpyro[3][1] = -gravedad * tmv * tmv + 5.5 * tmv + terrain.getHeightTerrain(modelMatrixSpyro[3][0], modelMatrixSpyro[3][2]);
		tmv = currTime - startTimeJump;
		if (modelMatrixSpyro[3][1] < terrain.getHeightTerrain(modelMatrixSpyro[3][0], modelMatrixSpyro[3][2])) {
			isJump = false;
			modelMatrixSpyro[3][1] = terrain.getHeightTerrain(modelMatrixSpyro[3][0], modelMatrixSpyro[3][2]);
		}
	}
	else if(!spyroAlive && !restarting)
		modelMatrixSpyro[3][1] = terrain.getHeightTerrain(modelMatrixSpyro[3][0], modelMatrixSpyro[3][2]);

	glm::mat4 modelMatrixSpyroBody = glm::mat4(modelMatrixSpyro);
	modelMatrixSpyroBody = glm::scale(modelMatrixSpyroBody, glm::vec3(0.015, 0.015, 0.015));
	if(isJump)
		spyroModelAnimate.setAnimationIndex(2);
	else if(isRun)
		spyroModelAnimate.setAnimationIndex(3);
	else
		spyroModelAnimate.setAnimationIndex(animationIndex);
	spyroModelAnimate.render(modelMatrixSpyroBody);
	glEnable(GL_CULL_FACE);

	// Enemigo

	for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
		glm::vec3 enemigoDirection = glm::normalize(glm::vec3(modelMatrixSpyro[3][0] - modelMatrixEnemigoArray[i][3][0],
			modelMatrixSpyro[3][1] - modelMatrixEnemigoArray[i][3][1],
			modelMatrixSpyro[3][2] - modelMatrixEnemigoArray[i][3][2]));
		glm::vec3 enemigoUp = glm::vec3(0.0, 1.0, 0.0);
		glm::vec3 enemigoRight = glm::normalize(glm::cross(enemigoUp, enemigoDirection));
		glm::vec3 enemigoPos = glm::vec3(modelMatrixEnemigoArray[i][3][0], modelMatrixEnemigoArray[i][3][1], modelMatrixEnemigoArray[i][3][2]);

		modelMatrixEnemigoArray[i][3][1] = terrain.getHeightTerrain(modelMatrixEnemigoArray[i][3][0], modelMatrixEnemigoArray[i][3][2]);
		glm::mat4 modelMatrixEnemigoBody = glm::mat4(modelMatrixEnemigoArray[i]);

		modelMatrixEnemigoBody[0][0] = enemigoRight.x;		modelMatrixEnemigoBody[0][1] = enemigoRight.y;		modelMatrixEnemigoBody[0][2] = enemigoRight.z;
		modelMatrixEnemigoBody[1][0] = enemigoUp.x;			modelMatrixEnemigoBody[1][1] = enemigoUp.y;			modelMatrixEnemigoBody[1][2] = enemigoUp.z;
		modelMatrixEnemigoBody[2][0] = enemigoDirection.x;	modelMatrixEnemigoBody[2][1] = enemigoDirection.y;	modelMatrixEnemigoBody[2][2] = enemigoDirection.z;

		modelMatrixEnemigoBody = glm::scale(modelMatrixEnemigoBody, glm::vec3(0.01, 0.01, 0.01));
		enemigoModelAnimate.setAnimationIndex(enemigo_anim_index_Array[i]);
		enemigoModelAnimate.render(modelMatrixEnemigoBody);
	}

	/**********
	 * Sorter with alpha objects
	 */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++) {
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
		if (it->second.first.compare("explosion") == 0 && enableExplosion) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesExplosion = modelMatrixActiveExplosion;
			//modelMatrixParticlesExplosion = glm::translate(modelMatrixParticlesExplosion, it->second.second);
			modelMatrixParticlesExplosion[3][1] = terrain.getHeightTerrain(modelMatrixParticlesExplosion[3][0], modelMatrixParticlesExplosion[3][2]);
			modelMatrixParticlesExplosion = glm::scale(modelMatrixParticlesExplosion, glm::vec3(3.0, 3.0, 3.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleExplosionID);
			shaderParticlesExplosion.turnOn();
			shaderParticlesExplosion.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesExplosion.setFloat("ParticleLifetime", 1.5f);
			shaderParticlesExplosion.setInt("ParticleTex", 0);
			shaderParticlesExplosion.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, -0.3f, 0.0f)));
			shaderParticlesExplosion.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesExplosion));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesExplosion.turnOff();
			/**********
			 * End Render particles systems
			 */
		}
		else if (it->second.first.compare("fire") == 0) {
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1 - drawBuf]);
			glVertexAttribDivisor(0, 0);
			glVertexAttribDivisor(1, 0);
			glVertexAttribDivisor(2, 0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			glm::mat4 modelFireParticles = glm::mat4(modelMatrixSpyro);
			modelFireParticles = glm::translate(modelFireParticles, glm::vec3(0.0, 1.0, 0.0));
			glm::vec3 rayDirection = glm::normalize(glm::vec3(modelFireParticles[2]));
			glm::vec3 ori = glm::vec3(modelFireParticles[3]);
			glm::vec3 tar = ori + 5.0f * rayDirection;
			glm::vec3 tarm = ori + 1.0f * rayDirection;
			modelFireParticles[3] = glm::vec4(tarm, 1.0f);
			modelFireParticles = glm::rotate(modelFireParticles, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
			
			tmv_shot = currTime - startTimeShot;
			if (tmv_shot >= 1.0f) {
				isShot = false;
				//modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.0, 0.0, 0.0));
				shaderParticlesFire.turnOff();
			}
			else if (isShot)
			{
				modelFireParticles = glm::scale(modelFireParticles, glm::vec3(0.1, 5.0f * tmv_shot, 0.1));
				shaderParticlesFire.setInt("Pass", 2);
				shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));
				shaderParticlesFire.setVectorFloat3("colorHumo", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));

				shaderParticlesFire.turnOn();
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
				glDepthMask(GL_FALSE);
				glBindVertexArray(particleArray[drawBuf]);
				glVertexAttribDivisor(0, 1);
				glVertexAttribDivisor(1, 1);
				glVertexAttribDivisor(2, 1);
				glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
				glBindVertexArray(0);
				glDepthMask(GL_TRUE);
				drawBuf = 1 - drawBuf;
				shaderParticlesFire.turnOff();
			}
				

			

			/**********
			 * End Render particles systems
			 */
		}

	}
	glEnable(GL_CULL_FACE);
}
bool buscaColliderSBB(std::string nombre, std::string second_nombre) {
	for (int i = 0; i < diamantes_isVivo_Array.size(); i++) {
		std::string nombreIterador = "diamante-" + std::to_string(i);
		if (nombreIterador == nombre && second_nombre == "spyro") {
			if (diamantes_isVivo_Array[i]) {
				diamantes_isVivo_Array[i] = false;
				diamantesCount += 2;
			}
			return false;
		}
	}
	return true;
}

bool buscaColliderOBB(std::string nombre, std::string second_nombre) {
	for (int i = 0; i < enemigo_isVivo_Array.size(); i++) {
		std::string nombreIterador = "enemigo-" + std::to_string(i);
		if (nombreIterador == nombre | second_nombre == nombreIterador) {
			if (isShot && enemigo_isVivo_Array[i]) {
				enemigo_isVivo_Array[i] = false;
				enemigo_anim_index_Array[i] = 0;
				diamantesCount += 10;
				enemigosCount += 1;
				return false;
			}
			else if (!enemigo_isVivo_Array[i])
				return false;
			else if (enemigo_isVivo_Array[i] && second_nombre == "spyro") {
				if (spyroLifeCount > 0) {
					spyroLifeCount -= 1.0f;
					return true;
				}
				else {
					gameOver = true;
					spyroAlive = false;
					isJump = false;
					isRun = false;
					isShot = false;
					spyroLifeCount = 0.0f;
					vectorDirectorSpyro = glm::vec3(modelMatrixSpyro[3][0],
						30.0f - modelMatrixSpyro[3][1],
						modelMatrixSpyro[3][2]);
				}
			}
		}
	}
	for (int i = 0; i < cofres_isVivo_Array.size(); i++) {
		std::string nombreIterador = "cofre-" + std::to_string(i);
		if (nombreIterador == nombre | second_nombre == nombreIterador) {
			if (cofres_isVivo_Array[i] && (isShot | isRun)) {
				modelMatrixActiveExplosion[3][0] = CofresInitialPosition[i].x;
				modelMatrixActiveExplosion[3][1] = CofresInitialPosition[i].y;
				modelMatrixActiveExplosion[3][2] = CofresInitialPosition[i].z;
				if (cofres_isVivo_Array[i] && !enableExplosion) {
					enableExplosion = true;
					contadorExplosion = 0.0f;
				}
				cofres_isVivo_Array[i] = false;
				diamantesCount += 5;
				return false;
			}
			else if (!cofres_isVivo_Array[i])
				return false;
		}
	}
	return true;
}

bool restartSpyroInitialPosition() {
	if (restartingCount >= 10.f) {
		restartAllPositions();
		return false;
	}
	else
		restartingCount += deltaTime;
	float magnitud = sqrtf(	vectorDirectorSpyro.x * vectorDirectorSpyro.x +
							vectorDirectorSpyro.y * vectorDirectorSpyro.y +
							vectorDirectorSpyro.z * vectorDirectorSpyro.z);
	if (magnitud >= 0.1f) {
		vectorDirectorSpyro = glm::vec3(vectorDirectorSpyro.x / magnitud,
										vectorDirectorSpyro.y / magnitud,
										vectorDirectorSpyro.z / magnitud);
		modelMatrixSpyro = glm::translate(modelMatrixSpyro, glm::vec3(vectorDirectorSpyro.x * 0.4, vectorDirectorSpyro.y * 0.4, vectorDirectorSpyro.z * 0.4));
		return true;
	}
	else
		return true;
}

void restartAllPositions() {
	// Reiniciar Variables Spyro
	modelMatrixSpyro = glm::mat4(1.0f);
	modelMatrixSpyro = glm::translate(modelMatrixSpyro, glm::vec3(0.0f, 30.0f, 0.0f));
	isShot = false;
	isJump = true;
	isRun = false;
	spyroAlive = true;
	// Reiniciar Variables Enemigo
	for (int i = 0; i < modelMatrixEnemigoArray.size(); i++) {
		modelMatrixEnemigoArray[i] = glm::mat4(1.0f);
		modelMatrixEnemigoArray[i] = glm::translate(modelMatrixEnemigoArray[i], EnemigoInitialPosition[i]);
		enemigo_isVivo_Array[i] = true;
	}
	// Reiniciar Variables Cofres
	for (int i = 0; i < modelMatrixCofresArray.size(); i++) {
		cofres_isVivo_Array[i] = true;
	}
	// Reiniciar Variables Diamanes
	for (int i = 0; i < modelMatrixDiamantesArray.size(); i++) {
		diamantes_isVivo_Array[i] =true;
	}
	// Reiniciar Otras Variables
	spyroLifeCount = 100.0f;
	diamantesCount = 0;
	enemigosCount = 0;
	ready2Play = true;
	
}

int main(int argc, char **argv) {
	init(1920, 1080, "Spyro VS Zombies", true);
	applicationLoop();
	destroy();
	return 1;
}
