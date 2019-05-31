CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -Iinclude -I$(HOME)/include -DDEBUG
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

#main is a special snowflake, mostly for testing and the main function weirdness
ALL_SRC = $(shell find src -name "*.cpp")
SRC = $(filter-out src/main.cpp, $(ALL_SRC))
ALL_OBJ = $(patsubst src/%.cpp, build/%.o, $(ALL_SRC))
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

WAVEFRONT_SRC = $(wildcard models/wavefront/*.obj)
WAVEFRONT_BIN = $(patsubst models/wavefront/%.obj, assets/models/%.bin, $(WAVEFRONT_SRC))
WAVEFRONT_TOOL = tools/bin/wavefrontparser
WAVEFRONT_BLD = build/wavefrontparser
WAVEFRONT_PARSER = tools/bin/wavefrontparser

SHADER_TYPES = frag vert
SHADER_SRC = $(foreach type, $(SHADER_TYPES), $(wildcard shaders/*.$(type)))
SHADER_SPV = $(patsubst shaders/%, assets/shaders/%.spv, $(SHADER_SRC))

SPIKE_SRC = $(wildcard spike/*.cpp)
SPIKE_BIN = $(patsubst spike/%.cpp, bin/spike_%, $(SPIKE_SRC))

BUILD_DIRS = $(sort $(dir $(ALL_OBJ) $(WAVEFRONT_BIN) $(SHADER_SPV)) bin)

EXE = bin/drone

all: $(BUILD_DIRS) $(EXE) $(ALL_OBJ) $(WAVEFRONT_BIN) shaders

shaders: $(BUILD_DIRS) $(SHADER_SPV)	

$(SHADER_SPV): assets/shaders/%.spv: shaders/%
	$(VULKAN_SDK_PATH)/bin/glslangValidator -V -o $@ $^

$(EXE): $(ALL_OBJ)
	g++ -g $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(ALL_OBJ): build/%.o: src/%.cpp
	g++ -g $(CFLAGS) -o $@ -c $^ $(LDFLAGS)

spike: $(BUILD_DIRS) $(OBJ) $(SPIKE_BIN)

bin/spike_%: spike/%.cpp
	g++ -g $(CFLAGS) -o $@ $^ $(OBJ) $(LDFLAGS)

$(BUILD_DIRS):
	@mkdir -p $@

wavefront: $(WAVEFRONT_BIN)

$(WAVEFRONT_BIN): assets/models/%.bin: models/wavefront/%.obj $(WAVEFRONT_TOOL)
	$(WAVEFRONT_PARSER) $(filter-out $(WAVEFRONT_TOOL), $^) $@
	@touch $(WAVEFRONT_BLD)

$(WAVEFRONT_TOOL): $(WAVEFRONT_BLD)
	$(MAKE) -C tools wavefrontparser

$(WAVEFRONT_BLD):

clean:
	rm -rf $(BUILD_DIRS)
	@$(MAKE) -C tools clean

.PHONY: all wavefront shaders spike
