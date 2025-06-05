// shape/triangle.c

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include "shape.h"
#include <stdio.h>
#include <stdlib.h>

// -----------------------------------------------------------------------------
// This file implements init_triangle() and draw_triangle() so that you get
// a spinning pyramid without touching cube.c at all.
// -----------------------------------------------------------------------------

// 1) Embedded GLSL source (no external .vert/.frag files)
// ---------------------------------------------------------------------------

// Vertex shader: applies uMVP to position and passes color through.
static const char* triVertexSrc =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "uniform mat4 uMVP;\n"
    "out vec3 vColor;\n"
    "void main() {\n"
    "    gl_Position = uMVP * vec4(aPos, 1.0);\n"
    "    vColor = aColor;\n"
    "}\n";

// Fragment shader: just writes out the interpolated color.
static const char* triFragmentSrc =
    "#version 330 core\n"
    "in  vec3 vColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(vColor, 1.0);\n"
    "}\n";

// -----------------------------------------------------------------------------
// 2) Helper functions to compile/link a shader & check for errors
// -----------------------------------------------------------------------------

static GLuint compile_shader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    // Check compile status
    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen);
        glGetShaderInfoLog(shader, logLen, NULL, log);
        fprintf(stderr, "Triangle shader compile error:\n%s\n", log);
        free(log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint link_program(GLuint vert, GLuint frag) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);

    // Check link status
    GLint success = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        GLint logLen = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen);
        glGetProgramInfoLog(prog, logLen, NULL, log);
        fprintf(stderr, "Triangle program link error:\n%s\n", log);
        free(log);
        glDeleteProgram(prog);
        return 0;
    }
    return prog;
}

// -----------------------------------------------------------------------------
// 3) Pyramid vertex data (18 vertices: 6 faces × 3 vertices each)
//    Each vertex is (x, y, z,   r, g, b)
// -----------------------------------------------------------------------------
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

    // Base: Triangle A – cyan
    -1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f,

    // Base: Triangle B – cyan
    -1.0f,  0.0f, -1.0f,   0.0f, 1.0f, 1.0f,
     1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,   0.0f, 1.0f, 1.0f
};
#define TRIANGLE_NUM_VERTICES 18

// -----------------------------------------------------------------------------
// 4) “Global” handles for the pyramid’s VAO/VBO and shader‐program
// -----------------------------------------------------------------------------
static GLuint triangleVAO = 0;
static GLuint triangleVBO = 0;
static GLuint triProgram  = 0;
static GLint  tri_uMVPLoc  = -1;

// -----------------------------------------------------------------------------
// 5) init_triangle(): compile/link shaders, set up VAO/VBO for the pyramid
//     → call this once at startup (e.g. in your main.c after initGL())
// -----------------------------------------------------------------------------
void init_triangle(void) {
    // 5.1) Compile the vertex & fragment shaders
    GLuint vs = compile_shader(GL_VERTEX_SHADER, triVertexSrc);
    if (vs == 0) {
        fprintf(stderr, "Failed to compile pyramid vertex shader\n");
        return;
    }
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, triFragmentSrc);
    if (fs == 0) {
        fprintf(stderr, "Failed to compile pyramid fragment shader\n");
        glDeleteShader(vs);
        return;
    }

    // 5.2) Link them into triProgram
    triProgram = link_program(vs, fs);
    // We can delete the individual shader objects once linked:
    glDeleteShader(vs);
    glDeleteShader(fs);

    if (triProgram == 0) {
        fprintf(stderr, "Failed to link pyramid shader program\n");
        return;
    }

    // 5.3) Get location of the “uMVP” uniform in triProgram
    tri_uMVPLoc = glGetUniformLocation(triProgram, "uMVP");
    if (tri_uMVPLoc < 0) {
        fprintf(stderr, "Warning: cannot find uniform 'uMVP' in pyramid shader\n");
    }

    // 5.4) Create VAO + VBO, and upload pyramidVertices[] to GPU
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(pyramidVertices),
                 pyramidVertices,
                 GL_STATIC_DRAW);

    // Position attribute → layout(location = 0) in the vertex shader
    glVertexAttribPointer(
        0,                  // location = 0
        3,                  // 3 floats per position (x, y, z)
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),  // stride: 6 floats per vertex (pos + color)
        (void*)0            // offset = 0
    );
    glEnableVertexAttribArray(0);

    // Color attribute → layout(location = 1) in the vertex shader
    glVertexAttribPointer(
        1,
        3,                          // 3 floats per color (r, g, b)
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),          // same stride
        (void*)(3 * sizeof(float))  // offset = skip the first 3 floats
    );
    glEnableVertexAttribArray(1);

    // Unbind VAO for cleanliness
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// -----------------------------------------------------------------------------
// 6) draw_triangle(): set up a rotating model matrix, compute mvp, draw
//     → call this each frame for a spinning pyramid
// -----------------------------------------------------------------------------
void draw_triangle(void) {
    if (triProgram == 0) {
        // If the shader program did not compile/link, do nothing.
        return;
    }

    // 6.1) Build Model / View / Projection matrices (using cglm)
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 view  = GLM_MAT4_IDENTITY_INIT;
    mat4 proj  = GLM_MAT4_IDENTITY_INIT;
    mat4 mvp;   // combined MVP

    // Spin 45° per second around the Y‐axis:
    float t = SDL_GetTicks() / 1000.0f; // time in seconds
    glm_rotate_y(model, glm_rad(45.0f * t), model);

    // Camera: translate “world” by (0, 0, -5) so we’re at (0,0,5) looking at origin
    glm_translate(view, (vec3){0.0f, 0.0f, -5.0f});

    // Use a 45° vertical FOV, aspect 800/600, near=0.1, far=100
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, proj);

    // mvp = proj * view * model
    glm_mat4_mulN((mat4*[]){&proj, &view, &model}, 3, mvp);

    // 6.2) Draw the pyramid
    glUseProgram(triProgram);
    glUniformMatrix4fv(tri_uMVPLoc, 1, GL_FALSE, (float*)mvp);

    glBindVertexArray(triangleVAO);
    glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_NUM_VERTICES);
    glBindVertexArray(0);

    glUseProgram(0);
}

