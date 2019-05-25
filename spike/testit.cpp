#include <dlfcn.h>
extern "C" typedef void  (*foo_t)(void);

int main() {
	void* lib = dlopen("lib/libModelparser.so", RTLD_LAZY);
	foo_t foo = (foo_t)dlsym(lib, "foo");
	foo();	
	dlclose(lib);
}
