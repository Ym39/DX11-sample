#include "ReflectionShaderClass.h"

ReflectionShaderClass::ReflectionShaderClass():m_vertexShader(nullptr),m_pixelShader(nullptr),m_layout(nullptr),m_matrixBuffer(nullptr),m_sampleState(nullptr),m_reflectionBuffer(nullptr)
{
}

ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass&)
{
}

ReflectionShaderClass::~ReflectionShaderClass()
{
}

bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix)
{
	// 상수 버퍼의 내용을 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져옵니다.
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 합니다
	worldMatrix = XMMatrixTranspose(worldMatrix);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatrix = XMMatrixTranspose(projectionMatrix);

	reflectionMatrix = XMMatrixTranspose(reflectionMatrix);

	// 상수 버퍼에 행렬을 복사합니다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 풉니다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정합니다.
	unsigned int bufferNumber = 0;

	// 마지막으로 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꿉니다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	if (FAILED(deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
    
	ReflectionBufferType* dataPtr2 = (ReflectionBufferType*)mappedResource.pData;

	dataPtr2->reflectionMatrix = reflectionMatrix;

	deviceContext->Unmap(m_reflectionBuffer, 0);
	
	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);

	// 픽셀 셰이더에서 셰이더 텍스처 리소스를 설정합니다.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

	return true;
}

void ReflectionShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정합니다.
	deviceContext->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더와 픽셀 셰이더를 설정합니다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// 픽셀 쉐이더에서 샘플러 상태를 설정합니다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 그립니다.
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

bool ReflectionShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	return InitializeShader(device, hwnd, L"Reflection.vs", L"Reflection.ps");
}

void ReflectionShaderClass::Shutdown()
{
	ShutdownShader();
	return;
}

bool ReflectionShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, XMMATRIX reflectionMatrix)
{
	// 렌더링에 사용할 셰이더 매개 변수를 설정합니다.
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,reflectionTexture,reflectionMatrix))
	{
		return false;
	}

	// 설정된 버퍼를 셰이더로 렌더링한다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ReflectionShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage = nullptr;

	// 버텍스 쉐이더 코드를 컴파일한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "ReflectionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 픽셀 쉐이더 코드를 컴파일한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	result = D3DCompileFromFile(psFilename, NULL, NULL, "ReflectionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// 셰이더 컴파일 실패시 오류메시지를 출력합니다.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// 컴파일 오류가 아니라면 셰이더 파일을 찾을 수 없는 경우입니다.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// 버퍼로부터 정점 셰이더를 생성한다.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 버퍼에서 픽셀 쉐이더를 생성합니다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// 정점 입력 레이아웃 구조체를 설정합니다.
	// 이 설정은 ModelClass와 셰이더의 VertexType 구조와 일치해야합니다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// 레이아웃의 요소 수를 가져옵니다.
	UINT numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 만듭니다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 더 이상 사용되지 않는 정점 셰이더 퍼버와 픽셀 셰이더 버퍼를 해제합니다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// 정점 셰이더에 있는 행렬 상수 버퍼의 구조체를 작성합니다.
	D3D11_BUFFER_DESC constantBufferDesc;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateBuffer(&constantBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 텍스처 샘플러 상태 구조체를 생성 및 설정합니다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 샘플러 상태를 만듭니다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_BUFFER_DESC reflectionBufferDesc;
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0;
	reflectionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&reflectionBufferDesc, NULL, &m_reflectionBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void ReflectionShaderClass::ShutdownShader()
{
	// 샘플러 상태를 해제한다.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 행렬 상수 버퍼를 해제합니다.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 레이아웃을 해제합니다.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 쉐이더를 해제합니다.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 버텍스 쉐이더를 해제합니다.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	if (m_reflectionBuffer)
	{
		m_reflectionBuffer->Release();
		m_reflectionBuffer = 0;
	}
}

void ReflectionShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	// Get a pointer to the error message text buffer.
	char* compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	unsigned long long bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	ofstream fout;
	fout.open("shader-error.txt");

	// Write out the error message.
	for (int index = 0; index < bufferSize; ++index)
	{
		fout << compileErrors[index];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = nullptr;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}
