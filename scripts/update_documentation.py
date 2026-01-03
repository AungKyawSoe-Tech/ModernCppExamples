#!/usr/bin/env python3
"""
Automatic Documentation Updater for Modern C++ Examples
Updates README.md alphabetical listing when new source files are added
"""

import os
import re
import sys
from pathlib import Path
from typing import List, Tuple, Dict

def extract_file_description(filepath: Path) -> Tuple[str, List[str]]:
    """
    Extract description and key functions from a C++ source file.
    Reads the header comment block to get description.
    """
    description = ""
    key_items = []
    
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            
            # Extract first comment block (description)
            comment_match = re.search(r'/\*\*?(.*?)\*/', content, re.DOTALL)
            if comment_match:
                comment_text = comment_match.group(1)
                lines = [line.strip().lstrip('*').strip() for line in comment_text.split('\n')]
                lines = [line for line in lines if line and not line.startswith('=')]
                if lines:
                    description = lines[0][:100]  # First line, max 100 chars
            
            # Try to extract from single-line comments if no block comment
            if not description:
                single_line_match = re.search(r'^//\s*(.+?)(?:\n|$)', content, re.MULTILINE)
                if single_line_match:
                    description = single_line_match.group(1)[:100]
            
            # Extract key functions/classes (look for main patterns)
            namespace_matches = re.findall(r'namespace\s+(\w+)', content)
            class_matches = re.findall(r'class\s+(\w+)(?:\s*[:{])', content)
            function_matches = re.findall(r'^(?:void|int|auto|bool)\s+(\w+)\s*\([^)]*\)', content, re.MULTILINE)
            
            if namespace_matches:
                key_items.append(f"Namespaces: {', '.join(set(namespace_matches[:3]))}")
            if class_matches:
                key_items.append(f"Classes: {', '.join(set(class_matches[:3]))}")
            if function_matches:
                key_items.append(f"Functions: {', '.join(set(function_matches[:3]))}")
                
    except Exception as e:
        print(f"Warning: Could not read {filepath}: {e}", file=sys.stderr)
        description = "C++ example file"
    
    if not description:
        description = "Modern C++ example demonstrating various features"
    
    return description, key_items

def get_all_source_files(src_dir: Path) -> List[Tuple[str, Path]]:
    """Get all .cpp and .cppm files from src directory, sorted alphabetically."""
    files = []
    
    for ext in ['*.cpp', '*.cppm']:
        files.extend(src_dir.glob(ext))
    
    # Sort by filename
    files.sort(key=lambda p: p.name.lower())
    
    return [(f.name, f) for f in files]

def generate_alphabetical_section(src_dir: Path) -> str:
    """Generate the alphabetical listing section for README.md"""
    
    files = get_all_source_files(src_dir)
    
    output = []
    output.append("## 📋 **Complete Examples - Alphabetical Reference**\n")
    output.append("Below is a complete alphabetical listing of all C++ example files in this repository. ")
    output.append("Each entry links to the source file with a brief description of its contents.\n")
    
    current_letter = None
    file_number = 1
    
    for filename, filepath in files:
        first_letter = filename[0].upper()
        
        # Add section header for new letter
        if first_letter != current_letter:
            if current_letter is not None:
                output.append("")  # Blank line between sections
            output.append(f"### {first_letter}\n")
            current_letter = first_letter
        
        # Extract description
        description, key_items = extract_file_description(filepath)
        
        # Generate entry
        rel_path = f"src/{filename}"
        output.append(f"{file_number}. **[{filename}]({rel_path})**")
        output.append(f"   - {description}")
        
        if key_items:
            for item in key_items[:2]:  # Max 2 key items
                output.append(f"   - {item}")
        
        output.append("")
        file_number += 1
    
    return "\n".join(output)

