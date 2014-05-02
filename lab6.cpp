#include "initShaders.h"
#include <cstdlib>
using namespace std;

void rotate(GLuint locate);

GLuint vaoID,vboID[2],eboID;
GLuint program;

GLfloat pit,yaw,scalar=1;
glm::vec3 cubeTran, cameraTran;

GLfloat size=6;

GLfloat vertexarray[]={size,size,-size,
                       size,-size,-size,
                       -size,-size,-size,
                       -size,size,-size,
                       size,size,size,
                       size,-size,size,
                       -size,-size,size,
                       -size,size,size
                       };

GLfloat colorarray[]={0.9f,0.6f,0.4f,1.0f,
                      0.5f,0.2f,0.6f,1.0f,
                      0.7f,0.4f,0.2f,1.0f,
                      0.6f,0.3f,0.5f,1.0f,
                      0.8f,0.7f,0.3f,1.0f,
                      0.4f,0.8f,0.9f,1.0f,
                      0.3f,0.5f,0.8f,1.0f,
                      0.2f,0.9f,0.7f,1.0f
                                        };

 GLubyte elems[]={0,1,2,3,7,4,5,6,
                  7,3,0,4,5,6,2,1,
                  0,1,5,4,7,3,2,6
                 };
                 
  virtual void RenderSceneCB()// rotate object with a shadow following it
  {
    m_pGameCamera->OnRender();
    m_scale += 0.05f;

    ShadowMapPass();
    RenderPass();

    glutSwapBuffers();
  }
  
  virtual void ShadowMapPass()//pass the shasow throught the prgramm and enable it
  {
    m_shadowMapFBO.BindForWriting();

    glClear(GL_DEPTH_BUFFER_BIT);

    m_pShadowMapEffect->Enable();

    Pipeline p;
    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, m_scale, 0.0f);
    p.WorldPos(0.0f, 0.0f, 3.0f);
    p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
    p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
    m_pShadowMapEffect->SetWVP(p.GetWVPTrans());
    m_pMesh->Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
  
  virtual void RenderPass()//replace the shadow map technique with the lighting technique and bind the shadow map frame buffer object
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pLightingEffect->Enable();

    m_shadowMapFBO.BindForReading(GL_TEXTURE1);

    Pipeline p;
    p.SetPerspectiveProj(30.0f, WINDOW_WIDTH, WINDOW_HEIGHT, 1.0f, 50.0f);
    p.Scale(10.0f, 10.0f, 10.0f);
    p.WorldPos(0.0f, 0.0f, 1.0f);
    p.Rotate(90.0f, 0.0f, 0.0f);
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    m_pLightingEffect->SetWVP(p.GetWVPTrans());
    m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
    p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
    m_pLightingEffect->SetLightWVP(p.GetWVPTrans());
    m_pLightingEffect->SetEyeWorldPos(m_pGameCamera->GetPos());
    m_pGroundTex->Bind(GL_TEXTURE0);
    m_pQuad->Render();

    p.Scale(0.1f, 0.1f, 0.1f);
    p.Rotate(0.0f, m_scale, 0.0f);
    p.WorldPos(0.0f, 0.0f, 3.0f);
    p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());
    m_pLightingEffect->SetWVP(p.GetWVPTrans());
    m_pLightingEffect->SetWorldMatrix(p.GetWorldTrans());
    p.SetCamera(m_spotLight.Position, m_spotLight.Direction, Vector3f(0.0f, 1.0f, 0.0f));
    m_pLightingEffect->SetLightWVP(p.GetWVPTrans());

    m_pMesh->Render();
  }

void init(){
         glEnable(GL_DEPTH_TEST);
         glViewport(0, 0, 600, 600);

        glGenVertexArrays(1,&vaoID);
        glBindVertexArray(vaoID);

        glGenBuffers(2, vboID);
        glBindBuffer(GL_ARRAY_BUFFER,vboID[0]);
        glBufferData(GL_ARRAY_BUFFER,sizeof(vertexarray),vertexarray,GL_STATIC_DRAW);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, vboID[1]);
  glBufferData(GL_ARRAY_BUFFER,sizeof(colorarray),colorarray,GL_STATIC_DRAW);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glGenBuffers(1,&eboID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,eboID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(elems),elems,GL_STATIC_DRAW);

        ShaderInfo shaders[]={
  { GL_VERTEX_SHADER , "vertexshader.glsl"},
  { GL_FRAGMENT_SHADER , "fragmentshader.glsl"},
  { GL_NONE , NULL}
  };

  program=initShaders(shaders);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  
  m_pLightingEffect = new LightingTechnique();

  if (!m_pLightingEffect->Init()) {
    printf("Error initializing the lighting technique\n");
    return false;
  }

  m_pLightingEffect->Enable();
  m_pLightingEffect->SetSpotLights(1, &m_spotLight);
  m_pLightingEffect->SetTextureUnit(0);
  m_pLightingEffect->SetShadowMapTextureUnit(1);

  GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

  GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
    GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

  GLfloat lightColor1[] = {0.5f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos1[] = {-1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
    glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);

}


