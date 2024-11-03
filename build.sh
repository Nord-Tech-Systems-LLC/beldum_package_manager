#!/bin/bash

# Define variables
BUILD_DIR="target/debug/build"
BUILD_TYPE="Debug" # Change to "Release" if needed

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

# Step 2: Run CMake to configure the project
echo "Configuring project with CMake in $BUILD_DIR..."
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" || error_exit "CMake configuration failed."

# Step 3: Build the project
echo "Building project in $BUILD_DIR..."
cmake --build "$BUILD_DIR" --target all || error_exit "Build failed."

# Step 4: Optionally install the executable (requires sudo)
read -p "Do you want to install the executable to /usr/local/bin? (y/n): " install_choice
if [[ "$install_choice" == "y" || "$install_choice" == "Y" ]]; then
    echo "Installing executable..."
    sudo cmake --install "$BUILD_DIR" || error_exit "Installation failed."
    echo "Build and installation complete!"
fi

if [[ "$install_choice" == "n" || "$install_choice" == "N" ]]; then
    echo "Build complete, but not installed"
    error_exit "Installation failed."
fi

