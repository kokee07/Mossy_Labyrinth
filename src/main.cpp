/******************************************************************************\
| OpenGL 4 Example Code.                                                       |
| Accompanies written series "Anton's OpenGL 4 Tutorials"                      |
| Email: anton at antongerdelan dot net                                        |
| First version 27 Jan 2014                                                    |
| Copyright Dr Anton Gerdelan, Trinity College Dublin, Ireland.                |
| See individual libraries for separate legal notices                          |
|******************************************************************************|
| This demo uses the Assimp library to load a mesh from a file, and supports   |
| many formats. The library is VERY big and complex. It's much easier to write |
| a simple Wavefront .obj loader. I have code for this in other demos. However,|
| Assimp will load animated meshes, which will we need to use later, so this   |
| demo is a starting point before doing skinning animation                     |
\******************************************************************************/
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "GLDebugDrawer.hpp"

#include <bullet/btBulletDynamicsCommon.h>

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);


// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 30.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
btVector3 vel = btVector3(0.0f,0.0f,0.0f);
btScalar acel =0.0f;
btVector3 pos = btVector3(0, 5, -2);
bool reset = false;
btScalar velCaida =0.0f;

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
btDiscreteDynamicsWorld* dynamicsWorld;
btRigidBody* crear_Objeto(btVector3 posicion, btCollisionShape *objetoShape,btQuaternion rotacion, float masa)
{
	btTransform objetoTransform;
	objetoTransform.setIdentity();
	objetoTransform.setRotation(rotacion);
	objetoTransform.setOrigin(posicion); // Posicion incial
	btScalar objetoMass(masa); // Masa
	bool isDynamicobjeto = (objetoMass != 0.f);
	btVector3 localInertiaobjeto(1, 0, 0);
	if (isDynamicobjeto)
		objetoShape->calculateLocalInertia(objetoMass, localInertiaobjeto);
	btDefaultMotionState* objetoMotionState = new btDefaultMotionState(objetoTransform);
	btRigidBody::btRigidBodyConstructionInfo objetoRbInfo(objetoMass, objetoMotionState, objetoShape, localInertiaobjeto);
	btRigidBody* objeto;
	objeto = new btRigidBody(objetoRbInfo);
    objeto->setActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addRigidBody(objeto); 
	return objeto;
}
int main(){
	restart_gl_log ();
	start_gl ();
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor (0.2, 0.2, 0.2, 1.0); // grey background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height);

    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    glfwSetCursorPosCallback(g_window, mouse_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /*-------------------------------CREATE SHADERS-------------------------------*/
	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);

    malla *ball = new malla((char*)"mallas/personajeprincipal.obj");
    malla *floor = new malla((char*)"mallas/floor.obj");
    malla *lab = new malla((char*)"mallas/laberintowo.obj");
    malla *deidad = new malla((char*)"mallas/Deidad1.obj");
    
    glm::mat4 projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	int view_mat_location = glGetUniformLocation (shader_programme, "view");
	glUseProgram (shader_programme);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);
	int proj_mat_location = glGetUniformLocation (shader_programme, "proj");
	glUseProgram (shader_programme);
	glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);
	int model_mat_location = glGetUniformLocation (shader_programme, "model");
    // Configuracion inical del mundo fisico Bullet
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    // Creacion del mundo fisico - Uno por aplicacion
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    // Vector de gravedad
	dynamicsWorld->setGravity(btVector3(0, -10, 0));

    // Creacion de las figura colisionadora
	btCollisionShape* ballShape = new btSphereShape(btScalar(1.));
    btRigidBody* bodyBall= crear_Objeto(pos,ballShape,btQuaternion(0,0,0), 100.f);
    btCollisionShape* FloorShape = new btBoxShape(btVector3(9, 0.4f, 8));
    btRigidBody* floors = crear_Objeto(btVector3(0,-5,0),FloorShape,btQuaternion(0,0,0),0.f);
  
   

    glm::mat4 aux;
    GLDebugDrawer* debug = new GLDebugDrawer();    
    debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
    debug->setView(&view);
    debug->setProj(&projection);
    dynamicsWorld->setDebugDrawer(debug);
    while (!glfwWindowShouldClose(g_window)){
        
        // Se aumenta en un paso la simulacion (calculo del dt)
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(g_window);
        if(reset){
            btTransform transform = bodyBall -> getCenterOfMassTransform();
            transform.setOrigin(pos); //<- set orientation / position that you like
            //ballShape->calculateLocalInertia(ballMass, localInertiaBall);
            bodyBall -> setCenterOfMassTransform(transform);  
            bodyBall -> setLinearVelocity(vel);
            reset=false;
        }
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (shader_programme);

        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

        btTransform trans;

        bodyBall->getMotionState()->getWorldTransform(trans); 
        // Se guarda la informacion de transformaciones de bodyBall en trans
        // Y se obtiene la matrix model directamente desde bullet
        bodyBall->applyImpulse(btVector3(acel,0.0f,0.0f),btVector3(1,0,0));
        acel=0;
        trans.getOpenGLMatrix(&aux[0][0]);
        ball->setModelMatrix(aux);
       
        ball->draw(model_mat_location);
        floors->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyBall en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        lab->setModelMatrix(aux);
        lab->draw(model_mat_location);
        /*for(int i=0;i<4;i++){
            floors[i]->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyBall en trans
        // Y se obtiene la matrix model directamente desde bullet
            trans.getOpenGLMatrix(&aux[0][0]);
            floor->setModelMatrix(aux);
            floor->draw(model_mat_location);
        }*/
        
        debug->setView(&view);
        debug->setProj(&projection);
        dynamicsWorld->debugDrawWorld();
        debug->drawLines();
        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        acel =-10;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
       acel =10;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
        reset=true;
    }
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
