#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
#include <GL/glew.h> // include GLEW and new version of GL on Windows
#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include "gl_utils.h"
#include "try.h"
#include <bullet/btBulletDynamicsCommon.h>


#define PI 3.14159265359

bool load_mesh (const char* file_name, GLuint* vao, int* point_count,btCollisionShape* shape, bool b) {
	const aiScene* scene = aiImportFile(file_name, aiProcess_Triangulate);
	if (!scene) {
		fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
		return false;
    }
	/* get first mesh in file only */
	const aiMesh* mesh = scene->mMeshes[0];
	printf ("    %i vertices in mesh[0]\n", mesh->mNumVertices);
	
	/* pass back number of vertex points in mesh */
	*point_count = mesh->mNumVertices;
	
	/* generate a VAO, using the pass-by-reference parameter that we give to the
	function */
	glGenVertexArrays (1, vao);
	glBindVertexArray (*vao);
	
	/* we really need to copy out all the data from AssImp's funny little data
	structures into pure contiguous arrays before we copy it into data buffers
	because assimp's texture coordinates are not really contiguous in memory.
	i allocate some dynamic memory to do this. */
	GLfloat* points = NULL; // array of vertex points
	GLfloat* normals = NULL; // array of vertex normals
	GLfloat* texcoords = NULL; // array of texture coordinates
	if (mesh->HasPositions ()) {
		points = (GLfloat*)malloc (*point_count * 3 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vp = &(mesh->mVertices[i]);
			points[i * 3] = (GLfloat)vp->x;
			points[i * 3 + 1] = (GLfloat)vp->y;
			points[i * 3 + 2] = (GLfloat)vp->z;
		}
	}
    if(b){
        printf("yey");
		/*btConvexHullShape* newShape= new btConvexHullShape();
        for (int i = 0; i < *point_count; i++)
		{
            btVector3 btv = btVector3(points[3*i], points[3*i+1], points[3*i+2]);
            newShape->addPoint(btv);
        }
		shape = new btBvhTriangleMeshShape(newShape,true); //&newShape;*/
		btTriangleMesh* trimesh = new btTriangleMesh();
		for (int i=0; i < *point_count; i += 3)
        {
            GLfloat*  v1 = &points[i];
            GLfloat*  v2 = &points[i+1];
            GLfloat*  v3 = &points[i+2];
            
            btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
            btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
            btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);
            
            trimesh->addTriangle(bv1, bv2, bv3);
        }  
		btConvexShape *tmpshape = new btConvexTriangleMeshShape(trimesh);
		shape=&tmpshape; 
	}
    else
    {
        
        btTriangleMesh* mesh = new btTriangleMesh();
        for (int i=0; i < *point_count; i += 3)
        {
            GLfloat*  v1 = &points[i];
            GLfloat*  v2 = &points[i+1];
            GLfloat*  v3 = &points[i+2];
            
            btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
            btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
            btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);
            
            mesh->addTriangle(bv1, bv2, bv3);
        }        
           *shape = new btBvhTriangleMeshShape(mesh, true);
    }
	if (mesh->HasTextureCoords (0)) {
		texcoords = (GLfloat*)malloc (*point_count * 2 * sizeof (GLfloat));
		for (int i = 0; i < *point_count; i++) {
			const aiVector3D* vt = &(mesh->mTextureCoords[0][i]);
			texcoords[i * 2] = (GLfloat)vt->x;
			texcoords[i * 2 + 1] = (GLfloat)vt->y;
		}
	}
	
	/* copy mesh data into VBOs */
	if (mesh->HasPositions ()) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			3 * *point_count * sizeof (GLfloat),
			points,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (0);
		free (points);
	}
	if (mesh->HasTextureCoords (0)) {
		GLuint vbo;
		glGenBuffers (1, &vbo);
		glBindBuffer (GL_ARRAY_BUFFER, vbo);
		glBufferData (
			GL_ARRAY_BUFFER,
			2 * *point_count * sizeof (GLfloat),
			texcoords,
			GL_STATIC_DRAW
		);
		glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray (2);
		free (texcoords);
	}
	aiReleaseImport (scene);
	printf ("mesh loaded\n");
	
	return true;
}
