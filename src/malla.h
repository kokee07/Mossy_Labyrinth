#ifndef MALLA_H
#define MALLA_H
#include <bullet/btBulletDynamicsCommon.h>
using namespace std;

class malla{
    private:
        GLuint vao, vbo, tex;
        int numVertices;
        GLfloat* vPoints;
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
        glm::mat4 modelMatrix;
        char* filename;
        float angle;

    public:
        malla(char *filename);
        malla(char *filename,char *texture);
        bool load_texture(char* file_name, GLuint* tex);
       // void draw();
        // gets
        GLuint getVao();
        int getNumVertices();
       
        GLfloat* getPoints();
        glm::vec3 getPosition();
        glm::vec3 getRotation();
        char* getFilename();

        // sets
        void setVao(GLuint vao);
        void setNumVertices(int numVertices);
        void setPoints(GLfloat* points);
        void setPosition(glm::vec3 pos);
        void setRotation(float ang, glm::vec3 rot);
        void setFilename(char *f);
        void setModelMatrix(glm::mat4 model);

        void draw(int matloc);
};

#endif
