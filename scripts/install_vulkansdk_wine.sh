#!/usr/bin/env bash
set -e

# ================================
# Resolve script location
# ================================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-windows"

# ================================
# Vulkan SDK config
# ================================
SDK_VERSION="1.4.304.1"
SDK_EXE="VulkanSDK-${SDK_VERSION}-Installer.exe"
SDK_URL="https://sdk.lunarg.com/sdk/download/${SDK_VERSION}/windows/${SDK_EXE}"

WINEPREFIX="$HOME/.wine-vulkan"
ENV_SCRIPT="$BUILD_DIR/wine-vulkan-env.sh"

# ================================
# Prepare build dir
# ================================
mkdir -p "$BUILD_DIR"

# ================================
# Sanity checks
# ================================
command -v wine >/dev/null || { echo "wine not installed"; exit 1; }
command -v wget >/dev/null || { echo "wget not installed"; exit 1; }

# ================================
# Init wine prefix
# ================================
export WINEPREFIX
wineboot --init

# ================================
# Download SDK
# ================================
cd "$PROJECT_ROOT"
if [ ! -f "$SDK_EXE" ]; then
    echo "Downloading Vulkan SDK $SDK_VERSION"
    wget -O "$SDK_EXE" "$SDK_URL"
fi

# ================================
# Install SDK
# ================================
wine "$SDK_EXE"

SDK_PATH="$WINEPREFIX/drive_c/VulkanSDK/${SDK_VERSION}"
if [ ! -d "$SDK_PATH" ]; then
    echo "Vulkan SDK install not found!"
    exit 1
fi

# ================================
# Generate env script
# ================================
cat > "$ENV_SCRIPT" <<EOF
#!/usr/bin/env bash
export WINEPREFIX="$WINEPREFIX"
export VK_LAYER_PATH="\
