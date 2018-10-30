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
#include "try.h"
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
glm::vec3 cameraPos   = glm::vec3(0.0f, 13.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
btVector3 vel = btVector3(0.0f,0.0f,0.0f);
btScalar acelx =0.0f;
btScalar acelz =0.0f;
btVector3 pos = btVector3(0.5f, 5, -2);
bool reset = false;

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
	btVector3 localInertiaobjeto(0, 1, 0);
	if (isDynamicobjeto)
		objetoShape->calculateLocalInertia(objetoMass, localInertiaobjeto);
	btDefaultMotionState* objetoMotionState = new btDefaultMotionState(objetoTransform);
	btRigidBody::btRigidBodyConstructionInfo objetoRbInfo(objetoMass, objetoMotionState, objetoShape, localInertiaobjeto);
	btRigidBody* objeto;
	objeto = new btRigidBody(objetoRbInfo);
    if( isDynamicobjeto){
        objeto->setActivationState(DISABLE_DEACTIVATION);
    }
    dynamicsWorld->addRigidBody(objeto); 
	return objeto;
}
btCollisionShape* crear_shape(GLfloat* points, int nV, bool convex){
    btConvexShape* shape= new btBoxShape(btVector3(0.2f,0.3f,0.2f));
    
    /*if(convex){
        printf("yey");
		shape= new btConvexHullShape();
        for (int i = 0; i < nV-1; i+=3)
		{
           //printf("-> %f",points[i]);
            btVector3 btv = btVector3(points[i], points[i+1], points[i+2]);
            ((btConvexHullShape*)shape)->addPoint(btv);
        }
		//shape = new btBvhTriangleMeshShape(newShape,true); //&newShape;
		
    }else{
        printf("yaaay");
        btTriangleMesh* mesh = new btTriangleMesh();
        for (int i=0; i < nV; i += 3)
        { 
          //  btVector3 bv1 = btVector3(points[3*i], points[3*i+1], points[3*i+2]);
           // btVector3 bv2 = btVector3(points[3*i+1], points[3*i+2], points[3*i+3]);
           //btVector3 bv3 = btVector3(points[3*i+2], points[3*i+3], points[3*i+4]);
            
           // mesh->addTriangle(bv1, bv2, bv3);
        }        
           //btConcaveShape *tmp2 = new btBvhTriangleMeshShape(mesh,true);
           ///shape=new btConvexTriangleMeshShape(shape);
    }*/
    printf("Generado:");
    return shape;
}

