#include <iostream>
#include <stdio.h>

struct MyStruct {
	int a;
	int b;
	float c;
	float d;
};

const char* fName = "/home/dale/local/drone/spike/testfile";

void writeIt() {
	MyStruct foo = {
		1, 2, 3.4, 3.2		
	};
	FILE* f = fopen(fName, "wb+");
	fwrite(&foo, sizeof(MyStruct), 1, f);
	fclose(f);
}

void readIt() {
	FILE* f = fopen(fName, "rb");
	MyStruct foo;	
	fread(&foo, sizeof(MyStruct), 1, f);
	fclose(f);
	std::cout << "{" << foo.a << ", " << foo.b << ", " << foo.c << ", " << foo.d << "}" << std::endl;
}

int main(void) {
	readIt();
}
