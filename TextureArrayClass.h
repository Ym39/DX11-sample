#pragma once
#include<d3d11.h>
#include<stdio.h>
#include"DirectXTex.h"
#include<string>
using namespace DirectX;
using namespace std;

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, char*, char*);
	bool Initialize(ID3D11Device*, char*, char*,char*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ScratchImage LoadTextureFromFile(LPCSTR);
private:
	ID3D11ShaderResourceView* m_textures[3];
};

