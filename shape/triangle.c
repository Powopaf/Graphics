// shape/triangle.c
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cglm/cglm.h>
#include "shape.h"

// We still reuse cubeProgram & cube_uMVP_loc from cube.c
extern GLuint cubeProgram;
extern GLint  cube_uMVP_loc;

static GLuint triangleVAO = 0, triangleVBO = 0;
#define TRIANGLE_NUM_VERTICES 18

static const float pyramidVertices[] = {
    // Side 1 (Front) – red
     0.0f,  1.0f,  0.0f,   1.0f, 0.0f, 0.0f,
    -1.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
     1.0f,  0.0f, -1.0f,   1.0f, 0.0f, 0.0f,

    // Side 2 (Right) – green
     0.0f,  1.0f,  0.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 0.0f,

    // Side 3 (Back) – blue
     0.0f,  1.0f,  0.0f,   0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,   0.0f, 0.0f, 1.0f,

    // Side 4 (Left) – yellow
     0.0f,  1.0f,  0.0f,   1.0f, 1.0f, 0.0f,
    -1.0f,  0.0f,  1.0f,   1.0f, 1.0f, 0.0f,
    -1.0f,  0.0f, -1.0f,   1.0f, 1.0f, 0.0f,

    // Base (Triangle A) – cyan
    -1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f,

    // Base (Triangle B) – cyan
    -1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f
};

void init_triangle(void) {
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    // Position @ location 0
    glVertexAttribPointer(
        0,                  // layout(location = 0)
        3,                  // x, y, z
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),  // stride: 6 floats per vertex
        (void*)0
    );
    glEnableVertexAttribArray(0);

    // Color @ location 1
    glVertexAttribPointer(
        1,                          // layout(location = 1)
        3,                          // r, g, b
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),          // same stride
        (void*)(3 * sizeof(float))  // offset: skip x,y,z
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void draw_triangle(void) {
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 view  = GLM_MAT4_IDENTITY_INIT;
    mat4 proj  = GLM_MAT4_IDENTITY_INIT;
    mat4 mvp;

    // CHANGED: apply a Y‐axis rotation based on time
    float t = SDL_GetTicks() / 1000.0f;                   // time in seconds
    glm_rotate_y(model, glm_rad(45.0f * t), model);       // rotates 45°/sec

    // Camera at (0,0,5)
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f});
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, proj);

    glm_mat4_mulN((mat4*[]){&proj, &view, &model}, 3, mvp);

    // Reuse the cube’s shader program and uniform location:
    glUseProgram(cubeProgram);
    glUniformMatrix4fv(cube_uMVP_loc, 1, GL_FALSE, (float*)mvp);

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_VERTICES);
    glBindVertexArray(0);
    glUseProgram(0);
}

