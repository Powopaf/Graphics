#ifndef SHAPE_H
#define SHAPE_H

#include <glad/glad.h>

// ------------------------------------------------------------------
// Cube functions (already existing):
// ------------------------------------------------------------------
void init_cube(void);
void draw_cube(void);

// *** MODIFIED ***
// Export the cube’s shader handle and its uMVP uniform location, so
// triangle.c can reuse exactly the same shader:
extern GLuint cubeProgram;
extern GLint  cube_uMVP_loc;

// ------------------------------------------------------------------
// Triangle/Pyramid functions (renamed from “pyramid” to match your code):
// ------------------------------------------------------------------
void init_triangle(void);
void draw_triangle(void);

#endif // SHAPE_H

