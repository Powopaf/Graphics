// cube.c
#include <SDL2/SDL.h>      // for SDL_GetTicks()
#include <cglm/cglm.h>     // for mat4, glm_* helpers
#include <stdio.h>
#include <glad/glad.h>
#include "../shader/shader.h"

// ---- static state for the cube ----
static GLuint cubeVAO = 0, cubeVBO = 0;
static GLuint cubeProgram;
static GLint  uMVP_loc;

// CHANGED: replaced old indexed cube data with flat 36-vertex list,
// so each face can be a single solid R, G or B color.
static const float vertices[] = {
    // Face Z- (red) – all vertices use (1,0,0)
    -1,-1,-1,  1,0,0,
     1,-1,-1,  1,0,0,
     1, 1,-1,  1,0,0,
     1, 1,-1,  1,0,0,
    -1, 1,-1,  1,0,0,
    -1,-1,-1,  1,0,0,

    // Face X+ (green) – (0,1,0)
     1,-1,-1,  0,1,0,
     1,-1, 1,  0,1,0,
     1, 1, 1,  0,1,0,
     1, 1, 1,  0,1,0,
     1, 1,-1,  0,1,0,
     1,-1,-1,  0,1,0,

    // Face Y+ (blue) – (0,0,1)
    -1, 1,-1,  0,0,1,
     1, 1,-1,  0,0,1,
     1, 1, 1,  0,0,1,
     1, 1, 1,  0,0,1,
    -1, 1, 1,  0,0,1,
    -1, 1,-1,  0,0,1,

    // Face Z+ (red)
    -1,-1, 1,  1,0,0,
     1,-1, 1,  1,0,0,
     1, 1, 1,  1,0,0,
     1, 1, 1,  1,0,0,
    -1, 1, 1,  1,0,0,
    -1,-1, 1,  1,0,0,

    // Face X- (green)
    -1,-1,-1,  0,1,0,
    -1, 1,-1,  0,1,0,
    -1, 1, 1,  0,1,0,
    -1, 1, 1,  0,1,0,
    -1,-1, 1,  0,1,0,
    -1,-1,-1,  0,1,0,

    // Face Y- (blue)
    -1,-1,-1,  0,0,1,
     1,-1,-1,  0,0,1,
     1,-1, 1,  0,0,1,
     1,-1, 1,  0,0,1,
    -1,-1, 1,  0,0,1,
    -1,-1,-1,  0,0,1
};

void init_cube(void) {
    // 1) Create VAO & VBO (unchanged)
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2) Set vertex attributes (unchanged layout, changed stride to 6 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);

    // 3) Inline shaders with CHANGED: alpha=1.0 for full opacity
    char *vs_src =
        "#version 330 core\n"
        "layout(location=0) in vec3 aPos;\n"
        "layout(location=1) in vec3 aColor;\n"
        "uniform mat4 uMVP;\n"
        "out vec3 vColor;\n"
        "void main() {\n"
        "  vColor = aColor;\n"
        "  gl_Position = uMVP * vec4(aPos,1.0);\n"
        "}\n";
    char *fs_src =
        "#version 330 core\n"
        "in vec3 vColor;\n"
        "out vec4 outColor;\n"
        "void main() {\n"
        "  outColor = vec4(vColor,1.0);  // CHANGED: fully opaque\n"
        "}\n";

    cubeProgram = create_shader(vs_src, fs_src);
    if (!cubeProgram) {
        fprintf(stderr, "Failed to build cube shader program\n");
    }

    // 4) Cache uniform location (unchanged)
    uMVP_loc = glGetUniformLocation(cubeProgram, "uMVP");
}

void draw_cube(void) {
    float t     = SDL_GetTicks() / 1000.0f;       
    float angle = glm_rad(45.0f) * t;            

    mat4 model, view, proj, tmp, mvp;
    glm_mat4_identity(model);

    // CHANGED: tilt ~30° on X so you see three faces clearly
    glm_rotate(model, glm_rad(30.0f), (vec3){1,0,0});
    // CHANGED: then spin around Y
    glm_rotate(model, angle,          (vec3){0,1,0});

    glm_mat4_identity(view);
    glm_translate(view, (vec3){0,0,-5});  // camera back

    glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, proj);

    glm_mat4_mul(proj, view, tmp);
    glm_mat4_mul(tmp,  model, mvp);

    // CHANGED: ensure no blending
    glDisable(GL_BLEND);
    
    glUseProgram(cubeProgram);
    glUniformMatrix4fv(uMVP_loc, 1, GL_FALSE, (float*)mvp);
    glBindVertexArray(cubeVAO);
    // CHANGED: draw 36 vertices directly, no EBO
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

