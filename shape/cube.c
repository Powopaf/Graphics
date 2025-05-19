#include <SDL2/SDL.h>      // for SDL_GetTicks()
#include <cglm/cglm.h>     // for mat4, glm_* helpers
#include <stdio.h>
#include <glad/glad.h>
#include "../shader/shader.h"
// your shader compile/link helper from shader.c
extern GLuint create_shader_program(const char *vert_src,
                                    const char *frag_src);

// ---- static state for the cube ----
static GLuint cubeVAO, cubeVBO, cubeEBO;
static GLuint cubeProgram;
static GLint  uMVP_loc;

// Cube vertex positions (x,y,z) followed by colors (r,g,b)
static const float vertices[] = {
    -1,-1,-1,   1,0,0,
     1,-1,-1,   0,1,0,
     1, 1,-1,   0,0,1,
    -1, 1,-1,   1,1,0,
    -1,-1, 1,   1,0,1,
     1,-1, 1,   0,1,1,
     1, 1, 1,   1,1,1,
    -1, 1, 1,   0,0,0
};

// Index list for 12 triangles (2 per face)
static const unsigned int indices[] = {
    0,1,2,  2,3,0,
    4,5,6,  6,7,4,
    4,0,3,  3,7,4,
    1,5,6,  6,2,1,
    4,5,1,  1,0,4,
    3,2,6,  6,7,3
};

void init_cube(void) {
    // 1) Generate and bind a Vertex Array Object 
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // 2) Create & upload vertex buffer (positions + colors)
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3) Create & upload element buffer (indices)
    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 4) Tell OpenGL how to interpret the interleaved data:
    //    - layout(location=0): vec3 position at offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //    - layout(location=1): vec3 color at offset 3
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO for safety
    glBindVertexArray(0);

    // 5) Compile & link the cube’s shaders
    const char *vs_src =
        "#version 330 core\n"
        "layout(location=0) in vec3 aPos;\n"
        "layout(location=1) in vec3 aColor;\n"
        "uniform mat4 uMVP;\n"
        "out vec3 vColor;\n"
        "void main() {\n"
        "  vColor = aColor;\n"
        "  gl_Position = uMVP * vec4(aPos,1.0);\n"
        "}\n";
    const char *fs_src =
        "#version 330 core\n"
        "in vec3 vColor;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "  outColor = vec4(vColor,1.0);\n"
        "}\n";

    cubeProgram = create_shader(vs_src, fs_src);
    if (!cubeProgram) {
        fprintf(stderr, "Failed to build cube shader program\n");
    }

    // 6) Cache the uniform location so draw_cube can set it fast
    uMVP_loc = glGetUniformLocation(cubeProgram, "uMVP");
}

void draw_cube(void) {
    // 1) Compute a rotating Model-View-Projection matrix:
    float t     = SDL_GetTicks() / 1000.0f;        // seconds since init
    float angle = glm_rad(45.0f) * t;              // 45° per second

    mat4 model, view, proj, tmp, mvp;
    glm_mat4_identity(model);
    glm_rotate(model, angle, (vec3){0,1,0});       // rotate around Y
    glm_mat4_identity(view);
    glm_translate(view, (vec3){0,0,-5});           // move camera back
    // perspective FOV=45°, aspect=window_w/h, near=0.1, far=100
    // You might query w/h from SDL if it can resize
    glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, proj);

    glm_mat4_mul(proj, view, tmp);
    glm_mat4_mul(tmp, model, mvp);

    // 2) Draw:
    glUseProgram(cubeProgram);                     // select our shader
    glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, (float*)mvp);
    glBindVertexArray(cubeVAO);                    // bind the cube VAO
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
    glBindVertexArray(0);
}

