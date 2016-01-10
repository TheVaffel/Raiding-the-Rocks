#pragma once

#include <cstdlib> //for RAND_MAX
#include <random>
#include "Model.hpp"

const int WALL_TYPE_SOFT_ROCK = 0;
const int WALL_TYPE_NORMAL_ROCK = 1;
const int WALL_TYPE_HARD_ROCK = 2;

class Tile;

float vary(int k, int i, int j , float variation){
  return ((((6*j + 5*i*k + (k+4)*7)^19)%11)/11.0  - 0.55)* variation;
}

class BreakingWall : public ColoredModel{
public:
  static const int r = 5;
  static constexpr float variation = 0.02f;
  static const int numFrags = r*r*4*2;//4 sides, 2 per square

  vec3 * originalMiddles = new vec3[numFrags];
  vec3 * originalPositions = new vec3[numFrags*4];


  BreakingWall(GLuint shaderP, GLuint shadowP)
    : ColoredModel(shaderP, shadowP){
      dataMode = GL_DYNAMIC_DRAW;
    makeData();
    process();
  }

  float dx[4] = {-0.5, -0.5, 0.5, 0.5};
  float dz[4] = {-0.5, 0.5, 0.5, -0.5};

  int varyX[4] = {1, 0, 1, 0};
  int varyZ[4] = {0, 1, 0, 1};

  void makeData(){
    numVertices = numFrags*4;
    numElements = numFrags*12;

    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[totalDataSize];

    elements = new GLushort[numElements];



    for(int k = 0; k<4; k++){
      for(int i = 0; i<r; i++){
        for(int j = 0; j<2*r; j++){
          float ri = 1/(float)r;
          int start = ((k*r + i)*2*r + j)*4;
          if(j % 2 == 0){
            originalPositions[start].x = j/2*ri*dx[(k+1)%4] + (1-j/2*ri)*dx[k] + varyX[k]*vary(0, i, j, variation);
            originalPositions[start + 1].x = originalPositions[start].x + varyX[k]*vary(1, i, j, variation);
            originalPositions[start + 2].x = originalPositions[start].x - dx[k]*ri + dx[(k+1)%4]*ri + varyX[k]*vary(2, i, j, variation);

            originalPositions[start].z = j/2*ri*dz[(k+1)%4] + (1-j/2*ri)*dz[k] + varyZ[k]*vary(0, i, j, variation);
            originalPositions[start + 1].z = originalPositions[start].z + varyZ[k]*vary(1, i, j, variation);
            originalPositions[start + 2].z = originalPositions[start].z - dz[k]*ri + dz[(k+1)%4]*ri + varyZ[k]*vary(2, i, j, variation);

            originalPositions[start].y = 1 - i*ri;
            originalPositions[start + 1].y = originalPositions[start].y - ri;
            originalPositions[start + 2].y = originalPositions[start].y;
          }else{
            originalPositions[start].x = j/2*ri*dx[(k+1)%4] + (1-j/2*ri)*dx[k] + varyX[k]*vary(0, i, j, variation);
            originalPositions[start + 1].x = originalPositions[start].x - dx[k]*ri + dx[(k+1)%4]*ri + varyX[k]*vary(1, i, j, variation);
            originalPositions[start + 2].x = originalPositions[start +1].x + varyX[k]*vary(1, i, j, variation);

            originalPositions[start].z = j/2*ri*dz[(k+1)%4] + (1-j/2*ri)*dz[k] + varyZ[k]*vary(0, i, j, variation);
            originalPositions[start + 1].z = originalPositions[start].z - dz[k]*ri + dz[(k+1)%4]*ri + varyZ[k]*vary(1, i, j, variation);
            originalPositions[start + 2].z = originalPositions[start + 1].z + varyZ[k]*vary(2, i, j, variation);

            originalPositions[start].y = 1 - (i+1)*ri;
            originalPositions[start + 1].y = originalPositions[start].y;
            originalPositions[start + 2].y = originalPositions[start].y + ri;
          }
          float hollowness = (1.0f/2);
          originalPositions[start + 3].x = (originalPositions[start + 0].x + originalPositions[start + 1].x+
              originalPositions[start + 2].x)*0.33f * hollowness;
          originalPositions[start + 3].z = (originalPositions[start + 0].z + originalPositions[start + 1].z+
              originalPositions[start + 2].z)*0.33f * hollowness;
          originalPositions[start + 3].y = ((originalPositions[start + 0].y + originalPositions[start + 1].y+
              originalPositions[start + 2].y )*0.33f - 0.5) * hollowness + 0.5;

          originalMiddles[k*2*r*r + i*2*r + j] = 0.25f*(originalPositions[start] + originalPositions[start + 1]
              + originalPositions[start + 2] + originalPositions[start + 3]);

          for(int u = 0; u<4; u++){
            vertexData[(start+u)*floatsPerVertex + 0] = originalPositions[start+u].x;
            vertexData[(start+u)*floatsPerVertex + 1] = originalPositions[start+u].y;
            vertexData[(start+u)*floatsPerVertex + 2] = originalPositions[start+u].z;

            vertexData[(start+u)*floatsPerVertex + 3] = 0.4;
            vertexData[(start+u)*floatsPerVertex + 4] = 0.4;
            vertexData[(start+u)*floatsPerVertex + 5] = 0.4;
            vertexData[(start+u)*floatsPerVertex + 6] = 1.0;
          }
          int fragnum = k*r*r*2 + 2*i*r + j;
          int elementsPerFrag = 12;
          for(int u = 0; u<3; u++){
            elements[fragnum*elementsPerFrag + u] = start + u;
            elements[fragnum*elementsPerFrag + 3 + 3*u + 0] = start + ((u+1)%3);
            elements[fragnum*elementsPerFrag + 3 + 3*u + 1] = start + u;
            elements[fragnum*elementsPerFrag + 3 + 3*u + 2] = start + 3;

          }

        }
      }
    }
  }

