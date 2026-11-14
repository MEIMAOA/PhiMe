param()
$ErrorActionPreference = "Stop"
$llvm = "C:\Program Files\LLVM\bin"
$root = $PSScriptRoot
$src = Join-Path $root "src"
$libs = Join-Path $root "libs"
$out = Join-Path $root "out\android"
New-Item -ItemType Directory -Force $out | Out-Null

$cpp = (Get-ChildItem $src -Filter "*.cpp" | Sort-Object Name | ForEach-Object { $_.FullName })
if (-not $cpp) { throw "no src\*.cpp" }

# Stub .so files (empty) exist only so the linker records the DT_NEEDED entries for the
# system libs; the real symbols are resolved at dlopen time from the process's own libs.
$stubNames = @("libc.so", "libdl.so", "libm.so", "liblog.so")
foreach ($n in $stubNames) {
    $p = Join-Path $libs $n
    if (-not (Test-Path $p)) {
        $tmp = Join-Path $libs "__stub.c"
        Set-Content -Path $tmp -Value '' -Encoding ASCII
        & "$llvm\clang.exe" '--target=aarch64-linux-android26' -shared -nostdlib -fPIC -o $p $tmp
        if ($LASTEXITCODE -ne 0) { throw "stub build failed: $n" }
        Remove-Item $tmp -Force
    }
}

$target = "aarch64-linux-android26"
$common = @(
    "--target=$target",
    "-std=c++17",
    "-ffreestanding",
    "-fno-exceptions",
    "-fno-rtti",
    "-fno-unwind-tables",
    "-fno-threadsafe-statics",
    "-fno-use-cxa-atexit",
    "-fvisibility=hidden",
    "-fPIC",
    "-O2",
    "-Wno-unused-value",
    "-Wno-unused-variable",
    "-Wno-unused-function"
)

$so = Join-Path $out "libPhiMeMod.so"
& "$llvm\clang++.exe" @common "-shared" "-nostdlib" "-fuse-ld=lld" "-Wl,-z,now" "-Wl,-soname,libPhiMeMod.so" @cpp "-L$libs" "-Wl,--no-as-needed" "-ldl" "-llog" "-lm" "-lc" "-o" $so
if ($LASTEXITCODE -ne 0) { throw "mod build failed" }

& "$llvm\llvm-strip.exe" --strip-unneeded $so
Write-Host "built $so"
