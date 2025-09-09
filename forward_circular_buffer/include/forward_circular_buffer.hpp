#pragma once

#include <iterator>

#include "stdfunc.hpp"

namespace {

#define requireNonEmpty() ( stdfunc::assert( !this->empty() ) )
#define requireInRange( _index ) \
    ( stdfunc::assert( ( _index ) < this->size() ) )

} // namespace

namespace fcb {

// TODO: Implement iterators
// TODO: Implement data
template < typename T, size_t N >
    requires( N > 0 )
struct forwardCircularBuffer {
    forwardCircularBuffer() = default;
    forwardCircularBuffer( const forwardCircularBuffer& ) = default;
    forwardCircularBuffer( forwardCircularBuffer&& ) = default;
    ~forwardCircularBuffer() = default;
    auto operator=( const forwardCircularBuffer& )
        -> forwardCircularBuffer& = default;
    auto operator=( forwardCircularBuffer&& )
        -> forwardCircularBuffer& = default;
    auto operator==( const forwardCircularBuffer& ) const -> bool = default;

    // DR 776
    constexpr void fill( const T& _value ) {
        std::fill_n( begin(), size(), _value );
    }

    constexpr void swap( forwardCircularBuffer& _buffer ) {
        std::swap_ranges( begin(), end(), _buffer.begin() );
    }

    // Iterators
    [[nodiscard]] CONST constexpr auto begin() -> T* {
        return ( ( T* )( data() ) );
    }

    [[nodiscard]]
    constexpr auto begin() const -> T* {
        return ( ( T* )( data() ) );
    }

    [[nodiscard]] CONST constexpr auto end() -> T* {
        return ( ( T* )( data() + N ) );
    }

    [[nodiscard]]
    constexpr auto end() const -> T* {
        return ( ( T* )( data() + N ) );
    }

    [[nodiscard]] CONST constexpr auto rbegin() -> std::reverse_iterator< T* > {
        return ( std::reverse_iterator< T* >( end() ) );
    }

    [[nodiscard]]
    constexpr auto rbegin() const -> std::reverse_iterator< const T* > {
        return ( std::reverse_iterator< const T* >( end() ) );
    }

    [[nodiscard]] CONST constexpr auto rend() -> std::reverse_iterator< T* > {
        return ( std::reverse_iterator< T* >( begin() ) );
    }

    [[nodiscard]]
    constexpr auto rend() const -> std::reverse_iterator< const T* > {
        return ( std::reverse_iterator< const T* >( begin() ) );
    }

    [[nodiscard]]
    constexpr auto cbegin() const -> T* {
        return ( ( T* )( data() ) );
    }

    [[nodiscard]]
    constexpr auto cend() const -> T* {
        return ( ( T* )( data() + N ) );
    }

    [[nodiscard]]
    constexpr auto crbegin() const -> std::reverse_iterator< const T* > {
        return ( std::reverse_iterator< const T* >( end() ) );
    }

    [[nodiscard]]
    constexpr auto crend() const -> std::reverse_iterator< const T* > {
        return ( std::reverse_iterator< const T* >( begin() ) );
    }

    // Capacity
    [[nodiscard]] CONST FORCE_INLINE constexpr auto size() const -> size_t {
        return ( _elementAmount );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto max_size() const -> size_t {
        return ( N );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto empty() const -> bool {
        return ( size() == 0 );
    }

    // Element access
    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) -> T& {
        requireInRange( _index );

        return ( _data[ _index ] );
    }

    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) const -> const T& {
        requireInRange( _index );

        return ( _data[ _index ] );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) -> T& {
        requireInRange( _index );

        return ( _data.at( _index ) );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) const -> const T& {
        requireInRange( _index );

        return ( _data.at( _index ) );
    }

    [[nodiscard]]
    constexpr auto front() -> T& {
        requireNonEmpty();

        return ( _data.at( ( _elementAmount == N ) ? ( _currentBufferIndex )
                                                   : ( 0 ) ) );
    }

    [[nodiscard]]
    constexpr auto front() const -> const T& {
        requireNonEmpty();

        return ( _data.at( ( _elementAmount == N ) ? ( _currentBufferIndex )
                                                   : ( 0 ) ) );
    }

    [[nodiscard]]
    constexpr auto back() -> T& {
        requireNonEmpty();

        return ( _data.at( _previousBufferIndex ) );
    }

    [[nodiscard]]
    constexpr auto back() const -> const T& {
        requireNonEmpty();

        return ( _data.at( _previousBufferIndex ) );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto data() -> T* {
        return ( _data.data() );
    }

    [[nodiscard]]
    constexpr auto data() const -> const T* {
        return ( _data.data() );
    }

private:
    std::array< T, N > _data;
    size_t _currentBufferIndex = 0;
    size_t _previousBufferIndex = 0;
    size_t _elementAmount = 0;
};

template < typename T, size_t N >
using forwardCircularBuffer_t = forwardCircularBuffer< T, N >;

} // namespace fcb
