#define GLSL(src) "#version 150 core\n" #src

const char * shadowVertexShaderSrc = GLSL(
in vec3 pos;

uniform mat4 transform;

void main(){
  gl_Position = transform*vec4(pos, 1.0);
  //gl_Position = vec4(pos, 1.0f);

}
);
