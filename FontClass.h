#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<fstream>
using namespace DirectX;
using namespace std;

#include"TextureClass.h"

class FontClass
{
private: 
	struct  FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char*, char*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char*);
	void ReleaseTexture();

private:
	FontType* m_Font;
	TextureClass* m_Texture;
	const unsigned int m_charNum = 95;

public: 
	FontClass();
	FontClass(const FontClass&);
	~FontClass();
};

