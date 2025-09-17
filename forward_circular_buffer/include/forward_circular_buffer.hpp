#pragma once

#include <iterator>
#include <type_traits>

#include "stdfunc.hpp"

namespace {

#define requireNonEmpty() ( stdfunc::assert( !this->empty() ) )
#define requireInRange( _index ) \
    ( stdfunc::assert( ( _index ) < this->size() ) )
#define ensureCapacity() ( stdfunc::assert( !this->full() ) )

} // namespace

namespace fcb {

// TODO: Implement view
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

    // Iterators (logical order: oldest -> newest)
    struct iterator {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr iterator( forwardCircularBuffer* _data, size_t _index )
            : _data( _data ), _index( _index ) {}
        iterator() = default;

        using iteratorCategory_t = std::random_access_iterator_tag;

        constexpr auto operator*() const -> T& {
            return ( _data->_data.at( _data->logical_to_physical( _index ) ) );
        }

        constexpr auto operator->() const -> T* {
            return ( std::addressof( operator*() ) );
        }

        constexpr auto operator++() -> iterator& {
            ++_index;

            return ( *this );
        }

        constexpr auto operator++( int ) -> iterator {
            iterator l_temporaryThis = *this;

            ++*this;

            return ( l_temporaryThis );
        }

        constexpr auto operator--() -> iterator& {
            --_index;

            return ( *this );
        }

        constexpr auto operator--( int ) -> iterator {
            iterator l_temporaryThis = *this;

            --*this;

            return ( l_temporaryThis );
        }

        constexpr auto operator+=( ptrdiff_t _difference ) -> iterator& {
            _index = static_cast< size_t >( static_cast< ptrdiff_t >( _index ) +
                                            _difference );

            return ( *this );
        }

        constexpr auto operator-=( ptrdiff_t _difference ) -> iterator& {
            _index = static_cast< size_t >( static_cast< ptrdiff_t >( _index ) -
                                            _difference );

            return ( *this );
        }

        constexpr auto operator+( ptrdiff_t _difference ) const -> iterator {
            return ( iterator( _data, static_cast< size_t >(
                                          static_cast< ptrdiff_t >( _index ) +
                                          _difference ) ) );
        }

        constexpr auto operator-( ptrdiff_t _difference ) const -> iterator {
            return ( iterator( _data, static_cast< size_t >(
                                          static_cast< ptrdiff_t >( _index ) -
                                          _difference ) ) );
        }

        constexpr auto operator-( const iterator& _o ) const -> ptrdiff_t {
            // if buffers differ, result is unspecified: give difference of
            // positions if same buffer, else 0
            return ( static_cast< ptrdiff_t >( _index ) -
                     static_cast< ptrdiff_t >( _o._index ) );
        }

        constexpr auto operator[]( ptrdiff_t _difference ) const -> T& {
            return ( *( *this + _difference ) );
        }

        constexpr auto operator==( const iterator& _o ) const -> bool {
            return ( ( _data == _o._data ) && ( _index == _o._index ) );
        }

        constexpr auto operator!=( const iterator& _o ) const -> bool {
            return ( !( *this == _o ) );
        }

        constexpr auto operator<( const iterator& _o ) const -> bool {
            return ( _index < _o._index );
        }

        constexpr auto operator<=( const iterator& _o ) const -> bool {
            return ( _index <= _o._index );
        }

        constexpr auto operator>( const iterator& _o ) const -> bool {
            return ( _index > _o._index );
        }

        constexpr auto operator>=( const iterator& _o ) const -> bool {
            return ( _index >= _o._index );
        }

    private:
        forwardCircularBuffer* _data = nullptr;
        size_t _index = 0;

        friend struct const_iterator;
    };

    struct const_iterator {
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::random_access_iterator_tag;

        constexpr const_iterator( const forwardCircularBuffer* _data,
                                  size_t _index )
            : _data( _data ), _index( _index ) {}
        constexpr const_iterator( const iterator& _iterator )
            : _data( _iterator._data ), _index( _iterator._index ) {}
        const_iterator() = default;

        using iteratorCategory_t = std::random_access_iterator_tag;

        constexpr auto operator*() const -> const T& {
            return ( _data->_data.at( _data->logical_to_physical( _index ) ) );
        }

        constexpr auto operator->() const -> T* {
            return ( std::addressof( operator*() ) );
        }

        constexpr auto operator++() -> const_iterator& {
            ++_index;

            return ( *this );
        }

        constexpr auto operator++( int ) -> const_iterator {
            const_iterator l_temporaryThis = *this;

            ++*this;

            return ( l_temporaryThis );
        }

        constexpr auto operator--() -> const_iterator& {
            --_index;

            return ( *this );
        }

        constexpr auto operator--( int ) -> const_iterator {
            const_iterator l_temporaryThis = *this;

            --*this;

            return ( l_temporaryThis );
        }

