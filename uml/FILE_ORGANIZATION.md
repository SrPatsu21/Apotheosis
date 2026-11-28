# ProjectD Organization

Orientation guide: what goes in each folder and what must **not** go there.
Logic and organization only, no code.

## Dependency rules

```text
common  <--  client
common  <--  server
```

- `common` **never** depends on Vulkan, windowing, UI or the GPU.
- `common` may depend on: glm, Assimp, a networking/serialization library.
- `client` and `server` never depend on each other, only on `common`.
- Client and server may run separately, together, or the client may connect
  to a remote server. Nothing in `common` may assume which case applies.

---

## `common/`

### `common/geometry/`
Pure geometric types.
- AABB, Ray, Triangle, Transform
- MeshData (positions, indices), SubMesh (range, bounds)
- Not allowed: buffers, Vulkan.

### `common/assets/`
Asset loading, identity and versioning. **The only Assimp entry point in the project.**
- Importer returns plain CPU data:
  - per-submesh geometry (always)
  - optional visual attributes (normals, UVs, tangents)
  - materials as plain data (texture paths, opaque/mask/blend flags)
  - skeleton and animation clips
- "Geometry only" mode for the server; full mode for the client.
- Submeshes stay **split by material** (they are not merged at import time).
- AssetStore / AssetRegistry:
  - resolves `namespace + version + asset-name` to a local path
  - the path is the ID: two assets can never share the same ID
  - stores a content hash per asset, used only for integrity checks
  - several versions of the same asset can coexist
- Not allowed: GPU upload, GPU textures.

### `common/animation/`
Deterministic animation, CPU only.
- Skeleton, clip sampling, pose computation.
- The server controls the animation step; the client advances using network time.
- Not allowed: GPU skinning (lives in `client`).

### `common/accel/`
Acceleration structures. Everything BVH/AS lives here.
- `builders/`: BinnedSAH and the builder templated on BVH width.
  Also supports **joining several BLAS trees into one** (used for logical body parts).
- `nodes/`: BVH nodes, BLASInstance, TLASInstance.
- `layout/`: flattened GPU formats, one per width.
- `query/`: CPU raycast, overlap and collision.
  Queries accept a **max depth**: traversal stops at that depth and the node
  bounds are used as the approximation (no triangle tests).
- `config/`: BVH width and constants. **A single header**, also used to generate
  the GLSL `#define`s (prevents shader/CPU drift).
- BLAS is **per submesh** (shared between instances, key = asset + submesh index).
  The TLAS references BLAS by ID.
- BLAS refit support (animation).
- Not allowed: Vulkan buffers, Vulkan native AS.

### `common/world/`
Facade used by both server and client.
- Registers assets (creates one BLAS per submesh).
- Adds / moves / removes instances (feeds the TLAS).
- Answers queries (raycast, overlap).
- **Single source of truth** for instances and transforms.
- Marks what changed ("dirty"); client backends consume this to stay in sync.

### `common/net/`
Shared networking. See the dedicated section below.
- `protocol/`: message definitions and serialization.
- `transport/`: transport abstraction (real network and local loopback).
- Not allowed: client-only or server-only session logic.

---

## Collision grouping and animated meshes (decisions)

**Submesh grouping**
- Submeshes remain separated by material in the asset.
- The server decides which submeshes form a logical part (for example "arrow tip"
  or "torso") and **joins their BLAS trees into one** when it builds its world.
- The grouping is server-side configuration, not data baked into the model file.
  The model stays neutral; different servers or mods may group it differently.

**Animated meshes**
- Animation itself must be precise (pose from `common/animation`, BLAS refit).
- The server does **not** test triangles. It sets a **maximum BLAS depth**, and
  tests stop at the node bounds at that depth. The result is an approximation
  by design, not a perfect geometry test.
- The depth limit is a parameter of the query, so it can differ per use
  (hit detection vs. broad checks).

---

## Assets: naming, versioning and distribution (decisions)

**Layout**

```
assets/<namespace>/<version>/<asset-name>
```

**Namespace**

| Namespace            | Meaning                                         |
|----------------------|-------------------------------------------------|
| `default`            | Official version of the game                    |
| `<name>_mod`         | A mod                                           |
| `server_<name>`      | A server that differs from the official version |

