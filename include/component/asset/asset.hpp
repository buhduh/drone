#ifndef COMPONENT_ASSET_HPP
#define COMPONENT_ASSET_HPP

#include <stdint.h>
#include <bitset>
#include <vector>

#include <glm/glm.hpp>

#include "utils/utils.hpp"

namespace component {
namespace asset {

//Do I want to worry about memory?
static std::vector<char> getSmallFileContents(const std::string& filename) {
	return utils::readFile(filename);
}


}
}

#endif
