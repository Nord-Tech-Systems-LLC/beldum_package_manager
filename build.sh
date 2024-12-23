#!/bin/bash

# Define variables
BUILD_DIR="target/debug/build"
BUILD_TYPE="Release" # Change to "Release" if needed

# Function to print error messages
function error_exit {
    echo "[ERROR] $1"
    exit 1
}

# Step 1: Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory at $BUILD_DIR..."
    mkdir -p "$BUILD_DIR" || error_exit "Failed to create build directory."
fi

# Step 3: Get the Git version and write to version file
git fetch --tags
VERSION=$(git describe --tags --abbrev=0)
if [ -z "$VERSION" ]; then
    VERSION="unknown"
fi

# Step 4: Run CMake to configure the project and pass the version as a variable
echo "Configuring project with CMake in $BUILD_DIR with version $VERSION..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DPROJECT_VERSION_NUM="$VERSION" || error_exit "CMake configuration failed."

# Step 5: Build the project
echo "Building project in $BUILD_DIR..."
cmake --build "$BUILD_DIR" --target all || error_exit "Build failed."

# Step 6: Optionally install the executable (requires sudo)
read -p "Do you want to install the executable to /usr/local/bin? (y/n): " install_choice
if [[ "${install_choice@L}" == "y" ]]; then
    echo "Installing executable..."
    sudo cmake --install "$BUILD_DIR" || error_exit "Installation failed."
    echo "Build and installation complete! Version: $VERSION"
elif [[ "${install_choice@L}"  ]]; then
    echo "Build complete, but not installed. Version: $VERSION"
    error_exit "Installation failed."
fi

