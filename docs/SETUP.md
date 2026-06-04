# SETUP

## VScode Extensions

### Needed

- Dev Containers \
    <https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers>
- Draw.io Integration
    <https://marketplace.visualstudio.com/items?itemName=hediet.vscode-drawio>

### Excential

- C/C++ Extension Pack \
    <https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack>
- Better C++ Syntax \
    <https://marketplace.visualstudio.com/items?itemName=jeff-hykin.better-cpp-syntax>
- markdownlint \
    <https://marketplace.visualstudio.com/items?itemName=DavidAnson.vscode-markdownlint>
- Better Comments \
    <https://marketplace.visualstudio.com/items?itemName=aaron-bond.better-comments>
- Trailing Spaces \
    <https://marketplace.visualstudio.com/items?itemName=shardulm94.trailing-spaces>
- Doxygen
    <https://marketplace.visualstudio.com/items?itemName=bbenoist.Doxygen>

### Highly Recommend

- Change Case \
    <https://marketplace.visualstudio.com/items?itemName=FinnTenzor.change-case>
- Easy CodeSnap \
    <https://marketplace.visualstudio.com/items?itemName=ArthurLobo.easy-codesnap>
- GitLens — Git supercharged \
    <https://marketplace.visualstudio.com/items?itemName=eamodio.gitlens>
- Better Readability Extension Pack \
    <https://marketplace.visualstudio.com/items?itemName=pinage404.better-readability-extension-pack>
- file-icons \
    <https://marketplace.visualstudio.com/items?itemName=file-icons.file-icons>

### Theme (recommendation)

- C/C++ Theme
    <https://marketplace.visualstudio.com/items?itemName=Xen.ccpp-theme>
- Dracula At Night \
    <https://marketplace.visualstudio.com/items?itemName=bceskavich.theme-dracula-at-night>
- GitHub Theme(any dark) \
    <https://marketplace.visualstudio.com/items?itemName=GitHub.github-vscode-theme>

## Docker

### Copy lib

- Linux

    ```shell
    set -e

    IMAGE="srpatsu21/dear-glfw-vulkan-compiler:1.3.0"
    DEST="."

    mkdir -p "$DEST"

    docker run --rm \
        -v "$(pwd)/$DEST":/copy-dest \
        "$IMAGE" \
        bash -c "
            cp -r /workspace/lib /copy-dest || true
        "

    echo "files from container copied to $DEST"
    sudo chmod 755 -R .
    ```

- Windows

    ```shell
    $ErrorActionPreference = "Stop"

    $image = "srpatsu21/dear-glfw-vulkan-compiler:1.3.0"
    $dest = "."

    if (-not (Test-Path $dest)) {
        New-Item -ItemType Directory -Path $dest | Out-Null
    }

    docker run --rm -v "${PWD}\$dest:/copy-dest" $image `
        bash -c "
            cp -r /workspace/lib /copy-dest || true
        "

    Write-Host "Files from container copied to $dest"
    ```

### Init container

```shell
docker compose up -d
```

### Connect to the container

<https://youtu.be/8gUtN5j4QnY>

### Compile

Use the scripts container terminal

```shell
./scripts/build_windows.sh
```

### To stop container

```shell
docker compose stop
```

### Run with wine (Linux)

- Script to install (not working)

```shell
sudo apt install winbind wine
chmod +x scripts/install_vulkansdk_wine.sh
./scripts/install_vulkansdk_wine.sh
```

- Run

```shell
cd build-windows
source ./wine-vulkan-env.sh
wine Apotheosis_client.exe
```

## Textures and Models

### Textures

Only acept __.ktx2__; download [here on GitHub](https://github.com/KhronosGroup/KTX-Software).

#### To compile

- BaseColor (sRGB):

    ```shell
    toktx --t2 --genmipmap --encode uastc --uastc_quality 2 --zcmp --assign_oetf srgb basecolorOut.ktx2 basecolorInput.png
    ```

- Normal map (linear):

    ```shell
    toktx --t2 --genmipmap --encode uastc --uastc_quality 2 --zcmp --normal_mode normalOut.ktx2 normalInput.png
    ```

- Metallic/Roughness (linear):

    ```shell
    toktx --t2 --genmipmap --encode uastc --uastc_quality 2 --zcmp mrOut.ktx2 mrInput.png
    ```

### Models

In Blender or any other graphics software tool you prefer, export as __.glb__. The code does not accept other formats.
