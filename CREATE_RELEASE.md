# How to Create a Release with PDF

## Quick Instructions

To create a GitHub release with the PDF, run these commands:

```bash
# Create and push a version tag
git tag -a v1.0.0 -m "Release v1.0.0 - Modern C++ Examples"
git push origin v1.0.0
```

Or in PowerShell:
```powershell
git tag -a v1.0.0 -m "Release v1.0.0 - Modern C++ Examples"
git push origin v1.0.0
```

## What Happens Next

1. GitHub Actions will automatically:
   - Build and test all examples
   - Generate the PDF
   - Create a GitHub Release at: https://github.com/AungKyawSoe-Tech/ModernCppExamples/releases
   - Attach the PDF to the release

2. The PDF will be publicly available for download from the Releases page

## Alternative: Download from Artifacts

If you don't want to create a release yet, you can download the PDF from the workflow artifacts:
1. Go to: https://github.com/AungKyawSoe-Tech/ModernCppExamples/actions
2. Click on your workflow run
3. Scroll to "Artifacts" section at the bottom
4. Download `ModernCppRefresherCourse-PDF`

## Version Numbering

Follow semantic versioning for tags:
- `v1.0.0` - Major release
- `v1.1.0` - Minor update (new features)
- `v1.0.1` - Patch (bug fixes)
