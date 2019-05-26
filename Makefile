#TODO I should probably fix the debugging flags...
#TODO if tools source changes I need to run the make for the tools
#AND remake the models binaries

CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -Iinclude -I$(HOME)/include -DDEBUG
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

SPIKE_SRC = $(wildcard spike/*.cpp)
SPIKE_BIN = $(patsubst spike/%.cpp,bin/%_spike,$(SPIKE_SRC))

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

EXE = bin/drone

WAVEFRONT_PARSER = tools/bin/wavefrontparser

MODELS_SRC = $(wildcard models/wavefront/*.obj)
MODELS = $(patsubst models/wavefront/%.obj,models/bin/%.bin,$(MODELS_SRC))

WAVEFRONTPARSER_SRC = $(wildcard tools/src/wavefrontparser/*.cpp)

drone: $(EXE) $(WAVEFRONT_PARSER) $(MODELS)

$(EXE): $(OBJ)
	g++ -g $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ): build/%.o: src/%.cpp
	g++ -g $(CFLAGS) -o $@ -c $^ $(LDFLAGS)

bin/%_spike: spike/%.cpp
	g++ $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(MODELS): models/bin/%.bin: models/wavefront/%.obj
	$(WAVEFRONT_PARSER) $^ $@

wavefrontparser: $(WAVEFRONT_PARSER)

$(WAVEFRONT_PARSER): $(WAVEFRONTPARSER_SRC)
	$(MAKE) -C tools wavefrontparser

spike: $(SPIKE_BIN)

#consider loading the tags file in vim
tags: $(SRC)
	@gcc -M $(CFLAGS) $^ include/graphics.hpp | \
	sed -e 's/[\\ ]/\n/g' | \
	sed -e '/^$$/d' -e '/\.o:[ \t]*$$/d' | \
	ctags -L - --c++-kinds=+p --fields=+iaS --extra=+q

clean:
	rm -rf build/* bin/*
	rm -rf models/bin/*
	$(MAKE) -C tools clean

clean_spikes:
	rm -rf bin/*_spike

.PHONY: test clean spike wavefrontparser
