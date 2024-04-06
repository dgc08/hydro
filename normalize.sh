#!/bin/bash

# Function to recursively replace Windows line endings with Unix line endings
replace_line_endings() {
    local dir=$1
    local file

    # Loop through all files and directories in the specified directory
    for file in "$dir"/*; do
        if [ -d "$file" ]; then
            # If it's a directory, call the function recursively
            replace_line_endings "$file"
        elif [ -f "$file" ]; then
            # If it's a file, replace line endings if it's a text file
            if [ -f "$file" -a -r "$file" ]; then
                # Check if the file is a text file (excluding binary files)
                if [[ $(file --mime-type -b "$file") == text/* ]]; then
                    # Replace Windows line endings (CRLF) with Unix line endings (LF)
                    sed -i 's/\r$//' "$file"
                fi
            fi
        fi
    done
}

# Check if the directory is provided as an argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 directory"
    exit 1
fi

# Check if the specified directory exists
if [ ! -d "$1" ]; then
    echo "Error: Directory '$1' does not exist."
    exit 1
fi

# Call the function to replace line endings recursively
replace_line_endings "$1"

echo "Windows line endings replaced with Unix line endings in directory '$1'."
