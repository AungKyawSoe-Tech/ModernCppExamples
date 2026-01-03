#!/bin/bash
# Wrapper script to update documentation when new files are added
# Runs the Python documentation updater

set -e

echo "Running documentation updater..."

# Check if Python is available
if ! command -v python3 &> /dev/null; then
    echo "Error: python3 is required but not installed"
    exit 1
fi

# Run the Python script
python3 "$(dirname "$0")/update_documentation.py"

echo "Documentation update complete!"
