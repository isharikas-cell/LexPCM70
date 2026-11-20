$ErrorActionPreference = "Stop"
$source = Join-Path $PSScriptRoot "build\PCM70Legacy_artefacts\Release\VST3\PCM70 Legacy.vst3"
$destinationRoot = Join-Path $env:CommonProgramFiles "VST3"
$destination = Join-Path $destinationRoot "PCM70 Legacy.vst3"

if (-not (Test-Path $source)) {
    throw "VST3 bundle not found: $source`nRun build_windows.ps1 first."
}

New-Item -ItemType Directory -Force -Path $destinationRoot | Out-Null
if (Test-Path $destination) {
    $backup = "$destination.backup-$(Get-Date -Format yyyyMMdd-HHmmss)"
    Move-Item $destination $backup
    Write-Host "Previous version backed up to: $backup"
}
Copy-Item -Recurse -Force $source $destination
Write-Host "Installed PCM70 Legacy to: $destination"
Write-Host "Rescan VST3 plug-ins in your DAW."
