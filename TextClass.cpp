#include "TextClass.h"

TextClass::TextClass() : m_Font(nullptr),m_FontShader(nullptr),m_sentence1(nullptr),m_sentence2(nullptr)
{
}

TextClass::TextClass(const TextClass&)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX& baseViewMatrix)
{
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	m_baseViewMatrix = baseViewMatrix;

	m_Font = new FontClass;
	if (!m_Font)
		return false;

	bool result = m_Font->Initialize(device, deviceContext, "data/fontdata.txt", "data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, "Hello", 10, 50, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 0.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create and initialize the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence1, "Hello", 100, 100, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Initialize the first sentence.
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// Now update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence2, "Goodbye", 100, 200, 1.0f, 0.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}

void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);

	// Release the second sentence.
	ReleaseSentence(&m_sentence2);

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = nullptr;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = nullptr;
	}
     
	return;
}

bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX& worldMatrix, XMMATRIX& orthoMatrix)
{
	// Draw the first sentence.
	bool result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	// Draw the second sentence.
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;

	if (fps > 9999)
		fps = 9999;

	_itoa_s(fps, tempString, 10);

	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	if (fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	result = UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
	if (!result)
		return false;

	return true;
}

bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;

	_itoa_s(cpu, tempString, 10);

	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	result = UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0, 1.0, 0.0, deviceContext);
	if (!result)
		return false;

	return true;
}

bool TextClass::SetRanderCount(int randerCount, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char renderCounterString[32];
	bool result;

	_itoa_s(randerCount, tempString, 10);

	strcpy_s(renderCounterString, "Render Count: ");
	strcat_s(renderCounterString, tempString);
	strcat_s(renderCounterString, "%");

	result = UpdateSentence(m_sentence2, renderCounterString, 20, 40, 0.0, 1.0, 0.0, deviceContext);
	if (!result)
		return false;

	return true;
}



bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	*sentence = new SentenceType;
	if (!*sentence)
		return false;

	(*sentence)->vertexBuffer = nullptr;
	(*sentence)->indexBuffer = nullptr;

	(*sentence)->maxLength = maxLength;

	(*sentence)->vertexCount = 6 * maxLength;
	(*sentence)->indexCount = (*sentence)->vertexCount;

	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
		return false;

	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
		return false;

	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	for (int i = 0; i < (*sentence)->indexCount; ++i)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = nullptr;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = nullptr;

	return true;
}

bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// Store the color of the sentence.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// Get the number of letters in the sentence.
	int numLetters = (int)strlen(text);

	// Check for possible buffer overflow.
	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	// Create the vertex array.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// Use the font class to build the vertex array from the sentence text and sentence draw location.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vertex buffer so it can be written to.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = nullptr;

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = nullptr;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = nullptr;
		}

		// Release the sentence.
		delete* sentence;
		*sentence = nullptr;
	}

	return;
}

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX& worldMatrix, XMMATRIX& orthoMatrix)
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	bool result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	if (!result)
	{
		false;
	}

	return true;
}


