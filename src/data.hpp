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
    int *ref_count{};
public:
    //! constructor using the given source data
    explicit data(Tp *dat, size_t length);

    //! constructor that malloc new memory of the given length
    explicit data(size_t length);

    //! soft copy constructor
    data(const data &p);

    //! hard copy
    data<Tp> clone();

    bool copy_to(data &dst);

    //! increase refcount
    void inc_ref_count();

    //! decrease refcount
    void dec_ref_count();

    //! override []
    Tp &operator[](size_t i) const;

    //! override assign operator
    data<Tp> &operator=(const data &p);

    //! override equation(strict equal)
    bool operator==(const data &p) const;

    //! equal with customized compare function
    bool equals(const data &p, bool (*equal)(Tp, Tp)) const;

    //! destructor
    ~data();
};

#ifdef SAFE

//! constructor using the given source data
template<typename Tp>
data<Tp>::data(Tp *dat, size_t length)
{
    if (dat == NULL || dat == nullptr)
    {
        throw std::invalid_argument(
                "Null Pointer Exception: The source pointer is null, data construction interrupted.\n");
    }
    this->length = length;
    this->value = dat;
    this->ref_count = new int[1]{1};
}

//! constructor that malloc new memory of the given length
template<typename Tp>
data<Tp>::data(size_t length):length(length)
{
    try
    {
        value = new Tp[length];
        ref_count = new int[1]{1};
    }
    catch (std::bad_alloc &e)
    {
        std::cerr << "Bad Alloc Exception: Failed to allocate memory of the given length " << length << "\n";
        throw std::bad_alloc();
    }
}

//! soft copy constructor
template<typename Tp>
data<Tp>::data(const data &p)
{
    length = p.length;
    delete[]value;
    value = p.value;
    delete[]ref_count;
    ref_count = p.ref_count;
    if (value == NULL || value == nullptr || ref_count == nullptr || ref_count == NULL)
    {
        throw std::invalid_argument(
                "Null Pointer Exception: The copy source pointer is null, copy construction interrupted.\n");
    }
}

//! hard copy
template<typename Tp>
data<Tp> data<Tp>::clone()
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

//! increase refcount
template<typename Tp>
void data<Tp>::inc_ref_count()
{
    (*ref_count)++;
}

//! decrease refcount
template<typename Tp>
void data<Tp>::dec_ref_count()
{
    (*ref_count)--;
    if (!(*ref_count))
    {
        this->~data();
    }
}

//! override []
template<typename Tp>
Tp &data<Tp>::operator[](size_t i) const
{
    if (value == NULL || value == nullptr)
    {
        throw std::invalid_argument("Null Pointer Exception: The value pointer is null, failed to get value.\n");
    }
    if (i >= length)
    {
        throw std::out_of_range("Out Of Range Exception: The index exceeds the max length of value.\n");
    }
    return value[i];
}

//! override assign operator
template<typename Tp>
data<Tp> &data<Tp>::operator=(const data &p)
{
    if (this == &p)
    {
        return (*this);
    }
    if (p.value == NULL || p.value == nullptr)
    {
        throw std::invalid_argument("Null Pointer Exception: The value pointer is null, assignment interrupted.\n");
    }
    length = p.length;
    ref_count = p.ref_count;
    value = p.value;
    return (*this);
}

//! override equation(strict equal)
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
    if (value == NULL || value == nullptr || p.value == NULL || p.value == nullptr)
    {
        throw std::invalid_argument("Null Pointer Exception: The value pointer is null, assignment interrupted.\n");
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

//! equal with customized compare function
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
    if (value == NULL || value == nullptr || p.value == NULL || p.value == nullptr)
    {
        throw std::invalid_argument("Null Pointer Exception: The value pointer is null, assignment interrupted.\n");
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

//! destructor
template<typename Tp>
data<Tp>::~data()
{
    if (value == NULL || value == nullptr)
    {
        throw std::invalid_argument(
                "Null Pointer Exception: The value pointer is null, data destruction interrupted.\n");
    }
    delete[] value;
    value = nullptr;
}

#else

//! constructor using the given source data
template<typename Tp>
data<Tp>::data(Tp *dat, size_t length)
{
    this->length = length;
    this->value = dat;
    this->ref_count = new int[1]{1};
}

//! constructor that malloc new memory of the given length
template<typename Tp>
data<Tp>::data(size_t length):length(length)
{
    value = new Tp[length];
    ref_count = new int[1]{1};
}

//! soft copy constructor
template<typename Tp>
data<Tp>::data(const data &p)
{
    length = p.length;
    delete[]value;
    value = p.value;
    delete[]ref_count;
    ref_count = p.ref_count;
}

//! hard copy
template<typename Tp>
data<Tp> data<Tp>::clone()
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

//! increase refcount
template<typename Tp>
void data<Tp>::inc_ref_count()
{
    (*ref_count)++;
}

//! decrease refcount
template<typename Tp>
void data<Tp>::dec_ref_count()
{
    (*ref_count)--;
    if (!(*ref_count))
    {
        this->~data();
    }
}

//! override []
template<typename Tp>
Tp &data<Tp>::operator[](size_t i) const
{
    return value[i];
}

//! override assign operator
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

//! override equation(strict equal)
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

//! equal with customized compare function
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

//! destructor
template<typename Tp>
data<Tp>::~data()
{
    delete[] value;
    value = nullptr;
}

#endif