# PDF Generation Instructions

This directory contains scripts to generate **ModernCppRefresherCourse.pdf** from all the examples and documentation.

## Prerequisites

### Windows (PowerShell)
1. **Install pandoc:**
   ```powershell
   # Using chocolatey
   choco install pandoc
   
   # Or download from: https://pandoc.org/installing.html
   ```

2. **Install LaTeX (MiKTeX) for PDF generation:**
   ```powershell
   # Using chocolatey
   choco install miktex
   
   # Or download from: https://miktex.org/download
   ```

### Linux
```bash
# Ubuntu/Debian
sudo apt-get install pandoc texlive-xetex texlive-fonts-recommended

# Fedora
sudo dnf install pandoc texlive

# Arch Linux
sudo pacman -S pandoc texlive-core texlive-latexextra
```

### macOS
```bash
# Using Homebrew
brew install pandoc
brew install --cask basictex

# Or install MacTeX: https://www.tug.org/mactex/
```

## Generating the PDF

### Windows (PowerShell)
```powershell
# Run the generation script
.\generate_pdf.ps1

# If you get execution policy errors:
Set-ExecutionPolicy -Scope CurrentUser -ExecutionPolicy RemoteSigned
.\generate_pdf.ps1
```

### Linux/macOS
```bash
# Make the script executable
chmod +x generate_pdf.sh

# Run the generation script
./generate_pdf.sh
```

## Output

The script will create:
- `pdf_output/ModernCppRefresherCourse_Combined.md` - Combined markdown file
- `pdf_output/ModernCppRefresherCourse.pdf` - Final PDF document

## PDF Structure

The generated PDF contains:

1. **Title Page** with version, authors, and repository link
2. **Table of Contents** (automatically generated, 3 levels deep)
3. **README.md** - Overview, quick start, complete alphabetical listing
4. **All Source Code Examples** (alphabetically sorted)
   - AdvancedExceptionHandling.cpp
   - ARMInstructionSets.cpp
   - ...
   - VirtualFunctionsInTemplates.cpp
5. **INDEX.md** (Appendix) - Comprehensive topic mapping

## Customization

### Modify PDF Settings

Edit the YAML front matter in `generate_pdf.ps1` or `generate_pdf.sh`:

```yaml
---
title: "Modern C++ Refresher Course"
subtitle: "Comprehensive Reference - C++11 through C++23"
geometry: margin=1in    # Change margins
fontsize: 11pt          # Change font size (10pt, 11pt, 12pt)
linkcolor: blue         # Change link color
---
```

### Change PDF Engine

By default, the script uses `xelatex`. You can change to:
- `pdflatex` - Faster, but less Unicode support
- `lualatex` - More features, slower

Edit the `--pdf-engine=xelatex` line in the script.

### Include/Exclude Files

Modify the source file selection in the script:
```powershell
# Windows: Change this line in generate_pdf.ps1
$sourceFiles = Get-ChildItem -Path "src\*.cpp", "src\*.cppm" | Where-Object { $_.Name -notlike "*Test*" }

# Linux: Change this line in generate_pdf.sh
SOURCE_FILES=$(find src -name "*.cpp" -o -name "*.cppm" | grep -v Test | sort)
```

## Troubleshooting

### Error: "pandoc: xelatex not found"
**Solution:** Install LaTeX distribution (MiKTeX/MacTeX/TeX Live)

### Error: "! LaTeX Error: File ... not found"
**Solution:** LaTeX is missing packages. With MiKTeX, enable auto-install:
```powershell
# MiKTeX settings -> Package installation -> Always install
```

### Error: "Out of memory"
**Solution:** The PDF is very large (70+ source files). Options:
1. Increase system memory
2. Split into multiple PDFs (by topic)
3. Use `pdflatex` instead of `xelatex`

### PDF generation is very slow
**Solution:** This is normal for large documents with code syntax highlighting. 
Expect 5-10 minutes for ~70 files.

### Code formatting looks wrong
**Solution:** The script uses the `tango` syntax highlighting style. 
Change `--highlight-style=tango` to:
- `pygments` (default)
- `kate`
- `monochrome`
- `espresso`
- `haddock`

## Manual PDF Generation

If the scripts don't work, generate manually:

```bash
# 1. Create combined markdown
cat README.md > combined.md
echo "\n\newpage\n" >> combined.md

# 2. Add all source files
for file in src/*.cpp src/*.cppm; do
    echo "\n# $file\n\`\`\`cpp" >> combined.md
    cat "$file" >> combined.md
    echo "\`\`\`\n\newpage\n" >> combined.md
done

# 3. Add index
echo "\n# Appendix: Index\n" >> combined.md
cat INDEX.md >> combined.md

# 4. Generate PDF
pandoc combined.md -o ModernCppRefresherCourse.pdf --pdf-engine=xelatex --toc
```

## Alternative: Markdown to HTML to PDF

If LaTeX issues persist, use HTML as intermediate:

```bash
# Generate HTML
pandoc combined.md -o ModernCppRefresherCourse.html --toc --standalone

# Convert HTML to PDF using wkhtmltopdf
wkhtmltopdf ModernCppRefresherCourse.html ModernCppRefresherCourse.pdf
```

## Contact

For issues with PDF generation:
- Check [pandoc documentation](https://pandoc.org/MANUAL.html)
- Open an issue on GitHub: https://github.com/yourusername/ModernCppExamples/issues

---

**Version:** 1.0.0  
**Last Updated:** January 3, 2026
