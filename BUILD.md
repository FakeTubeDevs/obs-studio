# Build instructions (Windows)

This project uses CMake and Visual Studio to build.

## Prerequisites
- Visual Studio 2022 or 2026 with Desktop C++ workload
- CMake (bundled with VS)
- Windows 10 SDK (installed with VS)
- Git (optional, but recommended)

## Configure version (fixes CMake version errors)
CMake reads version from `git describe`. If the repository has no semantic version tag, configure one of these:

- Option A: Create a local tag
  1. Open PowerShell in `C:\Users\pvurm\source\repos\obs-studio`.
  2. `git tag -a 30.0.0 -m "local build"`
  3. Verify: `git describe --always --tags --dirty=-modified` (should print `30.0.0`).

- Option B: Override version in CMake
  - Pass `-DOBS_VERSION_OVERRIDE="30.0.0"` when generating. If using PowerShell, quote the value.

## One-command build script (PowerShell)
Use `build.ps1` in the repo root to automate generate + build:

- Default build (VS 2026, x64, RelWithDebInfo):
  - `./build.ps1`
- Custom generator/config/version/arch:
  - `./build.ps1 -Generator "Visual Studio 17 2022" -Configuration Release -Version "30.0.0" -Arch x64`
- The script will:
  - Ensure a semantic version (or use override)
  - Clean and recreate `build_x64`
  - Run CMake generate + build
  - Print the path of `obs64.exe` if found

## Generate and build (Visual Studio generator)
- PowerShell:
  1. `cd C:\Users\pvurm\source\repos\obs-studio`
  2. Create build dir: `mkdir ..\build_x64`
  3. Generate solution:
     - VS 2026: `cmake -S . -B ..\build_x64 -G "Visual Studio 18 2026" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo`
     - VS 2022: `cmake -S . -B ..\build_x64 -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=RelWithDebInfo`
     - If using override: add `-DOBS_VERSION_OVERRIDE="30.0.0"`
  4. Build: `cmake --build ..\build_x64 --config RelWithDebInfo --parallel`

## Where is the executable?
- Search in the build folder: `Get-ChildItem ..\build_x64 -Recurse -Filter obs64.exe`
- Typical path: `..\build_x64\frontend\RelWithDebInfo\obs64.exe` (may vary per generator/layout).

## Clean rebuild
- Delete build dir and regenerate:
  - `rmdir ..\build_x64 -Recurse -Force`
  - Repeat the generate steps above.

## Notes
- If service updates re-add other platforms, ensure `plugins/rtmp-services/CMakeLists.txt` has service updates disabled and `services.json` retains only FakeTube.
- If the generator name is not recognized, omit `-G` and let CMake choose the default installed Visual Studio.
