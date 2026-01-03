# Automatic Documentation Updates

This document explains how documentation automatically updates when you add new C++ source files to the repository.

## 🤖 What Gets Updated Automatically

When you add new `.cpp` or `.cppm` files to the `src/` directory:

1. **README.md** ✅ **Fully Automatic**
   - Alphabetical listing automatically updated
   - File descriptions extracted from code comments
   - No manual intervention needed

2. **PDF (ModernCppRefresherCourse.pdf)** ✅ **Fully Automatic**
   - New files automatically included
   - Generated on every push to main/master/develop
   - Available as GitHub Actions artifact

3. **INDEX.md** ⚠️ **Semi-Automatic (Template Generated)**
   - Template generated for new files
   - Requires manual topic categorization
   - Script tells you exactly what to add

---

## 📝 How It Works

### Local Usage

Run the documentation updater script locally before committing:

**Linux/Mac:**
```bash
chmod +x scripts/update_docs.sh
./scripts/update_docs.sh
```

**Windows (PowerShell):**
```powershell
.\scripts\update_docs.ps1
```

**Direct Python:**
```bash
python3 scripts/update_documentation.py
```

### What the Script Does

1. **Scans** `src/` directory for all `.cpp` and `.cppm` files
2. **Extracts** descriptions from file header comments
3. **Updates** README.md alphabetical listing
4. **Detects** new files not yet in documentation
5. **Generates** INDEX.md update template for new files
6. **Reports** what needs manual attention

### GitHub Actions (Automatic)

The workflow automatically runs the updater:

```yaml
Push to main/master/develop
  ↓
Documentation Updater Script
  ├─ Updates README.md (automatic)
  ├─ Generates INDEX template (if new files)
  ├─ Commits README.md changes
  └─ Pushes back to repository
  ↓
PDF Generation
  ├─ Uses updated README.md
  ├─ Includes all source files (auto-detected)
  └─ Uploads PDF artifact
```

---

## 🆕 Adding New Source Files

### Step 1: Create Your New C++ File

```bash
# Add your new example file
vim src/MyNewExample.cpp
```

**Important:** Add a descriptive header comment:

```cpp
// ===================================================================
// MY NEW EXAMPLE - Brief Description
// ===================================================================
// More detailed description here. This will be extracted for README.md
// 
// Key features:
// - Feature 1
// - Feature 2
// 
// Build: g++ -std=c++20 MyNewExample.cpp
// ===================================================================

#include <iostream>

namespace my_namespace {
    void demonstrate() {
        std::cout << "Hello from MyNewExample!\n";
    }
}

int main() {
    my_namespace::demonstrate();
    return 0;
}
```

### Step 2: Update Documentation (Local)

```bash
# Run the updater
python3 scripts/update_documentation.py

# Output:
# ✅ README.md updated successfully
# 📋 Generating INDEX.md update template...
# ⚠️  ACTION REQUIRED: Review INDEX_UPDATE_TEMPLATE.txt
```

### Step 3: Update INDEX.md (Manual)

The script generates `INDEX_UPDATE_TEMPLATE.txt`:

```
==================================================
NEW FILES DETECTED - INDEX.md UPDATE TEMPLATE
==================================================

### MyNewExample.cpp
- **File:** [MyNewExample.cpp](src/MyNewExample.cpp)
- **Description:** Brief description
- **Topics:** [TODO: Add relevant topics/keywords]
- **Suggested Sections:**
  - [ ] C++ Standards Features (C++11/14/17/20/23)
  - [ ] Design Patterns & Idioms
  - [ ] Concurrency & Parallelism
  - [ ] Real-Time & Embedded Systems
  - [ ] Safety-Critical & Standards
  - [ ] Other: _____________
- **Key Items:** Namespaces: my_namespace, Functions: demonstrate
```

**Add to INDEX.md:**

1. Open `INDEX.md`
2. Find appropriate section (e.g., "Design Patterns & Idioms")
3. Add entry:
   ```markdown
   ### My Pattern
   - **File:** [MyNewExample.cpp](src/MyNewExample.cpp)
   - **Topics:** Pattern demonstration, best practices
   ```
4. Delete `INDEX_UPDATE_TEMPLATE.txt`

### Step 4: Add to CMakeLists.txt

```cmake
set(EXECUTABLES
    # ... existing files ...
    MyNewExample
)
```

### Step 5: Commit and Push

```bash
git add src/MyNewExample.cpp
git add README.md  # Updated automatically
git add INDEX.md   # Updated manually
git add CMakeLists.txt
git commit -m "Add MyNewExample demonstrating XYZ pattern"
git push origin main
```

### Step 6: GitHub Actions (Automatic)

1. Workflow runs on push
2. Documentation updater confirms README.md is up to date
3. PDF is generated with your new file included
4. Download from Actions → Artifacts → "ModernCppRefresherCourse-PDF"

---

## 🔍 What Gets Extracted from Your Code

The updater analyzes your source files:

### 1. File Description
```cpp
// First comment line or block becomes the description
// ===================================================================
// MY EXAMPLE - This is the description  <-- Extracted
// ===================================================================
```

### 2. Namespaces
```cpp
namespace my_example {  // <-- Detected
    // ...
}
```

### 3. Classes
```cpp
class MyClass {  // <-- Detected
    // ...
};
```

### 4. Key Functions
```cpp
void demonstrate() {  // <-- Detected
    // ...
}

int main() {  // <-- Detected
    return 0;
}
```

### Example Output in README.md

