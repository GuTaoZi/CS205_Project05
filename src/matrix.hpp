#pragma once

#include <cstddef>
#include <exception>
#include <stdexcept>
#include "data.hpp"

template<typename Tp>
class matrix
{
private:
    typedef matrix<bool> mask;
    typedef char channel_number;
    size_t rows;
    size_t cols;
    channel_number channels;
    size_t channel_pad;
    size_t step;
    size_t shift;
    data<Tp> *dat;
public:

    //! constructor that sets matrix elements to given source data
    matrix(size_t rows, size_t cols, Tp *src, channel_number channels = 1);

    //! constructor that sets each matrix element to specified value
    matrix(size_t rows, size_t cols, const Tp &value, channel_number channels = 1);

    //! submatrix constructor(ROI)
    matrix(matrix &src, size_t row1, size_t col1, size_t row2, size_t col2);

    //! copy constructor (soft copy)
    matrix(const matrix &p);

    //! hard copy
    matrix<int> clone();

    bool copy_to(matrix &dst);

    bool copy_to(matrix &dst, const mask &mask);

    matrix<Tp> split_channel(channel_number channel_id);

    //! get element from matrix (read only)
    Tp at(size_t row, size_t col, channel_number channel = 1) const;

    bool set(size_t row, size_t col, const Tp &value, channel_number channel = 1);

    bool fill(size_t row1, size_t col1, size_t row2, size_t col2, const Tp &value, channel_number channel = 1);

    //! equal with customized compare function
    bool equals(const matrix &p, bool (*equal)(Tp, Tp)) const;

    //customized type conversion
    template<typename target_type>
    matrix<target_type> convert_to(target_type (*convert_function)(Tp src) = default_convert<target_type, Tp>) const;

    //! get element from matrix
    Tp &operator()(size_t row, size_t col, channel_number channel = 1) const;

    //! override implicit type transform assign operator(convert & hard copy)
    template<typename rhs>
    matrix<Tp> &operator=(const matrix<rhs> &p);

    //! override same type assign operator(soft copy)
    matrix<Tp> &operator=(const matrix<Tp> &p);

    //! override equation(strict equal)
    bool operator==(const matrix &p) const;

    //basic calculation
    template<typename T2>
    matrix<decltype(Tp() + T2())> operator+(matrix<T2> &p);

    template<typename T2>
    matrix<decltype(Tp() - T2())> operator-(matrix<T2> &p);

    template<typename T2>
    matrix<decltype(Tp() * T2())> operator*(matrix<T2> &p);

    //customized calculation
    template<typename result_type>
    matrix<result_type> unary_calc(result_type (*unary_function)(Tp)) const;

    template<typename result_type, typename T2>
    matrix<result_type> binary_calc(const matrix<T2> &p, result_type (*binary_function)(Tp, T2)) const;

    ~matrix();

    size_t get_rows() const;

    size_t get_cols() const;

    channel_number get_channels() const;

    size_t get_step() const;

    size_t get_channel_pad() const;

    size_t get_shift() const;

    data<Tp> *get_dat() const;
};

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

//! submatrix constructor(ROI)
template<typename Tp>
matrix<Tp>::matrix(matrix &src, size_t row1, size_t col1, size_t row2, size_t col2)
{
    rows = row2 - row1 + 1;
    cols = col2 - col1 + 1;
    channels = src.channels;
    step = src.step;
    channel_pad = src.channel_pad;
    shift = (row1 - 1) * step + col1 - 1;
    dat = src.dat;
    dat->inc_ref_count();
}

