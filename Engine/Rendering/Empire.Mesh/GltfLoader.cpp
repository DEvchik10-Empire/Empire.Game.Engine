#include "Mesh.hpp"

#include "../ThirdParty/stb/stb_image.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

/*
 * Загрузчик glTF 2.0 (.glb / .gltf + .bin). Зависимостей нет: ниже маленький
 * JSON-парсер, разбор контейнера GLB, accessors и обход иерархии узлов.
 */

namespace Engine
{
    namespace Rendering
    {
        namespace
        {
            // -------------------------------------------------------------
            // Минимальный JSON
            // -------------------------------------------------------------
            struct Json
            {
                enum class Type { Null, Bool, Number, String, Array, Object };

                Type type = Type::Null;
                double number = 0.0;
                bool boolean = false;
                std::string string;
                std::vector<Json> array;
                std::map<std::string, Json> object;

                const Json* Find(const std::string& key) const
                {
                    if (type != Type::Object)
                    {
                        return nullptr;
                    }

                    const auto it = object.find(key);
                    return it == object.end() ? nullptr : &it->second;
                }

                bool Has(const std::string& key) const { return Find(key) != nullptr; }

                double Num(const std::string& key, double fallback) const
                {
                    const Json* v = Find(key);
                    return v && v->type == Type::Number ? v->number : fallback;
                }

                const Json& At(std::size_t i) const
                {
                    if (type != Type::Array || i >= array.size())
                    {
                        throw std::runtime_error("glTF: index out of range");
                    }

                    return array[i];
                }

                std::size_t Size() const { return array.size(); }
            };

            class JsonParser
            {
            public:
                JsonParser(const char* begin, const char* end) : p(begin), e(end) {}

                Json Parse()
                {
                    Json v = Value();
                    Skip();
                    return v;
                }

            private:
                const char* p;
                const char* e;

                void Skip()
                {
                    while (p < e && (*p == ' ' || *p == '\n' || *p == '\r' || *p == '\t'))
                    {
                        ++p;
                    }
                }

                [[noreturn]] void Fail(const char* what)
                {
                    throw std::runtime_error(std::string("glTF JSON: ") + what);
                }

                Json Value()
                {
                    Skip();

                    if (p >= e)
                    {
                        Fail("unexpected end");
                    }

                    if (*p == '{') return Object();
                    if (*p == '[') return Array();
                    if (*p == '"')
                    {
                        Json v;
                        v.type = Json::Type::String;
                        v.string = String();
                        return v;
                    }

                    if (e - p >= 4 && std::strncmp(p, "true", 4) == 0)
                    {
                        p += 4;
                        Json v; v.type = Json::Type::Bool; v.boolean = true; return v;
                    }

                    if (e - p >= 5 && std::strncmp(p, "false", 5) == 0)
                    {
                        p += 5;
                        Json v; v.type = Json::Type::Bool; return v;
                    }

                    if (e - p >= 4 && std::strncmp(p, "null", 4) == 0)
                    {
                        p += 4;
                        return Json();
                    }

                    // число
                    char* end = nullptr;
                    const double d = std::strtod(p, &end);

                    if (end == p)
                    {
                        Fail("bad value");
                    }

                    p = end;
                    Json v;
                    v.type = Json::Type::Number;
                    v.number = d;
                    return v;
                }

                std::string String()
                {
                    std::string out;
                    ++p; // "

                    while (p < e && *p != '"')
                    {
                        if (*p == '\\' && p + 1 < e)
                        {
                            ++p;

                            switch (*p)
                            {
                            case 'n': out += '\n'; break;
                            case 't': out += '\t'; break;
                            case 'r': out += '\r'; break;
                            case 'b': out += '\b'; break;
                            case 'f': out += '\f'; break;
                            case 'u':
                                // Имена нам не важны: \uXXXX заменяем на '?'.
                                out += '?';
                                p += 4;
                                break;
                            default: out += *p; break;
                            }

                            ++p;
                        }
                        else
                        {
                            out += *p++;
                        }
                    }

                    if (p >= e)
                    {
                        Fail("unterminated string");
                    }

                    ++p; // "
                    return out;
                }

                Json Array()
                {
                    Json v;
                    v.type = Json::Type::Array;
                    ++p;
                    Skip();

                    if (p < e && *p == ']')
                    {
                        ++p;
                        return v;
                    }

                    while (true)
                    {
                        v.array.push_back(Value());
                        Skip();

                        if (p < e && *p == ',') { ++p; continue; }
                        if (p < e && *p == ']') { ++p; break; }

                        Fail("expected , or ]");
                    }

                    return v;
                }

