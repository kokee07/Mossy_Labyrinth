#ifndef SKYBOX_H
#define SKYBOX_H

#define TRIANGLE_FACTOR 3
#define QUAD_FACTOR     4

using namespace std;

class skybox{
    public:
        GLuint vaosky, vbosky, nbo, tbo, ebo,tex;
        skybox();

        // functions
        bool load_cube_map_side( GLuint texture, GLenum side_target, const char *file_name );
        bool generate();
        void makevao(GLfloat *vertices, GLfloat* normals, GLfloat* texcoords, GLuint *indices);
        bool load_texture();
        void draw();
};

#endif
