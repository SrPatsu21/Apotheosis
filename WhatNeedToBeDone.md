# What Still Needs To Be Done

## 0. Set the license ✅

[LICENSE](./LICENSE)

## 1. Define the main idea ❎

### Now

- Roguelike/Soulslike
- 3D
- Rebirth
- procedural map
- random mobs
- do not lose items
- some action to level up, sleep, eat, rest
- punishment for death is losing xp and maybe more
- Every time you update, you can choose an attribute, attributes must be very generic

### Future

- after death, undead debuffs, something like that
- proficiency with each type of weapon gives different abilities, but the xp level is the same
- DNA system

## 2. Create an UML class diagram, using any toll. (Probably StarUML, but it can change) ❎

<!-- TODO -->
[UML](./uml/)

## 3. Define the libs and set how to use them ❎

### libraries

- Vulkan
  high-performance, modern GPU API. [Introduction](https://vulkan-tutorial.com/Introduction)
- DEARImGUI
  fast debug/dev UI (optional, good for tools/debug overlays). [Introduction](https://github.com/ocornut/imgui/?tab=readme-ov-file#dear-imgui)
- GLFW
  cross-platform window and input handling. [Introduction](https://www.glfw.org/docs/latest/quick.html)
- GLM
  OpenGL-style math library: matrices, vectors.
  Use GLM to build model, view, projection matrices.
- glslangValidator
  Compile GLSL to SPIR-V(Shaders) offline or runtime.
- Assimp
  Mesh Loading
- stb_image
  Help load PNG/JPG/TGA, etc. image files.

## 4. Set the cmake

[CMAKE](./CMakeLists.txt)

## 5. config gitignore

[gitignore](./.gitignore)

## 6. Define a name

<!-- TODO -->

## 7. Make a Prototype

## 8. Input assembly

<!-- TODO -->
implement VK_PRIMITIVE_TOPOLOGY_POINT_LIST and VK_PRIMITIVE_TOPOLOGY_LINE_LIST too.
fix transparenc with Two-pass (back faces first, then front faces)

## Sources used

- [Emojipedia ✅](https://emojipedia.org/check-mark-button) \
[Emojipedia ❎](https://emojipedia.org/cross-mark-button) \
[Emojipedia 👍](https://emojipedia.org/thumbs-up) \
[Emojipedia 👎](https://emojipedia.org/thumbs-down)
[understanding the concepts, Swapchain, Render Pass, Pipeline, Shader, and Buffers](https://itch.io/t/5019971/engine-dev-log-9-swapchain-render-pass-pipeline-shader-and-buffers)