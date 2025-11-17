#pragma once
#include <vector>
#include <complex>
#include <array>
#include <cstdio>
#include "interface/macros.h"
#include "enums.h"

template <class, template <class, class...> class>
struct is_instance : public std::false_type
{
};

template <class... Ts, template <class, class...> class U>
struct is_instance<U<Ts...>, U> : public std::true_type
{
};

struct CallInfo
{
    int newSamplesCount;
    double startBlockTime, endBlockTime;
};

BEGIN_NAMESPACE_PROCESSING

using DefaultSizeType = size_t;

template <typename T>
class DSVector
{
public:
    using size_type = DefaultSizeType;

    DSVector<T>(size_type size = 0)
        : container_(size, static_cast<T>(0.0)){};
    DSVector<T>(const std::vector<T> initializer)
        : container_(initializer){};
    DSVector<T>(std::initializer_list<T> initializer)
        : container_(initializer){};
    template <class InputIt>
    DSVector<T>(InputIt first, InputIt last)
    {
        container_ = std::vector<T>(first, last);
    }

    T* data()
    {
        return container_.data();
    }
    const T* data() const
    {
        return container_.data();
    }
    size_type size() const
    {
        return (size_type) container_.size();
    }
    void resize(const size_type size)
    {
        container_.resize(size);
    }
    void resize(const size_type size, const T& value)
    {
        container_.resize(size, value);
    }
    void fill(const T& value)
    {
        std::fill(container_.begin(), container_.end(), value);
    }
    T& operator()(size_type i)
    {
        return container_.at(i);
    }
    const T& operator()(size_type i) const
    {
        return container_.at(i);
    }
    T& operator[](size_type i)
    {
        return container_.at(i);
    }
    const T& operator[](size_type i) const
    {
        return container_.at(i);
    }
    operator std::vector<T>() const
    {
        return container_;
    }
    operator std::vector<T>()
    {
        return container_;
    }

    auto begin() const
    {
        return container_.begin();
    }

    auto end() const
    {
        return container_.end();
    }

    void clear()
    {
        container_.clear();
    }

    void push_back(const T& value)
    {
        container_.push_back(value);
    }

private:
    std::vector<T> container_;
};

template <typename T>
class DSMatrix
{
public:
    using size_type = DefaultSizeType;

    class DSMatrixRow
    {
    private:
        DSMatrix& matrix_;
        size_type i_;

    public:
        DSMatrixRow(DSMatrix& matrix, size_type i)
            : matrix_(matrix)
            , i_(i){};
        T& operator[](size_type j)
        {
            return matrix_(i_, j);
        }
    };

    class DSConstMatrixRow
    {
    private:
        const DSMatrix& matrix_;
        size_type i_;

    public:
        DSConstMatrixRow(const DSMatrix& matrix, size_type i)
            : matrix_(matrix)
            , i_(i){};
        const T& operator[](size_type j) const
        {
            return matrix_(i_, j);
        }
    };
    DSMatrix<T>(size_type m = 0, size_type n = 0)
        : m_(m)
        , n_(n)
        , container_(m * n, static_cast<T>(0.0)){};
    DSMatrix<T>(std::initializer_list<std::initializer_list<T>> initializer)
        : DSMatrix(size_type(initializer.size()), size_type(initializer.size() ? initializer.begin()->size() : 0))
    {
        size_type i = 0, j = 0;
        for (const auto& l : initializer)
        {
            for (const auto& v : l)
            {
                container_[i + j * m_] = v;
                ++j;
            }
            j = 0;
            ++i;
        }
    };
    template <class InputIt>
    DSMatrix<T>(InputIt first, InputIt last, size_type m, size_type n)
        : m_(m)
        , n_(n)
    {
        container_ = std::vector<T>(first, last);
    };

    T* data()
    {
        return container_.data();
    }
    const T* data() const
    {
        return container_.data();
    }
    size_type size() const
    {
        return (size_type) container_.size();
    }
    size_type n() const
    {
        return n_;
    }
    size_type m() const
    {
        return m_;
    }
    DSMatrixRow operator[](size_type i)
    {
        return DSMatrixRow(*this, i);
    }
    const DSConstMatrixRow operator[](size_type i) const
    {
        return DSConstMatrixRow(*this, i);
    }
    T& operator()(size_type i, size_type j)
    {
        if (i < 0 || m_ <= i || j < 0 || n_ <= j)
            throw std::out_of_range("Accessing element " + std::to_string(i) + "x" + std::to_string(j) + " of matrix with dimensions " +
                                    std::to_string(m_) + "x" + std::to_string(n_));
        return container_.at(i + j * m_);
    }
    const T& operator()(size_type i, size_type j) const
    {
        if (i < 0 || m_ <= i || j < 0 || n_ <= j)
            throw std::out_of_range("Accessing element " + std::to_string(i) + "x" + std::to_string(j) + " of matrix with dimensions " +
                                    std::to_string(m_) + "x" + std::to_string(n_));
        return container_.at(i + j * m_);
    }

    operator std::vector<T>()
    {
        return container_;
    }

    auto begin() const
    {
        return container_.begin();
    }

    auto end() const
    {
        return container_.end();
    }

