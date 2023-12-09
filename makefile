#!/usr/bin/env make

SrcDir=src
IncludeDir=$(SrcDir)
BuildDir = build
BuildBinDir = $(BuildDir)/bin
BuildObjectsDir = $(BuildDir)/objects

CC = g++
CFlags = -I $(IncludeDir) -g

# The new executable name
NewExecutable = cdm_cpp

all: $(BuildBinDir)/$(NewExecutable)

prerequisites:
	@ mkdir -p $(BuildBinDir)
	@ mkdir -p $(BuildObjectsDir)

$(BuildBinDir)/$(NewExecutable): \
		$(BuildObjectsDir)/main.o | prerequisites
		# $(BuildObjectsDir)/http_server.o \
		# $(BuildObjectsDir)/server_logging.o \
		# $(BuildObjectsDir)/route_handler.o | prerequisites
	@ echo Building $@ from $^
	@ $(CC) -o $@ $^

$(BuildObjectsDir)/%.o: $(SrcDir)/%.cpp | prerequisites
	@ echo Building $@ from $<
	@ $(CC) $(CFlags) -c -o $@ $<
