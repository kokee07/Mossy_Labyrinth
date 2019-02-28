#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <assimp/cimport.h>
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "gl_utils.h"
#include "tools.h"
#include "stb_image.h"
#include "skybox.h"


using namespace std;
skybox::skybox(){
    assert(generate());
	load_texture();
}

bool skybox::load_cube_map_side( GLuint texture, GLenum side_target, const char *file_name ) {
	
	glBindTexture( GL_TEXTURE_CUBE_MAP, texture );

	int x, y, n;
	int force_channels = 4;
	unsigned char *image_data = stbi_load( file_name, &x, &y, &n, force_channels );
	if ( !image_data ) {
		fprintf( stderr, "ERROR: could not load %s\n", file_name );
		return false;
	}
	// copy image data into 'target' side of cube map  GLuint vaosky;
	glTexImage2D( side_target, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data );
	free( image_data );
	
	return true;
}

bool skybox::generate(){
    float points[]={-20.0f, 20.0f,	-20.0f, -20.0f, -20.0f, -20.0f, 20.0f,	-20.0f, -20.0f,
		20.0f,	-20.0f, -20.0f, 20.0f,	20.0f,	-20.0f, -20.0f, 20.0f,	-20.0f,

		-20.0f, -20.0f, 20.0f,	-20.0f, -20.0f, -20.0f, -20.0f, 20.0f,	-20.0f,
		-20.0f, 20.0f,	-20.0f, -20.0f, 20.0f,	20.0f,	-20.0f, -20.0f, 20.0f,

		20.0f,	-20.0f, -20.0f, 20.0f,	-20.0f, 20.0f,	20.0f,	20.0f,	20.0f,
		20.0f,	20.0f,	20.0f,	20.0f,	20.0f,	-20.0f, 20.0f,	-20.0f, -20.0f,

		-20.0f, -20.0f, 20.0f,	-20.0f, 20.0f,	20.0f,	20.0f,	20.0f,	20.0f,
		20.0f,	20.0f,	20.0f,	20.0f,	-20.0f, 20.0f,	-20.0f, -20.0f, 20.0f,

		-20.0f, 20.0f,	-20.0f, 20.0f,	20.0f,	-20.0f, 20.0f,	20.0f,	20.0f,
		20.0f,	20.0f,	20.0f,	-20.0f, 20.0f,	20.0f,	-20.0f, 20.0f,	-20.0f,

		-20.0f, -20.0f, -20.0f, -20.0f, -20.0f, 20.0f,	20.0f,	-20.0f, -20.0f,
		20.0f,	-20.0f, -20.0f, -20.0f, -20.0f, 20.0f,	20.0f,	-20.0f, 20.0f};
        
    glGenBuffers( 1, &(vbosky));
    glBindBuffer( GL_ARRAY_BUFFER, vbosky );
    glBufferData( GL_ARRAY_BUFFER, 3 * 36 * sizeof( GLfloat ), &points,
                                GL_STATIC_DRAW );

    glGenVertexArrays( 1, &(vaosky) );
    glBindVertexArray( vaosky );
    glEnableVertexAttribArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, vbosky );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, NULL );
    return true;
}
bool skybox::load_texture(){
    glActiveTexture( GL_TEXTURE0);
	glGenTextures( 1,&(tex));
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "texturas/back3.tga" );
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "texturas/front3.tga"  );
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "texturas/up3.tga" ) ;
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "texturas/down3_b.tga"  );
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "texturas/left3.tga" ) ;
	 load_cube_map_side( tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, "texturas/right3.tga" ) ;

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	printf("Sky cargado\n");
    return true;
}

void skybox::draw(){
		glActiveTexture( GL_TEXTURE0 );
		glBindVertexArray(vaosky);
		glBindTexture( GL_TEXTURE_CUBE_MAP,tex );
       // printf("Sky dibujado\n");
		glDrawArrays( GL_TRIANGLES, 0, 36 );
		glBindVertexArray(0);
}