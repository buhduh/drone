#TODO I should probably fix the debugging flags...

CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -Iinclude -I$(HOME)/include -DDEBUG
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

SPIKE_SRC = $(wildcard spike/*.cpp)
SPIKE_BIN = $(patsubst spike/%.cpp,bin/%_spike,$(SPIKE_SRC))

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

EXE = bin/drone

MODELPARSER_SRC = $(wildcard src/modelparser/*.cpp)
MODELPARSER_OBJ = $(patsubst src/modelparser/%.cpp,build/modelparser/%.o,$(MODELPARSER_SRC))
MODELPARSER_LIB = lib/libModelparser.so
MODELPARSER_BLD = build/modelparser

drone: $(EXE)

$(EXE): $(OBJ)
	g++ -g $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ): build/%.o: src/%.cpp
	g++ -g $(CFLAGS) -o $@ -c $^ $(LDFLAGS)

spike: $(SPIKE_BIN)

bin/%_spike: spike/%.cpp
	g++ $(CFLAGS) -o $@ $^ $(LDFLAGS)

#consider loading the tags file in vim
tags: $(SRC)
	@gcc -M $(CFLAGS) $^ include/graphics.hpp | \
	sed -e 's/[\\ ]/\n/g' | \
	sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
	ctags -L - --c++-kinds=+p --fields=+iaS --extra=+q

modelparser: $(MODELPARSER_BLD) $(MODELPARSER_LIB)

$(MODELPARSER_BLD):
	@mkdir build/modelparser

$(MODELPARSER_LIB): $(MODELPARSER_OBJ)
	g++ -g $(CFLAGS) -shared -pie -o $@ $^ $(LDFLAGS)
	
$(MODELPARSER_OBJ): build/modelparser/%.o: src/modelparser/%.cpp
	g++ -g $(CFLAGS) -o $@ -c -fPIC $^ $(LDFLAGS)

clean:
	rm -rf build/* bin/*

clean_spikes:
	rm -rf bin/*_spike

.PHONY: test clean spike