  void update(float dt){
    if(dt > 3){
      for(int i = 0; i<numVertices; i++){
        vertexData[i*floatsPerVertex + 1] = -3;
      }
    }else
    if(dt < 0.3){
      for(int k = 0; k<4; k++){
        for(int i = 0; i< r; i++){
          for(int j = 0; j<2*r*4; j++){
            int vertStart = (k*2*r*r + i*2*r + j);
            int s = vertStart*floatsPerVertex;
            vertexData[s + 0] = originalPositions[vertStart].x;
            vertexData[s + 1] = originalPositions[vertStart].y;
            vertexData[s + 2] = originalPositions[vertStart].z;
          }
        }
      }
    }else{
      dt-= 0.3;
      for(int k = 0; k<4; k++){
        for(int i = 0; i< r; i++){
          for(int j = 0; j<2*r*4; j++){

            int vertStart = (k*2*r*r + i*2*r)*4 + j;
            int middleind = vertStart/4;
            int s = vertStart*floatsPerVertex;
            vertexData[s + 0] = originalPositions[vertStart].x + originalMiddles[middleind].x*(1.0f + vary(k, i, middleind, 2.0f)*dt) - originalMiddles[middleind].x;
            vertexData[s + 1] = originalPositions[vertStart].y + originalMiddles[middleind].y*(1.0f + (vary(k, i, middleind, 1.5f) + 1.5f  )*dt) - 3*dt*dt - originalMiddles[middleind].y;
            vertexData[s + 2] = originalPositions[vertStart].z + originalMiddles[middleind].z*(1.0f + vary(k, i, middleind*2, 2.0f)*dt) - originalMiddles[middleind].z;
          }
        }
      }
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*floatsPerVertex*numVertices, vertexData);
  }
};

class Wall : public ColoredModel{
  Tile* tile = 0;
  int r;
  int type;
  float variation = 0.005;
  float breaktime = -1;
public:
  static const int numTypes = 3;

  static const int MAX_EXTRA_VERTICES = 200;
  static const int MAX_EXTRA_ELEMENTS = 400;

  const int EXTRA_VERTICES[numTypes] = {0, 0, 0};
  const int EXTRA_ELEMENTS[numTypes] = {0, 0, 0};

  const float red[numTypes] = {0.4, 0.3, 0.2};
  const float green[numTypes] = {0.4, 0.3, 0.2};
  const float blue[numTypes] = {0.4, 0.3, 0.2};

  bool broken = false;
  bool breaking = false;
  Wall(Tile* nTile, int resolution, int nType, GLuint shaderProgram, GLuint shadowShader);
const float fallConst = 1.0;
  const float fallTime = 0.5;

