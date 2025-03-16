#pragma once

struct ID3D11Buffer;

class IndexBuffer
{
	friend class Renderer;

public:
	virtual ~IndexBuffer();
	IndexBuffer(size_t size);
	IndexBuffer(IndexBuffer const& copy) = delete;

public:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;
};