#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdint.h>
#include <bitset>
#include <vector>

//max number of verts an index can address
#define INDEX_SIZE_MAX UINT16_MAX
typedef uint16_t index_size_t;
typedef double vertex_size_t;

#define MODEL_FLAG_WIDTH 16
typedef std::bitset<MODEL_FLAG_WIDTH> MODEL_FLAGS;
const uint16_t MODEL_OK = 1;
const uint16_t BAD_READ = 2;

struct ModelHeader {
	index_size_t numVerts;	
	uint32_t numIndeces;
};


struct Vertex {
	vertex_size_t x;
	vertex_size_t y;
	vertex_size_t z;
};

class Model {
	public:
	Model(const char*);
	~Model();
	MODEL_FLAGS flags;
	std::vector<Vertex> vertices;
	std::vector<index_size_t> indeces;
	private:
	ModelHeader header;
};

#endif