  void breakAnim(float time);
  void makeData();
  void makeExtraDataAndElements();
};

class Gravel: public ColoredModel{
  struct Fragment{
    vec3 middle;
    bool falling = true;
    mat4 rot;
    static constexpr float s = 0.2f;
    static constexpr int numVertices = 6;
    static constexpr int floatsPerVertex = 7; //Not elegant, but..
    static constexpr int numElements = 24;

    Fragment();
    Fragment(float * vertexData, GLushort * elements, int fragnum, vec3& pos);
  };
  static const int numFragments = 6;
  Fragment fragments[numFragments];
  float startFall = -1;

  Tile* tile = 0;
public:
  bool isFalling = true;
  Gravel(Tile* tile, GLuint shaderP, GLuint shadowShaderP);
  void makeData();
  void fall(float t);
};


class GroundModel : public ColoredModel{
  int w, h;
public:
  GroundModel(int width, int length,
    GLuint shaderProgram, GLuint shadowProgram)
    : ColoredModel(shaderProgram, shadowProgram){
    w = width;
    h = length;
    makeData();
    process();

  }
private:
  void makeData(){
    int width = w;
    int height = h;
    numVertices = (width +1)* (height + 1);
    numElements = width*height*2*3; //Two triangles per area unit, three elements per triangle
    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[totalDataSize];

    elements = new GLushort[numElements];
    int nh = height + 1;
    int nw =  width +1;
    for(int i = 0; i<nw; i++){
      for(int j = 0; j<nh; j++){
        vertexData[(nh*i + j) * floatsPerVertex + 0] = i - nw/2.0f;
        vertexData[(nh*i + j) * floatsPerVertex + 1] = ((j*i ^ 23) % 11)*0.1;
        vertexData[(nh*i + j) * floatsPerVertex + 2] = j - nh/2.0f;

        vertexData[(nh*i + j) * floatsPerVertex + 3] = 0.2f;
        vertexData[(nh*i + j) * floatsPerVertex + 4] = 0.2f;//((j*i ^ 23) % 11)*0.09;
        vertexData[(nh*i + j) * floatsPerVertex + 5] = 0.2f;
        vertexData[(nh*i + j) * floatsPerVertex + 6] = 1.0f;
      }
    }

    for(int i = 0; i<width; i++){
      for(int j = 0; j<height; j++){
        elements[3*2*(i*height + j) + 0] = nh*i + j;
        elements[3*2*(i*height + j) + 1] = nh*i + j + 1;
        elements[3*2*(i*height + j) + 2] = nh*(i+1) + j;

        elements[3*2*(i*height + j) + 3] = nh*(i+1) + j;
        elements[3*2*(i*height + j) + 4] = nh*i + j + 1;
        elements[3*2*(i*height + j) + 5] = nh*(i+1) + j + 1;
      }
    }
  }
};

class Tile : public ColoredModel{
  public:
  int x, z;
  int r;
  float heightVariation = 0.005;
  float heights[4];

  Wall* wall = 0;
  Gravel* gravel = 0;

  Tile(int nx, int nz, float hs[4], int resolution,
    GLuint shaderProgram, GLuint shadowShader)
    : ColoredModel(shaderProgram, shadowShader){
      x = nx; z = nz;
      r = resolution;
      for(int i = 0; i<4; i++){
        heights[i] = hs[i];
      }
      makeData();
      process();
    }
    Tile():ColoredModel(){}
    int getResolution(){
      return r;
    }

  float baseHeight(){
    return (heights[0] +heights[1] + heights[2] + heights[3])*0.25;
  }

  void tick(float time){
    if(wall != 0){
      //cout<<"Hum?"<<endl;
      if(wall->breaking){
        gravel->fall(time);
        wall->breakAnim(time);
        if(wall->broken){
          delete wall;
          wall = 0;
        }
      }
      return;
    }
    if(gravel != 0){
      if(gravel->isFalling){
        gravel->fall(time);
      }
    }
  }

  void wallIsBreaking(){
    wall->breaking = true;
    gravel = new Gravel(this, shaderProgram, shadowShaderProgram);
  }

