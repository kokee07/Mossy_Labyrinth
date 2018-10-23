#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "tools.h"
#include "malla.h"

using namespace std;

malla::malla(char* filename){
	this->filename = filename;
    this->position = glm::vec3(0, 0, 0);
    this->rotation = glm::vec3(0, 1, 0);
    this->angle = 0.0f;
	assert(load_mesh(filename, &vao, &numVertices));
}

GLuint malla::getVao(){
    return this->vao;
}

int malla::getNumVertices(){
    return this->numVertices;
}

glm::vec3 malla::getPosition(){
    return this->position;
}

glm::vec3 malla::getRotation(){
    return this->rotation;
}

char* malla::getFilename(){
    return this->filename;
}

void malla::setPosition(glm::vec3 pos){
        this->position = pos;
}

void malla::setRotation(float ang, glm::vec3 rot){
        this->rotation = rot;
        this->angle = ang;
}

void malla::setModelMatrix(glm::mat4 model){
    this->modelMatrix = model;
}

void malla::draw(int matloc){
	    glUniformMatrix4fv(matloc, 1, GL_FALSE, &this->modelMatrix[0][0]);
        glBindVertexArray(this->getVao());
        glDrawArrays(GL_TRIANGLES, 0, this->getNumVertices());
}
