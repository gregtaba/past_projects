#!/bin/bash
# Check if no file arguments were provided
if [ $# -eq 0 ]; then
    echo "lack of files"
    exit 1
fi
# Go through each file 
for file in "$@"; do
    # Checking if the file exists 
    if [ ! -f "$file" ]; then
        echo "'$file' not found "
        continue
    fi
    # Read each line from the file 
    while IFS= read -r task || [ -n "$task" ]; do
        # Execute the jobCommander with the task in the line you just read
        ./jobCommander issueJob "$task"
    done < "$file"
    sleep 1
done

