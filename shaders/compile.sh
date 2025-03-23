#!/bin/bash

OUTPUT_DIR="."
mkdir -p "$OUTPUT_DIR"

for file in ./*.vert ./*.frag; do
  if [[ -f "$file" ]]; then # Check if it's a regular file
    base=$(basename "$file")
    name="${base%.*}"
    glslc "$file" -o "$OUTPUT_DIR/$name.spv"
  fi
done
