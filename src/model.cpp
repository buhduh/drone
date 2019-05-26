#include "model.hpp"

Model::Model(const char* fileName) 
	: header({0,0})
	, vertices(0)
	, indeces(0)
{
	FILE* inFile = fopen(fileName, "rb");
	if(inFile == nullptr) {
		flags.set(BAD_READ);
		return;
	}
	size_t numRead = fread(&header, sizeof(ModelHeader), 1, inFile);
	if(numRead != 1) {
		flags.set(BAD_READ);
		return;
	}
	vertices.resize(header.numVerts);
	numRead = fread(vertices.data(), sizeof(Vertex), header.numVerts, inFile);
	if(numRead != header.numVerts) {
		flags.set(BAD_READ);
		return;
	}
	indeces.resize(header.numIndeces);
	numRead = fread(indeces.data(), sizeof(index_size_t), header.numIndeces, inFile);
	if(numRead != header.numIndeces) {
		flags.set(BAD_READ);
		return;
	}
	flags.set(MODEL_OK);
}

Model::~Model(){}
