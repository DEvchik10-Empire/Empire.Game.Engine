1. Core / Foundation

База, на которой вообще всё держится:

* собственный Core
* базовые типы
* String
* StringView
* массивы
* динамические массивы
* hash map
* hash set
* queues
* stacks
* bitsets
* optional/result-подобные типы
* smart pointers / handles
* memory allocators
* arena allocator
* pool allocator
* linear allocator
* memory tracking
* assertions
* logging
* profiling
* timers
* UUID/ID система
* random
* platform-independent utilities
* error handling
* configuration system
* command-line arguments
* build/version information

⸻

2. Math

Полноценная игровая математика:

* Vec2
* Vec3
* Vec4
* integer vectors
* matrices
* Mat2
* Mat3
* Mat4
* quaternions
* Euler angles
* transforms
* translation
* rotation
* scale
* TRS
* dot product
* cross product
* normalization
* interpolation
* lerp
* slerp
* projection
* view matrices
* perspective
* orthographic
* planes
* rays
* AABB
* spheres
* frustum
* intersections
* bounding volumes
* color types
* coordinate conversions
* SIMD-оптимизации там, где реально нужны

И это как раз потом пригодится и движку, и твоему отдельному SDK.

⸻

3. Platform Module

Твой первый конкретный backend:

Linux/X11

* X11 connection
* window creation
* window destruction
* resize
* fullscreen
* window events
* keyboard
* mouse
* cursor
* clipboard
* monitor information
* DPI
* timing
* filesystem
* shared libraries
* threads
* synchronization primitives

А архитектуру сделать так, чтобы потом можно было добавить:

* Wayland
* Windows
* macOS

Но не обязательно делать их за первые 16 месяцев.

⸻

4. Renderer

Ты уже определил первый backend:

Vulkan

И тут можно очень далеко уйти:

* Vulkan instance
* physical device selection
* logical device
* queues
* swapchain
* command buffers
* command pools
* synchronization
* fences
* semaphores
* images
* image views
* buffers
* memory management
* descriptor sets
* descriptor pools
* pipelines
* pipeline layouts
* shaders
* shader modules
* render passes / современная Vulkan-архитектура
* frame management
* staging buffers
* texture upload
* mipmaps
* MSAA
* depth buffer
* stencil
* render targets
* framebuffer abstraction
* GPU resources
* resource lifetime
* GPU profiling

Потом поверх этого:

* Mesh
* Material
* Texture
* Sampler
* Camera
* Lights
* shadows
* instancing
* batching
* frustum culling
* LOD
* skybox
* environment maps
* HDR
* post-processing
* bloom
* tone mapping
* SSAO
* TAA/другой AA
* particles

А уже значительно позже можно думать про:

* ray tracing
* GPU-driven rendering
* indirect rendering
* compute-based culling
* bindless
* virtualized resources

⸻

5. Shader System

Отдельная нормальная система:

* shader loading
* shader compilation
* shader cache
* shader reflection
* shader parameters
* material parameters
* permutations
* hot reload
* shader dependencies
* automatic resource binding
* debug information
* возможно собственный shader format

И чтобы изменение .vert/.frag/.comp не требовало перезапуска редактора.

⸻

6. Resource / Asset System

Очень важная часть настоящего движка:

* Resource IDs
* resource manager
* asset registry
* asset loading
* asynchronous loading
* caching
* reference counting/handles
* dependency tracking
* asset lifetime
* asset hot reload
* asset serialization
* импорт ассетов
* конвертация ассетов
* asset metadata

Модели

* OBJ
* glTF
* возможно FBX позже

Текстуры

* PNG
* JPEG
* HDR
* KTX2/Basis или другой GPU-friendly формат

Shader assets

* исходники
* compiled cache
* metadata

⸻

7. Scene System

Чтобы уже не просто рендерить отдельные объекты:

* Scene
* Entity
* Entity ID
* Components
* Transform
* hierarchy
* parent/child
* scene serialization
* scene loading
* scene saving
* prefab/scene templates
* entity creation/deletion
* component add/remove
* component properties

И здесь как раз появляется твой ECS.

⸻

8. ECS

Можно сделать реально хорошую систему:

* Entity registry
* components
* component storage
* archetypes или sparse storage
* systems
* queries
* entity relationships
* entity lifecycle
* events
* deferred entity operations
* parallel systems
* system scheduling
* serialization
* editor integration
* debugging ECS

