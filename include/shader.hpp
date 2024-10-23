#pragma once

#include <array>
#include <GL/glew.h>
#include <iostream>

unsigned int createVertexShader(const char *vertexSource);
unsigned int createFragmentShader(const char *fragmentSource);
unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);