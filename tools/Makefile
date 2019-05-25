#Do your best to only need includes here
#TODO I should probably fix the debugging flags...

CFLAGS = -std=c++17 -Iinclude -I../include -I$(HOME)/include

MODELPARSER_BIN = bin/modelparser
MODELPARSER_BLD = build/modelparser
MODELPARSER_SRC = $(wildcard src/modelparser/*.cpp)
MODELPARSER_OBJ = $(patsubst src/modelparser/%.cpp,build/modelparser/%.o,$(MODELPARSER_SRC))

modelparser: $(MODELPARSER_BLD) $(MODELPARSER_BIN)

$(MODELPARSER_BLD):
	@mkdir $@

$(MODELPARSER_BIN): $(MODELPARSER_OBJ)
	g++ -g -Wall $(CFLAGS) -o $@ $^

$(MODELPARSER_OBJ): build/modelparser/%.o: src/modelparser/%.cpp
	g++ -g -Wall $(CFLAGS) -o $@ -c $^

clean:
	@rm -rf build/*
	@rm -rf bin/*

.PHONY: clean
