#define GLSL(src) "#version 150 core\n" #src

const char * shadowFragmentShaderSrc = GLSL(
out float depth;

void main(){
    depth = gl_FragCoord.z;
    for(int i = 0; i<10; i++){
      i++;
    }
    //depth = 0.0f;

}
);
