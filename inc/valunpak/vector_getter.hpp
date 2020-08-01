#pragma once

#define VALUNPAK_VECTOR_GETTER(parent_type, vector_type, vector_name) \
friend struct vector_name##_getter; \
class vector_name##_getter \
{ \
public: \
	vector_name##_getter(parent_type* a_parent) : m_parent(a_parent) {} \
 \
	vector_type::const_iterator begin() const { return m_parent->vector_name.begin(); } \
	vector_type::const_iterator end() const { return m_parent->vector_name.end(); } \
	const vector_type::value_type& operator[](size_t a_index) const { return m_parent->vector_name[a_index]; } \
\
	size_t size() const { return m_parent->vector_name.size(); } \
	bool empty() const { return m_parent->vector_name.empty(); } \
private: \
	parent_type* m_parent; \
}

#define VALUNPAK_MAP_GETTER(parent_type, vector_type, vector_name) \
friend struct vector_name##_getter; \
class vector_name##_getter \
{ \
public: \
	vector_name##_getter(parent_type* a_parent) : m_parent(a_parent) {} \
 \
	vector_type::const_iterator begin() const { return m_parent->vector_name.begin(); } \
	vector_type::const_iterator end() const { return m_parent->vector_name.end(); } \
\
	size_t size() const { return m_parent->vector_name.size(); } \
private: \
	parent_type* m_parent; \
}