                Json Object()
                {
                    Json v;
                    v.type = Json::Type::Object;
                    ++p;
                    Skip();

                    if (p < e && *p == '}')
                    {
                        ++p;
                        return v;
                    }

                    while (true)
                    {
                        Skip();

                        if (p >= e || *p != '"')
                        {
                            Fail("expected key");
                        }

                        std::string key = String();
                        Skip();

                        if (p >= e || *p != ':')
                        {
                            Fail("expected :");
                        }

                        ++p;
                        v.object[key] = Value();
                        Skip();

                        if (p < e && *p == ',') { ++p; continue; }
                        if (p < e && *p == '}') { ++p; break; }

                        Fail("expected , or }");
                    }

                    return v;
                }
            };

            // -------------------------------------------------------------
            // Матрицы 4x4, column-major как в glTF: m[col * 4 + row]
            // -------------------------------------------------------------
            struct M4
            {
                double m[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
            };

            M4 Mul(const M4& a, const M4& b)
            {
                M4 r;

                for (int c = 0; c < 4; ++c)
                {
                    for (int row = 0; row < 4; ++row)
                    {
                        double s = 0.0;

                        for (int k = 0; k < 4; ++k)
                        {
                            s += a.m[k * 4 + row] * b.m[c * 4 + k];
                        }

                        r.m[c * 4 + row] = s;
                    }
                }

                return r;
            }

            M4 FromTrs(const double t[3], const double q[4], const double s[3])
            {
                const double x = q[0], y = q[1], z = q[2], w = q[3];

                M4 r;

                r.m[0] = (1 - 2 * (y * y + z * z)) * s[0];
                r.m[1] = (2 * (x * y + z * w)) * s[0];
                r.m[2] = (2 * (x * z - y * w)) * s[0];

                r.m[4] = (2 * (x * y - z * w)) * s[1];
                r.m[5] = (1 - 2 * (x * x + z * z)) * s[1];
                r.m[6] = (2 * (y * z + x * w)) * s[1];

                r.m[8] = (2 * (x * z + y * w)) * s[2];
                r.m[9] = (2 * (y * z - x * w)) * s[2];
                r.m[10] = (1 - 2 * (x * x + y * y)) * s[2];

                r.m[12] = t[0];
                r.m[13] = t[1];
                r.m[14] = t[2];

                return r;
            }

            // Матрица для нормалей: обратная транспонированная верхней 3x3 (кофакторы).
            void NormalMatrix(const M4& a, double n[9])
            {
                // элементы 3x3: e[row][col] = a.m[col * 4 + row]
                const auto e = [&](int row, int col) { return a.m[col * 4 + row]; };

                n[0] = e(1, 1) * e(2, 2) - e(1, 2) * e(2, 1);
                n[1] = e(1, 2) * e(2, 0) - e(1, 0) * e(2, 2);
                n[2] = e(1, 0) * e(2, 1) - e(1, 1) * e(2, 0);

                n[3] = e(0, 2) * e(2, 1) - e(0, 1) * e(2, 2);
                n[4] = e(0, 0) * e(2, 2) - e(0, 2) * e(2, 0);
                n[5] = e(0, 1) * e(2, 0) - e(0, 0) * e(2, 1);

                n[6] = e(0, 1) * e(1, 2) - e(0, 2) * e(1, 1);
                n[7] = e(0, 2) * e(1, 0) - e(0, 0) * e(1, 2);
                n[8] = e(0, 0) * e(1, 1) - e(0, 1) * e(1, 0);
                // n - матрица кофакторов (строки), это и есть inverse-transpose с точностью до масштаба.
            }

            // -------------------------------------------------------------
            // Загрузка файла
            // -------------------------------------------------------------
            struct Gltf
            {
                Json json;
                std::vector<std::vector<std::uint8_t>> buffers;
            };

