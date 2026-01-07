#!/bin/bash
# Bash script to generate EPUB from Modern C++ Examples
# ModernCppRefresherCourse.epub Generator for WSL/Linux
# EPUB is a modern, cross-platform ebook format that works on all devices
# Version: 1.0.0

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
GRAY='\033[0;37m'
NC='\033[0m' # No Color

echo "========================================"
echo "Modern C++ Refresher Course - EPUB Generator"
echo "========================================"
echo ""

# Check if pandoc is installed
if ! command -v pandoc &> /dev/null; then
    echo -e "${RED}ERROR: pandoc is not installed!${NC}"
    echo ""
    echo -e "${YELLOW}To generate EPUB files, you need to install pandoc:${NC}"
    echo -e "${YELLOW}  sudo apt-get update${NC}"
    echo -e "${YELLOW}  sudo apt-get install pandoc${NC}"
    echo ""
    exit 1
fi

echo -e "${GREEN}✓ pandoc found: $(pandoc --version | head -n1)${NC}"
echo ""

# Create output directory
OUTPUT_DIR="epub_out"
mkdir -p "$OUTPUT_DIR"
echo -e "${GREEN}✓ Created output directory: $OUTPUT_DIR${NC}"

# Get all documentation files
DOC_COUNT=$(find MarkDownDocuments -name "*.md" 2>/dev/null | wc -l)
echo -e "${GREEN}✓ Found $DOC_COUNT documentation files${NC}"

# Get all source files
SRC_COUNT=$(find src -name "*.cpp" -o -name "*.cppm" 2>/dev/null | wc -l)
echo -e "${GREEN}✓ Found $SRC_COUNT source files${NC}"
echo ""

# Create combined markdown
COMBINED_MD="$OUTPUT_DIR/ModernCppRefresherCourse_Combined.md"
EPUB_OUTPUT="$OUTPUT_DIR/ModernCppRefresherCourse.epub"

echo -e "${CYAN}Generating combined markdown file...${NC}"
echo ""

# Start with YAML metadata and title
cat > "$COMBINED_MD" << EOF
---
title: Modern C++ Refresher Course
author: Aung K. Soe, Claude Sonnet 4.5
date: $(date +%Y-%m-%d)
lang: en-US
---

# Modern C++ Refresher Course

> **A comprehensive collection of Modern C++ examples covering C++11 through C++23**

EOF

# Add README if exists
if [ -f "README.md" ]; then
    cat README.md >> "$COMBINED_MD"
    echo "" >> "$COMBINED_MD"
fi

# Add all documentation files
echo -e "${CYAN}Adding documentation files...${NC}"
for doc in MarkDownDocuments/*.md; do
    if [ -f "$doc" ]; then
        basename_doc=$(basename "$doc")
        echo -e "${GRAY}  + $basename_doc${NC}"
        echo "" >> "$COMBINED_MD"
        echo "# $(basename "$doc" .md)" >> "$COMBINED_MD"
        echo "" >> "$COMBINED_MD"
        cat "$doc" >> "$COMBINED_MD"
        echo "" >> "$COMBINED_MD"
    fi
done

# Add source code section
cat >> "$COMBINED_MD" << 'EOF'

---

# Source Code Examples

Below are all the source code examples included in this course.

EOF

# Add all source files
echo ""
echo -e "${CYAN}Adding source code files...${NC}"
for src in src/*.cpp src/*.cppm; do
    if [ -f "$src" ]; then
        basename_src=$(basename "$src")
        echo -e "${GRAY}  + $basename_src${NC}"
        echo "" >> "$COMBINED_MD"
        echo "## $basename_src" >> "$COMBINED_MD"
        echo "" >> "$COMBINED_MD"
        echo '```cpp' >> "$COMBINED_MD"
        cat "$src" >> "$COMBINED_MD"
        echo '```' >> "$COMBINED_MD"
        echo "" >> "$COMBINED_MD"
    fi
done

echo ""
echo -e "${GREEN}✓ Combined markdown created: $COMBINED_MD${NC}"
echo ""

# Generate EPUB
echo -e "${CYAN}Generating EPUB...${NC}"
echo ""

PANDOC_ARGS=(
    "$COMBINED_MD"
    "-o" "$EPUB_OUTPUT"
    "--toc"
    "--toc-depth=3"
    "--highlight-style=tango"
    "--metadata" "title=Modern C++ Refresher Course"
    "--metadata" "author=Aung K. Soe, Claude Sonnet 4.5"
    "--metadata" "date=$(date +%Y-%m-%d)"
    "--metadata" "lang=en-US"
)

# Add cover image if exists
if [ -f "cover.png" ]; then
    PANDOC_ARGS+=("--epub-cover-image=cover.png")
fi

pandoc "${PANDOC_ARGS[@]}"

if [ -f "$EPUB_OUTPUT" ]; then
    FILE_SIZE=$(du -h "$EPUB_OUTPUT" | cut -f1)
    echo ""
    echo "========================================"
    echo -e "${GREEN}✓ SUCCESS!${NC}"
    echo "========================================"
    echo ""
    echo -e "${GREEN}EPUB file generated successfully!${NC}"
    echo -e "${CYAN}Output: $EPUB_OUTPUT${NC}"
    echo -e "${CYAN}Size: $FILE_SIZE${NC}"
    echo ""
    echo -e "${YELLOW}EPUB Features:${NC}"
    echo -e "${GREEN}  ✓ Works on Windows, Mac, Linux, Android, iOS${NC}"
    echo -e "${GREEN}  ✓ Interactive table of contents${NC}"
    echo -e "${GREEN}  ✓ Syntax-highlighted code${NC}"
    echo -e "${GREEN}  ✓ Searchable content${NC}"
    echo -e "${GREEN}  ✓ Adjustable font size${NC}"
    echo -e "${GREEN}  ✓ Night mode support${NC}"
    echo ""
    echo -e "${YELLOW}Read with:${NC}"
    echo -e "${CYAN}  Windows: Calibre, Microsoft Edge${NC}"
    echo -e "${CYAN}  Mac: iBooks${NC}"
    echo -e "${CYAN}  Linux: Calibre, FBReader${NC}"
    echo -e "${CYAN}  Android/iOS: Google Play Books, Apple Books${NC}"
    echo ""
    
    # Ask if user wants to open the EPUB file
    read -p "Would you like to open the EPUB file now? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if command -v xdg-open &> /dev/null; then
            xdg-open "$EPUB_OUTPUT" 2>/dev/null &
        elif command -v cmd.exe &> /dev/null; then
            # WSL - open in Windows
            cmd.exe /c start "$(wslpath -w "$(pwd)/$EPUB_OUTPUT")" 2>/dev/null &
        else
            echo -e "${YELLOW}Please open $EPUB_OUTPUT with your preferred EPUB reader${NC}"
        fi
    fi
else
    echo ""
    echo -e "${RED}ERROR: EPUB generation failed!${NC}"
    echo -e "${YELLOW}Check the output above for errors.${NC}"
    exit 1
fi