        constexpr auto operator+=( ptrdiff_t _difference ) -> const_iterator& {
            _index = static_cast< size_t >( static_cast< ptrdiff_t >( _index ) +
                                            _difference );

            return ( *this );
        }

        constexpr auto operator-=( ptrdiff_t _difference ) -> const_iterator& {
            _index = static_cast< size_t >( static_cast< ptrdiff_t >( _index ) -
                                            _difference );

            return ( *this );
        }

        constexpr auto operator+( ptrdiff_t _difference ) const
            -> const_iterator {
            return ( const_iterator(
                _data,
                static_cast< size_t >( static_cast< ptrdiff_t >( _index ) +
                                       _difference ) ) );
        }

        constexpr auto operator-( ptrdiff_t _difference ) const
            -> const_iterator {
            return ( const_iterator(
                _data,
                static_cast< size_t >( static_cast< ptrdiff_t >( _index ) -
                                       _difference ) ) );
        }

        constexpr auto operator-( const const_iterator& _o ) const
            -> ptrdiff_t {
            return ( static_cast< ptrdiff_t >( _index ) -
                     static_cast< ptrdiff_t >( _o._index ) );
        }

        constexpr auto operator[]( ptrdiff_t _difference ) const -> const T& {
            return ( *( *this + _difference ) );
        }

        constexpr auto operator==( const const_iterator& _o ) const -> bool {
            return ( ( _data == _o._data ) && ( _index == _o._index ) );
        }

        constexpr auto operator!=( const const_iterator& _o ) const -> bool {
            return ( !( *this == _o ) );
        }

        constexpr auto operator<( const const_iterator& _o ) const -> bool {
            return ( _index < _o._index );
        }

        constexpr auto operator<=( const const_iterator& _o ) const -> bool {
            return ( _index <= _o._index );
        }

        constexpr auto operator>( const const_iterator& _o ) const -> bool {
            return ( _index > _o._index );
        }

        constexpr auto operator>=( const const_iterator& _o ) const -> bool {
            return ( _index >= _o._index );
        }

    private:
        const forwardCircularBuffer* _data = nullptr;
        size_t _index = 0;
    };

protected:
    // logical -> physical index
    [[nodiscard]] constexpr auto logical_to_physical( size_t _index ) const
        -> size_t {
        requireInRange( _index );

        return ( ( full() ) ? ( ( _currentBufferIndex + _index ) % max_size() )
                            : ( _index ) );
    }

public:
    // DR 776
    constexpr void fill( const T& _value ) {
        std::fill_n( begin(), size(), _value );

        _elementAmount = max_size();
    }

    constexpr void swap( forwardCircularBuffer& _buffer ) {
        std::swap_ranges( begin(), end(), _buffer.begin() );
    }

    // Iterators (logical order)
    [[nodiscard]] CONST constexpr auto begin() -> iterator {
        return ( iterator( this, 0 ) );
    }

    [[nodiscard]] CONST constexpr auto begin() const -> const_iterator {
        return ( const_iterator( this, 0 ) );
    }

    [[nodiscard]] CONST constexpr auto end() -> iterator {
        return ( iterator( this, size() ) );
    }

    [[nodiscard]] CONST constexpr auto end() const -> const_iterator {
        return ( const_iterator( this, size() ) );
    }

    [[nodiscard]] CONST constexpr auto rbegin()
        -> std::reverse_iterator< iterator > {
        return ( std::reverse_iterator< iterator >( end() ) );
    }

    [[nodiscard]] CONST constexpr auto rbegin() const
        -> std::reverse_iterator< const_iterator > {
        return ( std::reverse_iterator< const_iterator >( end() ) );
    }

    [[nodiscard]] CONST constexpr auto rend()
        -> std::reverse_iterator< iterator > {
        return ( std::reverse_iterator< iterator >( begin() ) );
    }

    [[nodiscard]] CONST constexpr auto rend() const
        -> std::reverse_iterator< const_iterator > {
        return ( std::reverse_iterator< const_iterator >( begin() ) );
    }

    [[nodiscard]] CONST constexpr auto cbegin() const -> const_iterator {
        return ( const_iterator( this, 0 ) );
    }

    [[nodiscard]] CONST constexpr auto cend() const -> const_iterator {
        return ( const_iterator( this, size() ) );
    }

    [[nodiscard]] CONST constexpr auto crbegin() const
        -> std::reverse_iterator< const_iterator > {
        return ( std::reverse_iterator< const_iterator >( cend() ) );
    }

