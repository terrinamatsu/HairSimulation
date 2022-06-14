#include "Hair.h"

#include <ngl/Util.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include <math.h>

Hair::Hair()
{
  // Setup VAOs
  m_vao = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS);
  m_vaoSpring = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_LINES);
  m_vaoHinge = ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_LINES);

  // Generate Hair 
  m_hairNodes.push_back(HairMass({0,10,0}));
  // m_hairNodes.push_back(HairMass({2,8,1}));
  // m_hairNodes.back().m_parentHair = &m_hairNodes[0];
  // m_hairNodes.push_back(HairMass({4,6,2}));
  // m_hairNodes.back().m_parentHair = &m_hairNodes[1];

  // // Generate Springs
  // HairSpring spr1 = HairSpring();
  // spr1.LeftMass = &m_hairNodes[0];
  // spr1.RightMass = &m_hairNodes[1];

  // HairSpring spr2 = HairSpring();
  // spr2.LeftMass = &m_hairNodes[1];
  // spr2.RightMass = &m_hairNodes[2];

  // m_hairSprings.push_back(spr1);
  // m_hairSprings.push_back(spr2);

  // // Generate Hinges
  // HairHinge hg1 = HairHinge();
  // hg1.LeftMass = &m_hairNodes[0];
  // hg1.MiddleMass = &m_hairNodes[1];
  // hg1.RightMass = &m_hairNodes[2];

  // m_hairHinges.push_back(hg1);


  for(auto i = 1; i < 2; i+=2)
  {
    m_hairNodes.push_back(HairMass({i * 2, i * 2, i * 2}));
    m_hairNodes.back().m_parentHair = &m_hairNodes[i - 1];
    m_hairNodes.push_back(HairMass({i * 2 + 1.0f, i * 2 + 1.0f, i * 2 + 1.0f}));
    m_hairNodes.back().m_parentHair = &m_hairNodes[i];

    // Generate Springs
    HairSpring spr1 = HairSpring();
    spr1.LeftMass = &m_hairNodes[i-1];
    spr1.RightMass = &m_hairNodes[i];

    HairSpring spr2 = HairSpring();
    spr2.LeftMass = &m_hairNodes[i-1];
    spr2.RightMass = &m_hairNodes[i+1];

    m_hairSprings.push_back(spr1);
    m_hairSprings.push_back(spr2);

    // Generate Hinges
    HairHinge hg1 = HairHinge();
    hg1.LeftMass = &m_hairNodes[i-1];
    hg1.MiddleMass = &m_hairNodes[i];
    hg1.RightMass = &m_hairNodes[i+1];

    m_hairHinges.push_back(hg1);
  }

  //\ refactor into for loop to create many hairs of different lengths
  //\ also attach hairs to head. 
}

void Hair::update(float _dt)
{
  // Initial moving hair 
  float movement = (m_isHairMovingRight) ? (1.0f * _dt) : (-1.0f * _dt);  
  m_hairNodes[0].m_position.m_x += movement;
  if(m_hairNodes[0].m_position.m_x > 20)
  {
    m_isHairMovingRight = false;
  }
  else if(m_hairNodes[0].m_position.m_x < -20)
  {
    m_isHairMovingRight = true;
  }

  // And now for the complicated bit...

  float hairNodeMass = 5;
  float dragDampingConstant = 5.0f;
  float hingeConstant = 20.0f;
  float hingeDampingConstant = 1.0f;

  // Spring
  for(auto& spring : m_hairSprings)
  {
    ngl::Vec3 springForce;
    ngl::Vec3 springLength = spring.LeftMass->m_position - spring.RightMass->m_position;
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
    ngl::Vec3 leftVec = hinge.LeftMass->m_position - hinge.MiddleMass->m_position;
    ngl::Vec3 rightVec = hinge.MiddleMass->m_position - hinge.RightMass->m_position;

    float leftMag = sqrt(leftVec.m_x * leftVec.m_x + leftVec.m_y * leftVec.m_y + leftVec.m_z * leftVec.m_z);
    float rightMag = sqrt(rightVec.m_x * rightVec.m_x + rightVec.m_y * rightVec.m_y + rightVec.m_z * rightVec.m_z);

    float hingeTheta = acosf(leftVec.dot(rightVec) / (abs(leftMag) * abs(rightMag)));

    float hingeForce = hingeConstant * hingeTheta - hingeDampingConstant * (hingeConstant * hingeTheta);

    // leftVec.normalize();
    // rightVec.normalize();

    // ngl::Vec3 LeftForceVec = rightVec * -1;
    // LeftForceVec.normalize();
    // LeftForceVec = LeftForceVec - leftVec;

    // ngl::Vec3 RightForceVec = leftVec * -1;
    // RightForceVec.normalize();
    // RightForceVec = RightForceVec - rightVec;

    // ngl::Vec3 RightLeftForceVec = LeftForceVec + RightForceVec;
    // RightLeftForceVec.normalize();

    hinge.LeftMass->m_force += hingeForce;// * LeftForceVec;
    hinge.MiddleMass->m_force -= 2 * hingeForce;// * RightLeftForceVec;
    hinge.RightMass->m_force += hingeForce;// * RightForceVec;
  }

  // Gravity
  for(auto& hair : m_hairNodes)
  {
    hair.m_force += hairNodeMass * ngl::Vec3(0.0f, -9.8f, 0.0f) * 0.2;
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
    std::vector<ngl::Vec3> SpringVerts;
    for(auto Spring : m_hairSprings)
    {
      SpringVerts.push_back(Spring.LeftMass->m_position);
      SpringVerts.push_back(Spring.RightMass->m_position);
    }

    glLineWidth(5);

    m_vaoSpring->bind();

    m_vaoSpring->setData(ngl::SimpleVAO::VertexData(SpringVerts.size() * sizeof(ngl::Vec3), SpringVerts[0].m_x));
    m_vaoSpring->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(ngl::Vec3), 0);

    m_vaoSpring->setNumIndices(SpringVerts.size());

    m_vaoSpring->draw();
    m_vaoSpring->unbind();

    // Hair Hinge
    std::vector<ngl::Vec3> HingeVerts;
    for(auto Hinge : m_hairHinges)
    {
      HingeVerts.push_back(Hinge.LeftMass->m_position);
      HingeVerts.push_back(Hinge.RightMass->m_position);
    }

    m_vaoHinge->bind();

    m_vaoHinge->setData(ngl::SimpleVAO::VertexData(HingeVerts.size() * sizeof(ngl::Vec3), HingeVerts[0].m_x));
    m_vaoHinge->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(ngl::Vec3), 0);

    m_vaoHinge->setNumIndices(HingeVerts.size());

    m_vaoHinge->draw();
    m_vaoHinge->unbind();
}