    void push_back(const T& value)
    {
        container_.push_back(value);
    }

    void clear()
    {
        container_.clear();
    }

private:
    size_type m_, n_;
    std::vector<T> container_;
};

using Byte = uint8_t;
using ShortInt = int8_t;
using SmallInt = int16_t;
using Word = uint16_t;
using Integer = int32_t;
using Single = float;
using Int64 = int64_t;
using Double = double;
using Scalar = double;
using LongWord = uint32_t;
using Bytes = std::vector<unsigned char>;
using Vector = DSVector<Double>;
using Matrix = DSMatrix<Double>;
using Text = std::wstring;

#pragma pack(push, 1)
struct CANMessage
{
    LongWord arbId;
    std::array<Byte, 8> data;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct CANFDMessage
{
    LongWord arbId;
    bool extended;
    Byte reserved1;
    Byte reserved2;
    Byte dataSize;
    std::array<Byte, 64> data;
};
#pragma pack(pop)

using ComplexSingle = std::complex<float>;
using ComplexDouble = std::complex<double>;
using Complex = std::complex<double>;
using ComplexScalar = ComplexDouble;
using ComplexVector = DSVector<ComplexDouble>;
using ComplexMatrix = DSMatrix<ComplexDouble>;

using Bytes8 = std::array<Byte, 8>;
using Bytes16 = std::array<Byte, 16>;
using Bytes32 = std::array<Byte, 32>;
using Bytes64 = std::array<Byte, 64>;

class DSComplexNumber
{
};

class DSNumber
{
};

class DSOrdinal
{
};

class DSFloat
{
};

class DSComplex
{
};

using Time = double;

using Any = void;

template <ChannelDataType dataType>
struct DSDataType
{
};

template <>
struct DSDataType<ChannelDataType::Byte>
{
    using type = typename Byte;
};

template <>
struct DSDataType<ChannelDataType::ShortInt>
{
    using type = typename ShortInt;
};

template <>
struct DSDataType<ChannelDataType::SmallInt>
{
    using type = typename SmallInt;
};

template <>
struct DSDataType<ChannelDataType::Word>
{
    using type = typename Word;
};

template <>
struct DSDataType<ChannelDataType::Integer>
{
    using type = typename Integer;
};

template <>
struct DSDataType<ChannelDataType::Single>
{
    using type = typename Single;
};

template <>
struct DSDataType<ChannelDataType::Int64>
{
    using type = typename Int64;
};

template <>
struct DSDataType<ChannelDataType::Double>
{
    using type = typename Double;
};

template <>
struct DSDataType<ChannelDataType::Longword>
{
    using type = typename LongWord;
};

template <>
struct DSDataType<ChannelDataType::ComplexSingle>
{
    using type = typename ComplexSingle;
};


template <>
struct DSDataType<ChannelDataType::ComplexDouble>
{
    using type = typename ComplexDouble;
};


template <>
struct DSDataType<ChannelDataType::Text>
{
    using type = typename Text;
};

template <>
struct DSDataType<ChannelDataType::Binary>
{
    using type = typename Bytes;
};

template <>
struct DSDataType<ChannelDataType::CANMessage>
{
    using type = typename CANMessage;
};

template <>
struct DSDataType<ChannelDataType::CANFDMessage>
{
    using type = typename CANFDMessage;
};

template <>
struct DSDataType<ChannelDataType::Bytes8>
{
    using type = typename Bytes8;
};

template <>
struct DSDataType<ChannelDataType::Bytes16>
{
    using type = typename Bytes16;
};

template <>
struct DSDataType<ChannelDataType::Bytes32>
{
    using type = typename Bytes32;
};

template <>
struct DSDataType<ChannelDataType::Bytes64>
{
    using type = typename Bytes64;
};

template <ChannelDataType dataType>
using DSDataType_t = typename DSDataType<dataType>::type;

template <typename T>
struct common_type
{
    using type = typename T;
};

template <>
struct common_type<DSComplexNumber>
{
    using type = typename ComplexDouble;
};

template <>
struct common_type<DSComplex>
{
    using type = typename ComplexDouble;
};

template <>
struct common_type<DSNumber>
{
    using type = typename Double;
};

template <>
struct common_type<DSFloat>
{
    using type = typename Double;
};

template <>
struct common_type<DSOrdinal>
{
    using type = Int64;
};

template <typename T>
struct common_type<DSVector<T>>
{
    using type = DSVector<typename common_type<T>::type>;
};

template <typename T>
struct common_type<DSMatrix<T>>
{
    using type = DSMatrix<typename common_type<T>::type>;
};

template <typename T>
using common_type_t = typename common_type<T>::type;

template <typename T>
struct innermost
{
    using type = T;
};

template <template <typename> typename U, typename T>
struct innermost<U<T>>
{
    using type = typename innermost<T>::type;
};

template <typename T>
using innermost_t = typename innermost<T>::type;

template <typename T>
struct contained
{
    using type = T;
};

template <template <typename> typename U, typename T>
struct contained<U<T>>
{
    using type = T;
};

template <typename T>
using contained_t = typename contained<T>::type;

END_NAMESPACE_PROCESSING