**Version**
- Either the game version alone (`1.2.3`) or the game version plus the model
  version, in the form `<game version>_<model version>` (for example `1.2.3_m12`).
- Several versions coexist side by side. A new version is created only when an
  asset changes.

**Asset name**
- Lowercase ASCII, snake_case, no spaces.
- Unique inside its version folder.

**Identity**
- An asset is identified by `namespace + version + asset-name`, which is also its
  path. Two assets cannot share the same path, so ID conflicts (for example between
  mods) cannot happen by construction.
- The content hash is not identity; it only checks that the file is intact.

**Validation and transfer**
1. The server sends a manifest of the assets it uses (IDs and hashes).
2. The client checks what it has locally.
3. For anything missing or with a different hash, **the client is responsible for
   getting it**. The server never sends asset files.
   - The client ships with its **own built-in manifest of download sources**.
   - The server may also provide a source (for example a URL) for the assets it uses.
   - Both can be used; the order of precedence is described under Open points.
4. The client verifies the hash after downloading. A mismatch is an explicit error.
5. Assets of a server that differs from official go into that server's
   `_server_<name>` namespace, so no separate cache folder is needed.

---

## Networking: who decides what

The **server decides** which model is which. The client does not choose, it only resolves.

1. Client connects.
2. Server sends the **asset manifest**: namespace, version, asset-name, hash.
3. Client resolves each entry to a local file through the `AssetStore`.
4. Missing or mismatched assets are fetched by the client (flow above).
5. Server sends entity state: entity ID, asset ID, transform,
   animation state/time.

Consequences:
- Asset identity is stable and defined by namespace, version and name.
- Local mode (client + server on the same machine) uses the loopback transport:
  same protocol, no special path.
- Animation messages carry simulation time, not the final pose.

---

## `server/`

- `net/`: client sessions, manifest, state snapshots.
- `simulation/`: simulation loop, animation step control, decision of which asset
  each entity uses.
- `collision/`: server-side grouping of submeshes into logical parts and
  depth limits for queries. Uses `common/world`.
- Not allowed: rendering, Vulkan, sending asset files.

---

## `client/`

### `client/vulkan_core/`
Pure Vulkan infrastructure.
- Device, swapchain, framebuffers, command buffers, descriptors, buffers,
  image utilities, pipeline and render pass helpers.
- Knows nothing about scene, BVH or materials.

### `client/resources/`
GPU resource upload.
- Vertex/index buffers, textures, samplers, materials, material descriptors.
- Consumes the plain data from `common/assets`.

### `client/scene/`
Renderable scene state.
- Renderable instances, lights, camera, particles (data and registration).
- Fed by the state that arrives from the network.
- Not allowed: draw commands.

### `client/raytracing/`
- `IAccelerationBackend`: single interface.
- `shader_bvh/`: uploads the layouts from `common/accel/layout` into buffers;
  lightweight path for machines with Vulkan 1.2 only.
- `hardware_rt/`: native AS (`VK_KHR_acceleration_structure`), built from
  vertex/index buffers and the instance list.
- Not allowed: the CPU BVH itself (lives in `common/accel`).

### `client/render/`
Drawing.
- Passes: geometry/GBuffer, transparency, lighting, composite, particles.
- Asks the acceleration backend for the structure and picks the path at runtime.

### `client/net/`
- Server session, manifest handling, applying snapshots to `scene`.
- Asset downloader: fetches missing or mismatched assets into the right
  namespace folder and verifies their hash.
- Built-in source manifest shipped with the client, plus any source the server provides.

### `client/ui/`
- ImGui and screens. No game logic or heavy rendering.

### `client/shaders/`
- GLSL sources, separated from models.
- `raytracing/bvh.glsl` isolated; the rest (lighting, gbuffer) split per pass.

---

## Project root

- `assets/`: namespaced and versioned assets (layout above). The server also
  reads geometry from here.

---

## Open points

1. **Source precedence and trust** (proposal): the client's built-in sources come
   first for the namespaces they cover (for example `default`). A source provided
   by a server applies only to that server's own `_server_<name>` namespace (and
   anything it declares there), so a server can never redirect the download of
   `default` or other namespaces' assets.