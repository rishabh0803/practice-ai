#include <NDArrayView.h>
NDArrayView::NDArrayView(NDArray *ndArray, vector<uint32_t> from, vector<uint32_t> to, bool is_read_only)
{
	this->ndArray = ndArray;
	this->from = from;
	this->to = to;
	this->is_read_only = is_read_only;
	int i = 0;
	while (i < from.size())
	{
		if (from[i] != to[i])
			break;
		++i;
	}
	size_t from_sz = from.size();
	if (i == from_sz)
	{
		this->dimensions.resize(1); // one element 1D array
		this->dimensions[0] = 1;
		this->start_at = from_sz - 1;
	}
	else
	{
		size_t sz = this->from.size() - i;
		if (sz == 2)
		{
			if (from[from_sz - 1] == to[from_sz - 1]) // vertical 1D
			{
				this->start_at = from_sz - 2;
				this->dimensions.resize(1);
				this->dimensions[0] = to[from_sz - 2] - from[from_sz - 2] + 1;
			}
			else
			{
				this->start_at = from_sz - 2;
				this->dimensions.resize(sz);
				this->dimensions[0] = to[from_sz - 2] - from[from_sz - 2] + 1;
				this->dimensions[1] = to[from_sz - 1] - from[from_sz - 1] + 1;
			}
		}
		else if (sz == 1)
		{
			this->start_at = from_sz - 1;
			this->dimensions.resize(1);
			this->dimensions[0] = to[from_sz - 1] - from[from_sz - 1] + 1;
		}
		else
		{
			// this->dimensions.resize(sz);
			this->start_at = i;
			while (i < from.size())
			{
				this->dimensions.push_back(to[i] - from[i] + 1);
				++i;
			}
		}
	}
}
vector<uint32_t> NDArrayView::get_dimensions()
{
	return this->dimensions;
}
