set -e

BUILD_ROOT="${BUILD_ROOT:-/workspace}"

mkdir -p "$BUILD_ROOT/linux"
cd "$BUILD_ROOT/linux"
cmake /workspace
make -j$(nproc)
