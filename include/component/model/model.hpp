#ifndef COMPONENT_MODEL_MODEL_HPP
#define COMPONENT_MODEL_MODEL_HPP
/*
	A model component is all things that might be associated with the 
	visual aspects of a "model".  The mesh, texture, material, 
	the model matrix, etc etc
*/

#include <stdint.h>
#include <bitset>
#include <vector>
#include <string>

#include <glm/glm.hpp>

namespace component {
namespace model {

enum MODEL_STATUS {
	OK,
	BAD_LOAD
};

//max number of verts an index can address
typedef uint16_t index_size_t;
static const index_size_t INDEX_SIZE_MAX = UINT16_MAX;

typedef glm::vec3 vertex;

struct ModelHeader {
	index_size_t numVerts;	
	uint32_t numIndices;
};

/*
//removing templatizing for now
template <class T>
class Model {
	public:
	Model(T, MODEL_STATUS&);
	~Model();
	std::vector<vertex> vertices;
	std::vector<index_size_t> indices;
	private:
	ModelHeader header;
	//The "raw data"
	T asset;
};
*/

class Model {
	public:
	Model(const char*, MODEL_STATUS&);
	~Model();
	std::vector<vertex> vertices;
	std::vector<index_size_t> indices;
	private:
	ModelHeader header;
};

}
}

#endif
