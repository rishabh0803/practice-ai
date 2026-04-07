#pragma once
#include <NDArray.h>
#include <iostream>
#include <InfixExpression.h>

using namespace std;

class NDArrayView
{
private:
	NDArray *ndArray;
	bool is_read_only;
	vector<uint32_t> dimensions;
	vector<uint32_t> from;
	vector<uint32_t> to;
	uint32_t start_at;

public:
	NDArrayView(NDArray *ndArray, vector<uint32_t> from, vector<uint32_t> to, bool is_read_only);
	vector<uint32_t> get_dimensions();
	template <typename... TT>
	T1 get(TT...);
	friend class NDArray;
};

template <typename... TT>
T1 NDArrayView::get(TT... arguments)
{
	auto dimensions = this->get_dimensions();
	vector<uint32_t> indexes;
	(indexes.push_back((uint32_t)arguments), ...);
	if (indexes.size() != dimensions.size())
	{
		ostringstream oss;

		if (dimensions.size() > 1)
		{
			oss << "Array has " << dimensions.size() << " dimensions, expected " << dimensions.size() << " indexes, found " << indexes.size() << endl;
		}
		else
		{
			oss << "Array has " << dimensions.size() << " dimension, expected " << dimensions.size() << " index, found " << indexes.size() << endl;
		}

		throw NDArrayException(oss.str());
	}

	int x;
	for (x = indexes.size() - 1; x >= 0; --x)
	{
		if (indexes[x] >= dimensions[x])
		{
			ostringstream oss;
			oss << "Index out of bounds ";
			for (auto j : indexes)
				oss << "[" << j << "]";
			oss << ", size of array is ";
			for (auto j : dimensions)
				oss << "[" << j << "]";
			throw NDArrayException(oss.str());
		}
	}

	vector<uint32_t> ndarray_indexes = this->from;
	uint32_t i;
	i = this->start_at;
	uint32_t j;
	for (j = 0; j < this->dimensions.size(); ++j)
	{
		ndarray_indexes[i] += indexes[j];
		++i;
	}
	uint64_t idx;
	uint32_t multiplier = 1;
	idx = 0;
	for (x = ndarray_indexes.size() - 1; x >= 0; --x)
	{
		idx = idx + ndarray_indexes[x] * multiplier;
		multiplier = multiplier * this->ndArray->dimensions[x];
	}
	return this->ndArray->_get(idx);
}
