#include <iostream>
#define GLSL(src) "#version 150 core\n" #src

const char* vShader = GLSL(
in vec2 pos;
in vec3 color;
in float sides;

uniform mat4 view;
uniform mat4 proj;

out float vSides;

out vec3 vColor; // Output to geometry (or fragment) shader

void main() {
	gl_Position = proj*view*vec4(pos, 0.0,	1.0);
	vColor = color;
	vSides = sides;
}
);