def update_readme(readme_path: Path, src_dir: Path) -> bool:
    """Update the alphabetical section in README.md"""
    
    try:
        with open(readme_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # Find the alphabetical section markers
        start_marker = "## 📋 **Complete Examples - Alphabetical Reference**"
        end_marker = "## 🏗️ **Project Structure**"
        
        start_idx = content.find(start_marker)
        end_idx = content.find(end_marker)
        
        if start_idx == -1 or end_idx == -1:
            print("Error: Could not find section markers in README.md", file=sys.stderr)
            return False
        
        # Generate new alphabetical section
        new_section = generate_alphabetical_section(src_dir)
        
        # Replace the section
        new_content = (
            content[:start_idx] +
            new_section +
            "\n---\n\n" +
            content[end_idx:]
        )
        
        # Write back
        with open(readme_path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        
        print(f"✅ Updated {readme_path}")
        return True
        
    except Exception as e:
        print(f"❌ Error updating README.md: {e}", file=sys.stderr)
        return False

def check_new_files(src_dir: Path, readme_path: Path) -> List[str]:
    """Check if there are new files not listed in README.md"""
    
    # Get all source files
    source_files = {name for name, _ in get_all_source_files(src_dir)}
    
    # Get files mentioned in README
    try:
        with open(readme_path, 'r', encoding='utf-8') as f:
            readme_content = f.read()
        
        readme_files = set(re.findall(r'\[(\w+\.(?:cpp|cppm))\]', readme_content))
        
        # Find new files
        new_files = source_files - readme_files
        
        return sorted(new_files)
        
    except Exception as e:
        print(f"Warning: Could not check for new files: {e}", file=sys.stderr)
        return []

def generate_index_template(new_files: List[str], src_dir: Path) -> str:
    """Generate a template for INDEX.md updates"""
    
    if not new_files:
        return ""
    
    output = []
    output.append("\n" + "="*70)
    output.append("NEW FILES DETECTED - INDEX.md UPDATE TEMPLATE")
    output.append("="*70)
    output.append("\nAdd these files to appropriate sections in INDEX.md:\n")
    
    for filename in new_files:
        filepath = src_dir / filename
        description, key_items = extract_file_description(filepath)
        
        output.append(f"\n### {filename}")
        output.append(f"- **File:** [{filename}](src/{filename})")
        output.append(f"- **Description:** {description}")
        output.append(f"- **Topics:** [TODO: Add relevant topics/keywords]")
        output.append(f"- **Suggested Sections:**")
        output.append(f"  - [ ] C++ Standards Features (C++11/14/17/20/23)")
        output.append(f"  - [ ] Design Patterns & Idioms")
        output.append(f"  - [ ] Concurrency & Parallelism")
        output.append(f"  - [ ] Real-Time & Embedded Systems")
        output.append(f"  - [ ] Safety-Critical & Standards")
        output.append(f"  - [ ] Other: _____________")
        
        if key_items:
            output.append(f"- **Key Items:** {', '.join(key_items)}")
    
    output.append("\n" + "="*70)
    output.append("Copy relevant sections above into INDEX.md")
    output.append("="*70 + "\n")
    
    return "\n".join(output)

def main():
    # Find project root
    script_dir = Path(__file__).parent
    project_root = script_dir.parent
    src_dir = project_root / "src"
    readme_path = project_root / "README.md"
    index_path = project_root / "INDEX.md"
    
    print("\n" + "="*70)
    print("Modern C++ Examples - Documentation Updater")
    print("="*70 + "\n")
    
    # Check if paths exist
    if not src_dir.exists():
        print(f"❌ Error: src directory not found at {src_dir}", file=sys.stderr)
        return 1
    
    if not readme_path.exists():
        print(f"❌ Error: README.md not found at {readme_path}", file=sys.stderr)
        return 1
    
    # Check for new files
    new_files = check_new_files(src_dir, readme_path)
    
    if new_files:
        print(f"🔍 Found {len(new_files)} new file(s):")
        for f in new_files:
            print(f"   - {f}")
        print()
    else:
        print("ℹ️  No new files detected\n")
    
    # Update README.md
    print("📝 Updating README.md alphabetical listing...")
    if update_readme(readme_path, src_dir):
        print("✅ README.md updated successfully\n")
    else:
        print("❌ Failed to update README.md\n")
        return 1
    
    # Generate INDEX.md template if there are new files
    if new_files:
        print("📋 Generating INDEX.md update template...")
        template = generate_index_template(new_files, src_dir)
        
        template_file = project_root / "INDEX_UPDATE_TEMPLATE.txt"
        with open(template_file, 'w', encoding='utf-8') as f:
            f.write(template)
        
        print(template)
        print(f"✅ Template saved to: {template_file}")
        print("\n⚠️  ACTION REQUIRED:")
        print("   1. Review the template above")
        print("   2. Add new files to appropriate sections in INDEX.md")
        print("   3. Delete INDEX_UPDATE_TEMPLATE.txt when done\n")
    
    # Summary
    print("="*70)
    print("SUMMARY")
    print("="*70)
    
    all_files = get_all_source_files(src_dir)
    print(f"✅ Total source files: {len(all_files)}")
    print(f"✅ README.md: Updated")
    
    if new_files:
        print(f"⚠️  INDEX.md: Manual update needed for {len(new_files)} new file(s)")
        print(f"   See: INDEX_UPDATE_TEMPLATE.txt")
    else:
        print(f"✅ INDEX.md: No updates needed")
    
    print(f"✅ PDF: Will be automatically generated by GitHub Actions")
    print("="*70 + "\n")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
