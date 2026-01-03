# PowerShell script to generate PDF from Modern C++ Examples
# ModernCppRefresherCourse.pdf Generator
# Version: 1.0.0

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Modern C++ Refresher Course - PDF Generator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if pandoc is installed
$pandocExists = Get-Command pandoc -ErrorAction SilentlyContinue
if (-not $pandocExists) {
    Write-Host "ERROR: pandoc is not installed!" -ForegroundColor Red
    Write-Host ""
    Write-Host "To generate PDFs, you need to install pandoc:" -ForegroundColor Yellow
    Write-Host "1. Download from: https://pandoc.org/installing.html" -ForegroundColor Yellow
    Write-Host "2. Or use chocolatey: choco install pandoc" -ForegroundColor Yellow
    Write-Host "3. Or use scoop: scoop install pandoc" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "For better PDF output, also install LaTeX:" -ForegroundColor Yellow
    Write-Host "1. MiKTeX: https://miktex.org/download" -ForegroundColor Yellow
    Write-Host "2. Or use chocolatey: choco install miktex" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

Write-Host "✓ pandoc found: $($pandocExists.Version)" -ForegroundColor Green

# Get all source files in alphabetical order
$sourceFiles = Get-ChildItem -Path "src\*.cpp", "src\*.cppm" | Sort-Object Name

Write-Host "✓ Found $($sourceFiles.Count) source files" -ForegroundColor Green
Write-Host ""

# Create output directory
$outputDir = "pdf_output"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "✓ Created output directory: $outputDir" -ForegroundColor Green
}

# Combined markdown file
$combinedMd = Join-Path $outputDir "ModernCppRefresherCourse_Combined.md"
$pdfOutput = Join-Path $outputDir "ModernCppRefresherCourse.pdf"

Write-Host "Generating combined markdown file..." -ForegroundColor Cyan
Write-Host ""

# Start with README
$content = @"
---
title: "Modern C++ Refresher Course"
subtitle: "Comprehensive Reference - C++11 through C++23"
author:
  - Aung K. Soe
  - Claude Sonnet 4.5 (AI Assistant)
date: January 3, 2026
version: 1.0.0
repository: https://github.com/AungKyawSoe-Tech/ModernCppExamples
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

"@

# Add README content
Write-Host "  [1/3] Adding README.md..." -ForegroundColor Yellow
$content += Get-Content "README.md" -Raw
$content += "`n`n\newpage`n`n"

# Add source code examples (alphabetically)
Write-Host "  [2/3] Adding source code examples (alphabetically)..." -ForegroundColor Yellow
$counter = 0
foreach ($file in $sourceFiles) {
    $counter++
    Write-Host "    Processing [$counter/$($sourceFiles.Count)]: $($file.Name)" -ForegroundColor Gray
    
    $fileName = $file.Name
    $content += @"
# Source Code: $fileName

**File:** ``src/$fileName``  
**Repository:** [View on GitHub](https://github.com/AungKyawSoe-Tech/ModernCppExamples/blob/main/src/$fileName)

``````cpp
"@
    $content += Get-Content $file.FullName -Raw
    $content += @"

``````

\newpage

"@
}

# Add INDEX.md
Write-Host "  [3/3] Adding INDEX.md..." -ForegroundColor Yellow
$content += "`n`n# Appendix: Comprehensive Index`n`n"
$content += Get-Content "INDEX.md" -Raw

# Write combined markdown
$content | Out-File -FilePath $combinedMd -Encoding UTF8
Write-Host ""
Write-Host "✓ Combined markdown created: $combinedMd" -ForegroundColor Green
Write-Host "  Size: $([math]::Round((Get-Item $combinedMd).Length / 1MB, 2)) MB" -ForegroundColor Gray
Write-Host ""

# Generate PDF
Write-Host "Generating PDF with pandoc..." -ForegroundColor Cyan
Write-Host "  This may take a few minutes..." -ForegroundColor Yellow
Write-Host ""

$pandocArgs = @(
    $combinedMd,
    "-o", $pdfOutput,
    "--pdf-engine=xelatex",
    "--toc",
    "--toc-depth=3",
    "--number-sections",
    "--highlight-style=tango",
    "-V", "geometry:margin=1in",
    "-V", "fontsize=11pt",
    "-V", "linkcolor=blue",
    "-V", "urlcolor=blue"
)

try {
    & pandoc $pandocArgs 2>&1 | Out-Host
    
    if (Test-Path $pdfOutput) {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "✓ PDF GENERATED SUCCESSFULLY!" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        Write-Host "Output file: $pdfOutput" -ForegroundColor Cyan
        Write-Host "File size: $([math]::Round((Get-Item $pdfOutput).Length / 1MB, 2)) MB" -ForegroundColor Cyan
        Write-Host ""
        Write-Host "Opening PDF..." -ForegroundColor Yellow
        Start-Process $pdfOutput
    } else {
        Write-Host "ERROR: PDF generation failed!" -ForegroundColor Red
        Write-Host "Check the output above for errors." -ForegroundColor Red
    }
} catch {
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    Write-Host ""
    Write-Host "Common issues:" -ForegroundColor Yellow
    Write-Host "1. LaTeX not installed (required for PDF generation)" -ForegroundColor Yellow
    Write-Host "2. Large files may cause memory issues" -ForegroundColor Yellow
    Write-Host "3. Special characters in code may need escaping" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Generated files:" -ForegroundColor Cyan
Write-Host "  - Combined Markdown: $combinedMd" -ForegroundColor Gray
Write-Host "  - PDF Output: $pdfOutput" -ForegroundColor Gray
Write-Host "========================================" -ForegroundColor Cyan
