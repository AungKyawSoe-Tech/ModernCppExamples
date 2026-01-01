# Security Policy and IP Compliance

## Static Analysis and Security Scanning

This project uses multiple tools to ensure code quality, security, and IP compliance:

### 1. Static Analysis Tools

#### Clang-Tidy
- **Purpose**: Modern C++ code analysis
- **Checks**: Bug-prone patterns, CERT secure coding, C++ Core Guidelines
- **Configuration**: `.clang-tidy`
- **Run locally**: See `scripts/run-linters.sh`

#### Cppcheck
- **Purpose**: Portable C/C++ static analyzer
- **Checks**: Memory leaks, buffer overflows, unused variables
- **Configuration**: `.cppcheck-suppressions.txt`
- **Run locally**: `cppcheck --enable=all --std=c++20 src/`

### 2. Software Composition Analysis (SCA)

#### What We Check
- ✅ Third-party dependencies and licenses
- ✅ Open source license compliance
- ✅ GPL/AGPL contamination detection
- ✅ Known security vulnerabilities
- ✅ Hardcoded secrets scanning

#### Black Duck Integration
For enterprise users, this project can integrate with Black Duck (Synopsys):

```yaml
# Add to .github/workflows/static-analysis.yml
- name: Black Duck Scan
  uses: synopsys-sig/detect-action@v0.3.0
  with:
    blackduck-url: ${{ secrets.BLACKDUCK_URL }}
    blackduck-api-token: ${{ secrets.BLACKDUCK_API_TOKEN }}
    fail-on-policy-violation: true
```

**Requirements**:
- Black Duck Hub instance
- API token with scanning permissions
- Configure secrets in GitHub repository settings

#### Alternative: WhiteSource (Mend.io)
```yaml
- name: WhiteSource Scan
  uses: whitesource/whitesource-bolt-github-action@v1
  with:
    wssUrl: https://saas.whitesourcesoftware.com/agent
    apiKey: ${{ secrets.WSS_API_KEY }}
    productName: 'ModernCppExamples'
```

### 3. Current Dependencies

| Dependency | Version | License | IP Risk | Security Status |
|------------|---------|---------|---------|-----------------|
| Eigen3 | 3.3.9+ | MPL2 | LOW | ✅ Active |
| pybind11 | 2.10+ | BSD-style | LOW | ✅ Active |
| C++ Standard Library | Varies | Compiler-specific | NONE | ✅ Standard |

### 4. License Compliance

#### Permitted Licenses
- ✅ MIT
- ✅ BSD (2-clause, 3-clause)
- ✅ Apache 2.0
- ✅ MPL 2.0 (Mozilla)
- ✅ Boost Software License

#### Restricted Licenses
- ⚠️ LGPL (requires dynamic linking)
- ❌ GPL v2/v3 (copyleft, requires source disclosure)
- ❌ AGPL (network copyleft)

### 5. Security Vulnerability Scanning

#### Automated Scans
- **Frequency**: Weekly (Sundays)
- **Triggers**: Push to main/develop, Pull requests
- **Coverage**: Dependencies, source code patterns

#### Manual Review Required For
- New third-party dependencies
- Binary libraries
- External APIs or services
- Code from external contributors

### 6. IP Compliance Checklist

Before adding new code or dependencies:

- [ ] Verify license compatibility
- [ ] Check for GPL/AGPL contamination
- [ ] Review for hardcoded credentials
- [ ] Scan for vulnerable patterns
- [ ] Update SBOM (Software Bill of Materials)
- [ ] Document in dependency list

### 7. Reporting Security Issues

If you discover a security vulnerability:

1. **DO NOT** open a public issue
2. Email: [security contact - add your email]
3. Include:
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if available)

### 8. Running Security Scans Locally

#### Quick Scan
```bash
# Run all linters
./scripts/run-linters.sh all

# Run specific tool
./scripts/run-linters.sh clang-tidy
./scripts/run-linters.sh cppcheck
```

#### Full Security Audit
```bash
# 1. Static analysis
./scripts/run-linters.sh all

# 2. Check for secrets
git secrets --scan

# 3. Dependency check
grep -r "#include <" src/ | sort -u

# 4. License verification
grep -r "license\|copyright" src/ -i
```

### 9. CI/CD Integration

Our GitHub Actions workflows automatically run:
- ✅ Clang-Tidy on every push
- ✅ Cppcheck on every push
- ✅ Dependency analysis on every push
- ✅ Weekly security scans
- ✅ SBOM generation

See: `.github/workflows/static-analysis.yml`

### 10. Third-Party Scanning Services

#### Recommended Services (with Free Tiers)
1. **Snyk** - https://snyk.io
   - Free for open source
   - Scans dependencies and containers
   
2. **FOSSA** - https://fossa.com
   - License compliance
   - Dependency tracking
   
3. **WhiteSource Bolt** - https://www.whitesourcesoftware.com/free-developer-tools/bolt
   - Free for GitHub
   - Vulnerability detection

4. **Black Duck** - https://www.synopsys.com/software-integrity/security-testing/software-composition-analysis.html
   - Enterprise solution
   - Comprehensive SCA

### 11. Compliance Status

**Last Updated**: [Date]

| Category | Status | Last Scan |
|----------|--------|-----------|
| License Compliance | ✅ PASS | [Date] |
| Security Vulnerabilities | ✅ NONE | [Date] |
| Code Quality | ✅ PASS | [Date] |
| GPL Contamination | ✅ CLEAN | [Date] |

### 12. Contact

For security or compliance questions:
- Security Email: [Add email]
- GitHub Issues: https://github.com/[repo]/issues (non-security only)
