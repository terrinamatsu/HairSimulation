#include "Hair.h"

#include <ngl/Util.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>

Hair::Hair()
{
  // Setup VAOs
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);
  m_vaoSpring = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_LINES);
  m_vaoHinge = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);

  // Generate Hair 
  m_hairNodes.push_back(HairMass({0,10,0}));
  m_hairNodes.push_back(HairMass({2,8,1}));
  m_hairNodes.back().m_parentHair = &m_hairNodes[0];
  m_hairNodes.push_back(HairMass({4,6,2}));
  m_hairNodes.back().m_parentHair = &m_hairNodes[1];

  // Generate Springs
  HairSpring spr1 = HairSpring();
  spr1.LeftMass = &m_hairNodes[0];
  spr1.RightMass = &m_hairNodes[1];

  HairSpring spr2 = HairSpring();
  spr2.LeftMass = &m_hairNodes[1];
  spr2.RightMass = &m_hairNodes[2];

  m_hairSprings.push_back(spr1);
  m_hairSprings.push_back(spr2);
}

void Hair::update(float _dt)
{
  // And now for the complicated bit...

  float hairNodeMass = 5;
  float dragDampingConstant = 5.0f;

  // Spring
  for(auto& spring : m_hairSprings)
  {
    ngl::Vec3 springForce;
    ngl::Vec3 springLength = spring.LeftMass - spring.RightMass;
    ngl::Vec3 normalSpringLength = springLength;
    normalSpringLength.normalize();
    springForce = normalSpringLength * (spring.springConstant * (spring.restLength - springLength.length()));

    // Add damping
    springForce = springForce - (0.5f * springForce);

    spring.LeftMass->m_force += springForce;
    spring.RightMass->m_force -= springForce;
  }
   
  // Hinge
  for(auto hinge : m_hairHinges)
  {

  }

  // Gravity
  for(auto& hair : m_hairNodes)
  {
    hair.m_force += hairNodeMass * ngl::Vec3(0.0f, -9.8f, 0.0f);
  }

  // Aerodynamic Drag
  for(auto& hair : m_hairNodes)
  {
    hair.m_force -= hair.m_velocity * dragDampingConstant;
  }

  // Newton's Second Law (accelleration) & reset hair forces.
  for(auto& hair : m_hairNodes)
  {
    hair.m_accelleration = hair.m_force / hairNodeMass;
    hair.m_velocity += hair.m_accelleration * _dt;
    if(hair.m_parentHair != nullptr)
    {
      hair.m_position += hair.m_velocity * _dt;
    }
    hair.ResetForce();
  }
}

void Hair::render() const
{
    // Hair Masses
    glPointSize(5);

    m_vao->bind();

    m_vao->setData(ngl::SimpleVAO::VertexData(m_hairNodes.size() * sizeof(HairMass), m_hairNodes[0].m_position.m_x));
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(HairMass), 0);

    m_vao->setNumIndices(m_hairNodes.size());

    m_vao->draw();
    m_vao->unbind();

    // Hair Springs
    glLineWidth(5);

    m_vaoSpring->bind();

    m_vaoSpring->setData(ngl::SimpleVAO::VertexData(m_hairSprings.size() * sizeof(HairSpring), m_hairSprings[0].LeftMass->m_position.m_x));
    m_vaoSpring->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(HairMass), 0);

    m_vaoSpring->setNumIndices(m_hairSprings.size() * 2);

    m_vaoSpring->draw();
    m_vaoSpring->unbind();
}