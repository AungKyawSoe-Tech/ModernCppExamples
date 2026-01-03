# PowerShell script to update documentation when new files are added
# Wrapper for the Python documentation updater

Write-Host "Running documentation updater..." -ForegroundColor Cyan

# Check if Python is available
$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    $python = Get-Command python3 -ErrorAction SilentlyContinue
}

if (-not $python) {
    Write-Host "Error: Python is required but not installed" -ForegroundColor Red
    exit 1
}

# Run the Python script
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
& $python.Source "$scriptDir\update_documentation.py"

if ($LASTEXITCODE -eq 0) {
    Write-Host "Documentation update complete!" -ForegroundColor Green
} else {
    Write-Host "Documentation update failed!" -ForegroundColor Red
    exit $LASTEXITCODE
}