void display(SDL_Window* screen){
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::mat4 trans;
        /*
        trans=glm::translate(trans,cubeTran);
        trans=glm::rotate(trans,pit,glm::vec3(1,0,0));
        trans=glm::rotate(trans,yaw,glm::vec3(0,1,0));
        trans=glm::translate(trans,cubeTran);
        trans=glm::scale(trans,glm::vec3(scalar));
        */

        trans=glm::translate(trans,cameraTran);
        trans=glm::rotate(trans,pit,glm::vec3(1,0,0));
        trans=glm::rotate(trans,yaw,glm::vec3(0,1,0));
        trans=glm::translate(trans,cameraTran);
        trans=glm::scale(trans,glm::vec3(scalar));

  GLint tempLoc = glGetUniformLocation(program,"modelMatrix");
        glUniformMatrix4fv(tempLoc,1,GL_FALSE,&trans[0][0]);

        glDrawElements(GL_POLYGON,24,GL_UNSIGNED_BYTE,NULL);
        glFlush();
        SDL_GL_SwapWindow(screen);
}

void input(SDL_Window* screen){

SDL_Event event;
/*
        while (SDL_PollEvent(&event)){
                switch (event.type){
                        case SDL_QUIT:exit(0);break;
                        case SDL_KEYDOWN:
                                switch(event.key.keysym.sym){
                                        case SDLK_ESCAPE:exit(0);
                                        case SDLK_w:cubeTran.y+=2;break;
                                        case SDLK_s:cubeTran.y-=2;break;
                                        case SDLK_a:cubeTran.x-=2;break;
                                        case SDLK_d:cubeTran.x+=2;break;
                                        case SDLK_e:scalar+=.1f;break;
                                        case SDLK_q:scalar-=.1f;break;
                                        case SDLK_i:pit+=2;break;
                                        case SDLK_k:pit-=2;break;
                                        case SDLK_j:yaw+=2;break;
                                        case SDLK_l:yaw-=2;break;
                                }
                }
        }
}
*/

 while (SDL_PollEvent(&event)){
                switch (event.type){
                        case SDL_QUIT:exit(0);break;
                        case SDL_KEYDOWN:
                                switch(event.key.keysym.sym){
                                        case SDLK_ESCAPE:exit(0);
                                        case SDLK_w:cameraTran.y+=2;break;
                                        case SDLK_s:cameraTran.y-=2;break;
                                        case SDLK_a:cameraTran.x-=2;break;
                                        case SDLK_d:cameraTran.x+=2;break;
                                        case SDLK_e:scalar+=.1f;break;
                                        case SDLK_q:scalar-=.1f;break;
                                        case SDLK_i:pit+=2;break;
                                        case SDLK_k:pit-=2;break;
                                        case SDLK_j:yaw+=2;break;
                                        case SDLK_l:yaw-=2;break;
                                }
                }
        }
}




int main(int argc, char **argv){

        SDL_Window *window;

        if(SDL_Init(SDL_INIT_VIDEO)<0){
                fprintf(stderr,"Unable to create window: %s\n", SDL_GetError());
    SDL_Quit();
    exit(1);
        }

        window = SDL_CreateWindow(
                "Happy Days",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                500,
                500,
                SDL_WINDOW_OPENGL
        );

        if(window==NULL){
                fprintf(stderr,"Unable to create window: %s\n",SDL_GetError());
        }


        SDL_GLContext glcontext=SDL_GL_CreateContext(window);

  glewExperimental=GL_TRUE;
  if(glewInit()){
    fprintf(stderr, "Unable to initalize GLEW");
    exit(EXIT_FAILURE);
  }

        init();

        while(true){
          input(window);
                display(window);
        }

        SDL_GL_DeleteContext(glcontext);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}

