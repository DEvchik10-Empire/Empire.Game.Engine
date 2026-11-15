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


        namespace
        {
            // Классический 2D симплекс-шум (Gustavson), хэш вместо таблицы.
            double SimplexGrad(Types::i64 ix, Types::i64 iy, Types::u32 seed, double dx, double dy)
            {
                static const double gx[8] = {1, -1, 1, -1, 1, -1, 0, 0};
                static const double gy[8] = {1, 1, -1, -1, 0, 0, 1, -1};

                const Types::u32 h =
                    static_cast<Types::u32>(static_cast<Types::u32>(Hash2(ix, iy, seed) * 16777216.0) % 8u);

                return gx[h] * dx + gy[h] * dy;
            }

            double Simplex2(double x, double y, Types::u32 seed)
            {
                const double F2 = 0.5 * (std::sqrt(3.0) - 1.0);
                const double G2 = (3.0 - std::sqrt(3.0)) / 6.0;

                const double s = (x + y) * F2;
                const Types::i64 i = static_cast<Types::i64>(std::floor(x + s));
                const Types::i64 j = static_cast<Types::i64>(std::floor(y + s));

                const double t = static_cast<double>(i + j) * G2;
                const double x0 = x - (static_cast<double>(i) - t);
                const double y0 = y - (static_cast<double>(j) - t);

                const Types::i64 i1 = x0 > y0 ? 1 : 0;
                const Types::i64 j1 = x0 > y0 ? 0 : 1;

                const double x1 = x0 - static_cast<double>(i1) + G2;
                const double y1 = y0 - static_cast<double>(j1) + G2;
                const double x2 = x0 - 1.0 + 2.0 * G2;
                const double y2 = y0 - 1.0 + 2.0 * G2;

                const auto contribution = [&](double dx, double dy, Types::i64 gi, Types::i64 gj)
                {
                    double a = 0.5 - dx * dx - dy * dy;

                    if (a < 0.0)
                    {
                        return 0.0;
                    }

                    a *= a;

                    return a * a * SimplexGrad(gi, gj, seed, dx, dy);
                };

                return 70.0 * (
                    contribution(x0, y0, i, j) +
                    contribution(x1, y1, i + i1, j + j1) +
                    contribution(x2, y2, i + 1, j + 1));
            }
        }

        ImageR8 MakeSimplexFbmImage(
            Types::u32 size, Types::u32 octaves, Types::u32 seed, Types::f32 skirt, Types::f32 frequency)
        {
            // Исходник больше итоговой картинки на skirt (как в Godot), затем
            // края сшиваются перекрёстным смешиванием, чтобы картинка тайлилась.
            const Types::u32 skirtPx = static_cast<Types::u32>(std::lround(static_cast<double>(size) * skirt));
            const Types::u32 source = size + skirtPx;

            std::vector<double> src(static_cast<std::size_t>(source) * source, 0.0);

            double bounding = 0.0;
            double a = 1.0;

            for (Types::u32 o = 0; o < octaves; ++o)
            {
                bounding += a;
                a *= 0.5;
            }

            for (Types::u32 y = 0; y < source; ++y)
            {
                for (Types::u32 x = 0; x < source; ++x)
                {
                    double sum = 0.0;
                    double amp = 1.0;
                    double freq = frequency;

                    for (Types::u32 o = 0; o < octaves; ++o)
                    {
                        sum += Simplex2(x * freq, y * freq, seed + o * 1013u) * amp;
                        amp *= 0.5;
                        freq *= 2.0;
                    }

                    src[static_cast<std::size_t>(y) * source + x] = sum / bounding;
                }
            }

            std::vector<double> values(static_cast<std::size_t>(size) * size, 0.0);

            for (Types::u32 y = 0; y < size; ++y)
            {
                for (Types::u32 x = 0; x < size; ++x)
                {
                    const double wx = skirtPx > 0 && x < skirtPx ? static_cast<double>(x) / skirtPx : 1.0;
                    const double wy = skirtPx > 0 && y < skirtPx ? static_cast<double>(y) / skirtPx : 1.0;

                    const auto at = [&](Types::u32 px, Types::u32 py)
                    {
                        return src[static_cast<std::size_t>(py) * source + px];
                    };

                    double v = at(x, y) * wx * wy;

                    if (skirtPx > 0)
                    {
                        const Types::u32 xo = x < skirtPx ? x + size : x;
                        const Types::u32 yo = y < skirtPx ? y + size : y;

                        // at(x+size) только там, где x < skirtPx (иначе вес 0)
                        v = at(x, y) * wx * wy
                          + (x < skirtPx ? at(xo, y) * (1.0 - wx) * wy : 0.0)
                          + (y < skirtPx ? at(x, yo) * wx * (1.0 - wy) : 0.0)
                          + (x < skirtPx && y < skirtPx ? at(xo, yo) * (1.0 - wx) * (1.0 - wy) : 0.0);
                    }

                    values[static_cast<std::size_t>(y) * size + x] = v;
                }
            }

            return ToImage(values, size, 0.0);
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
