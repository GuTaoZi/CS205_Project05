#pragma once

#include <cstddef>
#include <exception>
#include <stdexcept>
#include "data.hpp"

template<typename Tp>
class matrix
{
public:
    typedef char channel_number;
    static int cnt;
    size_t rows;
    size_t cols;
    channel_number channels;
    size_t channel_pad;
    size_t step;
    size_t shift;
    data<Tp> *dat;

    int id;

    //! constructor that sets matrix elements to given source data
    matrix(size_t rows, size_t cols, Tp *src, channel_number channels = 1);

    //! constructor that sets each matrix element to specified value
    matrix(size_t rows, size_t cols, const Tp &value, channel_number channels = 1);

    //! submatrix constructor
    matrix(matrix &src, size_t row1, size_t col1, size_t row2, size_t col2);

    //! copy constructor (soft copy)
    matrix(const matrix &p);

    //! hard copy
    matrix<int> clone();

    bool copy_to(matrix &dst);

    matrix<Tp> split_channel(channel_number channel_id);

    //! get element from matrix (read only)
    Tp at(size_t row, size_t col, channel_number channel = 1) const;

    //! equal with customized compare function
    bool equals(const matrix &p, bool (*equal)(Tp, Tp)) const;

    //customized type conversion
    template<typename target_type>
    matrix<target_type> convert_to(target_type (*convert_function)(Tp src) = default_convert<target_type, Tp>) const;

    //! get element from matrix
    Tp &operator()(size_t row, size_t col, channel_number channel = 1) const;

    //! override assign operator
    matrix<Tp> &operator=(const matrix &p);

    //! override equation(strict equal)
    bool operator==(const matrix &p) const;

    template<typename T2>
    matrix<decltype(Tp() + T2())> operator+(matrix<T2> &p);

    template<typename T2>
    matrix<decltype(Tp() - T2())> operator-(matrix<T2> &p);

    template<typename T2>
    matrix<decltype(Tp() * T2())> operator*(matrix<T2> &p);

    ~matrix();
};

template<> int matrix<int>::cnt = 0;
template<> int matrix<float>::cnt = 0;
template<> int matrix<double>::cnt = 0;

//! constructor that sets matrix elements to given source data
template<typename Tp>
matrix<Tp>::matrix(size_t rows, size_t cols, Tp *src, channel_number channels):rows(rows), cols(cols),
                                                                               channels(channels)
{
    if (rows * cols == 0)
    {
        throw std::invalid_argument("Invalid argument exception: row number and column number should be positive.\n");
    }
    if (channels > 4 || channels <= 0)
    {
        throw std::invalid_argument("Invalid argument exception: channel number should be in [1,4].\n");
    }
    id = ++cnt;
    this->shift = 0;
    this->step = cols;
    this->channel_pad = rows * cols;
    this->dat = new data<Tp>(src, rows * cols * channels);
}

//! constructor that sets each matrix element to specified value
template<typename Tp>
matrix<Tp>::matrix(size_t rows, size_t cols, const Tp &value, channel_number channels):rows(rows), cols(cols),
                                                                                       channels(channels)
{
    id = ++cnt;
    size_t length = rows * cols * channels;
    Tp *src = new Tp[length]{};
    for (size_t i = 0; i < length; i++)
    {
        src[i] = value;
    }
    this->shift = 0;
    this->step = cols;
    this->channel_pad = rows * cols;
    this->dat = new data<Tp>(src, rows * cols * channels);
}

//! submatrix constructor
template<typename Tp>
matrix<Tp>::matrix(matrix &src, size_t row1, size_t col1, size_t row2, size_t col2)
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
matrix<Tp> matrix<Tp>::split_channel(matrix::channel_number channel_id)
{
    matrix<Tp> New(*this);
    New.channels = 1;
    New.shift += channel_pad * (channel_id - 1);
    return New;
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
    if (dat != p.dat)
    {
        dat->dec_ref_count();
    }
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
    target_type *NewArr = new target_type[rows * cols * channels]{};
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

template<typename T1>
template<typename T2>
matrix<decltype(T1() + T2())> matrix<T1>::operator+(matrix<T2> &p)
{
    typedef decltype(T1() + T2()) result_type;
    result_type *NewArr = new result_type[rows * cols * channels]{};
    size_t it = 0;
    for (char ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = at(r, c, ch) + p.at(r, c, ch);
            }
        }
    }
    return matrix<result_type>(rows, cols, NewArr, channels);
}

template<typename T1>
template<typename T2>
matrix<decltype(T1() - T2())> matrix<T1>::operator-(matrix<T2> &p)
{
    typedef decltype(T1() - T2()) result_type;
    result_type *NewArr = new result_type[rows * cols * channels]{};
    size_t it = 0;
    for (char ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = at(r, c, ch) - p.at(r, c, ch);
            }
        }
    }
    return matrix<result_type>(rows, cols, NewArr, channels);
}

template<typename T1>
template<typename T2>
matrix<decltype(T1() * T2())> matrix<T1>::operator*(matrix<T2> &p)
{
    typedef decltype(T1() * T2()) result_type;
    result_type *NewArr = new result_type[rows * cols * channels]{};
    matrix<result_type> New(rows, cols, NewArr, channels);
    for (char ch = 1; ch <= channels; ch++)
    {
        for (size_t i = 1; i <= rows; i++)
        {
            for (size_t k = 1; k <= cols; k++)
            {
                for (size_t j = 1; j <= p.cols; j++)
                {
                    New(i, j) = New(i, j) + at(i, k) * p.at(k, j);
                }
            }
        }
    }
    return New;
}
