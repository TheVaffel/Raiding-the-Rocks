#include <iostream>
#define GLSL(src) "#version 150 core\n" #src

const char*gShader = GLSL(
	layout(points) in;
layout(triangle_strip, max_vertices = 64) out;

uniform mat4 proj;
uniform mat4 view;

in vec3 vColor[];
in float vSides[];
out vec3 fColor;

const float PI = 3.1415926;

void main() {
	fColor = vColor[0];

	// Safe, GLfloats can represent small integers exactly
	for (int i = 0; i <= vSides[0]; i++) {
		// Angle between each side in radians
		float ang = PI * 2.0 / vSides[0] * i;

		// Offset from center of point (0.3 to accomodate for aspect ratio)
		vec4 offset = proj*view*vec4((i%2)/2.0 - 0.25, 0, -0.3*i, 1.0);
		gl_Position = gl_in[0].gl_Position + offset;

		EmitVertex();
	}

	EndPrimitive();
}
);
