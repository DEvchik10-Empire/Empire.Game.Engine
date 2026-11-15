#include "Mesh.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>

namespace Engine
{
    namespace Rendering
    {
        namespace
        {
            constexpr Types::f32 Pi = 3.14159265358979f;

            struct V3
            {
                Types::f32 x = 0, y = 0, z = 0;
            };

            V3 operator+(V3 a, V3 b) { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
            V3 operator-(V3 a, V3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
            V3 operator*(V3 a, Types::f32 s) { return {a.x * s, a.y * s, a.z * s}; }

            V3 Cross(V3 a, V3 b)
            {
                return {
                    a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x};
            }

            Types::f32 Dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

            V3 Normalize(V3 v)
            {
                const Types::f32 length = std::sqrt(Dot(v, v));
                return length > 1e-12f ? v * (1.0f / length) : V3{0, 1, 0};
            }

            Vertex MakeVertex(V3 position, V3 normal, Types::f32 u, Types::f32 v)
            {
                Vertex vertex;
                vertex.position[0] = position.x;
                vertex.position[1] = position.y;
                vertex.position[2] = position.z;
                vertex.normal[0] = normal.x;
                vertex.normal[1] = normal.y;
                vertex.normal[2] = normal.z;
                vertex.uv[0] = u;
                vertex.uv[1] = v;
                return vertex;
            }

            // Центр в ноль, радиус 1.
            void NormalizeToUnit(MeshData& mesh)
            {
                if (mesh.vertices.empty())
                {
                    return;
                }

                V3 minimum{1e30f, 1e30f, 1e30f};
                V3 maximum{-1e30f, -1e30f, -1e30f};

                for (const Vertex& vertex : mesh.vertices)
                {
                    minimum.x = std::min(minimum.x, vertex.position[0]);
                    minimum.y = std::min(minimum.y, vertex.position[1]);
                    minimum.z = std::min(minimum.z, vertex.position[2]);
                    maximum.x = std::max(maximum.x, vertex.position[0]);
                    maximum.y = std::max(maximum.y, vertex.position[1]);
                    maximum.z = std::max(maximum.z, vertex.position[2]);
                }

                const V3 center = (minimum + maximum) * 0.5f;

                Types::f32 radius = 0.0f;

                for (const Vertex& vertex : mesh.vertices)
                {
                    const V3 p = V3{vertex.position[0], vertex.position[1], vertex.position[2]} - center;
                    radius = std::max(radius, std::sqrt(Dot(p, p)));
                }

                const Types::f32 scale = radius > 1e-12f ? 1.0f / radius : 1.0f;

                for (Vertex& vertex : mesh.vertices)
                {
                    vertex.position[0] = (vertex.position[0] - center.x) * scale;
                    vertex.position[1] = (vertex.position[1] - center.y) * scale;
                    vertex.position[2] = (vertex.position[2] - center.z) * scale;
                }
            }

            void ComputeSmoothNormals(MeshData& mesh)
            {
                std::vector<V3> sums(mesh.vertices.size());

                for (std::size_t i = 0; i + 2 < mesh.indices.size(); i += 3)
                {
                    const Types::u32 ia = mesh.indices[i];
                    const Types::u32 ib = mesh.indices[i + 1];
                    const Types::u32 ic = mesh.indices[i + 2];

                    const auto position = [&](Types::u32 index)
                    {
                        const Vertex& v = mesh.vertices[index];
                        return V3{v.position[0], v.position[1], v.position[2]};
                    };

                    // Длина векторного произведения = 2 * площадь: большие
                    // треугольники влияют на нормаль сильнее.
                    const V3 n = Cross(position(ib) - position(ia), position(ic) - position(ia));

                    sums[ia] = sums[ia] + n;
                    sums[ib] = sums[ib] + n;
                    sums[ic] = sums[ic] + n;
                }

                for (std::size_t i = 0; i < mesh.vertices.size(); ++i)
                {
                    const V3 n = Normalize(sums[i]);
                    mesh.vertices[i].normal[0] = n.x;
                    mesh.vertices[i].normal[1] = n.y;
                    mesh.vertices[i].normal[2] = n.z;
                }
            }
        }

        MeshData MakeCube()
        {
            MeshData mesh;

            // Для каждой грани: нормаль и два ребра.
            struct Face { V3 normal, right, up; };

            const Face faces[6] =
            {
                {{ 1, 0, 0}, {0, 0, -1}, {0, 1, 0}},
                {{-1, 0, 0}, {0, 0,  1}, {0, 1, 0}},
                {{ 0, 1, 0}, {1, 0,  0}, {0, 0, -1}},
                {{ 0,-1, 0}, {1, 0,  0}, {0, 0,  1}},
                {{ 0, 0, 1}, {1, 0,  0}, {0, 1, 0}},
                {{ 0, 0,-1}, {-1,0,  0}, {0, 1, 0}},
            };

            const Types::f32 h = 0.57735f; // 1/sqrt(3): радиус ~1

            for (const Face& face : faces)
            {
                const Types::u32 base = static_cast<Types::u32>(mesh.vertices.size());

                const V3 corners[4] =
                {
                    face.normal * h - face.right * h - face.up * h,
                    face.normal * h + face.right * h - face.up * h,
                    face.normal * h + face.right * h + face.up * h,
                    face.normal * h - face.right * h + face.up * h,
                };

                const Types::f32 uvs[4][2] = {{0, 1}, {1, 1}, {1, 0}, {0, 0}};

                for (int i = 0; i < 4; ++i)
                {
                    mesh.vertices.push_back(MakeVertex(corners[i], face.normal, uvs[i][0], uvs[i][1]));
                }

                const Types::u32 quad[6] = {0, 1, 2, 0, 2, 3};

                for (const Types::u32 index : quad)
                {
                    mesh.indices.push_back(base + index);
                }
            }

            return mesh;
        }

        MeshData MakeSphere(Types::u32 segments, Types::u32 rings)
        {
            MeshData mesh;

            segments = std::max<Types::u32>(segments, 3);
            rings = std::max<Types::u32>(rings, 2);

            for (Types::u32 y = 0; y <= rings; ++y)
            {
                const Types::f32 v = static_cast<Types::f32>(y) / static_cast<Types::f32>(rings);
                const Types::f32 theta = v * Pi;

                for (Types::u32 x = 0; x <= segments; ++x)
                {
                    const Types::f32 u = static_cast<Types::f32>(x) / static_cast<Types::f32>(segments);
                    const Types::f32 phi = u * 2.0f * Pi;

                    const V3 n{
                        std::sin(theta) * std::cos(phi),
                        std::cos(theta),
                        std::sin(theta) * std::sin(phi)};

                    mesh.vertices.push_back(MakeVertex(n, n, u, v));
                }
            }

            const Types::u32 stride = segments + 1;

            for (Types::u32 y = 0; y < rings; ++y)
            {
                for (Types::u32 x = 0; x < segments; ++x)
                {
                    const Types::u32 a = y * stride + x;
                    const Types::u32 b = a + stride;

                    // Снаружи против часовой стрелки.
                    mesh.indices.insert(mesh.indices.end(), {a, a + 1, b, a + 1, b + 1, b});
                }
            }

            return mesh;
        }

        MeshData MakeTorusKnot(
            Types::u32 tubularSegments,
            Types::u32 radialSegments,
            Types::u32 p,
            Types::u32 q)
        {
            MeshData mesh;

            tubularSegments = std::max<Types::u32>(tubularSegments, 8);
            radialSegments = std::max<Types::u32>(radialSegments, 3);

            const Types::f32 tube = 0.35f;

            const auto curve = [&](Types::f32 u)
            {
                const Types::f32 qu = static_cast<Types::f32>(q) * u;
                const Types::f32 pu = static_cast<Types::f32>(p) * u;
                const Types::f32 r = 2.0f + std::cos(qu);

                return V3{r * std::cos(pu), r * std::sin(pu), -std::sin(qu)};
            };

            for (Types::u32 i = 0; i <= tubularSegments; ++i)
            {
                const Types::f32 u =
                    static_cast<Types::f32>(i) / static_cast<Types::f32>(tubularSegments) *
                    static_cast<Types::f32>(p) * 2.0f * Pi;

                const V3 p1 = curve(u);
                const V3 p2 = curve(u + 0.01f);

                const V3 tangent = p2 - p1;
                V3 normal = p2 + p1;
                const V3 binormal = Normalize(Cross(tangent, normal));
                normal = Normalize(Cross(binormal, tangent));

                for (Types::u32 j = 0; j <= radialSegments; ++j)
                {
                    const Types::f32 v =
                        static_cast<Types::f32>(j) / static_cast<Types::f32>(radialSegments) * 2.0f * Pi;

                    const Types::f32 cx = -tube * std::cos(v);
                    const Types::f32 cy = tube * std::sin(v);

                    const V3 position = p1 + normal * cx + binormal * cy;
                    const V3 outward = Normalize(position - p1);

                    mesh.vertices.push_back(MakeVertex(
                        position,
                        outward,
                        static_cast<Types::f32>(i) / static_cast<Types::f32>(tubularSegments),
                        static_cast<Types::f32>(j) / static_cast<Types::f32>(radialSegments)));
                }
            }

            const Types::u32 stride = radialSegments + 1;

            for (Types::u32 i = 0; i < tubularSegments; ++i)
            {
                for (Types::u32 j = 0; j < radialSegments; ++j)
                {
                    const Types::u32 a = i * stride + j;
                    const Types::u32 b = (i + 1) * stride + j;

                    mesh.indices.insert(mesh.indices.end(), {a, b, a + 1, b, b + 1, a + 1});
                }
            }

            NormalizeToUnit(mesh);
            return mesh;
        }

        Types::b8 LoadObj(const char* path, MeshData& out)
        {
            std::ifstream file(path);

            if (!file)
            {
                return false;
            }

            std::vector<V3> positions;
            std::vector<V3> normals;
            std::vector<std::pair<Types::f32, Types::f32>> uvs;

            using Key = std::tuple<int, int, int>;
            std::map<Key, Types::u32> unique;

            MeshData mesh;
            Types::b8 hasNormals = false;

            // Индекс в .obj: 1-based, отрицательный - от конца списка, 0 - "нет".
            const auto resolve = [](int index, std::size_t count) -> int
            {
                if (index > 0)
                {
                    return index - 1;
                }

                if (index < 0)
                {
                    return static_cast<int>(count) + index;
                }

                return -1;
            };

            std::string line;

            while (std::getline(file, line))
            {
                std::istringstream stream(line);
                std::string tag;
                stream >> tag;

                if (tag == "v")
                {
                    V3 v;
                    stream >> v.x >> v.y >> v.z;
                    positions.push_back(v);
                }
                else if (tag == "vn")
                {
                    V3 v;
                    stream >> v.x >> v.y >> v.z;
                    normals.push_back(v);
                }
                else if (tag == "vt")
                {
                    Types::f32 u = 0, v = 0;
                    stream >> u >> v;
                    uvs.emplace_back(u, v);
                }
                else if (tag == "f")
                {
                    std::vector<Types::u32> face;
                    std::string token;

                    while (stream >> token)
                    {
                        int vi = 0, ti = 0, ni = 0;

                        // Форматы: v, v/t, v//n, v/t/n
                        const char* s = token.c_str();
                        char* end = nullptr;

                        vi = static_cast<int>(std::strtol(s, &end, 10));

                        if (*end == '/')
                        {
                            s = end + 1;

                            if (*s != '/')
                            {
                                ti = static_cast<int>(std::strtol(s, &end, 10));
                                s = end;
                            }

                            if (*s == '/')
                            {
                                ni = static_cast<int>(std::strtol(s + 1, &end, 10));
                            }
                        }

                        const int pi = resolve(vi, positions.size());
                        const int tii = resolve(ti, uvs.size());
                        const int nii = resolve(ni, normals.size());

                        if (pi < 0 || pi >= static_cast<int>(positions.size()))
                        {
                            continue;
                        }

                        const Key key{pi, tii, nii};
                        const auto found = unique.find(key);

                        if (found != unique.end())
                        {
                            face.push_back(found->second);
                            continue;
                        }

                        V3 normal{0, 1, 0};
                        Types::f32 u = 0, v = 0;

                        if (nii >= 0 && nii < static_cast<int>(normals.size()))
                        {
                            normal = normals[nii];
                            hasNormals = true;
                        }

                        if (tii >= 0 && tii < static_cast<int>(uvs.size()))
                        {
                            u = uvs[tii].first;
                            v = uvs[tii].second;
                        }

                        const Types::u32 index = static_cast<Types::u32>(mesh.vertices.size());
                        mesh.vertices.push_back(MakeVertex(positions[pi], normal, u, v));
                        unique.emplace(key, index);
                        face.push_back(index);
                    }

                    // Веер: (0,1,2), (0,2,3), ...
                    for (std::size_t i = 1; i + 1 < face.size(); ++i)
                    {
                        mesh.indices.push_back(face[0]);
                        mesh.indices.push_back(face[i]);
                        mesh.indices.push_back(face[i + 1]);
                    }
                }
            }

            if (mesh.vertices.empty() || mesh.indices.empty())
            {
                return false;
            }

            // Нормали из файла нужны ВСЕ; если хоть у части нет - считаем сами.
            if (!hasNormals || normals.empty())
            {
                ComputeSmoothNormals(mesh);
            }

            NormalizeToUnit(mesh);

            out = std::move(mesh);
            return true;
        }
    }
}
