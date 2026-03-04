param(
  [string]$Mcu = "stm8s103f3",
  [string]$Ihx = "build/router_ups.ihx"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path $Ihx)) {
  throw "IHX file not found: $Ihx. Run scripts_build.ps1 first."
}

if (-not (Get-Command stm8flash -ErrorAction SilentlyContinue)) {
  throw "stm8flash not found in PATH. Install stm8flash first."
}

& stm8flash -c stlinkv2 -p $Mcu -w $Ihx
if ($LASTEXITCODE -ne 0) {
  throw "Flash failed"
}

Write-Host "Flash OK: $Ihx"
