#include <iostream>
#include <stdio.h>

#include "model.hpp"

int main() {	
	FILE* f = fopen("/home/dale/local/drone/models/bin/cubetriangles.bin", "rb");
	ModelHeader mHeader = {};
	size_t size;
	size = fread(&mHeader, sizeof(ModelHeader), 1, f);
	if(size != 1) {
		std::cout << "modelheader read is bad" << std::endl;
	}
	std::cout << "numVerts: " << mHeader.numVerts << std::endl;
	std::cout << "numIndeces: " << mHeader.numIndeces << std::endl;
	Vertex verts[mHeader.numVerts];
	size = fread(verts, sizeof(Vertex), mHeader.numVerts, f);
	if(size != mHeader.numVerts) {
		std::cout << "verts read is bad " << std::endl;
	}
	for(size_t i = 0; i < mHeader.numVerts; i++) {
		std::cout << "{" << verts[i].x << ", " << verts[i].y << ", " << verts[i].z << "}" << std::endl;
	}
	index_size_t indeces[mHeader.numIndeces];
	size = fread(indeces, sizeof(index_size_t), mHeader.numIndeces, f);
	if(size != mHeader.numIndeces) {
		std::cout << "index read is bad " << std::endl;
	}
	for(index_size_t i = 0; i < mHeader.numIndeces;) {
		std::cout << indeces[i++] << " " << indeces[i++] << " " << indeces[i++] << std::endl;
	}
	fclose(f);
}
