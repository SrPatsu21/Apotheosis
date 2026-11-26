set -e

BUILD_ROOT="${BUILD_ROOT:-/workspace}"

mkdir -p "$BUILD_ROOT/windows-release"
cd "$BUILD_ROOT/windows-release"
cmake /workspace -D CMAKE_TOOLCHAIN_FILE=/workspace/toolchain-mingw.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)