#include "Hair.h"
#include <ngl/VAOFactory.h>

void Hair::Hair()
{
  // Setup VAO
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);
}

void Hair::Render()
{
    glPointSize(5);

    m_vao->bind();

    m_vao->setData(ngl::SimpleVAO::VertexData(m_hairNodes.size() * sizeof(HairMass), m_hairNodes[0].m_position.m_x));
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(HairMass), 0);

    m_vao->setNumIndices(m_hairNodes.size());

    m_vao->draw();
    m_vao->unbind();
}