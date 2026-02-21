#pragma once
#include <string>
#include <cstdint>

using namespace std;

class csv_info
{
private:
    uint32_t _rows;
    uint32_t _columns;
    uint32_t _skip_lines;
    string _name;

public:
    csv_info(const string &name, uint32_t rows, uint32_t columns, uint32_t skip_lines = 0)
    {
        this->_name = name;
        this->_rows = rows;
        this->_columns = columns;
        this->_skip_lines = skip_lines;
    }

    uint32_t rows() const
    {
        return this->_rows;
    }

    uint32_t columns() const
    {
        return this->_columns;
    }

    uint32_t skip_lines() const
    {
        return this->_skip_lines;
    }

    string name() const
    {
        return this->_name;
    }
};
