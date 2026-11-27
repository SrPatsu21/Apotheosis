set -e

BUILD_ROOT="${BUILD_ROOT:-/workspace}"

mkdir -p "$BUILD_ROOT/linux-release"
cd "$BUILD_ROOT/linux-release"
cmake /workspace -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
