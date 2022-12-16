#pragma once

#include <cstddef>
#include <exception>
#include <stdexcept>
#include "data.hpp"

template<typename Tp>
class matrix
{
private:
    typedef char channel_number;
    static int cnt;
    int id;
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

    //! submatrix constructor
    matrix(const matrix &src, size_t row1, size_t col1, size_t row2, size_t col2);

    //! copy constructor (soft copy)
    matrix(const matrix &p);

    //! hard copy
    matrix<Tp>& clone();

    bool copy_to(const matrix &dst);

    //! get element from matrix (read only)
    Tp at(size_t row, size_t col, channel_number channel = 1);

    //! get element from matrix
    Tp &operator()(size_t row, size_t col, channel_number channel = 1) const;

    ~matrix();
};

template<> int matrix<int>::cnt = 0;

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
    this->dat = new data<Tp>(src,rows*cols*channels);
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
    dat = new data<Tp>(rows * cols * channels);
    for (size_t i = 0; i < rows * cols * channels; i++)
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
Tp matrix<Tp>::at(size_t row, size_t col, channel_number channel)
{
    return (*dat)[shift + (channel - 1) * channel_pad + (row - 1) * step + col - 1];
}

template<typename Tp>
matrix<Tp>::~matrix()
{
    dat->dec_ref_count();
}

template<typename Tp>
matrix<Tp> &matrix<Tp>::clone()
{
    Tp *NewArr=new Tp[rows*cols*channels]{};
    size_t it=0;
    for(size_t ch=1;ch<=channels;ch++)
    {
        for(size_t r=1;r<=rows;r++)
        {
            for(size_t c=1;c<=cols;c++)
            {
                NewArr[it++]=at(r,c,ch);
            }
        }
    }
    return matrix<Tp>(rows,cols,NewArr,channels);
}


template<typename Tp>
bool matrix<Tp>::copy_to(const matrix &dst)
{
    dst.~matrix();
    dst=clone();
}

