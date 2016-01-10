#define GLSL(src) "#version 150 core\n" #src

const char * colorGeometryShaderSrc = GLSL(

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VertexData {
  vec4 Color;
  vec4 pos2;
  vec4 pos1;
} VertexIn[3];

out VertexData {
  vec4 Color;
  vec4 pos2;
  vec4 pos1;
  vec3 normal;
} VertexOut;

void main(){
  VertexOut.normal = normalize(cross(gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz,
    gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz));
  for(int i = 0; i<3; i++){
    gl_Position = gl_in[i].gl_Position;
    VertexOut.pos2 = VertexIn[i].pos2;
    VertexOut.pos1 = VertexIn[i].pos1;
    VertexOut.Color = VertexIn[i].Color;
    //VertexOut.Color = vec4(1.0, 1.0, 1.0, 1.0);
    EmitVertex();
  }

  EndPrimitive();

}
);
