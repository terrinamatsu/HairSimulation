#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <iostream>

const auto ColourShader="ColourShader";

NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("Hair Simulation");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f, static_cast<float>(_w)/_h, 0.1f, 350.0f);
}


void NGLScene::initializeGL()
{
  // we must call that first before any other GL commands to load and link the
  // gl commands from the lib, if that is not done program will crash
  ngl::NGLInit::initialize();
  glClearColor(0.7f, 0.7f, 0.7f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  // Load Shaders
  ngl::ShaderLib::loadShader(ColourShader, "shaders/ColourVertex.glsl", "shaders/ColourFragment.glsl");
  ngl::ShaderLib::use(ColourShader);

  // Initialise Hair
  m_hairTest = std::make_unique<Hair>();

  // Create Circle
  ngl::VAOPrimitives::createSphere("sphere", 0.5f, 20);

  // Initialise View matrix
  m_view = ngl::lookAt({0,10,10}, {0,0,0}, {0,1,0});
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

  // Mouse Rotation
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  m_mouseGlobalTX = rotX * rotY;
  // Translation
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  ngl::VAOPrimitives::draw("sphere");

  ngl::ShaderLib::setUniform("MVP", m_project * m_view * m_mouseGlobalTX);

  m_hairTest->Render();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
  case Qt::Key_F: toggleFullScreen(); break;
  break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

void NGLScene::toggleFullScreen()
{
  if(m_isFullScreen)
  {
    showNormal();
    m_isFullScreen = false;
  }
  else
  {
    showFullScreen();
    m_isFullScreen = true;
  }
} 
