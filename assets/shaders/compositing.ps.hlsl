#include "compositing.common.hlsl"

float4 PS( PSIn IN ) : SV_Target0
{
    return float4(NormalTexture.Sample(BilinearSampler, IN.UV.xy).xyz, 1.f);
}