А потом:

* multithreaded systems
* task-based execution
* cache-friendly iteration

⸻

9. Gameplay Framework

Чтобы движок уже был пригоден для игры:

* game instance
* world
* scene
* entities
* components
* systems
* game loop
* fixed timestep
* variable timestep
* update phases
* initialization/shutdown
* game state
* input state
* timers
* events
* gameplay tags или аналог
* basic scripting interface

⸻

10. Physics

Не обязательно сразу писать собственный PhysX уровня монстра, но система должна существовать нормально:

* collision shapes
* box
* sphere
* capsule
* raycast
* overlap
* collision layers
* collision masks
* rigid bodies
* static bodies
* dynamic bodies
* kinematic bodies
* gravity
* velocity
* impulses
* friction
* restitution
* triggers
* character collision
* physics queries

Потом:

* joints
* constraints
* ragdoll
* vehicles
* cloth
* destruction

⸻

11. Animation

Это уже особенно актуально для твоей цели к концу 2027 делать 3D игры:

* skeleton
* bones
* skinning
* animation clips
* keyframes
* animation sampling
* interpolation
* animation player
* animation blending
* blend trees
* state machines
* animation events
* additive animations
* IK
* retargeting
* root motion
* animation compression
* GPU skinning

И уже потом можно делать motion matching и прочие тяжёлые вещи.

⸻

12. Audio

Чтобы игра была реально игрой:

* audio device
* sound loading
* WAV/OGG и т.п.
* sound effects
* music
* audio channels
* volume
* pitch
* looping
* spatial audio
* 3D positioning
* listener
* attenuation
* audio groups
* mixing
* streaming music
* audio assets

Позже:

* DSP
* effects
* reverb
* occlusion
* advanced spatialization

⸻

13. Input

Нормальная абстракция:

* keyboard
* mouse
* buttons
* axes
* mouse movement
* scroll
* gamepads
* controller mapping
* input contexts
* action mapping
* axis mapping
* rebinding

⸻

14. Networking

Это можно развивать уже после базового движка:

* sockets
* UDP
* TCP
* connection management
* packets
* serialization
* replication
* client/server
* RPC
* entity replication
* interpolation
* prediction
* server authority
* lag compensation
* network debugging

И вот это уже может стать отдельным огромным пластом.

⸻

15. GUI

Ты хотел свою GUI-систему, значит:

* UI renderer
* UI context
* widgets
* buttons
* labels
* images
* text
* text input
* checkboxes
* sliders
* dropdowns
* scroll areas
* panels
* layout system
* anchors
* docking
* focus
* keyboard navigation
* mouse interaction
* events
* fonts
* glyph atlas
* text shaping
* clipping
* UI styling

И поверх неё уже строится редактор.

⸻

16. Editor

Вот тут движок начинает выглядеть уже как полноценный продукт.

Главный viewport

* камера
* grid
* gizmos
* translate
* rotate
* scale
* selection
* multi-selection
* snapping
* viewport controls

Scene hierarchy

* entities
* parent/child
* создание
* удаление
* rename
* drag & drop

Inspector

* components
* properties
* vectors
* colors
* assets
* references
* editing values

Asset Browser

* folders
* assets
* thumbnails
* search
* import
* drag & drop
* metadata

Другие окна

* console
* profiler
* renderer statistics
* ECS inspector
* memory statistics
* project settings
* world settings

⸻

17. Editor Tools

Чтобы это был не просто набор окон:

* undo/redo
* copy/paste entities
* duplicate
* prefab system
* gizmos
* snapping
* selection tools
* multi-edit
* search
* command palette
* shortcuts
* hot reload
* play mode
* pause
* step frame
* simulation
* debug drawing

⸻

18. Serialization

Очень важная штука:

* binary serialization
* text serialization
* scene format
* component serialization
* asset metadata
* versioning
* migration
* backwards compatibility

Editor
   ↓
Save Scene
   ↓
scene.asset
   ↓
Engine
   ↓
Load Scene


19. Build / Project System

Потом:

* project files
* project configuration
* build configurations
* Debug
* Development
* Shipping
* asset cooking
* packaging
* dependency tracking
* generated files
* project launcher
* executable generation

⸻

20. Debugging / Profiling

