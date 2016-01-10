#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

using namespace glm;
using namespace std;
int tok = 0;

mat4 bias  = mat4(0.5, 0.0, 0.0, 0.0,
0.0, 0.5, 0.0, 0.0,
0.0, 0.0, 0.5, 0.0,
0.5, 0.5, 0.5, 1.0);

class Model{
protected:
  float *vertexData;
  GLuint vertexBufferObject;
  GLuint vertexArrayObject;
  GLushort *elements;
  GLuint elementBufferObject;

  int numElements;
  int numVertices;
  int floatsPerVertex;
  int totalDataSize;

  GLuint shaderProgram;
  GLuint uniView;
  GLuint uniProj;
  GLuint uniTrans;

  GLuint shadowShaderProgram;
  GLuint uniShadowTrans;
  GLuint shadowVao = 0;

  GLuint dataMode = GL_STATIC_DRAW;

  virtual void makeData() = 0; //Should make sure numElements,
  //numVertices, floatsPerVertex and totalDataSize are defined

  void specifyAttribute(const char * name, int sizeInFloats, int offset, GLuint shaderP, GLuint ind){

    glBindAttribLocation(shaderP, ind, name);

    glEnableVertexAttribArray(ind);


    glVertexAttribPointer(ind, sizeInFloats, GL_FLOAT, GL_FALSE, floatsPerVertex*sizeof(float), (void*)(offset*sizeof(float)));

  }

  void setUpPostMakeData(){

    glGenBuffers(1, &vertexBufferObject);

    int e = glGetError();
  	if(e != GL_NO_ERROR){
  		cout<<"Encountered ann error"<<endl;
  		if(e == GL_INVALID_OPERATION)
  			cout<<"This is a new GL_INVALID_ENUM error"<<endl;
  	}

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*floatsPerVertex*numVertices, vertexData, dataMode);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    vertexArrayObject = vao;
    glBindVertexArray(vao);

    glGenBuffers(1, &elementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*numElements, elements, dataMode);

    uniView = glGetUniformLocation(shaderProgram, "view");
    uniProj = glGetUniformLocation(shaderProgram, "proj");
    uniTrans = glGetUniformLocation(shaderProgram, "trans");

    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(mat4()));
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(mat4()));

    glUseProgram(shadowShaderProgram);

    glGenVertexArrays(1, &shadowVao);

    uniShadowTrans = glGetUniformLocation(shadowShaderProgram, "transform");

    glUseProgram(shaderProgram);

  }

public:

  int getTotalDataSize(){
    return totalDataSize;
  }

  float* getVertexData(){
    return vertexData;
  }

  GLushort* getElements(){
    return elements;
  }

  int getFloatsPerVertex(){
    return floatsPerVertex;
  }
  int getNumElements(){
    return numElements;
  }
  int getNumVertices(){
    return numVertices;
  }

  void drawToShadowMap(mat4 lightMat, mat4 trans){
    mat4 transmat = lightMat*trans;
    glUseProgram(shadowShaderProgram);
    glUniformMatrix4fv(uniShadowTrans, 1, GL_FALSE, value_ptr(transmat));
    glBindVertexArray(shadowVao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    //cout<<"HELLO"<<endl;
    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
    //cout<<"HELLO2"<<endl;
  }

  void draw(){
    glUseProgram(shaderProgram);
    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);

    glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_SHORT, 0);
  }

  void draw(mat4 view, mat4 trans){
    glUseProgram(shaderProgram);
    setViewMat(view);
    setTransMat(trans);
    draw();
  }

  void draw(mat4 proj, mat4 view, mat4 trans){
    setProjMat(proj);
    draw(view, trans);
  }

  mat4 proj;
  Model(GLuint sp, GLuint shadowP){
    shaderProgram = sp;
    shadowShaderProgram = shadowP;
    glUseProgram(shaderProgram);
    Model::proj = perspective(45.0f, 1.0f, 0.1f, 100.0f);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    vertexBufferObject = vbo;
  }

  Model(){}

  ~Model(){
    //delete[] vertexData;
    //delete[] elements;
  }

  void setTransMat(mat4& m){
    glUniformMatrix4fv(uniTrans, 1, GL_FALSE, glm::value_ptr(m));
  }
  void setViewMat(mat4& m){
    glUniformMatrix4fv(uniView,1, GL_FALSE, glm::value_ptr(m));
  }
  void setProjMat(mat4& m){
    glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(m));
  }

};
/* A subclass of this should, in its constructor:
  0. Make sure the super-constructor is called at first, also passing necessary arguments
  1. Set member variables (passed to constructor)
  2. Call a (self-defined) makeData(), making the data (duh)
  3. Call process()
*/
class ColoredModel : public Model{
protected:
  bool linked = false;

