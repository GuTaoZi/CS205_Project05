#pragma once

#include <cstddef>
#include <exception>
#include <stdexcept>
#include "data.hpp"

template<typename T1, typename T2>
T1 default_convert(T2 src)
{
    return (T1) src;
}

template<typename Tp>
class matrix
{
private:
    typedef char channel_number;
    static int cnt;
    size_t rows;
    size_t cols;
    channel_number channels;
    size_t channel_pad;
    size_t step;
    size_t shift;
    data<Tp> *dat;

public:
    int id;

    //! constructor that sets matrix elements to given source data
    matrix(size_t rows, size_t cols, Tp *src, channel_number channels = 1);

    //! constructor that sets each matrix element to specified value
    matrix(size_t rows, size_t cols, const Tp &value, channel_number channels = 1);

    //! submatrix constructor
    matrix(const matrix &src, size_t row1, size_t col1, size_t row2, size_t col2);

    //! copy constructor (soft copy)
    matrix(const matrix &p);

    //! hard copy
    matrix<int> clone();

    bool copy_to(matrix &dst);

    //! get element from matrix (read only)
    Tp at(size_t row, size_t col, channel_number channel = 1) const;

    //! equal with customized compare function
    bool equals(const matrix &p, bool (*equal)(Tp, Tp)) const;

    //customized type conversion
    template<typename target_type>
    matrix<target_type> convert_to(target_type (*convert_function)(Tp src) = default_convert) const;

    //! get element from matrix
    Tp &operator()(size_t row, size_t col, channel_number channel = 1) const;

    //! override assign operator
    matrix<Tp> &operator=(const matrix &p);

    //! override equation(strict equal)
    bool operator==(const matrix &p) const;

    template<typename augendtype>
    matrix<decltype()>

    ~matrix();
};

template<> int matrix<int>::cnt = 0;
template<> int matrix<float>::cnt = 0;

//! constructor that sets matrix elements to given source data
template<typename Tp>
matrix<Tp>::matrix(size_t rows, size_t cols, Tp *src, channel_number channels)
{
    id = ++cnt;
    if (rows * cols == 0)
    {
        throw std::invalid_argument("Invalid argument exception: row number and column number should be positive.\n");
    }
    if (channels > 4 || channels <= 0)
    {
        throw std::invalid_argument("Invalid argument exception: channel number should be in [1,4].\n");
    }
    this->shift = 0;
    this->rows = rows;
    this->cols = cols;
    this->step = cols;
    this->channel_pad = rows * cols;
    this->channels = channels;
    this->dat = new data<Tp>(src, rows * cols * channels);
}

//! constructor that sets each matrix element to specified value
template<typename Tp>
matrix<Tp>::matrix(size_t rows, size_t cols, const Tp &value, channel_number channels):rows(rows), cols(cols),
                                                                                       channels(channels)
{
    id = ++cnt;
    shift = 0;
    step = cols;
    channel_pad = rows * cols;
    size_t length = rows * cols * channels;
    dat = new data<Tp>(length);
    for (size_t i = 0; i < length; i++)
    {
        this->dat[i] = value;
    }
}

//! submatrix constructor
template<typename Tp>
matrix<Tp>::matrix(const matrix &src, size_t row1, size_t col1, size_t row2, size_t col2)
{
    id = ++cnt;
    rows = row2 - row1 + 1;
    cols = col2 - col1 + 1;
    channels = src.channels;
    step = src.step;
    channel_pad = src.channel_pad;
    shift = (row1 - 1) * step + col1 - 1;
    dat = src.dat;
    src.dat->inc_ref_count();
}

template<typename Tp>
matrix<Tp>::matrix(const matrix &p)
{
    id = ++cnt;
    rows = p.rows;
    cols = p.cols;
    channels = p.channels;
    step = p.step;
    shift = p.shift;
    dat = p.dat;
    dat->inc_ref_count();
}

template<typename Tp>
Tp &matrix<Tp>::operator()(size_t row, size_t col, channel_number channel) const
{
    return (*dat)[shift + (channel - 1) * channel_pad + (row - 1) * step + col - 1];
}

template<typename Tp>
Tp matrix<Tp>::at(size_t row, size_t col, channel_number channel) const
{
    return (*dat)[shift + (channel - 1) * channel_pad + (row - 1) * step + col - 1];
}

template<typename Tp>
matrix<Tp>::~matrix()
{
    dat->dec_ref_count();
}

template<typename Tp>
matrix<int> matrix<Tp>::clone()
{
    Tp *NewArr = new Tp[rows * cols * channels]{};
    size_t it = 0;
    for (channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = at(r, c, ch);
            }
        }
    }
    return matrix<Tp>(rows, cols, NewArr, channels);
}


template<typename Tp>
bool matrix<Tp>::copy_to(matrix &dst)
{
    dst.~matrix();
    dst = clone();
    return true;
}

template<typename Tp>
matrix<Tp> &matrix<Tp>::operator=(const matrix &p)
{
    if (this == &p)
    {
        return (*this);
    }
    id = p.id;
    rows = p.rows;
    cols = p.cols;
    channels = p.channels;
    step = p.step;
    shift = p.shift;
    channel_pad = p.channel_pad;
    dat = p.dat;
    dat->inc_ref_count();
    return (*this);
}

template<typename Tp>
bool matrix<Tp>::operator==(const matrix &p) const
{
    if (this == &p)
    {
        return true;
    }
    if (rows != p.rows || cols != p.cols || channels != p.channels)
    {
        printf("%d %d %d %d %d %d", rows, p.rows, cols, p.cols, channels, p.channels);
        return false;
    }
    for (channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                if (at(r, c, ch) != p.at(r, c, ch))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

template<typename Tp>
bool matrix<Tp>::equals(const matrix &p, bool (*equal)(Tp, Tp)) const
{
    if (this == &p)
    {
        return true;
    }
    if (rows != p.rows || cols != p.cols || channels != p.channels)
    {
        return false;
    }
    for (channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                if (!equal(at(r, c, ch), p.at(r, c, ch)))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

template<typename Tp>
template<typename target_type>
matrix<target_type> matrix<Tp>::convert_to(target_type (*convert_function)(Tp src)) const
{
    auto *NewArr = new target_type[rows * cols * channels]{};
    size_t it = 0;
    for (char ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = convert_function(at(r, c, ch));
            }
        }
    }
    return matrix<target_type>(rows, cols, NewArr, channels);
}
