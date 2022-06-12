#ifndef HAIR_H_
#define HAIR_H_

#include <ngl/Vec3.h>
#include <memory>

struct HairMass
{
  ngl::Vec3 m_position;
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
    Hair() = default;

  private:
    std::vector<HairMass> m_hairNodes;
    std::vector<HairSpring> m_hairSprings;
    std::vector<HairHinge> m_hairHinges;

    float m_dampingConstant = 0.5;
}

#endif // HAIR_H_