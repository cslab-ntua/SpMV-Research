#!/bin/bash

concatenate_files() {
    # The directories to search
    local search_dirs=("$@")
    
    # The destination directory
    local dest_dir="validation"

    # Create the destination directory if it doesn't exist
    mkdir -p "$dest_dir"

    # Use an associative array to keep track of files to concatenate
    declare -A file_map

    # Find all files in the specified directories
    for dir in "${search_dirs[@]}"; do
        while IFS= read -r -d '' file; do
            # Get the relative path and filename
            relative_path="${file#$dir/}"
            filename=$(basename "$relative_path")

            # Append the file path to the array for the filename
            file_map["$relative_path"]+="$file "
        done < <(find "$dir" -type f -print0)
    done

    # Concatenate files with the same name
    for relative_path in "${!file_map[@]}"; do
        # Create the necessary directories in the destination folder
        mkdir -p "$dest_dir/$(dirname "$relative_path")"

        # Concatenate the files and place them in the destination folder
        cat ${file_map["$relative_path"]} > "$dest_dir/$relative_path"
    done
}

# Example usage:
# concatenate_files "rep1/validation" "rep2/validation" "rep3/validation"
concatenate_files "rep1/validation" "rep2/validation" "rep3/validation" "rep4/validation" "rep5/validation"
