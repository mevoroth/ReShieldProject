#include "compositing.common.hlsl"

float4 PS( PSIn IN ) : SV_Target0
{
	return IN.UV.x;
}