            std::vector<std::uint8_t> ReadAll(const std::string& path)
            {
                std::ifstream file(path, std::ios::binary);

                if (!file)
                {
                    throw std::runtime_error("cannot open " + path);
                }

                return std::vector<std::uint8_t>(
                    (std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            }

            std::uint32_t Read32(const std::uint8_t* p)
            {
                return static_cast<std::uint32_t>(p[0]) |
                       (static_cast<std::uint32_t>(p[1]) << 8) |
                       (static_cast<std::uint32_t>(p[2]) << 16) |
                       (static_cast<std::uint32_t>(p[3]) << 24);
            }

            std::string DirectoryOf(const std::string& path)
            {
                const std::size_t slash = path.find_last_of("/\\");
                return slash == std::string::npos ? std::string() : path.substr(0, slash + 1);
            }

            Gltf Load(const std::string& path)
            {
                const std::vector<std::uint8_t> file = ReadAll(path);

                Gltf g;
                std::vector<std::uint8_t> glbBin;
                bool hasGlbBin = false;

                if (file.size() >= 12 && std::memcmp(file.data(), "glTF", 4) == 0)
                {
                    const std::uint32_t total = Read32(file.data() + 8);
                    std::size_t offset = 12;
                    const std::size_t limit = std::min<std::size_t>(total, file.size());
                    bool haveJson = false;

                    while (offset + 8 <= limit)
                    {
                        const std::uint32_t length = Read32(file.data() + offset);
                        const std::uint32_t type = Read32(file.data() + offset + 4);
                        offset += 8;

                        if (offset + length > file.size())
                        {
                            throw std::runtime_error("GLB chunk is out of file");
                        }

                        if (type == 0x4E4F534Au) // JSON
                        {
                            const char* b = reinterpret_cast<const char*>(file.data() + offset);
                            g.json = JsonParser(b, b + length).Parse();
                            haveJson = true;
                        }
                        else if (type == 0x004E4942u && !hasGlbBin) // BIN
                        {
                            glbBin.assign(file.begin() + offset, file.begin() + offset + length);
                            hasGlbBin = true;
                        }

                        offset += (length + 3u) & ~3u;
                    }

                    if (!haveJson)
                    {
                        throw std::runtime_error("GLB has no JSON chunk");
                    }
                }
                else
                {
                    const char* b = reinterpret_cast<const char*>(file.data());
                    g.json = JsonParser(b, b + file.size()).Parse();
                }

                // Сжатые расширения не поддерживаем.
                if (const Json* req = g.json.Find("extensionsRequired"))
                {
                    for (const Json& ext : req->array)
                    {
                        if (ext.string == "KHR_draco_mesh_compression" ||
                            ext.string == "EXT_meshopt_compression")
                        {
                            throw std::runtime_error(
                                "extension " + ext.string + " is not supported "
                                "(re-export without compression)");
                        }
                    }
                }

                const std::string dir = DirectoryOf(path);

                if (const Json* buffers = g.json.Find("buffers"))
                {
                    for (std::size_t i = 0; i < buffers->Size(); ++i)
                    {
                        const Json& b = buffers->At(i);
                        const Json* uri = b.Find("uri");

                        if (!uri)
                        {
                            if (!hasGlbBin)
                            {
                                throw std::runtime_error("buffer without uri and no GLB BIN chunk");
                            }

                            g.buffers.push_back(glbBin);
                        }
                        else if (uri->string.compare(0, 5, "data:") == 0)
                        {
                            throw std::runtime_error(
                                "base64 data URI is not supported (export as .glb)");
                        }
                        else
                        {
                            g.buffers.push_back(ReadAll(dir + uri->string));
                        }
                    }
                }

                return g;
            }

            // -------------------------------------------------------------
            // Accessors
            // -------------------------------------------------------------
            int Components(const std::string& type)
            {
                if (type == "SCALAR") return 1;
                if (type == "VEC2") return 2;
                if (type == "VEC3") return 3;
                if (type == "VEC4") return 4;
                if (type == "MAT2") return 4;
                if (type == "MAT3") return 9;
                if (type == "MAT4") return 16;

                throw std::runtime_error("unknown accessor type " + type);
            }

            std::size_t ComponentSize(int componentType)
            {
                switch (componentType)
                {
                case 5120: case 5121: return 1;
                case 5122: case 5123: return 2;
                case 5125: case 5126: return 4;
                }

                throw std::runtime_error("unknown componentType");
            }

            // Читает accessor как float (с учётом normalized) в плоский массив count * comps.
            std::vector<float> ReadAccessor(const Gltf& g, std::size_t index, int& comps)
            {
                const Json& acc = g.json.Find("accessors")->At(index);

                const std::size_t count = static_cast<std::size_t>(acc.Num("count", 0));
                const int componentType = static_cast<int>(acc.Num("componentType", 0));
                const bool normalized = acc.Find("normalized") && acc.Find("normalized")->boolean;
                comps = Components(acc.Find("type")->string);

                std::vector<float> out(count * comps, 0.0f);

                if (!acc.Has("bufferView"))
                {
                    return out; // нули по спецификации
                }

                if (acc.Has("sparse"))
                {
                    throw std::runtime_error("sparse accessors are not supported");
                }

                const Json& view = g.json.Find("bufferViews")->At(
                    static_cast<std::size_t>(acc.Num("bufferView", 0)));

                const std::size_t bufferIndex = static_cast<std::size_t>(view.Num("buffer", 0));

                if (bufferIndex >= g.buffers.size())
                {
                    throw std::runtime_error("bad buffer index");
                }

                const std::vector<std::uint8_t>& buffer = g.buffers[bufferIndex];

                const std::size_t base =
                    static_cast<std::size_t>(view.Num("byteOffset", 0)) +
                    static_cast<std::size_t>(acc.Num("byteOffset", 0));

                const std::size_t elementSize = ComponentSize(componentType) * comps;
                std::size_t stride = static_cast<std::size_t>(view.Num("byteStride", 0));

                if (stride == 0)
                {
                    stride = elementSize;
                }

                if (count > 0 && base + stride * (count - 1) + elementSize > buffer.size())
                {
                    throw std::runtime_error("accessor is out of buffer");
                }

                for (std::size_t i = 0; i < count; ++i)
                {
                    const std::uint8_t* src = buffer.data() + base + i * stride;

                    for (int c = 0; c < comps; ++c)
                    {
                        float value = 0.0f;

                        switch (componentType)
                        {
                        case 5126: // float
                        {
                            float f;
                            std::memcpy(&f, src + c * 4, 4);
                            value = f;
                            break;
                        }
                        case 5121: // ubyte
                            value = src[c];
                            if (normalized) value /= 255.0f;
                            break;
                        case 5120: // byte
                            value = static_cast<std::int8_t>(src[c]);
                            if (normalized) value = std::max(value / 127.0f, -1.0f);
                            break;
                        case 5123: // ushort
                        {
                            std::uint16_t u;
                            std::memcpy(&u, src + c * 2, 2);
                            value = u;
                            if (normalized) value /= 65535.0f;
                            break;
                        }
                        case 5122: // short
                        {
                            std::int16_t s;
                            std::memcpy(&s, src + c * 2, 2);
                            value = s;
                            if (normalized) value = std::max(value / 32767.0f, -1.0f);
                            break;
                        }
                        case 5125: // uint
                        {
                            std::uint32_t u;
                            std::memcpy(&u, src + c * 4, 4);
                            value = static_cast<float>(u);
                            break;
                        }
                        }

                        out[i * comps + c] = value;
                    }
                }

                return out;
            }

            // Индексы читаем отдельно (uint32 без потери точности через float).
            std::vector<Types::u32> ReadIndices(const Gltf& g, std::size_t index)
            {
                const Json& acc = g.json.Find("accessors")->At(index);
                const std::size_t count = static_cast<std::size_t>(acc.Num("count", 0));
                const int componentType = static_cast<int>(acc.Num("componentType", 0));

                const Json& view = g.json.Find("bufferViews")->At(
                    static_cast<std::size_t>(acc.Num("bufferView", 0)));

                const std::vector<std::uint8_t>& buffer =
                    g.buffers.at(static_cast<std::size_t>(view.Num("buffer", 0)));

                const std::size_t base =
                    static_cast<std::size_t>(view.Num("byteOffset", 0)) +
                    static_cast<std::size_t>(acc.Num("byteOffset", 0));

                const std::size_t size = ComponentSize(componentType);
                std::size_t stride = static_cast<std::size_t>(view.Num("byteStride", 0));

                if (stride == 0)
                {
                    stride = size;
                }

                if (count > 0 && base + stride * (count - 1) + size > buffer.size())
                {
                    throw std::runtime_error("index accessor is out of buffer");
                }

                std::vector<Types::u32> out(count);

                for (std::size_t i = 0; i < count; ++i)
                {
                    const std::uint8_t* src = buffer.data() + base + i * stride;

                    if (componentType == 5121)
                    {
                        out[i] = src[0];
                    }
                    else if (componentType == 5123)
                    {
                        std::uint16_t u;
                        std::memcpy(&u, src, 2);
                        out[i] = u;
                    }
                    else if (componentType == 5125)
                    {
                        std::uint32_t u;
                        std::memcpy(&u, src, 4);
                        out[i] = u;
                    }
                    else
                    {
                        throw std::runtime_error("bad index componentType");
                    }
                }

                return out;
            }


            // -------------------------------------------------------------
            // Материалы и картинки
            // -------------------------------------------------------------
            ImageRGBA8 DecodeImage(const std::uint8_t* data, std::size_t size)
            {
                int w = 0, h = 0, n = 0;

                stbi_uc* pixels = stbi_load_from_memory(
                    data, static_cast<int>(size), &w, &h, &n, 4);

                if (!pixels)
                {
                    throw std::runtime_error(std::string("image decode failed: ") + stbi_failure_reason());
                }

                ImageRGBA8 image;
                image.width = static_cast<Types::u32>(w);
                image.height = static_cast<Types::u32>(h);
                image.pixels.assign(pixels, pixels + static_cast<std::size_t>(w) * h * 4);
                stbi_image_free(pixels);

                // Огромные текстуры (8K и больше) уменьшаем вдвое, пока не влезут в 4096.
                while (image.width > 4096 || image.height > 4096)
                {
                    const Types::u32 nw = std::max(1u, image.width / 2);
                    const Types::u32 nh = std::max(1u, image.height / 2);
                    std::vector<Types::u8> half(static_cast<std::size_t>(nw) * nh * 4);

                    for (Types::u32 y = 0; y < nh; ++y)
                    {
                        for (Types::u32 x = 0; x < nw; ++x)
                        {
                            const Types::u32 x0 = std::min(x * 2, image.width - 1);
                            const Types::u32 x1 = std::min(x * 2 + 1, image.width - 1);
                            const Types::u32 y0 = std::min(y * 2, image.height - 1);
                            const Types::u32 y1 = std::min(y * 2 + 1, image.height - 1);

                            for (int c = 0; c < 4; ++c)
                            {
                                const auto at = [&](Types::u32 px, Types::u32 py)
                                {
                                    return static_cast<Types::u32>(
                                        image.pixels[(static_cast<std::size_t>(py) * image.width + px) * 4 + c]);
                                };

                                half[(static_cast<std::size_t>(y) * nw + x) * 4 + c] =
                                    static_cast<Types::u8>((at(x0, y0) + at(x1, y0) + at(x0, y1) + at(x1, y1) + 2) / 4);
                            }
                        }
                    }

                    image.width = nw;
                    image.height = nh;
                    image.pixels = std::move(half);
                }

                return image;
            }

            // Возвращает индекс в mesh.images или -1.
            Types::i32 LoadImageForTexture(
                const Gltf& g, const std::string& directory, std::size_t textureIndex,
                MeshData& mesh, std::map<std::size_t, Types::i32>& cache)
            {
                const Json* textures = g.json.Find("textures");

                if (!textures || textureIndex >= textures->Size())
                {
                    return -1;
                }

                const Json& texture = textures->At(textureIndex);

                if (!texture.Has("source"))
                {
                    return -1; // например, только расширение с webp/basisu
                }

                const std::size_t imageIndex = static_cast<std::size_t>(texture.Num("source", 0));

                const auto found = cache.find(imageIndex);

                if (found != cache.end())
                {
                    return found->second;
                }

                Types::i32 result = -1;

                try
                {
                    const Json& imageJson = g.json.Find("images")->At(imageIndex);

                    if (imageJson.Has("bufferView"))
                    {
                        const Json& view = g.json.Find("bufferViews")->At(
                            static_cast<std::size_t>(imageJson.Num("bufferView", 0)));

                        const std::vector<std::uint8_t>& buffer =
                            g.buffers.at(static_cast<std::size_t>(view.Num("buffer", 0)));

                        const std::size_t offset = static_cast<std::size_t>(view.Num("byteOffset", 0));
                        const std::size_t length = static_cast<std::size_t>(view.Num("byteLength", 0));

                        if (offset + length > buffer.size())
                        {
                            throw std::runtime_error("image bufferView is out of buffer");
                        }

                        mesh.images.push_back(DecodeImage(buffer.data() + offset, length));
                        result = static_cast<Types::i32>(mesh.images.size() - 1);
                    }
                    else if (const Json* uri = imageJson.Find("uri"))
                    {
                        if (uri->string.compare(0, 5, "data:") == 0)
                        {
                            throw std::runtime_error("base64 image is not supported (export as .glb)");
                        }

                        const std::vector<std::uint8_t> file = ReadAll(directory + uri->string);
                        mesh.images.push_back(DecodeImage(file.data(), file.size()));
                        result = static_cast<Types::i32>(mesh.images.size() - 1);
                    }
                }
                catch (const std::exception& e)
                {
                    std::cerr << "[LoadGltf] texture " << imageIndex << " skipped: " << e.what() << "\n";
                }

                cache[imageIndex] = result;
                return result;
            }

            void LoadMaterials(const Gltf& g, const std::string& directory, MeshData& mesh)
            {
                std::map<std::size_t, Types::i32> imageCache;

                if (const Json* materials = g.json.Find("materials"))
                {
                    for (const Json& m : materials->array)
                    {
                        MaterialData material;

                        if (const Json* pbr = m.Find("pbrMetallicRoughness"))
                        {
                            if (const Json* factor = pbr->Find("baseColorFactor"))
                            {
                                for (int i = 0; i < 4 && i < static_cast<int>(factor->Size()); ++i)
                                {
                                    material.baseColorFactor[i] = static_cast<Types::f32>(factor->At(i).number);
                                }
                            }

                            material.metallic = static_cast<Types::f32>(pbr->Num("metallicFactor", 1.0));
                            material.roughness = static_cast<Types::f32>(pbr->Num("roughnessFactor", 1.0));

                            if (const Json* tex = pbr->Find("baseColorTexture"))
                            {
                                // Второй набор UV не поддерживаем.
                                if (static_cast<int>(tex->Num("texCoord", 0)) == 0)
                                {
                                    material.baseColorImage = LoadImageForTexture(
                                        g, directory, static_cast<std::size_t>(tex->Num("index", 0)), mesh, imageCache);
                                }
                            }
                        }

                        if (const Json* mode = m.Find("alphaMode"))
                        {
                            if (mode->string == "MASK")
                            {
                                material.alphaCutoff = static_cast<Types::f32>(m.Num("alphaCutoff", 0.5));
                            }
                            else if (mode->string == "BLEND")
                            {
                                // Честной прозрачности пока нет: отсекаем почти прозрачное.
                                material.alphaCutoff = 0.5f;
                            }
                        }

                        mesh.materials.push_back(material);
                    }
                }

                // Материал по умолчанию (primitive без material).
                mesh.materials.push_back(MaterialData{});
            }

            // -------------------------------------------------------------
            // Меши и узлы
            // -------------------------------------------------------------
            struct Builder
            {
                const Gltf& g;
                MeshData& mesh;
                bool allNormals = true;
                std::size_t depthGuard = 0;
                Types::u32 defaultMaterial = 0;

                void AddPrimitive(const Json& prim, const M4& world)
                {
                    const int mode = static_cast<int>(prim.Num("mode", 4));

                    if (mode != 4 && mode != 5 && mode != 6)
                    {
                        return; // точки и линии пропускаем
                    }

                    const Json* attributes = prim.Find("attributes");

                    if (!attributes || !attributes->Has("POSITION"))
                    {
                        return;
                    }

                    int pc = 0;
                    const std::vector<float> positions =
                        ReadAccessor(g, static_cast<std::size_t>(attributes->Num("POSITION", 0)), pc);

                    if (pc != 3)
                    {
                        return;
                    }

                    const std::size_t vertexCount = positions.size() / 3;

                    std::vector<float> normals;
                    std::vector<float> uvs;
                    int nc = 0, uc = 0;

                    if (attributes->Has("NORMAL"))
                    {
                        normals = ReadAccessor(g, static_cast<std::size_t>(attributes->Num("NORMAL", 0)), nc);
                    }

                    if (attributes->Has("TEXCOORD_0"))
                    {
                        uvs = ReadAccessor(g, static_cast<std::size_t>(attributes->Num("TEXCOORD_0", 0)), uc);
                    }

                    const bool hasNormals = nc == 3 && normals.size() == vertexCount * 3;
                    const bool hasUvs = uc == 2 && uvs.size() == vertexCount * 2;

                    if (!hasNormals)
                    {
                        allNormals = false;
                    }

                    // Отрицательный определитель (зеркальный масштаб) переворачивает обход
                    // треугольников и знак кофакторов, поэтому учитываем его для нормалей.
                    const double det =
                        world.m[0] * (world.m[5] * world.m[10] - world.m[9] * world.m[6]) -
                        world.m[4] * (world.m[1] * world.m[10] - world.m[9] * world.m[2]) +
                        world.m[8] * (world.m[1] * world.m[6] - world.m[5] * world.m[2]);
                    const bool flip = det < 0.0;

                    double nm[9];
                    NormalMatrix(world, nm);

                    if (flip)
                    {
                        for (double& value : nm)
                        {
                            value = -value;
                        }
                    }

                    const Types::u32 first = static_cast<Types::u32>(mesh.vertices.size());
                    const Types::u32 firstIndexOfPrimitive = static_cast<Types::u32>(mesh.indices.size());

                    for (std::size_t i = 0; i < vertexCount; ++i)
                    {
                        const double x = positions[i * 3 + 0];
                        const double y = positions[i * 3 + 1];
                        const double z = positions[i * 3 + 2];

                        Vertex v;
                        v.position[0] = static_cast<float>(world.m[0] * x + world.m[4] * y + world.m[8] * z + world.m[12]);
                        v.position[1] = static_cast<float>(world.m[1] * x + world.m[5] * y + world.m[9] * z + world.m[13]);
                        v.position[2] = static_cast<float>(world.m[2] * x + world.m[6] * y + world.m[10] * z + world.m[14]);

                        if (hasNormals)
                        {
                            const double nx = normals[i * 3 + 0];
                            const double ny = normals[i * 3 + 1];
                            const double nz = normals[i * 3 + 2];

                            double ox = nm[0] * nx + nm[1] * ny + nm[2] * nz;
                            double oy = nm[3] * nx + nm[4] * ny + nm[5] * nz;
                            double oz = nm[6] * nx + nm[7] * ny + nm[8] * nz;

                            const double len = std::sqrt(ox * ox + oy * oy + oz * oz);

                            if (len > 1e-12)
                            {
                                ox /= len; oy /= len; oz /= len;
                            }

                            v.normal[0] = static_cast<float>(ox);
                            v.normal[1] = static_cast<float>(oy);
                            v.normal[2] = static_cast<float>(oz);
                        }

                        if (hasUvs)
                        {
                            v.uv[0] = uvs[i * 2 + 0];
                            v.uv[1] = uvs[i * 2 + 1];
                        }

                        mesh.vertices.push_back(v);
                    }

                    // индексы
                    std::vector<Types::u32> indices;

                    if (prim.Has("indices"))
                    {
                        indices = ReadIndices(g, static_cast<std::size_t>(prim.Num("indices", 0)));
                    }
                    else
                    {
                        indices.resize(vertexCount);

                        for (std::size_t i = 0; i < vertexCount; ++i)
                        {
                            indices[i] = static_cast<Types::u32>(i);
                        }
                    }

                    const auto emit = [&](Types::u32 a, Types::u32 b, Types::u32 c)
                    {
                        if (a >= vertexCount || b >= vertexCount || c >= vertexCount)
                        {
                            return;
                        }

                        mesh.indices.push_back(first + a);
                        mesh.indices.push_back(first + (flip ? c : b));
                        mesh.indices.push_back(first + (flip ? b : c));
                    };

                    if (mode == 4)
                    {
                        for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
                        {
                            emit(indices[i], indices[i + 1], indices[i + 2]);
                        }
                    }
                    else if (mode == 5) // strip
                    {
                        for (std::size_t i = 0; i + 2 < indices.size(); ++i)
                        {
                            if (i % 2 == 0) emit(indices[i], indices[i + 1], indices[i + 2]);
                            else            emit(indices[i + 1], indices[i], indices[i + 2]);
                        }
                    }
                    else // fan
                    {
                        for (std::size_t i = 1; i + 1 < indices.size(); ++i)
                        {
                            emit(indices[0], indices[i], indices[i + 1]);
                        }
                    }

                    const Types::u32 added = static_cast<Types::u32>(mesh.indices.size()) - firstIndexOfPrimitive;

                    if (added > 0)
                    {
                        const Types::i32 materialIndex = static_cast<Types::i32>(prim.Num("material", -1));

                        SubMesh sub;
                        sub.firstIndex = firstIndexOfPrimitive;
                        sub.indexCount = added;
                        sub.material =
                            materialIndex >= 0 && materialIndex < static_cast<Types::i32>(mesh.materials.size())
                                ? static_cast<Types::u32>(materialIndex)
                                : defaultMaterial;

                        // Соседние куски с тем же материалом склеиваем.
                        if (!mesh.submeshes.empty() &&
                            mesh.submeshes.back().material == sub.material &&
                            mesh.submeshes.back().firstIndex + mesh.submeshes.back().indexCount == sub.firstIndex)
                        {
                            mesh.submeshes.back().indexCount += sub.indexCount;
                        }
                        else
                        {
                            mesh.submeshes.push_back(sub);
                        }
                    }
                }

                void VisitNode(std::size_t nodeIndex, const M4& parent)
                {
                    if (++depthGuard > 100000)
                    {
                        throw std::runtime_error("glTF: node graph is too deep (cycle?)");
                    }

                    const Json& node = g.json.Find("nodes")->At(nodeIndex);

                    M4 local;

                    if (const Json* matrix = node.Find("matrix"))
                    {
                        for (int i = 0; i < 16 && i < static_cast<int>(matrix->Size()); ++i)
                        {
                            local.m[i] = matrix->At(i).number;
                        }
                    }
                    else
                    {
                        double t[3] = {0, 0, 0};
                        double q[4] = {0, 0, 0, 1};
                        double s[3] = {1, 1, 1};

                        if (const Json* v = node.Find("translation"))
                            for (int i = 0; i < 3; ++i) t[i] = v->At(i).number;

                        if (const Json* v = node.Find("rotation"))
                            for (int i = 0; i < 4; ++i) q[i] = v->At(i).number;

                        if (const Json* v = node.Find("scale"))
                            for (int i = 0; i < 3; ++i) s[i] = v->At(i).number;

                        local = FromTrs(t, q, s);
                    }

                    const M4 world = Mul(parent, local);

                    if (node.Has("mesh"))
                    {
                        const Json& meshJson = g.json.Find("meshes")->At(
                            static_cast<std::size_t>(node.Num("mesh", 0)));

                        if (const Json* prims = meshJson.Find("primitives"))
                        {
                            for (const Json& prim : prims->array)
                            {
                                AddPrimitive(prim, world);
                            }
                        }
                    }

                    if (const Json* children = node.Find("children"))
                    {
                        for (const Json& child : children->array)
                        {
                            VisitNode(static_cast<std::size_t>(child.number), world);
                        }
                    }
                }
            };
        }

        Types::b8 LoadGltf(const char* path, MeshData& out)
        {
            try
            {
                const Gltf g = Load(path);

                MeshData mesh;

                LoadMaterials(g, DirectoryOf(path), mesh);

                Builder builder{g, mesh};
                builder.defaultMaterial = static_cast<Types::u32>(mesh.materials.size() - 1);

                const M4 identity;

                const Json* scenes = g.json.Find("scenes");
                const Json* nodes = g.json.Find("nodes");

                if (!nodes)
                {
                    throw std::runtime_error("glTF has no nodes");
                }

                if (scenes && scenes->Size() > 0)
                {
                    const Json& scene = scenes->At(static_cast<std::size_t>(g.json.Num("scene", 0)));

                    if (const Json* roots = scene.Find("nodes"))
                    {
                        for (const Json& root : roots->array)
                        {
                            builder.VisitNode(static_cast<std::size_t>(root.number), identity);
                        }
                    }
                }
                else
                {
                    // Нет сцены: берём все узлы без родителя.
                    std::vector<bool> isChild(nodes->Size(), false);

                    for (const Json& n : nodes->array)
                    {
                        if (const Json* c = n.Find("children"))
                        {
                            for (const Json& ch : c->array)
                            {
                                const std::size_t id = static_cast<std::size_t>(ch.number);

                                if (id < isChild.size())
                                {
                                    isChild[id] = true;
                                }
                            }
                        }
                    }

                    for (std::size_t i = 0; i < isChild.size(); ++i)
                    {
                        if (!isChild[i])
                        {
                            builder.VisitNode(i, identity);
                        }
                    }
                }

                if (mesh.vertices.empty() || mesh.indices.empty())
                {
                    throw std::runtime_error("no triangle geometry found");
                }

                if (!builder.allNormals)
                {
                    ComputeMeshSmoothNormals(mesh);
                }

                NormalizeMeshToUnit(mesh);

                out = std::move(mesh);
                return true;
            }
            catch (const std::exception& e)
            {
                std::cerr << "[LoadGltf] " << path << ": " << e.what() << "\n";
                return false;
            }
        }
    }
}
