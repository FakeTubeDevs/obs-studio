param(
  [string]$Generator = "Visual Studio 18 2026",
  [string]$Configuration = "RelWithDebInfo",
  [string]$Version = "30.0.0",
  [string]$Arch = "x64",
  [switch]$Clean = $false,
  [string[]]$Targets,
  [switch]$NoPromptRun = $false
)

$ErrorActionPreference = "Stop"

function Invoke-CMake {
  param([string]$Src, [string]$Bld)
  Write-Host "==> Generating ($Generator, $Arch)" -ForegroundColor Cyan
  cmake -S $Src -B $Bld -G $Generator -A $Arch -DOBS_VERSION_OVERRIDE="$Version" -DCMAKE_BUILD_TYPE=$Configuration
  Write-Host "==> Building $Configuration" -ForegroundColor Cyan
  if ($Targets -and $Targets.Count -gt 0) {
    Write-Host "Building targets: $($Targets -join ', ')" -ForegroundColor Cyan
    cmake --build $Bld --config $Configuration --parallel --target $Targets
  } else {
    cmake --build $Bld --config $Configuration --parallel
  }
}

# Root detection: script is expected to be placed in repository root (which contains CMakeLists.txt)
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Proj = $Root
$BuildDir = Join-Path $Root "build_$Arch"

Write-Host "Project root: $Proj" -ForegroundColor Yellow

# Ensure semantic version available via git describe or override
try {
  $desc = (git -C $Proj describe --always --tags --dirty=-modified) 2>$null
} catch { $desc = $null }

if (-not $desc -or -not ($desc -match '^[0-9]+\.[0-9]+\.[0-9]+')) {
  Write-Host "No semantic version tag found, using override: $Version" -ForegroundColor Yellow
} else {
  Write-Host "Detected version: $desc" -ForegroundColor Yellow
}

# Prepare build dir (incremental by default)
if ($Clean) {
  if (Test-Path $BuildDir) {
    Write-Host "Cleaning $BuildDir" -ForegroundColor Yellow
    try {
      Remove-Item $BuildDir -Recurse -Force -ErrorAction Stop
    } catch {
      Write-Warning "Could not fully clean '$BuildDir' (files in use). Continuing with incremental build. Details: $($_.Exception.Message)"
    }
  }
}

if (-not (Test-Path $BuildDir)) {
  New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# Generate + Build (cmake will only compile changed sources)
Invoke-CMake -Src $Proj -Bld $BuildDir

# Locate runtime exe (staged with DLLs)
Write-Host "==> Searching for obs64.exe" -ForegroundColor Cyan
$rundirExe = Join-Path $BuildDir "rundir/$Configuration/bin/64bit/obs64.exe"
if (-not (Test-Path $rundirExe)) {
  # older layout fallback
  $rundirExe = Join-Path $BuildDir "rundir/$Configuration/obs64.exe"
}

$devExe = Join-Path $BuildDir "frontend/$Configuration/obs64.exe"
$foundExe = $null

if (Test-Path $rundirExe) {
  $foundExe = (Resolve-Path $rundirExe).Path
  Write-Host "Runtime build: $foundExe" -ForegroundColor Green
} elseif (Test-Path $devExe) {
  $foundExe = (Resolve-Path $devExe).Path
  Write-Host "Developer build (may miss DLLs): $foundExe" -ForegroundColor Yellow
} else {
  $any = Get-ChildItem $BuildDir -Recurse -Filter obs64.exe -ErrorAction SilentlyContinue | Select-Object -First 1
  if ($any) {
    $foundExe = $any.FullName
    Write-Host "Found exe (unknown layout): $foundExe" -ForegroundColor Yellow
  } else {
    Write-Host "Executable not found. Check build output folders." -ForegroundColor Red
  }
}

# Prompt to run
if ($foundExe -and -not $NoPromptRun) {
  $answer = Read-Host "Run OBS now? [Y/N]"
  if ($answer -match '^(y|yes)$') {
    Write-Host "Launching: $foundExe" -ForegroundColor Cyan
    Start-Process -FilePath $foundExe
  }
}
