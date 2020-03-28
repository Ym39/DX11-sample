#pragma once

//INCLUDES
#include<DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_position;

public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();
};

