#pragma once
#include"FontClass.h"
#include"FontShaderClass.h"

class TextClass
{
private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX&);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX&, XMMATRIX&);

	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetRanderCount(int, ID3D11DeviceContext*);
	bool SetTime(float time, ID3D11DeviceContext* deviceContext);
	bool SetText(char* text,int positionX,int positionY, ID3D11DeviceContext*deviceContext);
    
private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX&, XMMATRIX&);

private:
	FontClass* m_Font;
	FontShaderClass* m_FontShader;
	int m_screenWidth, m_screenHeight;
	XMMATRIX m_baseViewMatrix;

	SentenceType* m_sentence1;
	SentenceType* m_sentence2;

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

};

