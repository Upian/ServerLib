#pragma once
#include <memory>

template<typename T_Type>
class ObjectPool
{
public:
	ObjectPool();
	virtual ~ObjectPool();

	void Initialize(size_t _size);

	T_Type* Alloc();
	template<typename ...Args>
	T_Type* Alloc(Args&&... _args);

	void Release(T_Type* _obj);
protected:
	T_Type* m_objects = nullptr;
	size_t m_objectsCount = 0;
};

template<typename T_Type>
inline ObjectPool<T_Type>::ObjectPool() { }

template<typename T_Type>
inline ObjectPool<T_Type>::~ObjectPool() { }

template<typename T_Type>
inline void ObjectPool<T_Type>::Initialize(size_t _size)
{
	for (int i = 0; i < _size; ++i)
	{
		T_Type* temp = static_cast<T_Type*>(malloc(sizeof(T_Type)));
		*((T_Type**)temp) = m_objects;
		m_objects = temp;
	}
	m_objectsCount += _size;
}

template<typename T_Type>
inline T_Type* ObjectPool<T_Type>::Alloc()
{
	if (nullptr == m_objects)
		return new T_Type();

	T_Type* object = m_objects;
	m_objects = *((T_Type**)m_objects); //다음거 이어줌
	
	if (0 < m_objectsCount)
		--m_objectsCount;
	return object;
}

template<typename T_Type>
template<typename ...Args>
inline T_Type* ObjectPool<T_Type>::Alloc(Args&&... _args)
{
	if (nullptr == m_objects)
		return new T_Type(std::forward<Args>(_args)...);

	T_Type* object = new (m_objects) T_Type(std::forward<Args>(_args)...);
	m_objects = *((T_Type**)m_objects);

	if (0 < m_objectsCount)
		--m_objectsCount;
	return object;
}

template<typename T_Type>
inline void ObjectPool<T_Type>::Release(T_Type* _obj)
{
	*((T_Type**)_obj) = m_objects;
	m_objects = (T_Type*)_obj;
	++m_objectsCount;

	return;
}
