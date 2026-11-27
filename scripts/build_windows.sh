set -e

BUILD_ROOT="${BUILD_ROOT:-/workspace}"

mkdir -p "$BUILD_ROOT/windows"
cd "$BUILD_ROOT/windows"

cmake /workspace \
    -DCMAKE_TOOLCHAIN_FILE=/workspace/toolchain-mingw.cmake \
    -DCMAKE_BUILD_TYPE=Debug

make -j$(nproc)