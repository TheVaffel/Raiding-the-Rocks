#define GLSL(src) "#version 150 core\n" #src

const char * colorVertexShaderSrc = GLSL(
in vec3 pos;
in vec4 color;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 trans;

uniform mat4 shadowMat;

out VertexData{
  vec4 Color;
  vec4 pos2;
  vec4 pos1;
}vs;



void main(){
  gl_Position = proj*view*trans*vec4(pos, 1.0);
  //gl_Position = shadowMat*trans*vec4(pos, 1.0);
  vs.pos2 = shadowMat*trans*vec4(pos, 1.0);
  vs.Color = color;
  vs.pos1 = gl_Position;
  //Color = vec4(1.0, 1.0, 1.0, 1.0);
}
);
