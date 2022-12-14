#include "matrix_decl.h"

template <typename _Tp>
data<_Tp>::data(const _Tp *dat)
{
    this->dat = dat;
}

template <typename _Tp>
data<_Tp>::data(const size_t &size)
{
    this->dat = new _Tp[size]{};
    this->ref_count = 1;
}
