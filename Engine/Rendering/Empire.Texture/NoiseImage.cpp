#include "NoiseImage.hpp"

#include <algorithm>
#include <cmath>

namespace Engine
{
    namespace Rendering
    {
        namespace
        {
            constexpr double Pi = 3.14159265358979323846;

            // Целочисленный хэш -> [0, 1).
            double Hash2(Types::i64 ix, Types::i64 iy, Types::u32 seed)
            {
                Types::u32 h =
                    static_cast<Types::u32>(ix) * 374761393u +
                    static_cast<Types::u32>(iy) * 668265263u +
                    seed * 2246822519u;

                h = (h ^ (h >> 13)) * 1274126177u;
                h = h ^ (h >> 16);

                return static_cast<double>(h & 0xFFFFFFu) / 16777216.0;
            }

            Types::i64 Wrap(Types::i64 value, Types::i64 period)
            {
                const Types::i64 r = value % period;
                return r < 0 ? r + period : r;
            }

            double Fade(double t)
            {
                return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
            }

            // Шум Перлина, бесшовный с периодом period ячеек.
            double Perlin(double x, double y, Types::i64 period, Types::u32 seed)
            {
                const Types::i64 ix = static_cast<Types::i64>(std::floor(x));
                const Types::i64 iy = static_cast<Types::i64>(std::floor(y));
                const double fx = x - static_cast<double>(ix);
                const double fy = y - static_cast<double>(iy);

                const auto gradient = [&](Types::i64 cx, Types::i64 cy, double dx, double dy)
                {
                    const double a = Hash2(Wrap(cx, period), Wrap(cy, period), seed) * 2.0 * Pi;
                    return std::cos(a) * dx + std::sin(a) * dy;
                };

                const double n00 = gradient(ix,     iy,     fx,       fy);
                const double n10 = gradient(ix + 1, iy,     fx - 1.0, fy);
                const double n01 = gradient(ix,     iy + 1, fx,       fy - 1.0);
                const double n11 = gradient(ix + 1, iy + 1, fx - 1.0, fy - 1.0);

                const double u = Fade(fx);
                const double v = Fade(fy);

                return (n00 + (n10 - n00) * u) * (1.0 - v) + (n01 + (n11 - n01) * u) * v;
            }

            // Растягивает значения на 0..1, затем контраст (сигмоида), сдвиг, байты.
            ImageR8 ToImage(
                const std::vector<double>& values,
                Types::u32 size,
                double gain,
                Types::u32 shiftX = 0,
                Types::u32 shiftY = 0,
                double bias = 0.0)
            {
                const auto [minIt, maxIt] = std::minmax_element(values.begin(), values.end());
                const double lo = *minIt;
                const double range = std::max(*maxIt - lo, 1e-12);

                ImageR8 image;
                image.width = size;
                image.height = size;
                image.pixels.resize(values.size());

                for (Types::u32 y = 0; y < size; ++y)
                {
                    for (Types::u32 x = 0; x < size; ++x)
                    {
                        // out[y][x] = in[y + shiftY][x + shiftX] (с заворотом)
                        const std::size_t source =
                            static_cast<std::size_t>((y + shiftY) % size) * size + ((x + shiftX) % size);

                        double v = (values[source] - lo) / range;

                        if (gain > 0.0)
                        {
                            v = 1.0 / (1.0 + std::exp(-(gain * (v - 0.5) + bias)));
                        }

                        image.pixels[static_cast<std::size_t>(y) * size + x] =
                            static_cast<Types::u8>(std::lround(std::clamp(v, 0.0, 1.0) * 255.0));
                    }
                }

                return image;
            }
        }

        ImageR8 MakeFbmNoiseImage(
            Types::u32 size, Types::u32 baseCells, Types::u32 octaves,
            Types::u32 seed, Types::f32 gain, Types::u32 shiftX, Types::u32 shiftY, Types::f32 bias)
        {
            std::vector<double> values(static_cast<std::size_t>(size) * size, 0.0);

            double amplitude = 1.0;

            for (Types::u32 octave = 0; octave < octaves; ++octave)
            {
                const Types::i64 period = static_cast<Types::i64>(baseCells) << octave;

                for (Types::u32 y = 0; y < size; ++y)
                {
                    for (Types::u32 x = 0; x < size; ++x)
                    {
                        const double px = static_cast<double>(x) / size * static_cast<double>(period);
                        const double py = static_cast<double>(y) / size * static_cast<double>(period);

                        values[static_cast<std::size_t>(y) * size + x] +=
                            Perlin(px, py, period, seed + octave) * amplitude;
                    }
                }

                amplitude *= 0.5;
            }

            return ToImage(values, size, gain, shiftX, shiftY, bias);
        }

        ImageR8 MakeCellularImage(Types::u32 size, Types::u32 cells, Types::u32 seed)
        {
            std::vector<double> values(static_cast<std::size_t>(size) * size, 0.0);

            for (Types::u32 y = 0; y < size; ++y)
            {
                for (Types::u32 x = 0; x < size; ++x)
                {
                    const double px = static_cast<double>(x) / size * cells;
                    const double py = static_cast<double>(y) / size * cells;

                    const Types::i64 ix = static_cast<Types::i64>(std::floor(px));
                    const Types::i64 iy = static_cast<Types::i64>(std::floor(py));
                    const double fx = px - static_cast<double>(ix);
                    const double fy = py - static_cast<double>(iy);

                    double best = 9.0;

                    for (int dy = -1; dy <= 1; ++dy)
                    {
                        for (int dx = -1; dx <= 1; ++dx)
                        {
                            const Types::i64 cx = Wrap(ix + dx, cells);
                            const Types::i64 cy = Wrap(iy + dy, cells);

                            const double jx = Hash2(cx, cy, seed);
                            const double jy = Hash2(cx, cy, seed + 77);

                            best = std::min(best, std::hypot(dx + jx - fx, dy + jy - fy));
                        }
                    }

                    values[static_cast<std::size_t>(y) * size + x] = best;
                }
            }

            return ToImage(values, size, 0.0);
        }

        ImageR8 MakeShootingStarImage(Types::u32 size)
        {
            ImageR8 image;
            image.width = size;
            image.height = size;
            image.pixels.assign(static_cast<std::size_t>(size) * size, 0);

            for (Types::u32 y = 0; y < size; ++y)
            {
                const double v = (static_cast<double>(y) + 0.5) / size;

                // Голова около v = 0.1, дальше плавный хвост до ~0.85.
                const double head = std::clamp((v - 0.06) / 0.04, 0.0, 1.0);
                const double tail = std::pow(std::clamp(1.0 - (v - 0.1) / 0.75, 0.0, 1.0), 1.6);
                const double along = v < 0.1 ? head : tail;

                // Тоньше к хвосту.
                const double width = 0.022 + 0.018 * (1.0 - std::clamp((v - 0.1) / 0.75, 0.0, 1.0));

                for (Types::u32 x = 0; x < size; ++x)
                {
                    const double u = (static_cast<double>(x) + 0.5) / size - 0.5;
                    const double across = std::exp(-(u * u) / (2.0 * width * width));

                    image.pixels[static_cast<std::size_t>(y) * size + x] =
                        static_cast<Types::u8>(std::lround(std::clamp(along * across, 0.0, 1.0) * 255.0));
                }
            }

            return image;
        }
    }
}
