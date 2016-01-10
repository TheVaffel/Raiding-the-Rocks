#include <iostream>
#define GLSL(src) "#version 150 core\n" #src

const char* fShader = GLSL(
in vec3 fColor;

out vec4 outColor;

void main() {
	outColor = vec4(fColor, 1.0);
}
);
