#!/usr/bin/env make

SHELL := bash

SrcDir=src
IncludeDir=$(SrcDir)
BuildDir=build
BuildBinDir=$(BuildDir)/bin
BuildObjectsDir=$(BuildDir)/objects
LibDir=program_dependencies

CXX=g++
CXXFLAGS= -I $(IncludeDir) -I $(LibDir) -g -Wall # added -I for library folder

# header file and folder paths
JSON_DEPENDENCY_PATH=$(LibDir)/json/single_include/nlohmann/json.hpp
JSON_GITHUB_PATH=git@github.com:nlohmann/json.git

# the new executable name
NewExecutable=beldum

all: check_dependencies $(BuildBinDir)/$(NewExecutable) install

# check if dependencies exists
check_dependencies:
	@if [ -f '$(JSON_DEPENDENCY_PATH)' ]; then \
		echo '\nJSON library dependency found.\n'; \
		$(eval CXXFLAGS += -D JSON_DEPENDENCY_EXIST) \
	else \
		echo '\nJSON library dependency not found. Please wait while we download...\n'; \
		git clone $(JSON_GITHUB_PATH) $(LibDir)/json/; \
	fi

# TODO: need to modify this
install: 
	@ sudo /usr/bin/install --mode=755 --owner=root --group=root $(BuildBinDir)/beldum /usr/local/bin/beldum

prerequisites:
	@ mkdir -p $(BuildBinDir)
	@ mkdir -p $(BuildObjectsDir)

$(BuildBinDir)/$(NewExecutable): \
		$(BuildObjectsDir)/main.o \
		$(BuildObjectsDir)/package_manager.o | prerequisites
	@ echo Building $@ from $^
	@ $(CXX) -o $@ $^

$(BuildObjectsDir)/%.o: $(SrcDir)/%.cpp | prerequisites
	@ echo Building $@ from $<
	@ $(CXX) $(CXXFLAGS) -c -o $@ $<
