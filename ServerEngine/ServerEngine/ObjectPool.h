#pragma once
#include <memory>

template<typename _Type>
class ObjectPool
{
public:
	ObjectPool();
	virtual ~ObjectPool();

	void Initialize(size_t _size);

	_Type* Alloc();
	void Release(_Type* _obj);
protected:
	_Type* m_objects = nullptr;
	size_t m_objectsCount = 0;
};

template<typename _Type>
inline ObjectPool<_Type>::ObjectPool() { }

template<typename _Type>
inline ObjectPool<_Type>::~ObjectPool() { }

template<typename _Type>
inline void ObjectPool<_Type>::Initialize(size_t _size)
{
	for (int i = 0; i < _size; ++i)
	{
		_Type* temp = static_cast<_Type*>(malloc(sizeof(_Type)));
		*((_Type**)temp) = m_objects;
		m_objects = temp;
	}
	m_objectsCount += _size;
}

template<typename _Type>
inline _Type* ObjectPool<_Type>::Alloc()
{
	if (nullptr == m_objects)
		return malloc(sizeof(_Type));

	_Type* object = m_objects;
	m_objects = *((_Type**)m_objects);
	--m_objectsCount;
	return object;
}

template<typename _Type>
inline void ObjectPool<_Type>::Release(_Type* _obj)
{
	*((_Type**)_obj) = m_objects;
	m_objects = (_Type*)_obj;
	++m_objectsCount;

	return;
}
