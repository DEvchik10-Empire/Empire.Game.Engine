Запуск сцены (папка Empire.Engine):

    xmake run Empire.Rendering.Test.Scene                 # Assets/asset.glb, если есть, иначе куб
    xmake run Empire.Rendering.Test.Scene --show=cube     # куб (--mesh=sphere|knot - другие примитивы)
    xmake run Empire.Rendering.Test.Scene --show=model    # Assets/asset.glb
    xmake run Empire.Rendering.Test.Scene --show=none     # только небо
    xmake run Empire.Rendering.Test.Scene --model=/путь/файл.glb
    xmake run Empire.Rendering.Test.Scene --spin=y --spin-speed=-0.9   # скорость и направление вращения
    xmake run Empire.Rendering.Test.Scene --camera=orbit  # камера облетает (по умолчанию стоит)

Загрузчик glTF: Empire.Mesh/GltfLoader.cpp (картинки декодирует stb_image, ThirdParty/stb).
Поддерживается: геометрия с иерархией узлов, base color (текстура + множитель), металл/шероховатость
