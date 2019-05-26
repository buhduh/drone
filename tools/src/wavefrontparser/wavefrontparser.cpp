//TODO test the buffer realloc functionality
/*
	assumes triangular topology indexed counterclockwise
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <unordered_map>
#include <regex>

#include "wavefrontparser/cli.hpp"
#include "model.hpp"

const static int CHUNK_SIZE = 1024;

enum TYPE {
	VERTEX,
	FACE
};

const static std::unordered_map<std::string, TYPE> TYPE_MAP {
	{"v ", VERTEX},
	{"f ", FACE}
};

//TODO need to match vertex sizes to acceptable gpu memory inputs
int main(int argc, char* argv[]) {
	Args args = parseCLI(argc, argv);
	std::ifstream inFile(args.input);
	if(inFile.fail()) {
		std::cerr << "Unable to open '" << args.input << "' for reading, stopping." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string line;
	index_size_t vertCount = 0;
	uint32_t indexCount = 0;
	uint32_t lineN = 1;
	std::regex vertPat(R"(v (-?\d+\.\d+) (-?\d+\.\d+) (-?\d+\.\d+)$)");
	std::regex facePat(R"(f (\d+)/\d*/\d* (\d+)/\d*/\d* (\d+)/\d*/\d*$)");
	std::cmatch match;
	size_t vertBufferSize = CHUNK_SIZE;
	size_t indexBufferSize = CHUNK_SIZE;
	Vertex* vertBuffer = (Vertex*) malloc(sizeof(Vertex)*vertBufferSize);
	index_size_t* indexBuffer = (index_size_t*) malloc(sizeof(index_size_t)*indexBufferSize);
		(index_size_t*) malloc(sizeof(index_size_t)*indexBufferSize);
	for(; std::getline(inFile, line); lineN++) {
		if(line.empty()) continue;
		TYPE type;
		try {
			type = TYPE_MAP.at(line.substr(0, 2));
		} catch(std::out_of_range e) {
			continue;
		}
		switch(type) {
			case VERTEX:
			{
				if(!std::regex_match(line.c_str(), match, vertPat)) {
					std::cerr << "Could not parse vertex line in '" 
						<< args.input << "' line number: "
						<< lineN << std::endl;
					exit(EXIT_FAILURE);
				}
				vertex_size_t x, y, z;
				x = atof(match.str(1).c_str());
				y = atof(match.str(2).c_str());
				z = atof(match.str(3).c_str());
				//*= 2 could really blow this thing up for large v numbers.
				if(vertCount == vertBufferSize) {
					vertBufferSize *= 2;		
					vertBuffer = (Vertex*) realloc(vertBuffer, sizeof(Vertex)*vertBufferSize);
				}
				vertBuffer[vertCount++] = Vertex{x,y,z};
				//index reference type can no longer reference vertices
				if(vertCount >=  INDEX_SIZE_MAX) {
					std::cerr << "Too many vertices for index type to handle!" << std::endl;
					exit(EXIT_FAILURE);
				}
				break;
			}
			case FACE:
			{
				if(!std::regex_match(line.c_str(), match, facePat)) {
					std::cerr << "Could not parse face line in '" 
						<< args.input << "' line number: " 
						<< lineN << std::endl;
					exit(EXIT_FAILURE);
				}
				//should always be multiples of 3, what happens if we end doing different
				//topologies?
				if(indexCount + 3 > indexBufferSize) {
					indexBufferSize *= 2;
					indexBuffer = (index_size_t*) realloc(indexBuffer, sizeof(index_size_t)*indexBufferSize);
				}
				indexBuffer[indexCount++] = static_cast<index_size_t>(
					atoi(match.str(1).c_str())
				);
				indexBuffer[indexCount++] = static_cast<index_size_t>(
					atoi(match.str(2).c_str())
				);
				indexBuffer[indexCount++] = static_cast<index_size_t>(
					atoi(match.str(3).c_str())
				);
				break;
			}
		}
	}
	inFile.close();
	FILE* oFile = fopen(args.output.c_str(), "wb+");
	if(oFile == nullptr) {
		//I should probably try to open this at start of program just to save time for problems
		std::cerr << "Failed opening '" << args.output << "' for writing, stopping." << std::endl;
		exit(EXIT_FAILURE);
	}
	ModelHeader mHeader = {vertCount, indexCount};
	fwrite(&mHeader, sizeof(ModelHeader), 1, oFile);
	fwrite(vertBuffer, sizeof(Vertex), vertCount, oFile);
	fwrite(indexBuffer, sizeof(index_size_t), indexCount, oFile);
	fclose(oFile);
	free(vertBuffer);
	free(indexBuffer);
}
