#!/usr/bin/env make

SrcDir=src
IncludeDir=$(SrcDir)
BuildDir=build
BuildBinDir=$(BuildDir)/bin
BuildObjectsDir=$(BuildDir)/objects
LibDir=program_dependencies

CC=g++
CFlags= -I $(IncludeDir) -I $(LibDir) -g -Wall # added -I for library folder

# header file and folder paths
JSON_DEPENDENCY_PATH=$(LibDir)/json/single_include/nlohmann/json.hpp
JSON_GITHUB_PATH=git@github.com:nlohmann/json.git

# the new executable name
NewExecutable=cdm_cpp

all: check_dependencies $(BuildBinDir)/$(NewExecutable)

# check if dependencies exists
check_dependencies:
	@if [ -f '$(JSON_DEPENDENCY_PATH)' ]; then \
		echo '\n\nJSON library dependency found.\n\n'; \
		$(eval CFlags += -D JSON_DEPENDENCY_EXIST) \
	else \
		echo '\n\nJSON library dependency not found. Please wait while we download...\n\n'; \
		git clone $(JSON_GITHUB_PATH) $(LibDir)/json/; \
	fi

prerequisites:
	@ mkdir -p $(BuildBinDir)
	@ mkdir -p $(BuildObjectsDir)

$(BuildBinDir)/$(NewExecutable): \
		$(BuildObjectsDir)/main.o \
		$(BuildObjectsDir)/package_manager.o | prerequisites
	@ echo Building $@ from $^
	@ $(CC) -o $@ $^

$(BuildObjectsDir)/%.o: $(SrcDir)/%.cpp | prerequisites
	@ echo Building $@ from $<
	@ $(CC) $(CFlags) -c -o $@ $<
