#define GLSL(src) "#version 150 core\n" #src

const char * colorFragmentShaderSrc = GLSL(
in VertexData{
  vec4 Color;
  vec4 pos2;
  vec4 pos1;
  vec3 normal;
}vs;

uniform sampler2DShadow depthTex;

out vec4 outColor;

void main(){

  float visibility = 0.5;
  /*if(texture(depthTex, pos2.xy).z == 0.0)
  if(texture(depthTex, pos2.xy).z < pos2.z - 0.001){
    visibility = 0.0;
  }*/
  float zbias = -0.001;
  visibility = texture(depthTex, vec3(vs.pos2.xy, vs.pos2.z/vs.pos2.w + zbias));
  //visibility = 0.0;
  //outColor = visibility*Color + 0.5*Color*(-1-pos1.z*0.1);
  outColor = vs.Color*max(0.4, abs(pow(dot(vs.normal, vec3(0.0,0.0, 1.0)), 1))) + vs.Color*0.2*dot(vec3(0.0, 1.0, 0.0), vs.normal); //+ 0.5*vs.Color*(-1-vs.pos1.z*0.1);
  //outColor = Color;
}
);
