/*
 * Небо: вершинный шейдер.
 * Полноэкранный треугольник без вершинного буфера (вершины по SV_VertexID).
 */

struct VSOutput
{
    float4 position : SV_Position;
    float2 ndc      : TEXCOORD0;
};

VSOutput main(uint vertexId : SV_VertexID)
{
    // id 0,1,2 -> (0,0) (2,0) (0,2) -> перекрывает весь экран.
    float2 uv = float2((vertexId << 1) & 2, vertexId & 2);

    VSOutput output;
    output.ndc = uv * 2.0 - 1.0;
    output.position = float4(output.ndc, 1.0, 1.0);

    return output;
}
