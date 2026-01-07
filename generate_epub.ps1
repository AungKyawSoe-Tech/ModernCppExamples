# PowerShell script to generate EPUB from Modern C++ Examples
# ModernCppRefresherCourse.epub Generator
# EPUB is a modern, cross-platform ebook format that works on all devices
# Version: 1.0.0

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Modern C++ Refresher Course - EPUB Generator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if pandoc is installed
$pandocExists = Get-Command pandoc -ErrorAction SilentlyContinue
if (-not $pandocExists) {
    Write-Host "ERROR: pandoc is not installed!" -ForegroundColor Red
    Write-Host ""
    Write-Host "To generate EPUB files, you need to install pandoc:" -ForegroundColor Yellow
    Write-Host "1. Download from: https://pandoc.org/installing.html" -ForegroundColor Yellow
    Write-Host "2. Or use chocolatey: choco install pandoc" -ForegroundColor Yellow
    Write-Host "3. Or use scoop: scoop install pandoc" -ForegroundColor Yellow
    Write-Host ""
    exit 1
}

Write-Host "✓ pandoc found: $($pandocExists.Version)" -ForegroundColor Green
Write-Host ""

# Create output directory
$outputDir = "epub_out"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "✓ Created output directory: $outputDir" -ForegroundColor Green
}

# Get all documentation files
$docFiles = Get-ChildItem -Path "MarkDownDocuments\*.md" | Sort-Object Name
Write-Host "✓ Found $($docFiles.Count) documentation files" -ForegroundColor Green

# Get all source files
$sourceFiles = Get-ChildItem -Path "src\*.cpp", "src\*.cppm" | Sort-Object Name
Write-Host "✓ Found $($sourceFiles.Count) source files" -ForegroundColor Green
Write-Host ""

# Create combined markdown with proper structure
$combinedMd = Join-Path $outputDir "ModernCppRefresherCourse_Combined.md"
$epubOutput = Join-Path $outputDir "ModernCppRefresherCourse.epub"

Write-Host "Generating combined markdown file..." -ForegroundColor Cyan
Write-Host ""

# Start with title and README
$content = @"
---
title: Modern C++ Refresher Course
author: Aung K. Soe, Claude Sonnet 4.5
date: $(Get-Date -Format 'yyyy-MM-dd')
lang: en-US
---

# Modern C++ Refresher Course

> **A comprehensive collection of Modern C++ examples covering C++11 through C++23**

"@

# Add README content
if (Test-Path "README.md") {
    $readmeContent = Get-Content "README.md" -Raw
    $content += "`n`n$readmeContent`n`n"
}

# Add all documentation files
Write-Host "Adding documentation files..." -ForegroundColor Cyan
foreach ($doc in $docFiles) {
    Write-Host "  + $($doc.Name)" -ForegroundColor Gray
    $docContent = Get-Content $doc.FullName -Raw
    $content += "`n`n# $($doc.BaseName)`n`n$docContent`n`n"
}

# Add source code section
$content += "`n`n---`n`n# Source Code Examples`n`n"
$content += "Below are all the source code examples included in this course.`n`n"

Write-Host ""
Write-Host "Adding source code files..." -ForegroundColor Cyan
foreach ($src in $sourceFiles) {
    Write-Host "  + $($src.Name)" -ForegroundColor Gray
    $srcContent = Get-Content $src.FullName -Raw
    $content += "`n`n## $($src.Name)`n`n"
    $content += "\`\`\`cpp`n$srcContent`n\`\`\``n`n"
}

# Save combined markdown
Set-Content -Path $combinedMd -Value $content -Encoding UTF8
Write-Host ""
Write-Host "✓ Combined markdown created: $combinedMd" -ForegroundColor Green
Write-Host ""

# Generate EPUB
Write-Host "Generating EPUB..." -ForegroundColor Cyan
Write-Host ""

$pandocArgs = @(
    $combinedMd,
    "-o", $epubOutput,
    "--toc",
    "--toc-depth=3",
    "--epub-cover-image=cover.png",  # Optional: add if you have a cover
    "--highlight-style=tango",
    "--metadata", "title=Modern C++ Refresher Course",
    "--metadata", "author=Aung K. Soe, Claude Sonnet 4.5",
    "--metadata", "date=$(Get-Date -Format 'yyyy-MM-dd')",
    "--metadata", "lang=en-US"
)

# Remove cover image arg if it doesn't exist
if (-not (Test-Path "cover.png")) {
    $pandocArgs = $pandocArgs | Where-Object { $_ -ne "--epub-cover-image=cover.png" }
}

& pandoc $pandocArgs

if ($LASTEXITCODE -eq 0) {
    $fileSize = (Get-Item $epubOutput).Length / 1MB
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "✓ SUCCESS!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "EPUB file generated successfully!" -ForegroundColor Green
    Write-Host "Output: $epubOutput" -ForegroundColor Cyan
    Write-Host "Size: $([math]::Round($fileSize, 2)) MB" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "EPUB Features:" -ForegroundColor Yellow
    Write-Host "  ✓ Works on Windows, Mac, Linux, Android, iOS" -ForegroundColor Green
    Write-Host "  ✓ Interactive table of contents" -ForegroundColor Green
    Write-Host "  ✓ Syntax-highlighted code" -ForegroundColor Green
    Write-Host "  ✓ Searchable content" -ForegroundColor Green
    Write-Host "  ✓ Adjustable font size" -ForegroundColor Green
    Write-Host "  ✓ Night mode support" -ForegroundColor Green
    Write-Host ""
    Write-Host "Read with:" -ForegroundColor Yellow
    Write-Host "  Windows: Calibre, Microsoft Edge" -ForegroundColor Cyan
    Write-Host "  Mac: iBooks" -ForegroundColor Cyan
    Write-Host "  Linux: Calibre, FBReader" -ForegroundColor Cyan
    Write-Host "  Android/iOS: Google Play Books, Apple Books" -ForegroundColor Cyan
    Write-Host ""
    
    # Ask if user wants to open the EPUB file
    $response = Read-Host "Would you like to open the EPUB file now? (Y/N)"
    if ($response -eq 'Y' -or $response -eq 'y') {
        Start-Process $epubOutput
    }
} else {
    Write-Host ""
    Write-Host "ERROR: EPUB generation failed!" -ForegroundColor Red
    Write-Host "Check the output above for errors." -ForegroundColor Yellow
    Write-Host ""
    exit 1
}
