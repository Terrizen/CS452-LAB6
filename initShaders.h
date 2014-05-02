#ifndef INITSHADERS_H_
#define INITSHADERS_H_
#endif

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cstdio>
#include <iostream>
using namespace std;

GLuint createShader(GLenum type, const GLchar* shadeSource);
const GLchar* inputShader(const char* filename);
GLuint createProgram(const vector<GLuint> shadeList);
void transform(GLuint program);


typedef struct{
  GLenum type;
  const char* filename;
} ShaderInfo;


GLuint initShaders(ShaderInfo* shaders){

  ShaderInfo* shade=shaders;

  vector<GLuint> shadeList;

  while(shade->type != GL_NONE){
    shadeList.push_back(createShader(shade->type,inputShader(shade->filename)));
    ++shade;
  }

  GLuint program=createProgram(shadeList);

        glUseProgram(program);

  glm::mat4 view;
  view = glm::lookAt(
         glm::vec3(0.0f, 0.0f, 50.0f),
         glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3(0.0f, 1.0f, 0.0f)
    );
  GLint tempLoc = glGetUniformLocation(program, "viewMatrix");
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &view[0][0]);

  glm::mat4 mainProjMatrix;
  mainProjMatrix = glm::perspective(57.0,1.0,.1,500.0);
  tempLoc = glGetUniformLocation(program, "Matrix");
  glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &mainProjMatrix[0][0]);

  return program;

}

const GLchar* inputShader(const char* filename){

  FILE* fshade = fopen(filename, "rb");

  if(!fshade){
    fprintf(stderr,"unable to open file '%s'\n",filename);
    return NULL;
  }

  fseek(fshade, 0, SEEK_END);
  long filesize=ftell(fshade);
  fseek(fshade, 0, SEEK_SET);


  GLchar* shadingSource= new GLchar[filesize+1];
  fread(shadingSource, 1, filesize, fshade);


  if(ftell(fshade) == 0){
    fprintf(stderr, "File '%s' is empty.\n",filename);
    return NULL;
  }

  fclose(fshade);

  shadingSource[filesize] = 0;

  return const_cast<const GLchar*>(shadingSource);

}

GLuint createShader(GLenum type, const GLchar* shadeSource){

  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &shadeSource, NULL);

  glCompileShader(shader);

  GLint compileStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

  if(!compileStatus){
    GLint logSize;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);

    GLchar* infoLog = new GLchar[logSize+1];
    glGetShaderInfoLog(shader,logSize,&logSize,infoLog);

    const char *shadeInfo= NULL;
    switch(type){
      case GL_VERTEX_SHADER: shadeInfo = "vertex"; break;
      case GL_GEOMETRY_SHADER_EXT: shadeInfo = "geometric"; break;
      case GL_FRAGMENT_SHADER: shadeInfo = "fragment"; break;
    }
    fprintf(stderr,"\nCompile failure in %u shader: %s\n Error message:\n%s\n",type,shadeInfo,infoLog);
    delete[] infoLog;
  }
  return shader;
}

GLuint createProgram(const vector<GLuint> shadeList){

  GLuint program = glCreateProgram();

  for(GLuint i=0;i<shadeList.size();i++){glAttachShader(program,shadeList[i]);}

  glBindAttribLocation(program, 0, "in_position");
  glBindAttribLocation(program, 1, "in_color");
  glLinkProgram(program);

  GLint linkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

  if(!linkStatus){
    GLint logSize;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

    GLchar *infoLog = new GLchar[logSize+1];
    glGetProgramInfoLog(program,logSize,&logSize,infoLog);

    fprintf(stderr,"\nShader linking failed: %s\n",infoLog);
    delete[] infoLog;

    for(GLuint i=0;i<shadeList.size();i++){glDeleteShader(shadeList[i]);}
  }
  return program;
}
