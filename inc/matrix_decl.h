#pragma once

#include <cstddef>
template <typename _Tp>
class data
{
private:
    _Tp *dat;
    size_t ref_count;

public:
    data(const _Tp *dat);
    data(const size_t &size);
};

template <typename _Tp>
class matrix
{
private:
    typedef char channel_type;
    size_t rows;
    size_t cols;
    channel_type channels;
    size_t step;
    data<_Tp> *dat;
public:
    matrix(size_t rows, size_t cols);
    //! constructor that sets each matrix element to specified value
    matrix(size_t rows, size_t cols, const _Tp &value);
};
