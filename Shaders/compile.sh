#!/usr/bin/env bash
# Ручная компиляция шейдеров HLSL -> SPIR-V (если не хочется через xmake).
#
#   ./Shaders/compile.sh [папка_вывода]
#
# Нужен DXC (DirectXShaderCompiler): пакет `directx-shader-compiler`
# (Arch: directx-shader-compiler, Debian/Ubuntu: через LunarG Vulkan SDK).
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT="${1:-$HERE/../build/shaders}"
DXC="${DXC:-dxc}"

mkdir -p "$OUT"

compile() # файл профиль
{
    local source="$1" profile="$2"
    local name; name="$(basename "$source" .hlsl)"

    echo "dxc  $name  ($profile)"
    "$DXC" -spirv -fspv-target-env=vulkan1.1 \
        -T "$profile" -E main -I "$HERE" "$source" -Fo "$OUT/$name.spv"
}

for file in "$HERE"/*.vs.hlsl; do compile "$file" vs_6_0; done
for file in "$HERE"/*.ps.hlsl; do compile "$file" ps_6_0; done

echo "Готово: $OUT"
