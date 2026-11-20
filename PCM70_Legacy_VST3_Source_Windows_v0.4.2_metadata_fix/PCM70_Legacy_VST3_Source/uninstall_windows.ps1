$ErrorActionPreference = "Stop"
$destination = Join-Path (Join-Path $env:CommonProgramFiles "VST3") "PCM70 Legacy.vst3"
if (Test-Path $destination) {
    Remove-Item -Recurse -Force $destination
    Write-Host "Removed: $destination"
} else {
    Write-Host "PCM70 Legacy is not installed in the system VST3 folder."
}
