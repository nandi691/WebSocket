#!/bin/bash

# Set the project root and build directory
PROJECT_DIR=$(pwd)
BUILD_DIR="${PROJECT_DIR}/build"
EXE_DIR="${PROJECT_DIR}/exe"

# Function to clean the build directory
clean_build() {
    echo "Cleaning build directory..."
    rm -rf $BUILD_DIR
    echo "Build directory cleaned."

    echo "Cleaning exe directory..."
    rm -rf "$EXE_DIR/*"
    echo "Exe directory cleaned."
}

# Function to delete the build directory entirely
delete_build_dir() {
    echo "Deleting the build directory..."
    rm -rf $BUILD_DIR
    echo "Build directory deleted."
}

# Function to compile the project
compile_project() {
    # Check if build directory exists, if not, create it
    if [ ! -d "$BUILD_DIR" ]; then
        echo "Creating build directory..."
        mkdir "$BUILD_DIR"
    fi

    # Navigate to build directory
    cd "$BUILD_DIR"

    # Run cmake to configure the project
    echo "Running cmake..."
    cmake ..

    # Check if cmake was successful
    if [ $? -ne 0 ]; then
        echo "CMake configuration failed. Exiting."
        exit 1
    fi

    # Compile the project using make
    echo "Compiling project..."
    make

    # Check if make was successful
    if [ $? -eq 0 ]; then
        echo "Compilation successful!"
        echo "Executable is located in: ${BUILD_DIR}/../exe/WebSocket"
    else
        echo "Compilation failed."
        exit 1
    fi
}

# Function to commit changes using git
commit_changes() {
    echo "Staging changes..."
    git add .
    
    echo "Committing changes..."
    read -p "Enter commit message: " commit_message
    git commit -m "$commit_message"
    
    # Check if the commit was successful
    if [ $? -eq 0 ]; then
        echo "Commit successful!"
    else
        echo "Commit failed."
        exit 1
    fi
}

# Display options to the user
echo "Select an option:"
echo "1. Clean and Compile"
echo "2. Compile only"
echo "3. Delete build directory before commit"
echo "4. Exit"
read -p "Enter your choice (1/2/3/4): " choice

# Handle user input
case $choice in
    1)
        clean_build
        compile_project
        ;;
    2)
        compile_project
        ;;
    3)
        # Ask if user wants to delete the build directory before commit
        read -p "Do you want to delete the build directory before committing? (y/n): " delete_choice
        if [ "$delete_choice" == "y" ]; then
            delete_build_dir
        fi
        commit_changes
        ;;
    4)
        echo "Exiting."
        exit 0
        ;;
    *)
        echo "Invalid option. Exiting."
        exit 1
        ;;
esac
