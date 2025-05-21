// shader.c
#include <stdlib.h>
#include <stdio.h>
#include <glad/glad.h>

GLuint compile_shader(GLenum shader_type, const char* src) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        fprintf(stderr, "Error shader %s compile:\n%s\n",
                (shader_type == GL_VERTEX_SHADER ? "vertex" :
                 shader_type == GL_FRAGMENT_SHADER ? "fragment" : "unknown"),
                log);
        glDeleteShader(shader);
        return (GLuint)-1;
    }
    return shader;
}

GLuint create_shader(char* vertex, char* fragment) {
    GLuint shader_v = compile_shader(GL_VERTEX_SHADER,   vertex);
    if (shader_v == (GLuint)-1) return (GLuint)-1;
    GLuint shader_f = compile_shader(GL_FRAGMENT_SHADER, fragment);
    if (shader_f == (GLuint)-1) {
        glDeleteShader(shader_v);
        return (GLuint)-1;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader_v);
    glAttachShader(program, shader_f);
    glLinkProgram(program);

    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), NULL, log);
        fprintf(stderr, "Error linking shader program:\n%s\n", log);
        glDeleteProgram(program);
        program = (GLuint)-1;
    }

    glDeleteShader(shader_v);
    glDeleteShader(shader_f);
    return program;
}