Очень желательно заложить рано:

* logging
* assertions
* crash handling
* stack traces
* CPU profiler
* GPU profiler
* frame timings
* memory profiler
* ECS statistics
* renderer statistics
* draw calls
* triangles
* VRAM
* RAM
* resource usage
* debug visualization

Например прямо в редакторе:

FPS:       144
Frame:     6.9 ms
Game:      1.8 ms
Render:    3.2 ms
GPU:       4.7 ms
Draw calls: 842
Triangles:  1.2M
VRAM:       2.4 GB
Entities:   18 392

21. Tools / Pipeline

И вот это часто забывают, хотя для настоящего движка оно охуенно важно:

* asset importer
* model converter
* texture converter
* shader compiler pipeline
* asset validator
* dependency scanner
* project generator
* packaging tools
* command-line tools
* automated builds
* asset cache
* editor automation

И твоя идея с подготовкой ассетов из Blender сюда прекрасно ложится:

Blender
   ↓
Export
   ↓
Your Asset Pipeline
   ↓
Mesh / Material / Skeleton / Animation
   ↓
Engine Asset Format
   ↓
Editor

22. Particles / VFX

Потом:

* particle emitter
* particle systems
* lifetime
* velocity
* forces
* colors
* size
* textures
* GPU particles
* trails
* effects
* decals

⸻

23. Lighting

* directional light
* point light
* spot light
* ambient lighting
* shadow maps
* cascaded shadows
* reflection probes
* irradiance
* HDR
* environment lighting

Потом уже:

* GI
* DDGI
* SSR
* RT reflections
* RT shadows

⸻

24. 3D World Systems

Когда база будет готова:

* terrain
* water
* sky
* fog
* volumetrics
* vegetation
* instanced meshes
* LOD
* occlusion culling
* world streaming
* level streaming

⸻

25. Gameplay Utilities

Для реальных игр:

* timers
* coroutines
* state machines
* event bus
* delegates
* callbacks
* gameplay tags
* curves
* spline system
* camera system
* character controller
* interaction system
* save/load
* localization

⸻

26. Developer Experience

А это вообще то, что отличает «движок» от огромной папки с кодом:

* документация
* examples
* templates
* error messages
* useful diagnostics
* project creation
* editor startup
* asset previews
* hot reload
* crash reports
* profiling tools
* clean API
* стабильные интерфейсы
* нормальная структура проекта


Renderer — что должно войти за 16 месяцев

Базовые ресурсы

* Mesh
* Material
* Texture
* Sampler
* Shader
* Shader parameters
* Render targets
* Buffers
* Cameras
* Lights
* Shadow maps

Геометрия и производительность

* instancing
* batching
* frustum culling
* occlusion culling
* LOD
* mesh LOD
* возможно GPU-driven rendering
* indirect drawing

Освещение

* directional light
* point light
* spot light
* environment lighting
* skybox
* environment maps
* HDR
* reflection probes
* shadow systems

Post-processing

* bloom
* tone mapping
* exposure
* color grading
* SSAO
* SSR — если успеешь
* TAA
* FXAA/SMAA
* motion blur — по желанию
* depth of field — уже позже, если останется время

VFX

* particles
* particle materials
* particle emitters
* GPU particles — желательно, но не обязательный первый вариант

⸻

Материалы

А вот Material Editor я бы действительно поставил в план на эти 16 месяцев.

Причём твоя идея с двумя совершенно разными типами материалов вполне нормальная:

3D Material

Условно:


Material
├── Shader
├── Base Color
├── Normal
├── Roughness
├── Metallic
├── Emission
├── Opacity
└── Custom Parameters

И редактор:

Texture ─────┐
              ├── Multiply ──┐
Color ───────┘               │
                             ├── Material
Roughness ───────────────────┤
Normal ──────────────────────┘



То есть уже node-based material editor, если ты именно это имеешь в виду.

UI Material

А здесь вообще другая система:

UI Material
├── background
├── border
├── radius
├── shadow
├── font
├── text color
├── padding
├── margin
└── etc.

По сути действительно что-то в духе CSS, но твоё собственное и адаптированное под ECS/UI.

И я бы их архитектурно не пытался запихнуть в один огромный Material класс.

Например:

Rendering
├── MeshMaterial
└── UIMaterial

Asset Pipeline

