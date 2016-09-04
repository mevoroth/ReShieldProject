#include "opaque.common.hlsl"

PSOut PS( PSIn IN ) : SV_TARGET
{
	PSOut OUT = (PSOut)0;

	OUT.Diffuse						= float4(DiffuseTexture.Sample(OpaqueSampler, IN.UV).xyz, 1.0f);
	OUT.Emissive					= float4(EmissiveTexture.Sample(OpaqueSampler, IN.UV).xyz, 1.0f);
	OUT.MetallicRoughnessSpecular	= float4(MetallicRoughnessSpecularTexture.Sample(OpaqueSampler, IN.UV).xyz, 0.0f);
	OUT.Normal						= float4(NormalTexture.Sample(OpaqueSampler, IN.UV).xyz, 0.0f);

	return OUT;
}
