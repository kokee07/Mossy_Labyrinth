#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <GLFW/glfw3.h> // GLFW helper library
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "maths_funcs.h"
#include "gl_utils.h"
#include "tools.h"
#include "malla.h"
#include "skybox.h"
#include "GLDebugDrawer.hpp"
#include <time.h>
#include "tablero.h"
#include <map>

#define GL_LOG_FILE "log/gl.log"
#define VERTEX_SHADER_FILE "shaders/test_vs.glsl"
#define FRAGMENT_SHADER_FILE "shaders/test_fs.glsl"
#define SKYBOX_VERTEX_SHADER_FILE "shaders/sky_vert.glsl"
#define SKYBOX_FRAGMENT_SHADER_FILE "shaders/sky_frag.glsl"

int g_gl_width = 920;
int g_gl_height = 720;
GLFWwindow* g_window = NULL;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool isAvailable(btVector3 toCheck,int action);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
bool** generateTab();
bool checkWin(btVector3 toCheck,btVector3 toWin);

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 34.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(-0.004f, -0.95f, -0.303f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 15.0f, 0.0f);
btScalar acelx =0.0f;
btScalar acelz =0.0f;
float acel = 1.0f;
//btVector3 pos = btVector3(-0.5f, 2.5f, 0.0f);//-2);
btVector3 pos = btVector3(-7.5f, 0.5f, -6.5f);//-2);
bool reset = false;
bool place=false;
bool playing=true;
int blockNum=0;
bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  g_gl_width / 2.0;
float lastY =  g_gl_height / 2.0;
float fov   =  45.0f;
map<int, btVector3> my_map = {
    { 0, btVector3(1.0f,0.0f,0.0f) },
    { 1, btVector3(-1.0f,0.0f,0.0f) },
    { 2, btVector3(0.0f,0.0f,-1.0f) },
    { 3, btVector3(0.0f,0.0f,1.0f) }
};
map<int, btVector3> win_map = {
    { 0, btVector3(0.5f,-3.0f,3.5f) },
    { 1, btVector3(7.5f,-3.0f,4.5f) },
    { 2, btVector3(4.5f,-3.0f,6.5f) }
};
bool** tab;

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
    if(isDynamicobjeto){
        objeto->setActivationState(DISABLE_DEACTIVATION);
    }
    dynamicsWorld->addRigidBody(objeto); 
	return objeto;
}
btRigidBody* crear_block(btVector3 posicion)
{
	btTransform objetoTransform;
    btCollisionShape* bloque=new btBoxShape(btVector3(.5f,.5f,.5f));
	objetoTransform.setIdentity();
	objetoTransform.setOrigin(posicion); // Posicion incial
	btScalar objetoMass(0.0f); // Masa
	btVector3 localInertiaobjeto(0, 0, 0);
    bool isDynamicobjeto = (objetoMass != 0.f);
	if (isDynamicobjeto)
	    bloque->calculateLocalInertia(objetoMass, localInertiaobjeto);
	btDefaultMotionState* objetoMotionState = new btDefaultMotionState(objetoTransform);
	btRigidBody::btRigidBodyConstructionInfo objetoRbInfo(objetoMass, objetoMotionState, bloque, localInertiaobjeto);
	btRigidBody* objeto;
	objeto = new btRigidBody(objetoRbInfo);
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
	glClearColor (0.1, 0.1, 0.1, 1.0); // grey background to help spot mistakes
	glViewport (0, 0, g_gl_width, g_gl_height);

    glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);
    glfwSetKeyCallback(g_window, key_callback);
    glfwSetScrollCallback(g_window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(g_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    srand(time(NULL));

    /*-------------------------------CREATE SHADERS-------------------------------*/
	GLuint shader_programme = create_programme_from_files (
		VERTEX_SHADER_FILE, FRAGMENT_SHADER_FILE
	);
    malla** deidades =(malla**)malloc (4 * sizeof (malla*));
    deidades[0] = new malla((char*)"mallas/fire.obj",(char*)"texturas/fireLava.png");//Izquierda
	deidades[1] = new malla((char*)"mallas/water.obj",(char*)"texturas/WaterIce.png");//Derecha
	deidades[2] = new malla((char*)"mallas/Arbolito.obj",(char*)"texturas/Arbolito.png");//Abajo
	deidades[3] = new malla((char*)"mallas/Thunder.obj",(char*)"texturas/ThunderWind.png");//Arriba
	malla* cajacaja= new malla((char*)"mallas/caja_5_t.obj", (char*)"texturas/caja.jpg");
    malla *personaje = new malla((char*)"mallas/personaje.obj",(char*)"texturas/fondo.jpg");
    malla** deidadesOff =(malla**)malloc (4 * sizeof (malla*));
    malla *flooor=new malla((char*)"mallas/piso10.obj", (char*)"texturas/down3.tga");
    malla *win_platform=new malla((char*)"mallas/winPlatform.obj", (char*)"texturas/rojo.jpg");
    deidadesOff[0] = new malla((char*)"mallas/fire.obj",(char*)"texturas/grunge.jpg");//Izquierda
	deidadesOff[1] = new malla((char*)"mallas/water.obj",(char*)"texturas/grunge.jpg");//Derecha
	deidadesOff[2] = new malla((char*)"mallas/Arbolito.obj",(char*)"texturas/grunge.jpg");//Abajo
	deidadesOff[3] = new malla((char*)"mallas/Thunder.obj",(char*)"texturas/grunge.jpg");//Arriba
    malla *base = new malla((char*)"mallas/BASE8X1_t.obj",(char*)"texturas/pared.png");
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
	dynamicsWorld->setGravity(btVector3(0, -10, 0));
    // Creacion de las figura colisionadora
	btCollisionShape* personajeShape =new btBoxShape(btVector3(0.3f,0.6f,.3f));
    btRigidBody* bodyPersonaje= crear_Objeto(pos,personajeShape,btQuaternion(0,0,0), 100.f);
    btRigidBody** deidadesBody =(btRigidBody**)malloc (4 * sizeof (btRigidBody*));
    btRigidBody** obstaculos =(btRigidBody**)malloc (200 * sizeof (btRigidBody*));

    deidadesBody[0]= crear_Objeto(btVector3(-10.0f,-5.0f,0.0f),personajeShape,btQuaternion(90,0,0), 0.0f);
    deidadesBody[1]=crear_Objeto(btVector3(10.0f,-5.0f,0.0f),personajeShape,btQuaternion(-90,0,0), 0.0f);
    deidadesBody[2]=crear_Objeto(btVector3(0.0f,-5.0f,10.0f),personajeShape,btQuaternion(60,0,0), 0.0f);
    deidadesBody[3]=crear_Objeto(btVector3(0.0f,-5.0f,-10.0f),personajeShape,btQuaternion(0,0,0), 0.0f);

    btCollisionShape* pisoShape =new btBoxShape(btVector3(9,0.2f,9));
    btRigidBody* piso= crear_Objeto(btVector3(0,-5.5f,0),pisoShape,btQuaternion(0,0,0),0.f);
    glm::mat4 aux;
    GLDebugDrawer* debug = new GLDebugDrawer();    
    debug->setDebugMode(btIDebugDraw::DBG_DrawWireframe );
    debug->setView(&view);
    debug->setProj(&projection);
    dynamicsWorld->setDebugDrawer(debug);
    btTransform trans;
    GLuint skybox_shader = create_programme_from_files ( SKYBOX_VERTEX_SHADER_FILE, SKYBOX_FRAGMENT_SHADER_FILE);
	glUseProgram (skybox_shader);
    int view_skybox = glGetUniformLocation (skybox_shader, "view");
    glUniformMatrix4fv (view_skybox, 1, GL_FALSE, &view[0][0]);
    glUseProgram (skybox_shader);
    int proj_skybox = glGetUniformLocation (skybox_shader, "proj");
    glm::mat4 proj =glm::perspective(67.0f, (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
	glUniformMatrix4fv (proj_skybox, 1, GL_FALSE, &(projection[0][0]));
    int activo= rand()%4;
    int indexWin=rand()%3;
    skybox *box = new skybox();
    tab=generateTab();
    btTransform winTransform;
    btVector3 winPos;
    while (!glfwWindowShouldClose(g_window)){
        
        // Se aumenta en un paso la simulacion (calculo del dt)
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if(reset){
            dynamicsWorld->removeRigidBody(bodyPersonaje);
            bodyPersonaje->clearForces();
            bodyPersonaje->setLinearVelocity(btVector3(0,0,0));
            bodyPersonaje->setAngularVelocity(btVector3(0,0,0));
            btTransform transform = bodyPersonaje -> getCenterOfMassTransform();
            transform.setOrigin(pos); //<- set orientation / position that you like
            bodyPersonaje -> setCenterOfMassTransform(transform); 
            dynamicsWorld->addRigidBody(bodyPersonaje);
            for(int i = 0; i < blockNum; i++)
            {
                dynamicsWorld->removeCollisionObject(obstaculos[i]);
                delete obstaculos[i];
            }
            blockNum=0;
            delete tab;
            tab=generateTab();
            activo= (activo+(rand()%3)+1)%4;
            indexWin=(indexWin+(rand()%2)+1)%3;
            playing=true;
            reset=false;
        }
        winPos= win_map[indexWin];
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram (skybox_shader);
        glDepthMask( GL_FALSE );
        box->draw();//REVISAR FUNCION
        glDepthMask( GL_TRUE );
		
        glUseProgram (shader_programme);
        projection = glm::perspective(glm::radians(fov), (float)g_gl_width / (float)g_gl_height, 0.1f, 100.0f);
        glUniformMatrix4fv (proj_mat_location, 1, GL_FALSE, &projection[0][0]);

        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	    glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, &view[0][0]);

        bodyPersonaje->getMotionState()->getWorldTransform(trans); 
        // Se guarda la informacion de transformaciones de bodyPersonaje en trans
        // Y se obtiene la matrix model directamente desde bullet
        trans.getOpenGLMatrix(&aux[0][0]);

        if(acelx || acelz){
            btVector3 toMove=bodyPersonaje -> getCenterOfMassPosition()+btVector3(acelx,0.0f,acelz);
            if (!isAvailable(toMove,0)){
                bodyPersonaje->translate(btVector3(acelx,0.0f,acelz));
                btVector3 actual = bodyPersonaje -> getCenterOfMassPosition();   
                if(checkWin(actual,winPos)){
                    playing=false;
                    btTransform finalTransform;
                    finalTransform.setIdentity();
                    finalTransform.setOrigin(btVector3(0.0f,0.0f,0.0f)); // Posicion final
                    bodyPersonaje->setCenterOfMassTransform(finalTransform);
                    
                }else{
                    btVector3 toBlock = actual+my_map[activo];
                    if(isAvailable(toBlock,1)){
                        obstaculos[blockNum]=crear_block(toBlock);
                        blockNum++;
                    }
                    activo= (activo+(rand()%3+1))%4;
                } 
            }
        }
        acelx=0;
        acelz=0;
        trans.getOpenGLMatrix(&aux[0][0]);
        personaje->setModelMatrix(aux);
        personaje->draw(model_mat_location);
        if(playing){
            deidadesBody[activo]->getMotionState()->getWorldTransform(trans);
            trans.getOpenGLMatrix(&aux[0][0]);
            deidades[activo]->setModelMatrix(aux);
            deidades[activo]->draw(model_mat_location);
            for(int i = 0; i < 4; i++)
            {   
                if(i!=activo){
                    deidadesBody[i]->getMotionState()->getWorldTransform(trans);
                    trans.getOpenGLMatrix(&aux[0][0]);
                    deidadesOff[i]->setModelMatrix(aux);
                    deidadesOff[i]->draw(model_mat_location);
                }
            }
            for(int i = 0; i < blockNum; i++)
            {   
                obstaculos[i]->getMotionState()->getWorldTransform(trans); 
                trans.getOpenGLMatrix(&aux[0][0]);
                cajacaja->setModelMatrix(aux);
                cajacaja->draw(model_mat_location);
            }
        
            winTransform.setIdentity();
            winTransform.setOrigin(winPos); // Posicion de victoria
            btVector3 a = winTransform.getOrigin();
            winTransform.getOpenGLMatrix(&aux[0][0]);
            win_platform->setModelMatrix(aux);
            win_platform->draw(model_mat_location);
            piso->getMotionState()->getWorldTransform(trans);
            trans.getOpenGLMatrix(&aux[0][0]);
            a =trans.getOrigin();
            base->setModelMatrix(aux);
            base->draw(model_mat_location);
            flooor->setModelMatrix(aux);
            flooor->draw(model_mat_location);
            
        }
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
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (key == GLFW_KEY_A && action == GLFW_PRESS){
        acelx =-acel;
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS){
        acelx =acel;
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS){
        acelz =acel;
    }
    if (key == GLFW_KEY_W && action == GLFW_PRESS){
       acelz =-acel;
    }
    if (key == GLFW_KEY_PAGE_UP && action == GLFW_PRESS){
       reset=true;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

bool isAvailable(btVector3 toCheck, int action){
    float x = toCheck.x()+9.0f;
    float z = toCheck.z()+9.0f;
    int posX = (int) x;
    int posZ = (int) z;
    bool save=tab[posZ][posX];
    if(action){
        if(!save){
            tab[posZ][posX]=true;
            return true;
        }else{
            return false;
        }
    }
    //printf("Checkea: %i,%i -> %d\n",posZ,posX,save);
    return save; 
}
bool checkWin(btVector3 toCheck,btVector3 toWin){
    
    if(((int)toCheck.x())==((int)toWin.x())){
        if(((int)toCheck.z())==((int)toWin.z())){
            return true;
        }
        return false;
    }
    return false;
}
bool** generateTab(){
    bool** tab=new bool*[18];
    for (int i=0;i<18;i++){
        tab[i]= new bool[18];
        for(int j=0;j<18;j++){
            tab[i][j]=false;
        }
    }
    tab[1][1]=true;
    tab[1][2]=true;
    tab[1][3]=true;
    tab[1][4]=true;
    tab[2][6]=true;
    tab[2][7]=true;
    tab[2][8]=true;
    tab[2][9]=true;
    tab[2][11]=true;
    tab[2][12]=true;
    tab[2][13]=true;
    tab[2][14]=true;
    tab[2][16]=true;
    tab[3][2]=true;
    tab[3][3]=true;
    tab[3][6]=true;
    tab[3][7]=true;
    tab[3][8]=true;
    tab[3][9]=true;
    tab[3][16]=true;
    tab[4][2]=true;
    tab[4][3]=true;
    tab[4][11]=true;
    tab[4][12]=true;
    tab[4][13]=true;
    tab[4][14]=true;
    tab[4][16]=true;
    tab[5][5]=true;
    tab[5][6]=true;
    tab[5][7]=true;
    tab[5][9]=true;
    tab[5][11]=true;
    tab[5][12]=true;
    tab[5][13]=true;
    tab[5][14]=true;
    tab[5][16]=true;
    tab[6][1]=true;
    tab[6][2]=true;
    tab[6][4]=true;
    tab[6][5]=true;
    tab[6][6]=true;
    tab[6][7]=true;
    tab[6][9]=true;
    tab[6][11]=true;
    tab[6][12]=true;
    tab[6][13]=true;
    tab[6][14]=true;
    tab[6][16]=true;
    tab[7][1]=true;
    tab[7][2]=true;
    tab[7][4]=true;
    tab[7][5]=true;
    tab[7][6]=true;
    tab[7][7]=true;
    tab[7][9]=true;
    tab[7][11]=true;
    tab[7][12]=true;
    tab[7][13]=true;
    tab[7][14]=true;
    tab[7][16]=true;
    tab[8][9]=true;
    tab[9][2]=true;
    tab[9][3]=true;
    tab[9][5]=true;
    tab[9][6]=true;
    tab[9][11]=true;
    tab[9][12]=true;
    tab[9][13]=true;
    tab[9][15]=true;
    tab[10][2]=true;
    tab[10][3]=true;
    tab[10][7]=true;
    tab[10][8]=true;
    tab[10][15]=true;
    tab[11][2]=true;
    tab[11][3]=true;
    tab[11][5]=true;
    tab[11][7]=true;
    tab[11][8]=true;
    tab[11][10]=true;
    tab[11][11]=true;
    tab[11][12]=true;
    tab[11][13]=true;
    tab[11][15]=true;
    tab[12][2]=true;
    tab[12][3]=true;
    tab[12][5]=true;
    tab[12][10]=true;
    tab[12][11]=true;
    tab[12][12]=true;
    tab[12][13]=true;
    tab[12][15]=true;
    tab[13][5]=true;
    tab[13][7]=true;
    tab[13][8]=true;
    tab[13][10]=true;
    tab[13][11]=true;
    tab[13][12]=true;
    tab[13][13]=true;
    tab[14][2]=true;
    tab[14][3]=true;
    tab[14][5]=true;
    tab[14][7]=true;
    tab[14][8]=true;
    tab[14][10]=true;
    tab[14][11]=true;
    tab[14][12]=true;
    tab[14][13]=true;
    tab[14][14]=true;
    tab[14][15]=true;
    tab[15][2]=true;
    tab[15][3]=true;
    tab[15][14]=true;
    tab[15][15]=true;
    tab[16][5]=true;
    tab[16][6]=true;
    tab[16][7]=true;
    tab[16][8]=true;
    tab[16][9]=true;
    tab[16][10]=true;
    tab[16][11]=true;
    tab[16][12]=true;
    for(int i=0;i<18;i++){
        tab[0][i]=true;
        tab[17][i]=true;
        tab[i][0]=true;
        tab[i][17]=true;
    }
    return tab;
}