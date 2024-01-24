#!/usr/bin/env make

SHELL := bash

SrcDir=src
IncludeDir=$(SrcDir)
BuildDir=build
BuildBinDir=$(BuildDir)/bin
BuildObjectsDir=$(BuildDir)/objects
LibDir=program_dependencies

CXX=g++
CXXSTD=-std=c++20
CXXFLAGS= \
	-I $(IncludeDir) \
	-I $(LibDir) \
	-g \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Wshadow
	# added -I for library folder
LDFLAGS=

DEBUG?= 1
ifeq ($(DEBUG),1)
	CXXFLAGS+= -ggdb3
	CXXFLAGS+= -fsanitize=undefined,address
	LDFLAGS+= -fsanitize=undefined,address
else
	CXXFLAGS+= -O2
	CXXFLAGS+= -Werror
endif


# header file and folder paths
JSON_DEPENDENCY_PATH=$(LibDir)/json/single_include/nlohmann/json.hpp
JSON_GITHUB_PATH=git@github.com:nlohmann/json.git

# the new executable name
NewExecutable=beldum

all: $(BuildBinDir)/$(NewExecutable)

check_dependencies_exist:
	@if [ -f '$(JSON_DEPENDENCY_PATH)' ]; then \
		echo '\nJSON library dependency found.\n'; \
		$(eval CXXFLAGS += -D JSON_DEPENDENCY_EXIST) \
	else \
		echo '\nJSON library dependency not found. Please wait while we download...\n'; \
		git clone $(JSON_GITHUB_PATH) $(LibDir)/json/; \
	fi

install: $(BuildBinDir)/$(NewExecutable)
	@ sudo /usr/bin/install --mode=755 --owner=root --group=root $(BuildBinDir)/beldum /usr/local/bin/beldum

prerequisites:;
	@ mkdir -p $(BuildBinDir)
	@ mkdir -p $(BuildObjectsDir)

$(BuildBinDir)/$(NewExecutable): \
		$(BuildObjectsDir)/main.o \
		$(BuildObjectsDir)/package_manager.o | prerequisites check_dependencies_exist
	@ echo Building $@ from $^
	@ $(CXX) $(CXXSTD) $(LDFLAGS) -o $@ $^

$(BuildObjectsDir)/%.o: $(SrcDir)/%.cpp | prerequisites check_dependencies_exist
	@ echo Building $@ from $<
	$(CXX) $(CXXSTD) $(CXXFLAGS) -c -o $@ $<

.PHONY: check_dependencies_exist clean install prerequisites

clean:
	@ find -P "$(BuildDir)" -xdev -delete
