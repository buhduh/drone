#ifndef COMPONENT_ASSET_HPP
#define COMPONENT_ASSET_HPP

#include <stdint.h>
#include <bitset>
#include <vector>

#include <glm/glm.hpp>

//max number of verts an index can address
#define INDEX_SIZE_MAX UINT16_MAX
typedef uint16_t index_size_t;
typedef glm::vec3 vertex;

#define MODEL_FLAG_WIDTH 16
typedef std::bitset<MODEL_FLAG_WIDTH> MODEL_FLAGS;
const uint16_t MODEL_OK = 1;
const uint16_t BAD_READ = 2;

struct ModelHeader {
	index_size_t numVerts;	
	uint32_t numIndices;
};

class Model {
	public:
	Model(const char*);
	~Model();
	MODEL_FLAGS flags;
	std::vector<vertex> vertices;
	std::vector<index_size_t> indices;
	private:
	ModelHeader header;
};

#endif