  void process(){
    setUpPostMakeData();

    if(!tok){
      GLuint e = glGetError();
    	if(e != GL_NO_ERROR){
    		cout<<"Encountered ann error"<<endl;
    		if(e == GL_INVALID_VALUE)
    			cout<<"This is a new GL_INVALID_ENUM error"<<endl;
    	}
      tok = 1;
    }

    glBindVertexArray(vertexArrayObject);
    glUseProgram(shaderProgram);
    specifyAttribute("pos", 3, 0, shaderProgram, 9);
    specifyAttribute("color", 4, 3, shaderProgram, 10);
    /*if(!linked)
      glLinkProgram(shaderProgram);*/

    glUseProgram(shadowShaderProgram);
    glBindVertexArray(shadowVao);
    specifyAttribute("pos", 3, 0, shadowShaderProgram, 7);
    /*if(!linked)
      glLinkProgram(shadowShaderProgram);*/
    glUseProgram(shaderProgram);
    linked = true;
  }
public:
  ColoredModel(){}
  ColoredModel(GLuint shaderProgram, GLuint shadowP):Model(shaderProgram, shadowP){
    floatsPerVertex = 7;
  }
};

vec4 quadColors[]  = {vec4(0.5, 0.5, 0.0, 1.0), vec4(0.0, 1.0, 0.0, 1.0),
vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.0, 0.0, 1.0)};

class ColoredQuad : public ColoredModel{ //A is absolute, B and C are relative to A
public:

  vec3 origin;
  vec3 relativeB;
  vec3 relativeC;

  ColoredQuad(vec3 A, vec3 B, vec3 C, GLuint shaderProgram, GLuint shadowP)
    :ColoredModel(shaderProgram, shadowP){
      origin = A;
      relativeC = C;
      relativeB = B;
      makeData();
      process();
  }

  void makeData(){
    numVertices = 4;
    numElements = 6;
    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[4*(3 + 4)];

    elements = new GLushort[6];
    elements[0] = 0; elements[1] = 1; elements[2] = 2;
    elements[3] = 2; elements[4] = 3; elements[5] = 0;

    for(int i = 0; i<3; i++){
      vertexData[i] = origin[i];
      vertexData[i + 7] = origin[i] + relativeB[i];
      vertexData[i + 7*2] = origin[i] + relativeB[i] + relativeC[i];
      vertexData[i + 7*3] = origin[i] + relativeC[i];
    }
    for(int i = 0; i<4; i++){
      for(int j = 0; j<4; j++){
        vertexData[3 + i + 7*j] = quadColors[j][i];
      }
    }
  }
};

class ColoredPyramid : public ColoredModel{
protected:
  float w, d, h;

public:
  ColoredPyramid(float width, float depth, float height, GLuint sp, GLuint shadowP) : ColoredModel(sp, shadowP){
    w = width;
    d = depth;
    h = height;
    dataMode = GL_DYNAMIC_DRAW;

    makeData();
    process();
  }

  void makeData(){
    numVertices = 5;
    numElements = 12;
    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[numVertices*floatsPerVertex];

    elements = new GLushort[12];
    for(int i = 0; i<4; i++){
      elements[3*i] = 0;
      elements[3*i + 1] = i + 1;
      elements[3*i + 2] = (i + 1)%4 + 1;
    }

    float xs[] = {0.0f ,-w/2, w/2, w/2, -w/2};
    float ys[] = {h, 0.0f, 0.0f, 0.0f, 0.0f};
    float zs[] = {0.0f, d/2, d/2, -d/2, -d/2};

    float colors[][4] = {{1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},
    {0.0f, 1.0f, 0.0f, 1.0f},
    {0.0f, 0.0f, 1.0f, 1.0f},
    {0.0f, 0.0f, 0.0f, 1.0f}};

    for(int i = 0; i<5; i++){
      vertexData[floatsPerVertex*i] = xs[i];
      vertexData[floatsPerVertex*i +1] = ys[i];
      vertexData[floatsPerVertex*i +2] = zs[i];

      for(int j = 0; j<4; j++){
        vertexData[floatsPerVertex*i + 3 + j] = colors[i][j];
      }
    }
  }

  void alterData(float time){
    float nheight = 1.0f + 0.2*sin(time*4);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float), sizeof(float), &nheight);
  }
};