int main(){
	restart_gl_log ();
	start_gl ();
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable (GL_CULL_FACE); // cull face
	glCullFace (GL_BACK); // cull back face
	glFrontFace (GL_CCW); // set counter-clock-wise vertex order to mean the front
	glClearColor (1.0, 1.0, 1.0, 1.0); // grey background to help spot mistakes
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
    printf("mallas");
    malla *personaje = new malla((char*)"mallas/personaje.obj");
    malla *floor = new malla((char*)"mallas/floor.obj");
    printf("mallas lista");
    malla *lab = new malla((char*)"mallas/laberintoYoshi.obj");
    malla *deidad = new malla((char*)"mallas/suzanne.obj");
   // malla *deidad2 = new malla((char*)"mallas/suzanne.obj");
   // malla *deidad3 = new malla((char*)"mallas/suzanne.obj");
   // malla *deidad4 = new malla((char*)"mallas/suzanne.obj");
    printf("mallas listas");
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
	//btCollisionShape* personajeShape =new btBoxShape(btVector3(0.3f,0.6f,.3f));
    btCollisionShape* personajeShape = crear_shape(personaje->getPoints(),personaje->getNumVertices(),true);//new btSphereShape(btScalar(1.));
    btRigidBody* bodyPersonaje= crear_Objeto(pos,personajeShape,btQuaternion(0,0,0), 100.f);
    btRigidBody* bodyDeidad= crear_Objeto(btVector3(-10.0f,-5.0f,0.0f),personajeShape,btQuaternion(90,0,0), 0.0f);
    btRigidBody* bodyDeidad2=crear_Objeto(btVector3(10.0f,-5.0f,0.0f),personajeShape,btQuaternion(-90,0,0), 0.0f);
    btRigidBody* bodyDeidad3=crear_Objeto(btVector3(10.0f,-5.0f,0.0f),personajeShape,btQuaternion(0,0,0), 0.0f);
    btRigidBody* bodyDeidad4=crear_Objeto(btVector3(10.0f,-5.0f,0.0f),personajeShape,btQuaternion(-180,0,0), 0.0f);
    btCollisionShape* LabShape =new btBoxShape(btVector3(8, 0.1f, 8));
    btCollisionShape** paredes =(btCollisionShape**)malloc (50 * sizeof (btCollisionShape*));
    btRigidBody** walls =(btRigidBody**)malloc (50 * sizeof (btRigidBody*));
    paredes[0]= new btBoxShape(btVector3(0.05, 1,2.5f));
    paredes[1]= new btBoxShape(btVector3(0.05, 1,2.5f));
    paredes[2]= new btBoxShape(btVector3(0.05, 1, 2.5f));
    paredes[3]= new btBoxShape(btVector3(0.05, 1, 2.5f));
    paredes[4]= new btBoxShape(btVector3(0.05, 1, 2));
    paredes[5]= new btBoxShape(btVector3(0.05, 1, 2));
    paredes[6]= new btBoxShape(btVector3(0.05, 1, 2));
    paredes[7]= new btBoxShape(btVector3(0.05, 1, 2));//--
    paredes[8]= new btBoxShape(btVector3(0.05, 1, 1.5f));
    paredes[9]= new btBoxShape(btVector3(0.05, 1, 3));
    paredes[10]= new btBoxShape(btVector3(0.05, 1, 2.5f));
    paredes[11]= new btBoxShape(btVector3(0.05, 1, 2.5f));
    paredes[12]= new btBoxShape(btVector3(0.05, 1, 2.5f));
    paredes[13]= new btBoxShape(btVector3(0.05, 1, 1.5f));//--
    paredes[14]= new btBoxShape(btVector3(0.05, 1, 2));
    paredes[15]= new btBoxShape(btVector3(0.05, 1, 2));
    paredes[16]= new btBoxShape(btVector3(0.05, 1, 0.5f));//--
    paredes[17]= new btBoxShape(btVector3(0.05, 1, 1));
    paredes[18]= new btBoxShape(btVector3(0.05, 1, 1));
    paredes[19]= new btBoxShape(btVector3(0.05, 1, 1));
    paredes[20]= new btBoxShape(btVector3(0.05, 1, 1));
    paredes[21]= new btBoxShape(btVector3(0.05, 1, 1.5f));
    paredes[22]= new btBoxShape(btVector3(0.05, 1, 8));
    paredes[23]= new btBoxShape(btVector3(0.05, 1, 8));
    paredes[24]= new btBoxShape(btVector3(8, 1, 0.05));
    paredes[25]= new btBoxShape(btVector3(8, 1, 0.05));
    paredes[26]= new btBoxShape(btVector3(1.5f, 1, 0.05));
    paredes[27]= new btBoxShape(btVector3(1.5f, 1, 0.05));
    paredes[28]= new btBoxShape(btVector3(1.5f, 1, 0.05));
    paredes[29]= new btBoxShape(btVector3(1.5f, 1, 0.05));
    paredes[30]= new btBoxShape(btVector3(3.5f, 1, 0.05));
    paredes[31]= new btBoxShape(btVector3(3.5f, 1, 0.05));
    paredes[32]= new btBoxShape(btVector3(3.5f, 1, 0.05));
    paredes[33]= new btBoxShape(btVector3(3.5f, 1, 0.05));
    paredes[34]= new btBoxShape(btVector3(3.0f, 1, 0.05));
    paredes[35]= new btBoxShape(btVector3(2.0f, 1, 0.05));
    paredes[36]= new btBoxShape(btVector3(0.5f, 1, 0.05));
    paredes[37]= new btBoxShape(btVector3(0.5f, 1, 0.05));
    paredes[38]= new btBoxShape(btVector3(2.0f, 1, 0.05));
    paredes[39]= new btBoxShape(btVector3(1.5f, 1, 0.05));
    paredes[40]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[41]= new btBoxShape(btVector3(0.5f, 1, 0.05));
    paredes[42]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[43]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[44]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[45]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[46]= new btBoxShape(btVector3(1.0f, 1, 0.05));
    paredes[47]= new btBoxShape(btVector3(0.5f, 1, 0.05));
    paredes[48]= new btBoxShape(btVector3(0.05, 1, 0.5f));
    paredes[49]= new btBoxShape(btVector3(0.05, 1, 0.5f));
    btVector3* pots= (btVector3*)malloc(50*sizeof(btVector3));
    pots[0]=btVector3(-7,-4,4.5f);
    pots[1]=btVector3(-4,-4,4.5f);
    pots[2]=btVector3(0,-4,4.5f);
    pots[3]=btVector3(-3,-4,4.5f);
    pots[4]=btVector3(1,-4,4);
    pots[5]=btVector3(5,-4,4);
    pots[6]=btVector3(6,-4,4);
    pots[7]=btVector3(7,-4,4);//--
    pots[8]=btVector3(0,-4,-0.5f);
    pots[9]=btVector3(1,-4,-2);
    pots[10]=btVector3(4,-4,-1.5f);
    pots[11]=btVector3(5,-4,-1.5f);
    pots[12]=btVector3(7,-4,-1.5f);
    pots[13]=btVector3(-7,-4,-0.5f);//--
    pots[14]=btVector3(-7,-4,-5);
    pots[15]=btVector3(0,-4,-5);
    pots[16]=btVector3(3,-4,-4.5f);//-
    pots[17]=btVector3(1,-4,-7);
    pots[18]=btVector3(3,-4,-7);
    pots[19]=btVector3(4,-4,-6);
    pots[20]=btVector3(6,-4,-6);
    pots[21]=btVector3(7,-4,-6.5f);
    pots[22]=btVector3(-8,-4,0);
    pots[23]=btVector3(8,-4,0);//--
    pots[24]=btVector3(0,-4,-8);
    pots[25]=btVector3(0,-4,8);
    pots[26]=btVector3(-1.5f,-4,7);
    pots[27]=btVector3(-5.5f,-4,7);
    pots[28]=btVector3(-1.5f,-4,2);
    pots[29]=btVector3(-5.5f,-4,2);
    pots[30]=btVector3(-3.5f,-4,1);
    pots[31]=btVector3(-3.5f,-4,-2);
    pots[32]=btVector3(-3.5f,-4,-3);
    pots[33]=btVector3(-3.5f,-4,-7);//--
    pots[34]=btVector3(4.0f,-4,7);
    pots[35]=btVector3(3.0f,-4,6);
    pots[36]=btVector3(6.5f,-4,6);
    pots[37]=btVector3(6.5f,-4,2);
    pots[38]=btVector3(3.0f,-4,2);
    pots[39]=btVector3(2.5f,-4,1);
    pots[40]=btVector3(6.0f,-4,1);
    pots[41]=btVector3(3.5f,-4,-4);
    pots[42]=btVector3(6.0f,-4,-4);
    pots[43]=btVector3(2.0f,-4,-6);
    pots[44]=btVector3(2.0f,-4,-5);
    pots[45]=btVector3(5.0f,-4,-7);
    pots[46]=btVector3(5.0f,-4,-5);
    pots[47]=btVector3(7.5f,-4,-5);
    pots[48]=btVector3(1.0f,-4,7.5f);
    pots[49]=btVector3(7.0f,-4,7.5f);
    for(int i=0;i<50;i++){
        walls[i]= crear_Objeto(pots[i],paredes[i],btQuaternion(0,0,0),0.f);
    }
    //btCollisionShape* pisoShape =new btBoxShape(btVector3(8,0.2f,4));
    //btRigidBody* piso= crear_Objeto(btVector3(0,-5.5f,0),pisoShape,btQuaternion(0,0,0),0.f);
    //btCollisionShape* LabShape = crear_shape(lab->getPoints(),lab->getNumVertices(),true);//new btBoxShape(btVector3(9, 0.2f, 8));
    btRigidBody* laberinto= crear_Objeto(btVector3(0,-5,0),LabShape,btQuaternion(0,0,0),0.f);
  
   

    glm::mat4 aux;
    GLDebugDrawer* debug = new GLDebugDrawer();    
    debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
    debug->setView(&view);
    debug->setProj(&projection);
    dynamicsWorld->setDebugDrawer(debug);
    btTransform trans;
   
    
    while (!glfwWindowShouldClose(g_window)){
        
        // Se aumenta en un paso la simulacion (calculo del dt)
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(g_window);
        if(reset){
            btTransform transform = bodyPersonaje -> getCenterOfMassTransform();
            transform.setOrigin(pos); //<- set orientation / position that you like
            //ballShape->calculateLocalInertia(ballMass, localInertiaBall);
            bodyPersonaje -> setCenterOfMassTransform(transform);  
            bodyPersonaje -> setLinearVelocity(vel);
            reset=false;
        }
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (shader_programme);

        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

        bodyPersonaje->getMotionState()->getWorldTransform(trans); 
        // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        // Y se obtiene la matrix model directamente desde bullet
        if(acelx){
             bodyPersonaje->applyImpulse(btVector3(acelx,0.0f,0.0f),btVector3(1,0,0));
        }else{
             bodyPersonaje->applyImpulse(btVector3(0.0f,0.0f,acelz),btVector3(0,0,1));
        }
        acelx=0;
        acelz=0;
        trans.getOpenGLMatrix(&aux[0][0]);
        personaje->setModelMatrix(aux);
        personaje->draw(model_mat_location);

        laberinto->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        lab->setModelMatrix(aux);
        lab->draw(model_mat_location);
        bodyDeidad->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        deidad->setModelMatrix(aux);
        deidad->draw(model_mat_location);
        bodyDeidad2->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        deidad->setModelMatrix(aux);
        deidad->draw(model_mat_location);
       /* bodyDeidad3->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        deidad3->setModelMatrix(aux);
        deidad3->draw(model_mat_location);
        bodyDeidad4->getMotionState()->getWorldTransform(trans); // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        trans.getOpenGLMatrix(&aux[0][0]);
        deidad4->setModelMatrix(aux);
        deidad4->draw(model_mat_location);*/
        
      /*  piso->getMotionState()->getWorldTransform(trans);
        trans.getOpenGLMatrix(&aux[0][0]);
        floor->setModelMatrix(aux);
        floor->draw(model_mat_location);*/
        
        /*debug->setView(&view);
        debug->setProj(&projection);
        dynamicsWorld->debugDrawWorld();
        debug->drawLines();*/
        
        glfwSwapBuffers(g_window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window){
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        acelx =-6;
		acelz =0;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        acelx =6;
		acelz=0;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        acelz =6;
        acelx =0;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
       acelz =-6;
       acelx =0;
    }
    if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS){
        reset=true;
    }/*
    float cameraSpeed = 2.5 * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;*/
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