```markdown
### M

52. **[MyNewExample.cpp](src/MyNewExample.cpp)**
   - Brief description from header comment
   - Namespaces: my_example
   - Functions: demonstrate, main
```

---

## 📋 INDEX.md Update Checklist

When adding a new file, update these INDEX.md sections as appropriate:

### By Topic

- [ ] **C++ Standards Features**
  - Which C++ version? (C++11/14/17/20/23)
  - What language features demonstrated?

- [ ] **Design Patterns & Idioms**
  - CRTP, Pimpl, NVI, RAII, etc.?

- [ ] **Concurrency & Parallelism**
  - Threading, async, coroutines?

- [ ] **Real-Time & Embedded Systems**
  - Deterministic, bounded execution?

- [ ] **Safety-Critical & Standards**
  - MISRA, AUTOSAR, ISO 26262?

- [ ] **Memory Management**
  - Smart pointers, move semantics?

- [ ] **Template Metaprogramming**
  - Concepts, SFINAE, variadic templates?

- [ ] **Error Handling**
  - Exceptions, std::expected, std::optional?

### Quick Reference Tables

Update if relevant:

- [ ] **By Language Version** table
- [ ] **By Use Case** table
- [ ] **By Problem You're Trying to Solve** table

### Keywords Section

Add if your file demonstrates specific keywords:

- [ ] `auto`, `constexpr`, `concept`, `requires`, etc.

---

## 🤖 GitHub Actions Behavior

### On Every Push to Main

```
1. Documentation Updater runs
   ├─ Scans src/ for all files
   ├─ Updates README.md if needed
   ├─ Commits and pushes changes [skip ci]
   └─ Generates INDEX template if new files found

2. PDF Generation runs
   ├─ Uses updated README.md
   ├─ Includes ALL files from src/ (auto-detected)
   ├─ Generates ModernCppRefresherCourse.pdf
   └─ Uploads as artifact (90-day retention)

3. If New Files Detected
   └─ GitHub Actions warning displayed
       "::warning::New source files detected! Review INDEX_UPDATE_TEMPLATE.txt"
```

### On Tag Push (e.g., v1.0.0)

```
1-2. Same as above

3. Release Creation
   ├─ Downloads PDF artifact
   ├─ Creates GitHub Release
   ├─ Attaches PDF to release
   └─ Adds comprehensive release notes
```

---

## 🧪 Testing Locally

Before pushing, verify documentation updates:

```bash
# 1. Add your new file
vim src/MyNewFeature.cpp

# 2. Update docs
python3 scripts/update_documentation.py

# 3. Check what changed
git status
git diff README.md

# 4. Review INDEX template
cat INDEX_UPDATE_TEMPLATE.txt

# 5. Update INDEX.md manually
vim INDEX.md

# 6. Test PDF generation (optional)
./generate_pdf.sh  # or generate_pdf.ps1 on Windows

# 7. Verify everything
ls -lh pdf_output/ModernCppRefresherCourse.pdf
```

---

## 📊 Statistics

The updater provides statistics:

```
==================================================
SUMMARY
==================================================
✅ Total source files: 73
✅ README.md: Updated
⚠️  INDEX.md: Manual update needed for 1 new file(s)
   See: INDEX_UPDATE_TEMPLATE.txt
✅ PDF: Will be automatically generated by GitHub Actions
==================================================
```

---

## 🐛 Troubleshooting

### Issue: "No module named 'X'"

**Solution:** The Python script uses only standard library, no external dependencies needed.

### Issue: README.md not updating

**Possible causes:**
1. Section markers moved/changed
   - **Fix:** Restore markers in README.md:
     ```markdown
     ## 📋 **Complete Examples - Alphabetical Reference**
     ...
     ## 🏗️ **Project Structure**
     ```

2. File not in `src/` directory
   - **Fix:** Move `.cpp`/`.cppm` files to `src/`

### Issue: Description not extracted

**Solution:** Add a header comment to your file:
```cpp
// Brief description here
// Or use a block comment
/*
 * Brief description here
 */
```

### Issue: INDEX template overwhelming

**Solution:** You don't need to add every file to every section. Choose the most relevant 2-3 sections.

---

## 🎯 Best Practices

### 1. Write Good Header Comments
```cpp
// ✅ GOOD: Clear, concise description
// Demonstrates CRTP pattern for compile-time polymorphism

// ❌ BAD: No description
// File: example.cpp
```

### 2. Use Descriptive Filenames
```
✅ GOOD: CRTPvsVirtualFunctions.cpp
❌ BAD: example1.cpp
```

### 3. Organize by Topic in INDEX.md
```markdown
✅ GOOD: Add to specific topic sections
❌ BAD: Only add to "Other" or generic sections
```

### 4. Keep README.md Descriptions Brief
- Auto-extracted from first comment line
- Keep first line under 100 characters
- More details go in the file itself

### 5. Run Updater Before Each Commit
```bash
# Add to your git pre-commit hook
python3 scripts/update_documentation.py
```

---

## 📞 Getting Help

If the auto-updater fails:

1. **Check the script output** for specific errors
2. **Verify file locations** (src/, README.md, INDEX.md exist)
3. **Review section markers** in README.md
4. **Open an issue** with the error message

---

## 🔮 Future Enhancements

Planned improvements:

- [ ] Automatic INDEX.md updates using ML/NLP
- [ ] Category suggestion based on code analysis
- [ ] Automatic keyword extraction
- [ ] Duplicate detection
- [ ] Link validation
- [ ] Statistics dashboard

---

**Last Updated:** January 3, 2026  
**Version:** 1.0.0