    [[nodiscard]] CONST constexpr auto crend() const
        -> std::reverse_iterator< const_iterator > {
        return ( std::reverse_iterator< const_iterator >( cbegin() ) );
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

    [[nodiscard]] CONST FORCE_INLINE constexpr auto full() const -> bool {
        return ( size() == max_size() );
    }

    // Element access
    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) -> T& {
        requireInRange( _index );

        return ( _data[ logical_to_physical( _index ) ] );
    }

    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) const -> const T& {
        requireInRange( _index );

        return ( _data[ logical_to_physical( _index ) ] );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) -> T& {
        requireInRange( _index );

        return ( _data.at( logical_to_physical( _index ) ) );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) const -> const T& {
        requireInRange( _index );

        return ( _data.at( logical_to_physical( _index ) ) );
    }

    [[nodiscard]]
    constexpr auto front() -> T& {
        requireNonEmpty();

        return ( _data.at( ( full() ) ? ( _currentBufferIndex ) : ( 0 ) ) );
    }

    [[nodiscard]]
    constexpr auto front() const -> const T& {
        requireNonEmpty();

        return ( _data.at( ( full() ) ? ( _currentBufferIndex ) : ( 0 ) ) );
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

    // Vector operations
    constexpr void clear() {
        _currentBufferIndex = 0;
        _previousBufferIndex = 0;
        _elementAmount = 0;
    }

    constexpr void push_back( const T& _value ) {
        ensureCapacity();

        const size_t l_currentBufferIndex = _currentBufferIndex;

        if constexpr ( std::is_assignable_v< T&, T > ) {
            // simple case: assign into existing slot
            _data.at( l_currentBufferIndex ) = _value;
        } else {
            // fallback: destroy existing element and construct in-place
            static_assert( std::is_default_constructible_v< T >,
                           "T must be assignable or default-constructible to "
                           "use std::array<T,N> storage. "
                           "Use std::optional<T> or raw aligned storage for "
                           "move-only non-default-constructible types." );
            std::destroy_at(
                std::addressof( _data.at( l_currentBufferIndex ) ) );
            std::construct_at(
                std::addressof( _data.at( l_currentBufferIndex ) ), _value );
        }

#if 0
        _data.at( l_currentBufferIndex ) = _value;
#endif

        _currentBufferIndex = ( ( l_currentBufferIndex + 1 ) % max_size() );

        _previousBufferIndex = l_currentBufferIndex;

        _elementAmount++;
    }

    constexpr void push_back( T&& _value ) {
        ensureCapacity();

        const size_t l_currentBufferIndex = _currentBufferIndex;

#if 0
        _data.at( l_currentBufferIndex ) = std::move( _value );
#endif
        if constexpr ( std::is_assignable_v< T&, T > ) {
            _data.at( l_currentBufferIndex ) = std::move( _value );
        } else {
            static_assert( std::is_default_constructible_v< T >,
                           "T must be assignable or default-constructible to "
                           "use std::array<T,N> storage. "
                           "Use std::optional<T> or raw aligned storage for "
                           "move-only non-default-constructible types." );
            std::destroy_at(
                std::addressof( _data.at( l_currentBufferIndex ) ) );
            std::construct_at(
                std::addressof( _data.at( l_currentBufferIndex ) ),
                std::move( _value ) );
        }

        _currentBufferIndex = ( ( l_currentBufferIndex + 1 ) % max_size() );

        _previousBufferIndex = l_currentBufferIndex;

        _elementAmount++;
    }

    template < typename... Arguments >
        requires std::is_constructible_v< T, Arguments... >
    constexpr auto emplace_back( Arguments&&... _arguments ) -> T& {
        ensureCapacity();

        const size_t l_currentBufferIndex = _currentBufferIndex;

        if constexpr ( std::is_assignable_v< T&, T > ) {
            // If T supports assignment from a temporary, build a temporary and
            // assign
            _data.at( l_currentBufferIndex ) =
                T( std::forward< Arguments >( _arguments )... );
        } else if constexpr ( std::is_default_constructible_v< T > ) {
            // If T is not assignable but is default-constructible, destroy
            // current and construct in-place
            std::destroy_at(
                std::addressof( _data.at( l_currentBufferIndex ) ) );
            std::construct_at(
                std::addressof( _data.at( l_currentBufferIndex ) ),
                std::forward< Arguments >( _arguments )... );
        } else {
            static_assert(
                std::is_assignable_v< T&, T > ||
                    std::is_default_constructible_v< T >,
                "emplace_back: T must be assignable or default-constructible "
                "when using std::array<T,N> storage. "
                "For move-only non-default-constructible types use "
                "std::optional<T> or raw aligned storage + placement-new." );
        }

#if 0
        _data.at( l_currentBufferIndex ) =
            T(std::forward< Arguments >( _arguments )... );
#endif

        _elementAmount++;

        return ( back() );
    }

    /**
     * @brief Remove and return last element
     *
     * @return Removed element
     */
    constexpr auto pop_back() -> T {
        // TODO: Contract instead
        requireNonEmpty();

        const size_t l_removedIndex = _previousBufferIndex;

        T l_returnValue = std::move( back() );

        // TODO: Call destructor

        _currentBufferIndex--;
        // TODO: Improve
        _previousBufferIndex =
            ( ( l_removedIndex + max_size() - 1 ) % max_size() );
        _elementAmount--;

        return ( l_returnValue );
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
