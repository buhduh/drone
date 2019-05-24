#ifndef MODELPARSER_HPP
#define MODELPARSER_HPP

#include "model.hpp"

struct MP_Vertex : public Vertex {
  friend std::ostream& operator<<(std::ostream& os, const MP_Vertex& v) {
    os << "{" << v.x << "," << v.y << "," << v.z << "}";
  }
};

#endif
