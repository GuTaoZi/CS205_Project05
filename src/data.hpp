#pragma once

#include <cstdio>
#include <iostream>

template<typename T1, typename T2>
T1 default_convert(T2 src)
{
    return (T1) src;
}

int cnt = 0;

template<typename Tp>
class data
{
private:
    Tp *value;
    size_t length{};
    int *ref_count{};
public:
    int id;

    //! constructor using the given source data
    explicit data(Tp *dat, size_t length);

    //! constructor that malloc new memory of the given length
    explicit data(size_t length);

    //! soft copy constructor
    data(const data &p);

    //! hard copy
    data<Tp> clone();

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

    //! override equation(strict equal)
    bool operator==(const data &p) const;

    //! equal with customized compare function
    bool equals(const data &p, bool (*equal)(Tp, Tp)) const;
};

#ifdef SAFE
template<typename Tp>
data<Tp>::data(Tp *dat, size_t length)
{
    id = ++cnt;
    try
    {
        if (dat == NULL || dat == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
        }
        this->length = length;
        this->value = dat;
        this->ref_count = new int[1]{1};
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The source pointer is null, data construction interrupted.\n";
        abort();
    }
}

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
        abort();
    }
}

template<typename Tp>
data<Tp>::~data()
{
    try
    {
        if (value == NULL || value == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
        }
        delete[] value;
        value = nullptr;
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The value pointer is null, data destruction interrupted.\n";
    }
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
    try
    {
        length = p.length;
        delete[]value;
        value = p.value;
        delete[]ref_count;
        ref_count = p.ref_count;
        if (value == NULL || value == nullptr || ref_count == nullptr || ref_count == NULL)
        {
            throw std::invalid_argument("Null Pointer Exception");
        }
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The copy source pointer is null, copy construction interrupted.\n";
        abort();
    }
}

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

template<typename Tp>
Tp &data<Tp>::operator[](size_t i) const
{
    try
    {
        if (value == NULL || value == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
        }
        if (i >= length)
        {
            throw std::out_of_range("Out Of Range Exception");
        }
        return value[i];
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The value pointer is null, failed to get value[ " << i << " ].\n";
        abort();
    }
    catch (std::out_of_range &e)
    {
        std::cerr << e.what() << ": The index " << i << " exceeds the max length of value.\n";
        abort();
    }
}

template<typename Tp>
data<Tp> &data<Tp>::operator=(const data &p)
{
    if (this == &p)
    {
        return (*this);
    }
    try
    {
        if (p.value == NULL || p.value == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
        }
        length = p.length;
        ref_count = p.ref_count;
        value = p.value;
        return (*this);
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The value pointer is null, assignment interrupted.\n";
        abort();
    }
}

template<typename Tp>
bool data<Tp>::operator==(const data &p) const
{
    if (this == &p)
    {
        return true;
    }
    try
    {
        if (length != p.length)
        {
            return false;
        }
        if (value == NULL || value == nullptr || p.value == NULL || p.value == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
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
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The value pointer is null, assignment interrupted.\n";
        abort();
    }
}

template<typename Tp>
bool data<Tp>::equals(const data &p, bool (*equal)(Tp, Tp)) const
{
    if (this == &p)
    {
        return true;
    }
    try
    {
        if (length != p.length)
        {
            return false;
        }
        if (value == NULL || value == nullptr || p.value == NULL || p.value == nullptr)
        {
            throw std::invalid_argument("Null Pointer Exception");
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
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what() << ": The value pointer is null, assignment interrupted.\n";
        abort();
    }
}
#else

template<typename Tp>
data<Tp>::data(Tp *dat, size_t length)
{
    this->length = length;
    this->value = dat;
    this->ref_count = new int[1]{1};
}

template<typename Tp>
data<Tp>::data(size_t length):length(length)
{
    value = new Tp[length];
    ref_count = new int[1]{1};
}

template<typename Tp>
data<Tp>::~data()
{
    if (value == NULL || value == nullptr)
    {
        throw std::invalid_argument("Null Pointer Exception");
    }
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
    length = p.length;
    delete[]value;
    value = p.value;
    delete[]ref_count;
    ref_count = p.ref_count;
    if (value == NULL || value == nullptr || ref_count == nullptr || ref_count == NULL)
    {
        throw std::invalid_argument("Null Pointer Exception");
    }
}

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

#endif