  void issueDrawing(mat4 view, mat4 trans){
    if(wall != 0){
      wall->draw(view, trans);
      if(wall->breaking)
        gravel->draw(view, trans);
    }else{
      draw(view, trans);
      if(gravel != 0)
        gravel->draw(view, trans);
    }
  }

private:
  void makeData(){
    numVertices = (r +1)* (r + 1);
    numElements = r*r*2*3; //Two triangles per area unit, three elements per triangle
    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[totalDataSize];

    elements = new GLushort[numElements];
    int nh = r + 1;
    int nw =  r +1;

    for(int i = 0; i<nw; i++){
      for(int j = 0; j<nh; j++){
        vertexData[(nh*i + j) * floatsPerVertex + 0] = x + (i - r/2.0f)/r;
        float w1i = i/(float)r;
        float w1 = 1-w1i;
        float w2i = j/(float)r;
        float w2 = 1 - w2i;
        vertexData[(nh*i + j) * floatsPerVertex + 1] = w1*(w2*heights[0] + w2i*heights[2]) + (1-w1)*(w2*heights[1] + w2i*heights[3]);
        vertexData[(nh*i + j) * floatsPerVertex + 2] = z + (j - r/2.0f)/r;

        vertexData[(nh*i + j) * floatsPerVertex + 3] = 0.2f;
        vertexData[(nh*i + j) * floatsPerVertex + 4] = 0.2f;
        vertexData[(nh*i + j) * floatsPerVertex + 5] = 0.2f;
        vertexData[(nh*i + j) * floatsPerVertex + 6] = 1.0f;
      }
    }

    for(int i = 1; i<nw-1; i++){
      for(int j = 1; j<nh-1; j++){
        vertexData[(nh*i + j) * floatsPerVertex + 1] += ((((j*i*(x+1000)+z+1000) ^ 23) % 11) - 5)*heightVariation;

        vertexData[(nh*i + j) * floatsPerVertex + 4] = 0.2f;//(((j*i*(x+1000)+z+1000) ^ 23) % 11)*0.09;
      }
    }
    for(int i = 0; i<r; i++){
      for(int j = 0; j<r; j++){
        elements[3*2*(i*r + j) + 0] = nh*i + j;
        elements[3*2*(i*r + j) + 1] = nh*i + j + 1;
        elements[3*2*(i*r + j) + 2] = nh*(i+1) + j;

        elements[3*2*(i*r + j) + 3] = nh*(i+1) + j;
        elements[3*2*(i*r + j) + 4] = nh*i + j + 1;
        elements[3*2*(i*r + j) + 5] = nh*(i+1) + j + 1;
      }
    }
  }
};



Wall::Wall(Tile* nTile, int resolution, int nType, GLuint shaderProgram, GLuint shadowShader)
  : ColoredModel(shaderProgram, shadowShader){
    tile = nTile;
    r = resolution;
    type = nType;
    nTile->wall = this;
    dataMode = GL_DYNAMIC_DRAW;
    makeData();
    process();
  }

void Wall::breakAnim(float time){
    if(breaktime == -1){
      breaktime = time;
    }
    breaking = true;

    for(int k = 0; k<4; k++){
      for(int i = 0; i<r; i++){
        for(int j = 0; j<r; j++){
          int st = (k*r*(r+1) + i*r + j)*floatsPerVertex;
          float by = vertexData[floatsPerVertex*(k*r*(r+1) + r*r + j) +1];
          float vy = vertexData[st+ 1];
          vertexData[st + 0] = (vertexData[st + 0] - tile->x)*pow(0.5 + abs(i/(float)r-0.5), (time - breaktime)*fallConst) + tile->x;
          vertexData[st + 2] = (vertexData[st + 2] - tile->z)*pow(0.5 + abs(i/(float)r-0.5), (time - breaktime)*fallConst) + tile->z;
          vertexData[st + 1] = (vy - by)*pow(0.6, time - breaktime) + by;
        }
      }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*floatsPerVertex*numVertices, vertexData);

    if( time - breaktime > fallTime){
      broken =  true;
      breaking = false;
    }
  }

