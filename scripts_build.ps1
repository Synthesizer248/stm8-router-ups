param(
  [string]$Project = "router_ups",
  [string]$Source = "src/main.c"
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command sdcc -ErrorAction SilentlyContinue)) {
  throw "sdcc not found in PATH. Install SDCC first."
}

New-Item -ItemType Directory -Force build | Out-Null

$ihx = "build/$Project.ihx"

& sdcc -mstm8 --std-c11 --opt-code-size --out-fmt-ihx -Iinc -o $ihx $Source
if ($LASTEXITCODE -ne 0) {
  throw "Build failed"
}

Write-Host "Build OK: $ihx"
