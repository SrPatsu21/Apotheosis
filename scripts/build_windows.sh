set -e

BUILD_ROOT="${BUILD_ROOT:-/workspace}"

mkdir -p "$BUILD_ROOT/windows"
cd "$BUILD_ROOT/windows"
cmake /workspace -D CMAKE_TOOLCHAIN_FILE=/workspace/toolchain-mingw.cmake
make -j$(nproc)