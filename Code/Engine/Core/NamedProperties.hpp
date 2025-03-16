#pragma once


class TypedPropertyBase
{
};

template<typename T>
class TypedProperty : public TypedPropertyBase
{
	T m_data;
};


