
cbuffer MVP0 : register(b0)
{
	matrix mtWld;
	matrix mtViw;
	matrix mtPrj;
}

struct INPUT_PS
{
	float4 p : SV_POSITION;
	float4 d : COLOR0;
};

INPUT_PS main_vs(float4 pos : POSITION, float4 dif : COLOR)
{
	INPUT_PS o = (INPUT_PS) 0;
	o.p = mul(mtWld, pos);
	o.p = mul(mtViw, o.p);
	o.p = mul(mtPrj, o.p);
	o.d = dif;
	return o;
}

float4 main_ps(INPUT_PS vsi) : SV_Target
{
	return vsi.d;
}
