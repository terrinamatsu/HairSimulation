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
  m_hairNodes.push_back(HairMass({8,10,5}));
  m_hairNodes.push_back(HairMass({8,8,5}));
  m_hairNodes.push_back(HairMass({8,6,5}));

  // Generate Springs
  HairSpring spr1 = HairSpring();
  spr1.LeftMass = *m_hairNodes[0];
  spr1.RightMass = *m_hairNodes[1];

  HairSpring spr2 = HairSpring();
  spr2.LeftMass = *m_hairNodes[1];
  spr2.RightMass = *m_hairNodes[2];

  m_hairSprings.push_back(spr1);
  m_hairSprings.push_back(spr2);
}

void Hair::update(float _dt)
{
  // And now for the complicated bit...
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