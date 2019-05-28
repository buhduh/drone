//#include "model.hpp"

/*
namespace asset {

Model::Model(const char* fileName) 
	: header({0,0})
	, vertices(0)
	, indices(0)
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
	numRead = fread(vertices.data(), sizeof(vertex), header.numVerts, inFile);
	if(numRead != header.numVerts) {
		flags.set(BAD_READ);
		return;
	}
	indices.resize(header.numIndices);
	numRead = fread(indices.data(), sizeof(index_size_t), header.numIndices, inFile);
	if(numRead != header.numIndices) {
		flags.set(BAD_READ);
		return;
	}
	fclose(inFile);
	flags.set(MODEL_OK);
}

Model::~Model(){}

}
*/
