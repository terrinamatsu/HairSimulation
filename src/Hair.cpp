#include "Hair.h"

#include <ngl/Util.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>

Hair::Hair()
{
  // Setup VAO
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);

  // Generate Hair 
  m_hairNodes.push_back(HairMass(10,10,10));
  m_hairNodes.push_back(HairMass(8,8,10));
  m_hairNodes.push_back(HairMass(8,8,10));
}

void Hair::render() const
{
    glPointSize(5);

    m_vao->bind();

    m_vao->setData(ngl::SimpleVAO::VertexData(m_hairNodes.size() * sizeof(HairMass), m_hairNodes[0].m_position.m_x));
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(HairMass), 0);

    m_vao->setNumIndices(m_hairNodes.size());

    m_vao->draw();
    m_vao->unbind();
}