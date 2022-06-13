#ifndef HAIR_H_
#define HAIR_H_

#include <ngl/Vec3.h>
#include <ngl/SimpleVAO.h>
#include <memory>

struct HairMass
{
  ngl::Vec3 m_position;
  ngl::Vec3 m_force;
  ngl::Vec3 m_accelleration;
  ngl::Vec3 m_velocity;

  HairMass* m_parentHair = nullptr;

  HairMass(ngl::Vec3 _position)
  {
    m_position = _position;
  }

  void ResetForce()
  {
    m_force = ngl::Vec3();
  }
};

struct HairSpring
{
  HairMass* LeftMass;
  HairMass* RightMass;
  float restLength = 2.0f;
  float springConstant = 10.0f;
};

struct HairHinge
{
  HairMass* LeftMass;
  HairMass* RightMass;
};


class Hair
{
  public:
    Hair();
    ~Hair() = default;

    void update(float _dt);
    void render() const;

  private:
    std::vector<HairMass> m_hairNodes;
    std::vector<HairSpring> m_hairSprings;
    std::vector<HairHinge> m_hairHinges;

    std::unique_ptr<ngl::AbstractVAO> m_vao;
    std::unique_ptr<ngl::AbstractVAO> m_vaoSpring;
    std::unique_ptr<ngl::AbstractVAO> m_vaoHinge;


    float m_dampingConstant = 0.5;
};

#endif // HAIR_H_