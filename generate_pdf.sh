#!/bin/bash
# Bash script to generate PDF from Modern C++ Examples
# ModernCppRefresherCourse.pdf Generator
# Version: 1.0.0

echo "========================================"
echo "Modern C++ Refresher Course - PDF Generator"
echo "========================================"
echo ""

# Check if pandoc is installed
if ! command -v pandoc &> /dev/null; then
    echo "ERROR: pandoc is not installed!"
    echo ""
    echo "To generate PDFs, you need to install pandoc:"
    echo "  Ubuntu/Debian: sudo apt-get install pandoc texlive-xetex texlive-fonts-recommended"
    echo "  macOS: brew install pandoc basictex"
    echo "  Fedora: sudo dnf install pandoc texlive"
    echo ""
    exit 1
fi

echo "✓ pandoc found: $(pandoc --version | head -n1)"

# Get all source files in alphabetical order
SOURCE_FILES=$(find src -name "*.cpp" -o -name "*.cppm" | sort)
FILE_COUNT=$(echo "$SOURCE_FILES" | wc -l)

echo "✓ Found $FILE_COUNT source files"
echo ""

# Create output directory
OUTPUT_DIR="pdf_output"
mkdir -p "$OUTPUT_DIR"
echo "✓ Created output directory: $OUTPUT_DIR"

# Combined markdown file
COMBINED_MD="$OUTPUT_DIR/ModernCppRefresherCourse_Combined.md"
PDF_OUTPUT="$OUTPUT_DIR/ModernCppRefresherCourse.pdf"

echo "Generating combined markdown file..."
echo ""

# Start with YAML front matter and README
cat > "$COMBINED_MD" << 'EOF'
---
title: "Modern C++ Refresher Course"
subtitle: "Comprehensive Reference - C++11 through C++23"
author:
  - Aung K. Soe
  - Claude Sonnet 4.5 (AI Assistant)
date: January 3, 2026
version: 1.0.0
repository: https://github.com/yourusername/ModernCppExamples
toc: true
toc-depth: 3
linkcolor: blue
urlcolor: blue
geometry: margin=1in
fontsize: 11pt
documentclass: article
header-includes: |
  \usepackage{fancyhdr}
  \pagestyle{fancy}
  \fancyhead[L]{Modern C++ Refresher Course}
  \fancyhead[R]{v1.0.0}
  \usepackage{listings}
  \usepackage{xcolor}
  \lstset{
    basicstyle=\ttfamily\small,
    breaklines=true,
    frame=single,
    numbers=left,
    numberstyle=\tiny,
    language=C++,
    showstringspaces=false,
    commentstyle=\color{gray},
    keywordstyle=\color{blue},
    stringstyle=\color{red}
  }
---

EOF

# Add README content
echo "  [1/3] Adding README.md..."
cat README.md >> "$COMBINED_MD"
echo -e "\n\n\\newpage\n\n" >> "$COMBINED_MD"

# Add source code examples (alphabetically)
echo "  [2/3] Adding source code examples (alphabetically)..."
COUNTER=0
for file in $SOURCE_FILES; do
    ((COUNTER++))
    FILENAME=$(basename "$file")
    echo "    Processing [$COUNTER/$FILE_COUNT]: $FILENAME"
    
    cat >> "$COMBINED_MD" << EOF
# Source Code: $FILENAME

**File:** \`$file\`  
**Repository:** [View on GitHub](https://github.com/yourusername/ModernCppExamples/blob/main/$file)

\`\`\`cpp
EOF
    cat "$file" >> "$COMBINED_MD"
    cat >> "$COMBINED_MD" << EOF

\`\`\`

\newpage

EOF
done

# Add INDEX.md
echo "  [3/3] Adding INDEX.md..."
echo -e "\n\n# Appendix: Comprehensive Index\n\n" >> "$COMBINED_MD"
cat INDEX.md >> "$COMBINED_MD"

echo ""
echo "✓ Combined markdown created: $COMBINED_MD"
echo "  Size: $(du -h "$COMBINED_MD" | cut -f1)"
echo ""

# Generate PDF
echo "Generating PDF with pandoc..."
echo "  This may take a few minutes..."
echo ""

pandoc "$COMBINED_MD" \
    -o "$PDF_OUTPUT" \
    --pdf-engine=xelatex \
    --toc \
    --toc-depth=3 \
    --number-sections \
    --highlight-style=tango \
    -V geometry:margin=1in \
    -V fontsize=11pt \
    -V linkcolor=blue \
    -V urlcolor=blue

if [ $? -eq 0 ] && [ -f "$PDF_OUTPUT" ]; then
    echo ""
    echo "========================================"
    echo "✓ PDF GENERATED SUCCESSFULLY!"
    echo "========================================"
    echo ""
    echo "Output file: $PDF_OUTPUT"
    echo "File size: $(du -h "$PDF_OUTPUT" | cut -f1)"
    echo ""
    
    # Try to open PDF
    if command -v xdg-open &> /dev/null; then
        echo "Opening PDF..."
        xdg-open "$PDF_OUTPUT" &
    elif command -v open &> /dev/null; then
        echo "Opening PDF..."
        open "$PDF_OUTPUT"
    fi
else
    echo "ERROR: PDF generation failed!"
    echo "Check the output above for errors."
    echo ""
    echo "Common issues:"
    echo "1. LaTeX not installed (required for PDF generation)"
    echo "2. Large files may cause memory issues"
    echo "3. Special characters in code may need escaping"
fi

echo ""
echo "========================================"
echo "Generated files:"
echo "  - Combined Markdown: $COMBINED_MD"
echo "  - PDF Output: $PDF_OUTPUT"
echo "========================================"
