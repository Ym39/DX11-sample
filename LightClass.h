#pragma once

//INCLUDES
#include<DirectXMath.h>
using namespace DirectX;

class LightClass
{
public:
	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);
	void SetDirection(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetPosition();
	XMFLOAT4 GetSpecularColor();
	float GetSpecularPower();
	XMFLOAT3 GetDirection();

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_position;
	XMFLOAT4 m_specularColor;
	XMFLOAT3 m_direction;
	float m_specularPower;

public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();
};

