template<typename _Tp> class Mat_ : public Mat
{
public:
    typedef _Tp value_type;
    typedef typename DataTxype<_Tp>::channel_type channel_type;
    typedef MatIterator_<_Tp> iterator;
    typedef MatConstIterator_<_Tp> const_iterator;

    //! default constructor
    Mat_() CV_NOEXCEPT;
    //! equivalent to Mat(_rows, _cols, DataType<_Tp>::type)
    Mat_(int _rows, int _cols);
    //! constructor that sets each matrix element to specified value
    Mat_(int _rows, int _cols, const _Tp& value);
    //! equivalent to Mat(_size, DataType<_Tp>::type)
    explicit Mat_(Size _size);
    //! constructor that sets each matrix element to specified value
    Mat_(Size _size, const _Tp& value);
    //! n-dim array constructor
    Mat_(int _ndims, const int* _sizes);
    //! n-dim array constructor that sets each matrix element to specified value
    Mat_(int _ndims, const int* _sizes, const _Tp& value);
    //! copy/conversion constructor. If m is of different type, it's converted
    Mat_(const Mat& m);
    //! copy constructor
    Mat_(const Mat_& m);
    //! constructs a matrix on top of user-allocated data. step is in bytes(!!!), regardless of the type
    Mat_(int _rows, int _cols, _Tp* _data, size_t _step=AUTO_STEP);
    //! constructs n-dim matrix on top of user-allocated data. steps are in bytes(!!!), regardless of the type
    Mat_(int _ndims, const int* _sizes, _Tp* _data, const size_t* _steps=0);
    //! selects a submatrix
    Mat_(const Mat_& m, const Range& rowRange, const Range& colRange=Range::all());
    //! selects a submatrix
    Mat_(const Mat_& m, const Rect& roi);
    //! selects a submatrix, n-dim version
    Mat_(const Mat_& m, const Range* ranges);
    //! selects a submatrix, n-dim version
    Mat_(const Mat_& m, const std::vector<Range>& ranges);
    //! from a matrix expression
    explicit Mat_(const MatExpr& e);
    //! makes a matrix out of Vec, std::vector, Point_ or Point3_. The matrix will have a single column
    explicit Mat_(const std::vector<_Tp>& vec, bool copyData=false);
    template<int n> explicit Mat_(const Vec<typename DataType<_Tp>::channel_type, n>& vec, bool copyData=true);
    template<int m, int n> explicit Mat_(const Matx<typename DataType<_Tp>::channel_type, m, n>& mtx, bool copyData=true);
    explicit Mat_(const Point_<typename DataType<_Tp>::channel_type>& pt, bool copyData=true);
    explicit Mat_(const Point3_<typename DataType<_Tp>::channel_type>& pt, bool copyData=true);
    explicit Mat_(const MatCommaInitializer_<_Tp>& commaInitializer);

    Mat_(std::initializer_list<_Tp> values);
    explicit Mat_(const std::initializer_list<int> sizes, const std::initializer_list<_Tp> values);

    template <std::size_t _Nm> explicit Mat_(const std::array<_Tp, _Nm>& arr, bool copyData=false);

    Mat_& operator = (const Mat& m);
    Mat_& operator = (const Mat_& m);
    //! set all the elements to s.
    Mat_& operator = (const _Tp& s);
    //! assign a matrix expression
    Mat_& operator = (const MatExpr& e);

    //! iterators; they are smart enough to skip gaps in the end of rows
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;

    //reverse iterators
    std::reverse_iterator<iterator> rbegin();
    std::reverse_iterator<iterator> rend();
    std::reverse_iterator<const_iterator> rbegin() const;
    std::reverse_iterator<const_iterator> rend() const;

    //! template methods for operation over all matrix elements.
    // the operations take care of skipping gaps in the end of rows (if any)
    template<typename Functor> void forEach(const Functor& operation);
    template<typename Functor> void forEach(const Functor& operation) const;

    //! equivalent to Mat::create(_rows, _cols, DataType<_Tp>::type)
    void create(int _rows, int _cols);
    //! equivalent to Mat::create(_size, DataType<_Tp>::type)
    void create(Size _size);
    //! equivalent to Mat::create(_ndims, _sizes, DatType<_Tp>::type)
    void create(int _ndims, const int* _sizes);
    //! equivalent to Mat::release()
    void release();
    //! cross-product
    Mat_ cross(const Mat_& m) const;
    //! data type conversion
    template<typename T2> operator Mat_<T2>() const;
    //! overridden forms of Mat::row() etc.
    Mat_ row(int y) const;
    Mat_ col(int x) const;
    Mat_ diag(int d=0) const;
    CV_NODISCARD_STD Mat_ clone() const;

    //! overridden forms of Mat::elemSize() etc.
    size_t elemSize() const;
    size_t elemSize1() const;
    int type() const;
    int depth() const;
    int channels() const;
    size_t step1(int i=0) const;
    //! returns step()/sizeof(_Tp)
    size_t stepT(int i=0) const;

    //! overridden forms of Mat::zeros() etc. Data type is omitted, of course
    CV_NODISCARD_STD static MatExpr zeros(int rows, int cols);
    CV_NODISCARD_STD static MatExpr zeros(Size size);
    CV_NODISCARD_STD static MatExpr zeros(int _ndims, const int* _sizes);
    CV_NODISCARD_STD static MatExpr ones(int rows, int cols);
    CV_NODISCARD_STD static MatExpr ones(Size size);
    CV_NODISCARD_STD static MatExpr ones(int _ndims, const int* _sizes);
    CV_NODISCARD_STD static MatExpr eye(int rows, int cols);
    CV_NODISCARD_STD static MatExpr eye(Size size);

    //! some more overridden methods
    Mat_& adjustROI( int dtop, int dbottom, int dleft, int dright );
    Mat_ operator()( const Range& rowRange, const Range& colRange ) const;
    Mat_ operator()( const Rect& roi ) const;
    Mat_ operator()( const Range* ranges ) const;
    Mat_ operator()(const std::vector<Range>& ranges) const;

    //! more convenient forms of row and element access operators
    _Tp* operator [](int y);
    const _Tp* operator [](int y) const;

    //! returns reference to the specified element
    _Tp& operator ()(const int* idx);
    //! returns read-only reference to the specified element
    const _Tp& operator ()(const int* idx) const;

    //! returns reference to the specified element
    template<int n> _Tp& operator ()(const Vec<int, n>& idx);
    //! returns read-only reference to the specified element
    template<int n> const _Tp& operator ()(const Vec<int, n>& idx) const;

    //! returns reference to the specified element (1D case)
    _Tp& operator ()(int idx0);
    //! returns read-only reference to the specified element (1D case)
    const _Tp& operator ()(int idx0) const;
    //! returns reference to the specified element (2D case)
    _Tp& operator ()(int row, int col);
    //! returns read-only reference to the specified element (2D case)
    const _Tp& operator ()(int row, int col) const;
    //! returns reference to the specified element (3D case)
    _Tp& operator ()(int idx0, int idx1, int idx2);
    //! returns read-only reference to the specified element (3D case)
    const _Tp& operator ()(int idx0, int idx1, int idx2) const;

    _Tp& operator ()(Point pt);
    const _Tp& operator ()(Point pt) const;

    //! conversion to vector.
    operator std::vector<_Tp>() const;

    //! conversion to array.
    template<std::size_t _Nm> operator std::array<_Tp, _Nm>() const;

    //! conversion to Vec
    template<int n> operator Vec<typename DataType<_Tp>::channel_type, n>() const;
    //! conversion to Matx
    template<int m, int n> operator Matx<typename DataType<_Tp>::channel_type, m, n>() const;

    Mat_(Mat_&& m);
    Mat_& operator = (Mat_&& m);

    Mat_(Mat&& m);
    Mat_& operator = (Mat&& m);

    Mat_(MatExpr&& e);
};
