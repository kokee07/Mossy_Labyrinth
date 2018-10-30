#ifndef TRY_H
#define TRY_H
#include <bullet/btBulletDynamicsCommon.h>
// funcion que carga una malla desde filename
bool load_mesh (const char* file_name, GLuint* vao, int* point_count, GLfloat** vPoints);
// funcion gameplay
bool gameplay(float cam_speed, double elapsed_seconds, float *cam_pos, float *cam_yaw, float cam_yaw_speed);
#endif
