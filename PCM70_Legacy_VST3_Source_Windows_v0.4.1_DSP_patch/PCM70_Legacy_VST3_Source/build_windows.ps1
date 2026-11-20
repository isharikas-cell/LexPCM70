$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root "build"

Write-Host "PCM70 Legacy 0.4.0 - Windows x64 VST3 build"
Write-Host "Requires Visual Studio 2022 with Desktop development with C++, CMake and Git."

cmake -S $root -B $build -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed." }
cmake --build $build --config Release --target PCM70Legacy_VST3 --parallel
if ($LASTEXITCODE -ne 0) { throw "VST3 build failed." }

$bundle = Join-Path $build "PCM70Legacy_artefacts\Release\VST3\PCM70 Legacy.vst3"
if (-not (Test-Path $bundle)) { throw "Build finished but VST3 bundle was not found: $bundle" }
Write-Host "VST3 bundle created: $bundle"
Write-Host "To install (run PowerShell as Administrator): .\install_windows.ps1"
