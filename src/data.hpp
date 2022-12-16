#pragma once

#include <cstdio>

template<typename T1, typename T2>
T1 default_convert(T2 src)
{
    return (T1) src;
}

template<typename Tp>
class data
{
private:
    Tp *value;
    size_t length{};
    size_t *ref_count{};
public:
    //! constructor using the given source data
    explicit data(Tp *dat, size_t length);

    //! constructor that malloc new memory of the given length
    explicit data(size_t length);

    //! soft copy constructor
    data(const data &p);

    //! hard copy
    data<Tp> &clone();

    bool copy_to(data &dst);

    //destructor
    ~data();

    //! increase refcount
    void inc_ref_count();

    //! decrease refcount
    void dec_ref_count();

    //! override []
    Tp &operator[](size_t i) const;

    //! override assign operator
    data<Tp> &operator=(const data &p);

    template<typename target_type>
    data<target_type> convert_to(target_type (*convert_function)(Tp src) = default_convert<target_type, Tp>) const;

    //! override equation(strict equal)
    bool operator==(const data &p) const;

    //! equal with customized compare function
    bool equals(const data &p, bool (*equal)(Tp, Tp)) const;
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
data<Tp>::~data()
{
    delete[] value;
    value = nullptr;
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
    dst = clone();
    return true;
}

template<typename Tp>
Tp &data<Tp>::operator[](size_t i) const
{
    return value[i];
}

template<typename Tp>
data<Tp> &data<Tp>::operator=(const data &p)
{
    if (this == &p)
    {
        return (*this);
    }
    length = p.length;
    ref_count = p.ref_count;
    value = p.value;
    return (*this);
}

template<typename Tp>
bool data<Tp>::operator==(const data &p) const
{
    if (this == &p)
    {
        return true;
    }
    if (length != p.length)
    {
        return false;
    }
    for (size_t i = 0; i < length; i++)
    {
        if (value[i] != p[i])
        {
            return false;
        }
    }
    return true;
}

template<typename Tp>
bool data<Tp>::equals(const data &p, bool (*equal)(Tp, Tp)) const
{
    if (this == &p)
    {
        return true;
    }
    if (length != p.length)
    {
        return false;
    }
    for (size_t i = 0; i < length; i++)
    {
        if (!equal(value[i], p[i]))
        {
            return false;
        }
    }
    return true;
}

template<typename Tp>
template<typename target_type>
data<target_type> data<Tp>::convert_to(target_type (*convert_function)(Tp)) const
{
    target_type *New = new target_type[length]{};
    for (size_t i = 0; i < length; i++)
    {
        New[i] = convert_function(value[i]);
    }
    return data<target_type>(New, length);
}
