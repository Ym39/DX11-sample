////////////////////////////////////////////////////////////////////////////////
// Filename: Light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer LightBuffer
{
    float4 diffuseColor;
	float3 lightPosition;
	float padding;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float3 diffuse : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);

	// 빛의 방향 벡터 계산
	float3 lightDir = normalize((float3)output.position - lightPosition);

	//뷰, 투영 매트릭스에 대한 정점 위치 계산
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	//법선 벡터 계산
	float3 worldNormal = mul(input.normal, (float3x3)worldMatrix);
	worldNormal = normalize(worldNormal);

	//빛의 양을 계산
	output.diffuse = dot(-lightDir, worldNormal);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}