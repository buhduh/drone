#include "component/model/model.hpp"

namespace component {
namespace model {

//TODO this is probably a mesh

Model::Model(const char* fileName, MODEL_STATUS& status) 
	: header({0,0})
	, vertices(0)
	, indices(0)
{
	status = OK;
	FILE* inFile = fopen(fileName, "rb");
	if(inFile == nullptr) {
		status = BAD_LOAD;
		return;
	}
	size_t numRead = fread(&header, sizeof(ModelHeader), 1, inFile);
	if(numRead != 1) {
		status = BAD_LOAD;
		return;
	}
	vertices.resize(header.numVerts);
	numRead = fread(vertices.data(), sizeof(vertex), header.numVerts, inFile);
	if(numRead != header.numVerts) {
		status = BAD_LOAD;
		return;
	}
	indices.resize(header.numIndices);
	numRead = fread(indices.data(), sizeof(index_size_t), header.numIndices, inFile);
	if(numRead != header.numIndices) {
		status = BAD_LOAD;
		return;
	}
	fclose(inFile);
}

Model::~Model(){}

}
}
