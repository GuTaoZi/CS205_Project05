#pragma once

#include <cstddef>

template<typename Tp>
class data
{
private:
    Tp *value;
    size_t length;
    size_t *ref_count;
public:

    //! constructor using the given source data
    explicit data(Tp *dat, size_t length);

    //! constructor that malloc new memory of the given length
    explicit data(size_t length);

    //! soft copy constructor
    data(const data &p);

    data<Tp> &clone();

    bool copy_to(data &dst);

    ~data();

    size_t get_ref_count();

    //! increase refcount
    void inc_ref_count();

    //! decrease refcount
    void dec_ref_count();

    Tp &operator[](size_t i) const;
};

template<typename Tp>
data<Tp>::data(Tp *dat, size_t length)
{
    this->length = length;
    this->value = dat;
    this->ref_count = new size_t[1]{1};
}

template<typename Tp>
data<Tp>::data(size_t length)
{
    this->length = length;
    this->value = new Tp[length]{};
    this->ref_count = new size_t[1]{1};
}

template<typename Tp>
Tp &data<Tp>::operator[](size_t i) const
{
    return value[i];
}

template<typename Tp>
data<Tp>::~data()
{
    delete[] value;
    value = nullptr;
}

template<typename Tp>
size_t data<Tp>::get_ref_count()
{
    return (*ref_count);
}

template<typename Tp>
void data<Tp>::inc_ref_count()
{
    (*ref_count)++;
}

template<typename Tp>
void data<Tp>::dec_ref_count()
{
    (*ref_count)--;
    if (!(*ref_count))
    {
        this->~data();
    }
}

template<typename Tp>
data<Tp>::data(const data &p)
{
    this->length = p.length;
    this->value = p.value;
    this->ref_count = p.ref_count;
}

template<typename Tp>
data<Tp> &data<Tp>::clone()
{
    data<Tp> New(value, length);
    New.ref_count = new size_t[1]{1};
    return New;
}

template<typename Tp>
bool data<Tp>::copy_to(data &dst)
{
    dst.~data();
    dst=clone();
}
