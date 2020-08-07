#include "FrustumClass.h"

FrustumClass::FrustumClass()
{
}

FrustumClass::FrustumClass(const FrustumClass&)
{
}

FrustumClass::~FrustumClass()
{
}

void FrustumClass::ConstructFrustum(float screenDepth, XMMATRIX projectionMatrix, XMMATRIX viewMatrix)
{
	float zMinimum, r;
	XMMATRIX matrix;
	XMFLOAT4X4 projection;
	XMFLOAT4X4 view;
	XMStoreFloat4x4(&projection, projectionMatrix);
	XMStoreFloat4x4(&view, viewMatrix);

	zMinimum = -projection._43 / projection._33;
	r = screenDepth / (screenDepth - zMinimum);
	projection._33 = r;
	projection._43 = -r * zMinimum;

	X
}