void Wall::makeData(){
    numVertices = 4*(r)* (r + 1) + EXTRA_VERTICES[type];
    numElements = 4*r*r*2*3 + EXTRA_ELEMENTS[type]; //Two triangles per area unit, three elements per triangle
    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[totalDataSize];

    elements = new GLushort[numElements];

    int nw = r+1;
    int nh = r+1;
    int corn1s[] = {0, 2, 3, 1};//k is counter
    int corn2s[] = {2, 3, 1, 0};
    float dx[] = {-0.5, 0.5, -0.5, 0.5};//corner* is counter
    float dz[] = {-0.5, -0.5, 0.5, 0.5};
    int varx[] = {1, 0, 1, 0};//k is counter
    int varz[] = {0, 1, 0, 1};
    int corner1, corner2;

    for(int i= 0; i<numVertices; i++){
      vertexData[floatsPerVertex*i] = tile->x;
      vertexData[floatsPerVertex*i +2] = tile->z;
    }

    for(int k = 0; k<4; k++){
      corner1  = corn1s[k]; corner2 = corn2s[k];
      int s1 = (k*r*nh + r*r)*floatsPerVertex;
      vertexData[s1] = tile->x + dx[corner1];
      vertexData[s1 + 1] = tile->heights[corner1];//bottom left of side of wall.
      vertexData[s1 + 2] = tile->z + dz[corner1];

      vertexData[s1 + 3] = red[type];
      vertexData[s1 + 4] = green[type];
      vertexData[s1 + 5] = blue[type];
      vertexData[s1 + 6] = 1.0;

      vertexData[(((k+1)*r*nh + r*r) % (4*r*nh))*floatsPerVertex + 1] = tile->heights[corner2];
      for(int i = 1; i<r; i++){
        float ri = i/(float)r;
        int s = (k*r*nh + r*r +i)*floatsPerVertex;
        vertexData[s + 1] = ri*tile->heights[corner1] + (1-ri)*tile->heights[corner2];
        vertexData[s] = tile->x + ri*dx[corner2] + (1-ri)*dx[corner1];
        vertexData[s + 2] = tile->z + ri*dz[corner2] + (1-ri)*dz[corner1];

        vertexData[s + 3] = red[type];
        vertexData[s + 4] = green[type];
        vertexData[s + 5] = blue[type];
        vertexData[s + 6] = 1.0;
      }

      for(int i = 0; i<nh-1; i++){
        for(int j = 0; j<r; j++){
          int start = (k*r*nh+ i*r + j)*floatsPerVertex;
          float wj = j/(float)r;
          float iwj = 1 - wj;
          vertexData[start] = tile->x + wj*dx[corner2] + iwj*dx[corner1] + varx[k]*((((i*3 +j*2 + tile->x)^501)%19) - 10)*variation;
          vertexData[start + 1] = 1 - i/(float)r;
          vertexData[start + 2] = tile->z + wj*dz[corner2] + iwj*dz[corner1] + varz[k]*((((i*2 +j*5 + tile->z)^51)%19) - 10)*variation;

          vertexData[start + 3] = red[type];
          vertexData[start + 4] = green[type];
          vertexData[start + 5] = blue[type];
          vertexData[start + 6] = 1.0;
        }
      }
      for(int i = 0; i<nh-1; i++){
        int start = (k*r*nh  +i*r)*floatsPerVertex;
        vertexData[start] = tile->x + dx[corner1];
        vertexData[start + 2] = tile->z + dz[corner1];
      }

    }
    for(int i = 0; i<numElements; i++){
      elements[i] = 0;
    }

    for(int k = 0; k<4; k++){
      for(int i = 0; i<nh-1; i++){
        for(int j = 0; j<r; j++){
          int start = ((k*r + i)*r + j)*3*2;
          int startVertex = (k*(r+1) + i)*r + j;
          elements[start] = startVertex;
          elements[start + 1] = startVertex + r;
          elements[start + 2] = (j == r-1)?(r*r +startVertex + 1)%(4*(r+1)*r) : startVertex+1;

          elements[start + 3] = (j == r-1)?(r*r +startVertex + 1)%(4*(r+1)*r) : startVertex+1;
          elements[start + 4] = startVertex + r;
          elements[start + 5] = (j == r-1)?((r*r +startVertex +r+1)%(4*(r+1)*r)) : startVertex+1 + r ;
        }
      }
    }

    makeExtraDataAndElements();
  }


  void Wall::makeExtraDataAndElements(){

  }

