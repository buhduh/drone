#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <unordered_map>
#include <regex>
//#include <sstream>

#include "modelparser/cli.hpp"
#include "model.hpp"

//numIndeces can be of type last param for vkCmdBindIndexBuffer
//TODO should i make this a toggle so it can be applied acrossed the program?
typedef uint16_t index_count_t;
const static index_count_t INDEX_COUNT_MAX = UINT16_MAX;

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
	std::ifstream f(args.input);
	if(f.fail()) {
		std::cerr << "Unable to open '" << args.input << "' for reading, stopping." << std::endl;
		exit(EXIT_FAILURE);
	}
	std::string line;
	uint32_t numVerts = 0;
	//index_count_t numIndeces = 0;
	uint32_t lineN = 1;
	std::regex vertPat(R"(v (-?\d+\.\d+) (-?\d+\.\d+) (-?\d+\.\d+)$)");
	std::regex facePat(R"(f (\d+)/\d*/\d* (\d+)/\d*/\d* (\d+)/\d*/\d*$)");
	std::cmatch match;
	size_t vertBuffSize = CHUNK_SIZE;
	size_t indexBufferSize = CHUNK_SIZE;
	Vertex* vertBuffer = (Vertex*) malloc(sizeof(Vertex)*vertBuffSize);
	index_count_t* indexBuffer = 
		(index_count_t*) malloc(sizeof(index_count_t)*indexBufferSize);
	for(; std::getline(f, line); lineN++) {
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
				double x, y, z;
				x = atof(match.str(1).c_str());
				y = atof(match.str(2).c_str());
				z = atof(match.str(3).c_str());
				if(numVerts == vertBuffLength) {
					vertBuffLength *= 2;		
					vertBuffer = (Vertex*) realloc(vertBuffer, sizeof(Vertex)*vertBuffLength);
				}
				vertBuffer[numVerts++] = Vertex{x,y,z};
				//index reference type can no longer reference vertices
				if(numVerts >=  INDEX_COUNT_MAX) {
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
				//TODO here
				//indexBuffer[indexatoi(match.str(1).c_str())
				break;
			}
		}
	}
}
