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
 *   --time=day|sunset|night      время суток (по умолчанию sunset)
 *   --cycle                      непрерывная смена дня и ночи
 *   --cycle-seconds=N            длительность суток в секундах (по умолчанию 60)
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
    // Пресеты неба (цвета в sRGB, как в Godot; в шейдер уходят линейными).
    // =====================================================================
    struct SkyPreset
    {
        Vec3 top, bottom, scatter, cloudsLight, astroTint, shootingTint, light;

        float cloudsDensity;
        float starsIntensity;
        float shootingStars;
        float highClouds;
        float astroIntensity;
        float lightIntensity;
        float exposure;
    };

    const SkyPreset PresetDay =
    {
        {0.12f, 0.36f, 0.85f}, {0.65f, 0.80f, 0.97f}, {0.12f, 0.06f, 0.00f},
        {1.00f, 1.00f, 1.00f}, {1.00f, 0.95f, 0.80f}, {0.0f, 0.0f, 0.0f},
        {1.00f, 0.96f, 0.88f},
        0.40f, 0.0f, 0.0f, 0.45f, 1.0f, 3.2f, 1.0f
    };

    const SkyPreset PresetSunset =
    {
        {0.12f, 0.18f, 0.45f}, {0.98f, 0.55f, 0.30f}, {0.80f, 0.30f, 0.05f},
        {1.00f, 0.75f, 0.55f}, {1.00f, 0.70f, 0.40f}, {0.0f, 0.0f, 0.0f},
        {1.00f, 0.55f, 0.28f},
        0.42f, 0.0f, 0.0f, 0.40f, 2.0f, 2.6f, 1.0f
    };

    const SkyPreset PresetNight =
    {
        {0.015f, 0.03f, 0.10f}, {0.05f, 0.08f, 0.20f}, {0.00f, 0.01f, 0.05f},
        {0.25f, 0.30f, 0.50f}, {0.80f, 0.90f, 1.00f}, {0.80f, 0.90f, 1.00f},
        {0.45f, 0.58f, 1.00f},
        0.33f, 1.6f, 2.5f, 0.25f, 1.2f, 0.45f, 1.15f
    };

    float Smoothstep(float edge0, float edge1, float x)
    {
        const float t = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    float SrgbToLinear(float c)
    {
        return std::pow(std::max(c, 0.0f), 2.2f);
    }

    Float4 ToLinear(Vec3 srgb, float w = 0.0f)
    {
        return {SrgbToLinear(srgb.x), SrgbToLinear(srgb.y), SrgbToLinear(srgb.z), w};
    }

    // Смешивает три пресета по весам (сумма = 1).
    template <typename Member>
    auto Blend(const Member SkyPreset::*field, float wDay, float wSunset, float wNight)
    {
        return PresetDay.*field * wDay + PresetSunset.*field * wSunset + PresetNight.*field * wNight;
    }

    struct Options
    {
        Platform::WindowBackend backend = Platform::WindowBackend::Auto;

        unsigned int maxFrames = 0;
        bool validation = true;

        std::string mesh = "cube";
        std::string modelPath;
        std::string shaderDir;

        // Начальная высота солнца (радианы над горизонтом).
        float startAngle = 0.12f;
        bool cycle = false;
        float cycleSeconds = 60.0f;
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
            else if (std::strcmp(a, "--cycle") == 0)         options.cycle = true;
            else if (std::strncmp(a, "--frames=", 9) == 0)   options.maxFrames = static_cast<unsigned int>(std::strtoul(a + 9, nullptr, 10));
            else if (std::strncmp(a, "--mesh=", 7) == 0)     options.mesh = a + 7;
            else if (std::strncmp(a, "--model=", 8) == 0)    options.modelPath = a + 8;
            else if (std::strncmp(a, "--shaders=", 10) == 0) options.shaderDir = a + 10;
            else if (std::strncmp(a, "--cycle-seconds=", 16) == 0)
            {
                options.cycleSeconds = std::max(1.0f, static_cast<float>(std::atof(a + 16)));
            }
            else if (std::strncmp(a, "--time=", 7) == 0)
            {
                const char* value = a + 7;

                if (std::strcmp(value, "day") == 0)         options.startAngle = 1.0f;
                else if (std::strcmp(value, "sunset") == 0) options.startAngle = 0.12f;
                else if (std::strcmp(value, "night") == 0)  options.startAngle = -1.2f;
                else std::cerr << "[Scene] Unknown --time value: " << value << '\n';
            }
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
        const float fov = 60.0f * Pi / 180.0f;
        const float nearPlane = 0.1f;
        const float farPlane = 1000.0f;

        const Mat4 viewProj = Multiply(Perspective(fov, aspect, nearPlane, farPlane), LookAt(eye, target, up));
        const Mat4 invViewProj = Multiply(InverseLookAt(eye, target, up), InversePerspective(fov, aspect, nearPlane, farPlane));

        std::memcpy(data.viewProj, viewProj.m, sizeof(data.viewProj));
        std::memcpy(data.invViewProj, invViewProj.m, sizeof(data.invViewProj));

        // ---- солнце ----
        float angle = options.startAngle;

        if (options.cycle)
        {
            angle += 2.0f * Pi * seconds / options.cycleSeconds;
        }

        const float azimuth = 0.6f;
        const Vec3 sun{
            std::cos(angle) * std::sin(azimuth),
            std::sin(angle),
            std::cos(angle) * std::cos(azimuth)};

        // ---- веса пресетов по высоте солнца ----
        const float e = sun.y;

        const float wDay = Smoothstep(0.05f, 0.40f, e);
        const float wNight = 1.0f - Smoothstep(-0.25f, 0.0f, e);
        const float wSunset = std::max(0.0f, 1.0f - wDay - wNight);

        // Под горизонтом "светило" - луна на противоположной стороне неба.
        const Vec3 astro = e > -0.10f ? sun : sun * -1.0f;

        data.cameraPos = {eye.x, eye.y, eye.z, seconds};
        data.lightDir = {astro.x, astro.y, astro.z, 0.0f};

        const Vec3 light = Blend(&SkyPreset::light, wDay, wSunset, wNight);
        const float lightIntensity = Blend(&SkyPreset::lightIntensity, wDay, wSunset, wNight);

        // Цвет света уходит в шейдер уже линейным и с интенсивностью.
        const Float4 lightLinear = ToLinear(light);
        data.lightColor = {
            lightLinear.x * lightIntensity,
            lightLinear.y * lightIntensity,
            lightLinear.z * lightIntensity,
            0.0f};

        data.topColor = ToLinear(Blend(&SkyPreset::top, wDay, wSunset, wNight));
        data.bottomColor = ToLinear(Blend(&SkyPreset::bottom, wDay, wSunset, wNight));
        data.sunScatter = ToLinear(Blend(&SkyPreset::scatter, wDay, wSunset, wNight));
        data.cloudsLightColor = ToLinear(Blend(&SkyPreset::cloudsLight, wDay, wSunset, wNight));
        data.astroTint = ToLinear(Blend(&SkyPreset::astroTint, wDay, wSunset, wNight));
        data.shootingTint = ToLinear(Blend(&SkyPreset::shootingTint, wDay, wSunset, wNight));

        // clouds_density / scale / smoothness / shadow_intensity (по умолчанию как в Godot).
        data.cloudsParams = {
            Blend(&SkyPreset::cloudsDensity, wDay, wSunset, wNight),
            1.0f,
            0.06f,   // smoothness: край облака
            7.5f};   // shadow intensity

        // clouds_samples = 16, shadow_sample = 4, high_clouds_density.
        data.cloudsSamples = {
            16.0f,
            4.0f,
            Blend(&SkyPreset::highClouds, wDay, wSunset, wNight),
            0.0f};

        data.skyParams = {
            Blend(&SkyPreset::starsIntensity, wDay, wSunset, wNight),
            Blend(&SkyPreset::shootingStars, wDay, wSunset, wNight),
            Blend(&SkyPreset::astroIntensity, wDay, wSunset, wNight),
            4.0f}; // astro_scale: больше -> диск меньше

        data.misc = {encodeGamma ? 1.0f : 0.0f, Blend(&SkyPreset::exposure, wDay, wSunset, wNight), 0.0f, 0.0f};

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

        Vulkan::UniformSet uniforms(
            device, sizeof(SceneUniformData), Vulkan::FrameContext::FramesInFlight);

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
                0.35f + std::sin(seconds * 0.3f) * 0.25f,
                std::cos(orbit) * 3.2f};
            const Vec3 look{0.0f, 0.15f, 0.0f};

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