//! copy constructor (soft copy)
template<typename Tp>
matrix<Tp>::matrix(const matrix &p)
{
    rows = p.get_rows();
    cols = p.get_cols();
    channels = p.get_channels();
    step = p.get_step();
    shift = p.get_shift();
    dat = p.get_dat();
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

//! hard copy
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
template<typename rhs>
matrix<Tp> &matrix<Tp>::operator=(const matrix<rhs> &p)
{
    dat->dec_ref_count();
    (*this) = p.template convert_to<Tp>();
    return (*this);
}

template<typename Tp>
matrix<Tp> &matrix<Tp>::operator=(const matrix<Tp> &p)
{
    if (this == &p)
    {
        return (*this);
    }
    rows = p.get_rows();
    cols = p.get_cols();
    channels = p.get_channels();
    step = p.get_step();
    shift = p.get_shift();
    channel_pad = p.get_channel_pad();
    if (dat != p.get_dat())
    {
        dat->dec_ref_count();
    }
    dat = p.get_dat();
    dat->inc_ref_count();
}

template<typename Tp>
bool matrix<Tp>::operator==(const matrix &p) const
{
    if (this == &p)
    {
        return true;
    }
    if (rows != p.get_rows() || cols != p.get_cols() || channels != p.get_channels())
    {
        printf("%d %d %d %d %d %d", rows, p.get_rows(), cols, p.get_cols(), channels, p.get_channels());
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
    if (rows != p.get_rows() || cols != p.get_cols() || channels != p.get_channels())
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
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
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
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
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
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
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
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t i = 1; i <= rows; i++)
        {
            for (size_t k = 1; k <= cols; k++)
            {
                for (size_t j = 1; j <= p.get_cols(); j++)
                {
                    New(i, j) = New(i, j) + at(i, k) * p.at(k, j);
                }
            }
        }
    }
    return New;
}

template<typename Tp>
template<typename result_type>
matrix<result_type> matrix<Tp>::unary_calc(result_type (*unary_function)(Tp)) const
{
    result_type *NewArr = new result_type[rows * cols * channels]{};
    size_t it = 0;
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = unary_function(at(r, c, ch));
            }
        }
    }
    return matrix<result_type>(rows, cols, NewArr, channels);
}

template<typename Tp>
template<typename result_type, typename T2>
matrix<result_type> matrix<Tp>::binary_calc(const matrix<T2> &p, result_type (*binary_function)(Tp, T2)) const
{
    result_type *NewArr = new result_type[rows * cols * channels]{};
    size_t it = 0;
    for (matrix::channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                NewArr[it++] = binary_function(at(r, c, ch), p.at(r, c, ch));
            }
        }
    }
    return matrix<result_type>(rows, cols, NewArr, channels);
}

template<typename Tp>
bool matrix<Tp>::set(size_t row, size_t col, const Tp &value, matrix::channel_number channel)
{
    (*this)(row, col, channels) = value;
    return true;
}

template<typename Tp>
bool matrix<Tp>::copy_to(matrix &dst, const matrix::mask &mask)
{
    dst.~matrix();
    Tp *NewArr = new Tp[rows * cols * channels]{};
    size_t it = 0;
    for (channel_number ch = 1; ch <= channels; ch++)
    {
        for (size_t r = 1; r <= rows; r++)
        {
            for (size_t c = 1; c <= cols; c++)
            {
                it++;
                if (mask(r, c, ch))
                {
                    NewArr[it] = at(r, c, ch);
                }
            }
        }
    }
    dst = matrix<Tp>(rows, cols, NewArr, channels);
    return true;
}

template<typename Tp>
bool matrix<Tp>::fill(size_t row1, size_t col1, size_t row2, size_t col2, const Tp &value, channel_number channel)
{
    for (size_t r = row1; r <= row2; r++)
    {
        for (size_t c = col1; c <= col2; c++)
        {
            (*this)(r, c, channel) = value;
        }
    }
    return true;
}

template<typename Tp>
size_t matrix<Tp>::get_rows() const
{
    return rows;
}

template<typename Tp>
size_t matrix<Tp>::get_cols() const
{
    return cols;
}

template<typename Tp>
char matrix<Tp>::get_channels() const
{
    return channels;
}

template<typename Tp>
size_t matrix<Tp>::get_channel_pad() const
{
    return channel_pad;
}

template<typename Tp>
size_t matrix<Tp>::get_step() const
{
    return step;
}

template<typename Tp>
size_t matrix<Tp>::get_shift() const
{
    return shift;
}

template<typename Tp>
data<Tp> *matrix<Tp>::get_dat() const
{
    return dat;
}