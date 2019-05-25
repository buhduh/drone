#ifndef MODEL_HPP
#define MODEL_HPP

struct ModelHeader {
	uint32_t numVerts;	
	uint32_t numFaces;
};

struct Vertex {
	double x;
	double y;
	double z;
};

#endif