И тут твой выбор glTF как основного формата мне нравится.

Для 16 месяцев я бы сделал:

Обязательно

glTF 2.x

и поддержал:

* meshes
* materials
* textures
* nodes
* scenes
* skeletons
* animations
* skinning
* morph targets
* cameras
* metadata, насколько нужно движку

А уже внутри движка:

scene.gltf
      ↓
Importer
      ↓
Engine Asset Format
      ↓
Asset Database
      ↓
Renderer / ECS / Animation


USD можно оставить как второй крупный pipeline, особенно если тебе действительно нужен сложный обмен сценами/окружением.

FBX/OBJ и десять других форматов я бы действительно не тащил просто «потому что так принято».


ECS + Gameplay Framework

Твой аналог GAS тоже надо добавить в список.

Причём если всё строится вокруг ECS, можно получить очень интересную систему:

Entity
 ├── AbilityComponent
 ├── AttributeComponent
 ├── EffectComponent
 ├── GameplayTagComponent
 └── ...

И системы:

AbilitySystem
EffectSystem
AttributeSystem
TagSystem
CooldownSystem

Например:

Ability
 ↓
Cost
 ↓
Cooldown
 ↓
Gameplay Effect
 ↓
Attribute modification
 ↓
Events

И аудио обязательно надо считать частью полноценного движка.

Минимальная нормальная система:

Audio Core

* Audio Device
* Audio Context
* Audio Sources
* Audio Buffers
* Audio Listener
* Audio Channels
* Audio Groups
* volume
* pitch
* looping
* playback control
* pause/resume
* streaming

2D Audio

* UI sounds
* music
* ambient
* SFX

3D Audio

* spatial positioning
* distance attenuation
* listener
* Doppler
* stereo/panning
* 3D sources

Asset Pipeline

* WAV
* OGG
* возможно FLAC

И обязательно:

Audio Asset
     ↓
Audio Resource
     ↓
Audio Source
     ↓
Audio Mixer
     ↓
Output

И ещё несколько больших систем, которые нельзя забывать

Чтобы к концу этих 16 месяцев действительно можно было начинать делать игру, я бы держал обязательными:

Animation

* Skeleton
* Bone hierarchy
* Animation clips
* Animation player
* blending
* state machine
* blend trees
* skinning
* IK
* animation events

Physics

* rigid bodies
* collision
* triggers
* raycasts
* overlaps
* character controller
* constraints
* physics queries

Input

* keyboard
* mouse
* gamepad
* action mapping
* axis mapping
* rebinding

Scene / World

* Scene
* World
* Entity
* hierarchy
* transforms
* prefabs
* serialization
* scene loading/saving

Networking

Хотя бы фундамент:

* sockets
* packet system
* serialization
* client/server
* RPC
* replication foundation

Полноценный prediction/rollback/lag compensation можно уже после.

GUI

* собственный UI renderer
* widgets
* layout
* styling
* fonts
* text rendering
* events
* docking
* UI materials
* editor UI

Editor

И поверх всего этого:

* viewport
* hierarchy
* inspector
* asset browser
* material editor
* scene editor
* gizmos
* console
* profiler
* play mode
* pause
* frame stepping
* undo/redo
* project settings


ENGINE v0.x

Core
├── C++ foundation
├── Math
├── Memory
├── Containers
├── Serialization
├── Jobs / Tasks
└── ECS

Platform
└── X11

Graphics
├── Renderer
├── RHI
│   └── Vulkan
├── Mesh
├── Material
├── Texture
├── Lighting
├── Shadows
├── Culling
├── LOD
├── Instancing
├── Batching
├── Post-processing
├── Particles
└── etc.

Assets
├── glTF
├── Asset database
└── Import pipeline

Animation
├── Skeleton
├── Animation
├── Blending
├── Skinning
└── IK

Physics
├── Collision
├── Rigid bodies
├── Queries
└── Character movement

Audio
├── 2D
├── 3D
├── Mixer
└── Streaming

UI
├── Renderer
├── Layout
├── Widgets
├── Styling
└── UI materials

Gameplay
├── Components
├── Events
├── Tags
├── Abilities
├── Attributes
└── Effects

Editor
├── Viewport
├── Scene
├── Hierarchy
├── Inspector
├── Asset browser
├── Material editor
├── UI editor
├── Console
├── Gizmos
└── Profiler