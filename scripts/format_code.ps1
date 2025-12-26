$extensions = @("*.cpp", "*.h", "*.hpp", "*.c", "*.cc")
$srcPath = Join-Path $PSScriptRoot "..\src"
$testsPath = Join-Path $PSScriptRoot "..\tests"

# Check if clang-format is available
$clangFormat = "clang-format"
if (-not (Get-Command $clangFormat -ErrorAction SilentlyContinue)) {
    $commonPaths = @(
        "C:\Program Files\LLVM\bin\clang-format.exe",
        "C:\Program Files (x86)\LLVM\bin\clang-format.exe"
    )
    
    $found = $false
    foreach ($path in $commonPaths) {
        if (Test-Path $path) {
            $clangFormat = "& '$path'"
            $found = $true
            break
        }
    }

    if (-not $found) {
        Write-Error "clang-format not found. Please install it (e.g. 'winget install LLVM.LLVM') or add it to your PATH."
        exit 1
    }
}

function Format-Directory($path) {
    if (Test-Path $path) {
        Write-Host "Formatting files in: $path"
        Get-ChildItem -Path $path -Include $extensions -Recurse | ForEach-Object {
            Write-Host "Formatting: $($_.Name)"
            if ($clangFormat -match "^&") {
                Invoke-Expression "$clangFormat -i '$($_.FullName)'"
            } else {
                & $clangFormat -i $_.FullName
            }
        }
    } else {
        Write-Warning "Directory not found: $path"
    }
}

Format-Directory $srcPath
Format-Directory $testsPath

Write-Host "Done."
