// Pro-Pixel-Farbdaten an den Pixelshader übergeben.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// Eine Pass-Through-Funktion für die (interpolierten) Farbdaten.
float4 main(PixelShaderInput input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}
