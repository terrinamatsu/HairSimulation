#ifndef HAIR_H_
#define HAIR_H_

#include <ngl/Vec3.h>
#include <ngl/SimpleVAO.h>
#include <memory>

struct HairMass
{
  ngl::Vec3 m_position;
  HairMass(ngl::Vec3 _position)
  {
    m_position = _position;
  }
};

struct HairSpring
{
  HairMass* LeftMass;
  HairMass* RightMass;
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