float getRandom(float low, float high){
  return (rand())*(high - low)/RAND_MAX + low;
}
    Gravel::Fragment::Fragment(){}
    Gravel::Fragment::Fragment(float * vertexData, GLushort * elements, int fragnum, vec3& pos){
      int vertStart = floatsPerVertex*numVertices*fragnum;
      int elemStart = fragnum*numElements;

      middle = vec3(pos.x +(1 + (fragnum%3))/4.0 + getRandom(-0.15, 0.15) - 0.5, pos.y + 1 + getRandom(-0.15, 0.15), pos.z + (1 + (fragnum/3))/3.0 + getRandom(-0.15, 0.15) - 0.5);
      for(int i = 0; i<numVertices; i++){
        for(int j = 0; j<3; j++){
          vertexData[vertStart +i*floatsPerVertex + j] = middle[j];
        }
        for(int j = 0; j<4; j++){
          vertexData[vertStart +i*floatsPerVertex + 3 + j] = 0.3;
        }
      }
      vertexData[vertStart + 0*floatsPerVertex + 1] = middle.y + (float)sqrt(2)*s/2;
      vertexData[vertStart + 1*floatsPerVertex + 0] = middle.x + s/2;
      vertexData[vertStart + 1*floatsPerVertex + 2] = middle.z + s/2;
      vertexData[vertStart + 2*floatsPerVertex + 0] = middle.x + s/2;
      vertexData[vertStart + 2*floatsPerVertex + 2] = middle.z + -s/2;
      vertexData[vertStart + 3*floatsPerVertex + 0] = middle.x + -s/2;
      vertexData[vertStart + 3*floatsPerVertex + 2] = middle.z + -s/2;
      vertexData[vertStart + 4*floatsPerVertex + 0] = middle.x + -s/2;
      vertexData[vertStart + 4*floatsPerVertex + 2] = middle.z + s/2;
      vertexData[vertStart + 5*floatsPerVertex + 1] = middle.y + -(float)sqrt(2)*s/2;

      vec4 dist;
      float theta = getRandom(-M_PI, M_PI);
      float phi = getRandom(0, 2*M_PI);
      vec3 d1 = vec3(cos(theta)*cos(phi), sin(theta), cos(theta)*sin(phi));
      mat4 rot1 = rotate(mat4(), getRandom(0, 2*M_PI), d1);
      for(int i = 0; i<numVertices; i++){
        theta = getRandom(-M_PI, M_PI);
        phi = getRandom(0, 2*M_PI);
        dist = vec4(cos(theta)*cos(phi)*s*0.4, sin(theta)*0.4*s, cos(theta)*sin(phi)*0.4*s, 1.0);
        dist.x += vertexData[vertStart + i*floatsPerVertex + 0];
        dist.y += vertexData[vertStart + i*floatsPerVertex + 1];
        dist.z += vertexData[vertStart + i*floatsPerVertex + 2];
        dist = rot1*(dist - vec4(middle, 1.0)) + vec4(middle, 1.0);
        vertexData[vertStart +i*floatsPerVertex + 0] = dist.x;
        vertexData[vertStart +i*floatsPerVertex + 1] = dist.y;
        vertexData[vertStart +i*floatsPerVertex + 2] = dist.z;
      }
      rot = rotate(mat4(), 0.5f, d1);

      short startVertex = numVertices*fragnum;

      for(int i = 0; i<4; i++){
        elements[elemStart + i*3] = startVertex;
        elements[elemStart + i*3 + 1] = startVertex + i +1;
        elements[elemStart + i*3 + 2] = startVertex + (i+1)%4 +1;

        elements[elemStart + 12 + i*3] = startVertex + 5;
        elements[elemStart + 12 + i*3 + 1] = startVertex + (i+1)%4 +1;
        elements[elemStart + 12 + i*3 + 2] = startVertex + i + 1;
      }
    }


  Gravel::Gravel(Tile* ntile, GLuint shaderP, GLuint shadowShaderP)
    : ColoredModel(shaderP, shadowShaderP){
      tile = ntile;
      dataMode = GL_DYNAMIC_DRAW;
      srand(12345);
      makeData();
      process();

  }

  void Gravel::makeData(){
    numVertices = numFragments*Fragment::numVertices;
    numElements = numFragments*Fragment::numElements;

    totalDataSize = numVertices*floatsPerVertex;

    vertexData = new float[totalDataSize];

    elements = new GLushort[numElements];
    vec3 tilepos = vec3(tile->x, tile->baseHeight(), tile->z);
    for(int i = 0; i<numFragments; i++){
      fragments[i] = Fragment(vertexData, elements, i, tilepos);
    }
  }

  void Gravel::fall(float t){
    if(!isFalling)
      return;

    if(startFall == -1)
      startFall = t;
    float by = tile->baseHeight();
    bool okfall = false;

    for(int i = 0; i<numFragments; i++){
      if(!(fragments[i].falling)){
        continue;
      }
      okfall = true;
      float oy = fragments[i].middle.y;
      fragments[i].middle.y = (fragments[i].middle.y - by)*pow(0.6, t - startFall) + by;
      float dy = oy - fragments[i].middle.y;

      for(int j = 0; j<Fragment::numVertices; j++){

        int s = i*Fragment::numVertices*floatsPerVertex + j*floatsPerVertex;
        vertexData[s +1 ] -= dy;
        vec4 vert = vec4(vertexData[s], vertexData[s+1], vertexData[s+2] ,1.0);
        vert = fragments[i].rot*(vert - vec4(fragments[i].middle, 1.0)) + vec4(fragments[i].middle, 1.0);
        vertexData[s] = vert.x; vertexData[s+1] = vert.y; vertexData[s+2] = vert.z;

      }

      if(fragments[i].middle.y<by){
        fragments[i].falling = false;
      }
    }
    if(!okfall){
      isFalling = false;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*floatsPerVertex*numVertices, vertexData);
  }


