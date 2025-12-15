#include "Node4.hpp"

namespace cfg
{
// #########################################################

template <typename T>
T	*Node4::Value::getAs(void)
{
	return static_cast<T *>(data);
}

template <typename T>
void	Node4::Value::setAs(T data_)
{
	if (data)
		deleteData();
	data = new T(data_);
}

// #########################################################
};

