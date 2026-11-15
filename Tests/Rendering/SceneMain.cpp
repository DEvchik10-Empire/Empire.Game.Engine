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
 *   --mesh=knot|sphere|cube      встроенная модель (по умолчанию cube)
 *   --model=путь.obj             своя модель (центрируется и масштабируется сама)
 *   --sky-debug=N                отладка неба: 1 - текстура формы облаков,
 *                                2 - текстура шума, 3 - плотность облаков, 4 - mip-уровень
 *   --shaders=папка              где лежат .spv (по умолчанию: см. ResolveShaderDir)
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

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
        0.4f, 1.0f, 0.05f, 8.0f, 0.0f,
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
            else if (std::strncmp(a, "--mesh=", 7) == 0)     options.mesh = a + 7;
            else if (std::strncmp(a, "--model=", 8) == 0)    options.modelPath = a + 8;
            else if (std::strncmp(a, "--shaders=", 10) == 0) options.shaderDir = a + 10;
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

            if (!Rendering::LoadObj(options.modelPath.c_str(), mesh))
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

    struct ModelPush
    {
        float model[16];
    };

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
        // cloud_shape: fbm 10 окт. (у нас 8: выше 512 px октавы уже не видны),
        // cloud_noise: fbm 5 окт., high_clouds: клеточный шум, shooting_star: оригинал GDQuest.
        // cloud_shape: 1024 px, 6 октав (выше октавы мельче текселя и дают
        // «ступеньки» на краях облаков), gain 7 + bias 0.6 - контраст и покрытие
        // неба, сдвиг (224, 720) - какой участок шума окажется над головой.
        // Это подгонка под вид превью GDQuest.
        const Rendering::ImageR8 shapeImage = Rendering::MakeFbmNoiseImage(1024, 5, 6, 1, 7.0f, 224, 720, 0.6f);
        const Rendering::ImageR8 noiseImage = Rendering::MakeFbmNoiseImage(512, 5, 5, 101);
        const Rendering::ImageR8 highImage = Rendering::MakeCellularImage(512, 6, 201);
        const Rendering::ImageR8 starImage = Rendering::GetOriginalShootingStarImage();

        Vulkan::Texture2D cloudShapeTexture(device, shapeImage, true, true);
        Vulkan::Texture2D cloudNoiseTexture(device, noiseImage, true, true);
        Vulkan::Texture2D highCloudsTexture(device, highImage, true);
        Vulkan::Texture2D shootingStarTexture(device, starImage, false);

        Vulkan::UniformSet uniforms(
            device, sizeof(SceneUniformData), Vulkan::FrameContext::FramesInFlight,
            {&cloudShapeTexture, &cloudNoiseTexture, &highCloudsTexture, &shootingStarTexture});

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
        litDesc.vertexShader = &litVs;
        litDesc.fragmentShader = &litPs;
        litDesc.useVertexBuffer = true;
        litDesc.depthTest = true;
        litDesc.depthWrite = true;
        litDesc.pushConstantSize = sizeof(ModelPush);

        Vulkan::GraphicsPipeline skyPipeline(device, skyDesc);
        Vulkan::GraphicsPipeline litPipeline(device, litDesc);

        // ---- модель ----
        Vulkan::GpuMesh mesh(device, BuildMesh(options));

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

            // Камера медленно облетает модель, слегка качаясь по высоте.
            const float orbit = seconds * 0.12f;
            const Vec3 eye{
                std::sin(orbit) * 3.2f,
                0.10f + std::sin(seconds * 0.3f) * 0.15f,
                std::cos(orbit) * 3.2f};
            // Смотрим чуть выше модели: больше неба, горизонт ниже центра кадра.
            const Vec3 look{0.0f, 1.1f, 0.0f};

            const float aspect = static_cast<float>(frame.extent.width) /
                                 static_cast<float>(std::max(1u, frame.extent.height));

            const SceneUniformData data =
                BuildFrameData(options, seconds, aspect, encodeGamma, eye, look);

            uniforms.Update(frame.frameIndex, &data, sizeof(data));

            ModelPush push;
            const Mat4 model = Multiply(RotationY(seconds * 0.9f), RotationX(seconds * 0.6f));
            std::memcpy(push.model, model.m, sizeof(push.model));

            frames.BeginRenderPass(
                frame, target.GetRenderPass(), target.GetFramebuffer(frame.imageIndex),
                0.0f, 0.0f, 0.0f, 1.0f);

            // 1. Небо на весь экран (без записи глубины).
            skyPipeline.Bind(frame.commandBuffer);
            uniforms.Bind(frame.commandBuffer, skyPipeline.GetLayout(), frame.frameIndex);
            vkCmdDraw(frame.commandBuffer, 3, 1, 0, 0);

            // 2. Модель.
            litPipeline.Bind(frame.commandBuffer);
            uniforms.Bind(frame.commandBuffer, litPipeline.GetLayout(), frame.frameIndex);
            vkCmdPushConstants(
                frame.commandBuffer, litPipeline.GetLayout(),
                VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(ModelPush), &push);
            mesh.Draw(frame.commandBuffer);

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
    const Options options = ParseOptions(argc, argv);

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
