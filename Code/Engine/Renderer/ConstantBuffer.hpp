#pragma once

struct ID3D11Buffer;

class ConstantBuffer
{
	friend class Renderer;

public:
	virtual ~ConstantBuffer();
	ConstantBuffer(size_t size);
	ConstantBuffer(ConstantBuffer const& copy) = delete;

public:
	ID3D11Buffer*					m_buffer = nullptr;
	size_t							m_size = 0;
};