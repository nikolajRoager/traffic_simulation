#!/bin/bash

# Function to convert markdown files to HTML
convert_md_to_html() {
  local file="$1"
  local html_file="${file%.md}.html"
  pandoc -f gfm "$file" -o "$html_file"
  echo "Converted $file to $html_file"
}

export -f convert_md_to_html

# Find all .md files and convert them
find . -type f -name "*.md" -exec bash -c 'convert_md_to_html "$0"' {} \;