void fillTileElementAndVertexBuffers(Tile* tiles,int w, //width of tile-map. BTW: this function is probably faulty
   GLushort * elementBuffer, float *dataBuffer,
   int startx, int starty, int endx, int endy){ //endx and endy are exclusive
     int count = 0;
     GLushort *buf2 = tiles[0].getElements();
  for(int i = starty; i<endy; i++){
    for(int j = startx; j<endx; j++){
      float* buf = tiles[i*w + j].getVertexData();
      int v = tiles[0].getFloatsPerVertex()*tiles[0].getNumVertices();
      int u =count*v;
      for(int k = 0; k<v; k++){
        dataBuffer[u +k] = buf[k] + 1.0;
      }
      v = tiles[0].getNumElements();
      u = count*v;
      for(int k = 0; k<v; k++){
        elementBuffer[u + k] = (GLushort)(buf2[k] + u);
      }

      count++;
    }
  }
}

float getHeight(float* vertexData, int r, float relX, float relZ){
  /*cout<<"In getHeight:"<<endl;
  for(int i = 0; i<(r+1)*(r+1); i++){
    cout<<vertexData[7*i+1]<<", ";
  }
  cout<<endl;*/

  int relintX = (int)(relX*r);
  relX*= r;relX -= relintX;
  int relintZ = (int)(relZ*r);
  relZ*= r;relZ -= relintZ;
  //cout<<relX<<"  "<<relZ<<endl;
  if(relX + relZ > 1.0){
    //cout<<"Combination of: "<<hmap[relintX*(r + 1) + relintZ +1]<<", "<<hmap[relintX*(r + 1) + relintZ]<<", "<<hmap[(relintX+1)*(r + 1) + relintZ +1]<<endl;
    return (relZ*vertexData[7*(relintX*(r + 1) + relintZ +1)+1] + relX*vertexData[7*((relintX+1)*(r + 1) + relintZ)+1] + (1-relZ-relX)*vertexData[7*((relintX+1)*(r + 1) + relintZ +1)+1])/3;
  }else{
    //cout<<"Combination of: "<<hmap[relintX*(r + 1) + relintZ +1]<<", "<<hmap[relintX*(r + 1) + relintZ]<<", "<<hmap[(relintX)*(r + 1) + relintZ]<<endl;
    return ((1-relX)/2*vertexData[7*(relintX*(r + 1) + relintZ +1)+1] + (1-relZ)/2*vertexData[7*((relintX+1)*(r + 1) + relintZ)+1] + (relX+relZ -1)*vertexData[7*((relintX)*(r + 1) + relintZ)+1])/3;
  }
}
