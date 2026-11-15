/*
 * Тест 3D-сцены: небо (HLSL, порт Godot sky shader) + вращающаяся модель.
 *
 * Что проверяет:
 *   1. Окно + Vulkan (как в TestMain), но теперь с render pass и глубиной.
 *   2. Шейдеры HLSL -> SPIR-V (папка Shaders/ в корне движка) грузятся с диска.
 *   3. Два pipeline: небо (полноэкранный треугольник) и модель (вершинный буфер).
 *   4. Модель: куб / сфера / торический узел / свой .obj.
 *
 * Запуск:
 *   Empire.Rendering.Test.Scene [auto|wayland|legasy] [опции]
 *
 * Опции:
 *   --frames=N          после N кадров выйти (0 - работать до закрытия окна)
 *   --no-validation     без validation layers
 *   --show=auto|cube|model|none  что показывать: куб, свою модель или ничего (только небо).
 *                                auto (по умолчанию): модель Assets/asset.glb, если она есть, иначе куб
 *   --model=путь.glb|.gltf|.obj  своя модель (центрируется и масштабируется сама; включает --show=model)
 *   --mesh=cube|sphere|knot      какой встроенный примитив показывать (для --show=cube)
 *   --spin=y|xy|none             как крутить (по умолчанию: y для glb/obj, xy для встроенных примитивов)
 *   --spin-speed=N               скорость, рад/с, со знаком (по умолчанию 0.9; минус - в другую сторону)
 *   --camera=fixed|orbit         камера: стоит на месте (по умолчанию) или медленно облетает
 *   --no-model                   то же, что --show=none
 *   --sky-debug=N                отладка неба: 1 - текстура формы облаков,
 *                                2 - текстура шума, 3 - плотность облаков, 4 - mip-уровень
 *   --shaders=папка              где лежат .spv (по умолчанию: см. ResolveShaderDir)
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#ifndef EMPIRE_ASSET_DIR
#define EMPIRE_ASSET_DIR "Assets"
#endif

#include "Empire.Window/IWindowImpl.hpp"
#include "Empire.Window/WindowBackend.hpp"
#include "Empire.Window/WindowConfig.hpp"
#include "Empire.Window/WindowImplFactory.hpp"

#include "Empire.Device/Vulkan/VkDevice.hpp"
#include "Empire.Device/Vulkan/VkInstance.hpp"
#include "Empire.SwapChain/Vulkan/VkSurface.hpp"
#include "Empire.SwapChain/Vulkan/VkSwapChain.hpp"
#include "Empire.CommandBuffer/Vulkan/VkFrameContext.hpp"
#include "Empire.Buffer/Vulkan/VkMesh.hpp"
#include "Empire.Shader/Vulkan/VkShaderModule.hpp"
#include "Empire.Pipeline/Vulkan/VkSceneTarget.hpp"
#include "Empire.Pipeline/Vulkan/VkMaterialSet.hpp"
#include "Empire.Pipeline/Vulkan/VkUniformSet.hpp"
#include "Empire.Pipeline/Vulkan/VkGraphicsPipeline.hpp"
#include "Empire.Mesh/Mesh.hpp"
#include "Empire.Texture/NoiseImage.hpp"
#include "Empire.Texture/Vulkan/VkTexture2D.hpp"

#include "SceneMath.hpp"

namespace Platform = Engine::Platform;
namespace Rendering = Engine::Rendering;
namespace Vulkan = Engine::Rendering::Vulkan;

using namespace SceneMath;

#ifndef EMPIRE_SHADER_DIR
    #define EMPIRE_SHADER_DIR ""
#endif

namespace
{
    constexpr float Pi = 3.14159265359f;

    // =====================================================================
    // Данные кадра для шейдеров. ДОЛЖНЫ совпадать с cbuffer SceneParams
    // в Shaders/Common/Scene.hlsli (всё - float4, std140 без сюрпризов).
    // =====================================================================
    struct Float4
    {
        float x = 0, y = 0, z = 0, w = 0;
    };

    struct SceneUniformData
    {
        float viewProj[16] = {};
        float invViewProj[16] = {};

        Float4 cameraPos;        // xyz, w = время
        Float4 lightDir;
        Float4 lightColor;

        Float4 topColor;
        Float4 bottomColor;
        Float4 sunScatter;
        Float4 cloudsLightColor;

        Float4 cloudsParams;     // density, scale, smoothness, shadow intensity
        Float4 cloudsSamples;    // samples, shadow samples, high clouds density
        Float4 skyParams;        // stars, shooting stars, astro intensity, astro scale
        Float4 astroTint;
        Float4 shootingTint;

        Float4 misc;             // x - кодировать гамму, y - экспозиция
    };

    static_assert(sizeof(SceneUniformData) == 32 * 4 + 13 * 16,
                  "SceneUniformData must match cbuffer SceneParams");

    // =====================================================================
    // Пресеты неба: ТОЧНЫЕ значения из day_sky.tres / night_sky.tres
    // (gdquest-demos/godot-4-stylized-sky). Цвета - sRGB, как в Godot
    // (source_color); в шейдер уходят линейными.
    // =====================================================================
    struct SkyPreset
    {
        Vec3 top, bottom, sunScatter, cloudsLight, astroTint, shootingTint, light;

        float cloudsDensity;
        float cloudsScale;
        float cloudsSmoothness;
        float cloudsShadowIntensity;
        float highClouds;

        float astroScale;
        float astroIntensity;
        float astroEdge0;     // края радиального градиента astro_sampler
        float astroEdge1;

        float stars;
        float shootingStars;

        float lightIntensity; // для освещения модели (в Godot это DirectionalLight)
    };

    const SkyPreset PresetNight =
    {
        {0.027451f, 0.101961f, 0.250980f},
        {0.027451f, 0.101961f, 0.250980f},
        {0.125490f, 0.086275f, 0.372549f},
        {0.227451f, 0.447059f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 0.662745f, 0.419608f},
        {0.45f, 0.58f, 1.0f},
        0.5f, 1.0f, 0.05f, 8.0f, 0.0f,
        6.0f, 1.2f, 0.58f, 0.6f,
        5.0f, 4.0f,
        0.45f
    };

    // Сколько шагов ray-march (в Godot clouds_samples 8..32, shadow_sample 1..4).
    constexpr float CloudsSamples = 32.0f;
    constexpr float CloudsShadowSamples = 4.0f;

    // Тонемаппинг как в превью GDQuest (Filmic) + небольшой запас яркости.
    constexpr float Exposure = 1.1f;

    // Точная sRGB -> линейное.
    float SrgbToLinear(float c)
    {
        c = std::max(c, 0.0f);
        return c <= 0.04045f ? c / 12.92f : std::pow((c + 0.055f) / 1.055f, 2.4f);
    }

    Float4 ToLinear(Vec3 srgb, float w = 0.0f)
    {
        return {SrgbToLinear(srgb.x), SrgbToLinear(srgb.y), SrgbToLinear(srgb.z), w};
    }

    struct Options
    {
        Platform::WindowBackend backend = Platform::WindowBackend::Auto;

        unsigned int maxFrames = 0;
        bool validation = true;

        std::string mesh = "cube";
        std::string modelPath;
        std::string shaderDir;

        int skyDebug = 0;
        bool drawModel = true;
        std::string spin;
        std::string show = "auto";
        std::string camera = "fixed";
        bool meshGiven = false;
        float spinSpeed = 0.9f;
    };

    const char* BackendName(Platform::WindowBackend backend)
    {
        switch (backend)
        {
            case Platform::WindowBackend::Auto:    return "Auto";
            case Platform::WindowBackend::Wayland: return "Wayland";
            case Platform::WindowBackend::legasy:  return "legasy (X11)";
        }

        return "Unknown";
    }

    Options ParseOptions(int argc, char** argv)
    {
        Options options;

        for (int i = 1; i < argc; ++i)
        {
            const char* a = argv[i];

            if (std::strcmp(a, "wayland") == 0)              options.backend = Platform::WindowBackend::Wayland;
            else if (std::strcmp(a, "legasy") == 0)          options.backend = Platform::WindowBackend::legasy;
            else if (std::strcmp(a, "auto") == 0)            options.backend = Platform::WindowBackend::Auto;
            else if (std::strcmp(a, "--no-validation") == 0) options.validation = false;
            else if (std::strncmp(a, "--frames=", 9) == 0)   options.maxFrames = static_cast<unsigned int>(std::strtoul(a + 9, nullptr, 10));
            else if (std::strncmp(a, "--mesh=", 7) == 0)     { options.mesh = a + 7; options.meshGiven = true; }
            else if (std::strncmp(a, "--show=", 7) == 0)     options.show = a + 7;
            else if (std::strncmp(a, "--camera=", 9) == 0)   options.camera = a + 9;
            else if (std::strncmp(a, "--spin-speed=", 13) == 0) options.spinSpeed = static_cast<float>(std::atof(a + 13));
            else if (std::strncmp(a, "--model=", 8) == 0)    options.modelPath = a + 8;
            else if (std::strncmp(a, "--shaders=", 10) == 0) options.shaderDir = a + 10;
            else if (std::strncmp(a, "--spin=", 7) == 0)     options.spin = a + 7;
            else if (std::strcmp(a, "--no-model") == 0)      { options.drawModel = false; options.show = "none"; }
            else if (std::strncmp(a, "--sky-debug=", 12) == 0) options.skyDebug = std::atoi(a + 12);
            else
            {
                std::cerr << "[Scene] Unknown argument: " << a << '\n';
            }
        }

        return options;
    }

    bool FileExists(const std::string& path)
    {
        return std::ifstream(path, std::ios::binary).good();
    }

    // Где искать скомпилированные шейдеры: --shaders, путь из сборки xmake,
    // потом несколько типичных мест относительно текущей папки.
    std::string ResolveShaderDir(const Options& options)
    {
        std::string candidates[] =
        {
            options.shaderDir,
            EMPIRE_SHADER_DIR,
            "build/shaders",
            "shaders",
            "../build/shaders",
            "../../build/shaders"
        };

        for (const std::string& dir : candidates)
        {
            if (!dir.empty() && FileExists(dir + "/Sky.vs.spv"))
            {
                return dir;
            }
        }

        // Не нашли: вернём самый вероятный, ошибка загрузки подскажет что делать.
        return !options.shaderDir.empty() ? options.shaderDir : std::string("build/shaders");
    }

    Rendering::MeshData BuildMesh(const Options& options)
    {
        if (!options.modelPath.empty())
        {
            Rendering::MeshData mesh;

            const std::string& path = options.modelPath;
            const auto endsWith = [&](const char* ext)
            {
                const std::size_t n = std::strlen(ext);
                if (path.size() < n) return false;
                for (std::size_t i = 0; i < n; ++i)
                {
                    if (std::tolower(static_cast<unsigned char>(path[path.size() - n + i])) != ext[i]) return false;
                }
                return true;
            };

            const bool isGltf = endsWith(".glb") || endsWith(".gltf");

            if (!(isGltf ? Rendering::LoadGltf(path.c_str(), mesh)
                         : Rendering::LoadObj(path.c_str(), mesh)))
            {
                throw std::runtime_error("Cannot load model: " + options.modelPath);
            }

            std::cout << "[Scene] Model: " << options.modelPath << " ("
                      << mesh.vertices.size() << " vertices, "
                      << mesh.indices.size() / 3 << " triangles)\n";

            return mesh;
        }

        if (options.mesh == "sphere") return Rendering::MakeSphere();

        if (options.mesh == "knot")   return Rendering::MakeTorusKnot();

        return Rendering::MakeCube();
    }

    // Заполняет данные кадра: камера + небо для данного момента времени.
    SceneUniformData BuildFrameData(
        const Options& options,
        float seconds,
        float aspect,
        bool encodeGamma,
        const Vec3& eye,
        const Vec3& target)
    {
        SceneUniformData data;

        const Vec3 up{0, 1, 0};
        const float fov = 75.0f * Pi / 180.0f; // как камера по умолчанию в Godot
        const float nearPlane = 0.1f;
        const float farPlane = 1000.0f;

        const Mat4 viewProj = Multiply(Perspective(fov, aspect, nearPlane, farPlane), LookAt(eye, target, up));
        const Mat4 invViewProj = Multiply(InverseLookAt(eye, target, up), InversePerspective(fov, aspect, nearPlane, farPlane));

        std::memcpy(data.viewProj, viewProj.m, sizeof(data.viewProj));
        std::memcpy(data.invViewProj, invViewProj.m, sizeof(data.invViewProj));

        // ---- ночное небо: точные значения из night_sky.tres ----
        const SkyPreset& p = PresetNight;

        // Луна (LIGHT0_DIRECTION): на старте камеры в левой верхней части кадра,
        // как на превью GDQuest.
        const Vec3 moon = Normalize({-0.30f, 0.55f, -0.78f});

        data.cameraPos = {eye.x, eye.y, eye.z, seconds};
        data.lightDir = {moon.x, moon.y, moon.z, 0.0f};

        const Float4 lightLinear = ToLinear(p.light);
        data.lightColor = {
            lightLinear.x * p.lightIntensity,
            lightLinear.y * p.lightIntensity,
            lightLinear.z * p.lightIntensity,
            0.0f};

        data.topColor = ToLinear(p.top);
        data.bottomColor = ToLinear(p.bottom);
        data.sunScatter = ToLinear(p.sunScatter);
        data.cloudsLightColor = ToLinear(p.cloudsLight);
        data.shootingTint = ToLinear(p.shootingTint);
        data.astroTint = ToLinear(p.astroTint);

        data.cloudsParams = {p.cloudsDensity, p.cloudsScale, p.cloudsSmoothness, p.cloudsShadowIntensity};

        // w - режим отладки неба (0 - выключено).
        data.cloudsSamples = {CloudsSamples, CloudsShadowSamples, p.highClouds, static_cast<float>(options.skyDebug)};

        data.skyParams = {p.stars, p.shootingStars, p.astroIntensity, p.astroScale};

        data.misc = {encodeGamma ? 1.0f : 0.0f, Exposure, p.astroEdge0, p.astroEdge1};

        return data;
    }

    // Должно совпадать с PushConstants в Shaders/Lit.vs.hlsl и Lit.ps.hlsl.
    struct ModelPush
    {
        float model[16];
        float baseColor[4]; // множитель base color (линейный), a - непрозрачность
        float material[4];  // x: 0 - цвет "дышит", 1 - материал glTF; y: alphaCutoff (<0 - выкл); z: metallic; w: roughness
    };

    static_assert(sizeof(ModelPush) == 96, "ModelPush must match the shader push constants");

    void RunRendering(Platform::IWindowImpl& window, const Options& options)
    {
        Vulkan::InstanceDesc instanceDesc;
        instanceDesc.applicationName = "Empire.Rendering.Test.Scene";
        instanceDesc.enableValidation = options.validation;

        Vulkan::Instance instance(instanceDesc);

        Vulkan::Surface surface(instance, window.GetNativeHandle());
        Vulkan::Device device(instance, surface.Get());

        std::cout << "[Scene] GPU: " << device.GetName() << '\n';

        const auto [startWidth, startHeight] = window.GetSize();

        Vulkan::SwapChain swapChain(
            device, surface,
            static_cast<unsigned int>(startWidth),
            static_cast<unsigned int>(startHeight));

        Vulkan::FrameContext frames(device, swapChain);
        Vulkan::SceneTarget target(device, swapChain);

        // ---- шейдеры ----
        const std::string shaderDir = ResolveShaderDir(options);
        std::cout << "[Scene] Shaders: " << shaderDir << '\n';

        Vulkan::ShaderModule skyVs(device, shaderDir + "/Sky.vs.spv");
        Vulkan::ShaderModule skyPs(device, shaderDir + "/Sky.ps.spv");
        Vulkan::ShaderModule litVs(device, shaderDir + "/Lit.vs.spv");
        Vulkan::ShaderModule litPs(device, shaderDir + "/Lit.ps.spv");

        // ---- текстуры неба (замена NoiseTexture2D из Godot) ----
        // Точные картинки NoiseTexture2D из оригинала (выгружены из Godot 4.2.2).
        const Rendering::ImageR8 shapeImage = Rendering::GetGodotCloudShapeImage();
        const Rendering::ImageR8 noiseImage = Rendering::GetGodotCloudNoiseImage();
        const Rendering::ImageR8 highImage = Rendering::GetGodotHighCloudsImage();
        const Rendering::ImageR8 starImage = Rendering::GetOriginalShootingStarImage();

        Vulkan::Texture2D cloudShapeTexture(device, shapeImage, true, true);
        Vulkan::Texture2D cloudNoiseTexture(device, noiseImage, true, true);
        Vulkan::Texture2D highCloudsTexture(device, highImage, true);
        Vulkan::Texture2D shootingStarTexture(device, starImage, false);

        Vulkan::UniformSet uniforms(
            device, sizeof(SceneUniformData), Vulkan::FrameContext::FramesInFlight,
            {&cloudShapeTexture, &cloudNoiseTexture, &highCloudsTexture, &shootingStarTexture});

        // ---- модель и материалы ----
        Rendering::MeshData meshData = BuildMesh(options);

        const std::vector<Rendering::MaterialData> materials = meshData.materials;

        std::vector<std::unique_ptr<Vulkan::Texture2D>> imageTextures;

        for (const Rendering::ImageRGBA8& image : meshData.images)
        {
            imageTextures.push_back(std::make_unique<Vulkan::Texture2D>(device, image, true, true, true));
        }

        // Белая текстура 1x1 для материалов без картинки (и для примитивов).
        Rendering::ImageRGBA8 whiteImage;
        whiteImage.width = 1;
        whiteImage.height = 1;
        whiteImage.pixels = {255, 255, 255, 255};
        Vulkan::Texture2D whiteTexture(device, whiteImage, true, true, false);

        std::vector<const Vulkan::Texture2D*> materialTextures;

        for (const Rendering::MaterialData& material : materials)
        {
            materialTextures.push_back(
                material.baseColorImage >= 0 &&
                material.baseColorImage < static_cast<int>(imageTextures.size())
                    ? imageTextures[material.baseColorImage].get()
                    : &whiteTexture);
        }

        if (materialTextures.empty())
        {
            materialTextures.push_back(&whiteTexture);
        }

        Vulkan::MaterialSet materialSet(device, materialTextures);

        Vulkan::GraphicsPipelineDesc skyDesc;
        skyDesc.renderPass = target.GetRenderPass();
        skyDesc.setLayout = uniforms.GetLayout();
        skyDesc.vertexShader = &skyVs;
        skyDesc.fragmentShader = &skyPs;
        skyDesc.useVertexBuffer = false;
        skyDesc.depthTest = false;
        skyDesc.depthWrite = false;

        Vulkan::GraphicsPipelineDesc litDesc;
        litDesc.renderPass = target.GetRenderPass();
        litDesc.setLayout = uniforms.GetLayout();
        litDesc.materialLayout = materialSet.GetLayout();
        litDesc.vertexShader = &litVs;
        litDesc.fragmentShader = &litPs;
        litDesc.useVertexBuffer = true;
        litDesc.depthTest = true;
        litDesc.depthWrite = true;
        litDesc.pushConstantSize = sizeof(ModelPush);

        Vulkan::GraphicsPipeline skyPipeline(device, skyDesc);
        Vulkan::GraphicsPipeline litPipeline(device, litDesc);

        Vulkan::GpuMesh mesh(device, std::move(meshData));

        const bool encodeGamma = swapChain.GetFormat() != VK_FORMAT_B8G8R8A8_SRGB &&
                                 swapChain.GetFormat() != VK_FORMAT_R8G8B8A8_SRGB;

        std::cout << "[Scene] Rendering. Close the window to exit.\n\n";

        unsigned int swapWidth = static_cast<unsigned int>(startWidth);
        unsigned int swapHeight = static_cast<unsigned int>(startHeight);

        const auto startTime = std::chrono::steady_clock::now();
        auto fpsTime = startTime;

        unsigned int frameCount = 0;
        unsigned int fpsFrames = 0;

        const auto resizeAll = [&]()
        {
            if (swapChain.Resize(swapWidth, swapHeight))
            {
                target.Rebuild();
            }
        };

        while (window.IsOpen())
        {
            while (window.PollEvent())
            {
            }

            if (window.ShouldClose())
            {
                break;
            }

            const auto [width, height] = window.GetSize();

            if (width <= 0 || height <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                continue;
            }

            if (static_cast<unsigned int>(width) != swapWidth ||
                static_cast<unsigned int>(height) != swapHeight)
            {
                swapWidth = static_cast<unsigned int>(width);
                swapHeight = static_cast<unsigned int>(height);
                resizeAll();
            }

            Vulkan::FrameInfo frame;
            const Vulkan::FrameStatus began = frames.Begin(frame);

            if (began == Vulkan::FrameStatus::OutOfDate)
            {
                resizeAll();
                continue;
            }

            if (began == Vulkan::FrameStatus::Skipped)
            {
                continue;
            }

            const float seconds =
                std::chrono::duration<float>(std::chrono::steady_clock::now() - startTime).count();

            // Камера: стоит на месте или (--camera=orbit) медленно облетает модель, слегка качаясь по высоте.
            // --camera=fixed: стоит на месте (камера на +Z, смотрит на модель).
            const bool orbiting = options.camera == "orbit";
            const float orbit = orbiting ? seconds * 0.12f : 0.0f;
            const Vec3 eye{
                std::sin(orbit) * 3.2f,
                0.10f + (orbiting ? std::sin(seconds * 0.3f) * 0.15f : 0.05f),
                std::cos(orbit) * 3.2f};
            // Смотрим чуть выше модели: больше неба, горизонт ниже центра кадра.
            const Vec3 look{0.0f, 1.1f, 0.0f};

            const float aspect = static_cast<float>(frame.extent.width) /
                                 static_cast<float>(std::max(1u, frame.extent.height));

            const SceneUniformData data =
                BuildFrameData(options, seconds, aspect, encodeGamma, eye, look);

            uniforms.Update(frame.frameIndex, &data, sizeof(data));

            const bool isGltfModel = !materials.empty();

            const std::string spin =
                !options.spin.empty() ? options.spin : (options.modelPath.empty() ? "xy" : "y");

            // Поворот вокруг вертикальной оси при камере на +Z: ближняя к камере
            // сторона идёт справа налево при отрицательном угле, поэтому для
            // модели по умолчанию угол отрицательный (--spin-speed=-0.9 развернёт).
            const float angle = seconds * options.spinSpeed * (options.modelPath.empty() ? 1.0f : -1.0f);

            Mat4 model = RotationY(0.0f);

            if (spin == "y")       model = RotationY(angle);
            else if (spin == "xy") model = Multiply(RotationY(angle), RotationX(seconds * 0.6f));

            frames.BeginRenderPass(
                frame, target.GetRenderPass(), target.GetFramebuffer(frame.imageIndex),
                0.0f, 0.0f, 0.0f, 1.0f);

            // 1. Небо на весь экран (без записи глубины).
            skyPipeline.Bind(frame.commandBuffer);
            uniforms.Bind(frame.commandBuffer, skyPipeline.GetLayout(), frame.frameIndex);
            vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);

            // 2. Модель (--show=none отключает): по куску на материал.
            if (options.drawModel)
            {
                litPipeline.Bind(frame.commandBuffer);
                uniforms.Bind(frame.commandBuffer, litPipeline.GetLayout(), frame.frameIndex);
                mesh.Bind(frame.commandBuffer);

                for (const Rendering::SubMesh& sub : mesh.GetSubMeshes())
                {
                    ModelPush push{};
                    std::memcpy(push.model, model.m, sizeof(push.model));

                    push.baseColor[0] = push.baseColor[1] = push.baseColor[2] = push.baseColor[3] = 1.0f;

                    if (isGltfModel && sub.material < materials.size())
                    {
                        const Rendering::MaterialData& m = materials[sub.material];

                        std::memcpy(push.baseColor, m.baseColorFactor, sizeof(push.baseColor));
                        push.material[0] = 1.0f;
                        push.material[1] = m.alphaCutoff;
                        push.material[2] = m.metallic;
                        push.material[3] = m.roughness;
                    }
                    else
                    {
                        push.material[0] = 0.0f;
                        push.material[1] = -1.0f;
                        push.material[2] = 0.2f;
                        push.material[3] = 0.28f;
                    }

                    materialSet.Bind(
                        frame.commandBuffer, litPipeline.GetLayout(),
                        isGltfModel ? sub.material : 0);

                    vkCmdPushConstants(
                        frame.commandBuffer, litPipeline.GetLayout(),
                        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                        0, sizeof(ModelPush), &push);

                    mesh.DrawRange(frame.commandBuffer, sub.firstIndex, sub.indexCount);
                }
            }

            frames.EndRenderPass(frame);

            if (frames.End(frame) == Vulkan::FrameStatus::OutOfDate)
            {
                resizeAll();
            }

            ++frameCount;
            ++fpsFrames;

            const auto now = std::chrono::steady_clock::now();

            if (now - fpsTime >= std::chrono::seconds(1))
            {
                std::cout << "[Scene] " << fpsFrames << " fps  (frame " << frameCount << ")\n";
                fpsFrames = 0;
                fpsTime = now;
            }

            if (options.maxFrames != 0 && frameCount >= options.maxFrames)
            {
                break;
            }
        }

        device.WaitIdle();

        std::cout << "\n[Scene] Rendered frames: " << frameCount << '\n';
    }
}

int main(int argc, char** argv)
{
    Options options = ParseOptions(argc, argv);

    // Что показывать: --show=cube|model|none; auto - своя модель, если указана
    // через --model или лежит в Assets/asset.glb, иначе встроенный примитив.
    {
        const std::string defaultAsset = std::string(EMPIRE_ASSET_DIR) + "/asset.glb";

        if (options.show == "auto")
        {
            if (!options.modelPath.empty())     options.show = "model";
            else if (options.meshGiven)         options.show = "cube";
            else if (FileExists(defaultAsset))  options.show = "model";
            else                                options.show = "cube";
        }

        if (options.show == "model")
        {
            if (options.modelPath.empty())
            {
                options.modelPath = defaultAsset;
            }

            if (!FileExists(options.modelPath))
            {
                std::cerr << "[Scene] Model not found: " << options.modelPath
                          << "\n        Put it there or pass --model=path/to/file.glb "
                          << "(or use --show=cube / --show=none).\n";
                return 1;
            }
        }
        else if (options.show == "cube")
        {
            options.modelPath.clear();
        }
        else if (options.show == "none")
        {
            options.drawModel = false;
            options.modelPath.clear();
        }
        else
        {
            std::cerr << "[Scene] Unknown --show=" << options.show << " (use auto|cube|model|none)\n";
            return 1;
        }

        std::cout << "[Scene] Show: " << options.show
                  << (options.show == "model" ? " (" + options.modelPath + ")" : std::string()) << "\n";
    }

    std::cout << "=== Empire.Rendering scene test (sky + 3D model), window backend: "
              << BackendName(options.backend) << " ===\n";

    Platform::WindowConfig config;
    config.width = 1100;
    config.height = 700;
    config.title = "Empire Engine - Sky + 3D model";
    config.backend = options.backend;
    config.selfDraw = false;

    std::unique_ptr<Platform::IWindowImpl> window = Platform::CreateWindowImpl(config);

    if (!window)
    {
        std::cerr << "[Scene] FAILED: window was not created.\n";
        return 1;
    }

    window->Show();

    int result = 0;

    try
    {
        RunRendering(*window, options);
    }
    catch (const std::exception& error)
    {
        std::cerr << "[Scene] FAILED: " << error.what() << '\n';
        result = 1;
    }

    window->Close();
    window.reset();

    if (result == 0)
    {
        std::cout << "=== Scene test passed ===\n";
    }

    return result;
}
