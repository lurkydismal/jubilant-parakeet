#pragma once

#include <cstddef>
#include <iterator>

#include "forward_circular_buffer.hpp"
#include "input.hpp"
#include "stdfunc.hpp"

#define MAX_DELAY_BETWEEN_INPUTS ( 8 )

namespace input {

template < size_t N >
    requires( N > 0 )
struct inputBuffer {
    inputBuffer() = default;
    inputBuffer( const inputBuffer& ) = default;
    inputBuffer( inputBuffer&& ) = default;
    ~inputBuffer() = default;
    auto operator=( const inputBuffer& ) -> inputBuffer& = default;
    auto operator=( inputBuffer&& ) -> inputBuffer& = default;
    auto operator==( const inputBuffer& ) const -> bool = default;

    // DR 776
    constexpr void fill( const input_t& _value ) { _data.fill( _value ); }

    constexpr void swap( inputBuffer& _buffer ) { _data.swap( _buffer._data ); }

    // Iterators
    [[nodiscard]] CONST constexpr auto begin() -> input_t* {
        return ( _data.begin() );
    }

    [[nodiscard]]
    constexpr auto begin() const -> input_t* {
        return ( _data.begin() );
    }

    [[nodiscard]] CONST constexpr auto end() -> input_t* {
        return ( _data.end() );
    }

    [[nodiscard]]
    constexpr auto end() const -> input_t* {
        return ( _data.end() );
    }

    [[nodiscard]] CONST constexpr auto rbegin()
        -> std::reverse_iterator< input_t* > {
        return ( _data.rbegin() );
    }

    [[nodiscard]]
    constexpr auto rbegin() const -> std::reverse_iterator< const input_t* > {
        return ( _data.rbegin() );
    }

    [[nodiscard]] CONST constexpr auto rend()
        -> std::reverse_iterator< input_t* > {
        return ( _data.rend() );
    }

    [[nodiscard]]
    constexpr auto rend() const -> std::reverse_iterator< const input_t* > {
        return ( _data.rend() );
    }

    [[nodiscard]]
    constexpr auto cbegin() const -> input_t* {
        return ( _data.cbegin() );
    }

    [[nodiscard]]
    constexpr auto cend() const -> input_t* {
        return ( _data.cend() );
    }

    [[nodiscard]]
    constexpr auto crbegin() const -> std::reverse_iterator< const input_t* > {
        return ( _data.crbegin() );
    }

    [[nodiscard]]
    constexpr auto crend() const -> std::reverse_iterator< const input_t* > {
        return ( _data.crend() );
    }

    // Capacity
    [[nodiscard]] CONST FORCE_INLINE constexpr auto size() const -> size_t {
        return ( _data.size() );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto max_size() const -> size_t {
        return ( _data.max_size() );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto empty() const -> bool {
        return ( _data.empty() );
    }

    // Element access
    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) -> input_t& {
        return ( _data[ _index ] );
    }

    [[nodiscard]]
    constexpr auto operator[]( size_t _index ) const -> const input_t& {
        return ( _data[ _index ] );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) -> input_t& {
        return ( _data.at( _index ) );
    }

    [[nodiscard]] constexpr auto at( size_t _index ) const -> const input_t& {
        return ( _data.at( _index ) );
    }

    [[nodiscard]]
    constexpr auto front() -> input_t& {
        return ( _data.front() );
    }

    [[nodiscard]]
    constexpr auto front() const -> const input_t& {
        return ( _data.front() );
    }

    [[nodiscard]]
    constexpr auto back() -> input_t& {
        return ( _data.back() );
    }

    [[nodiscard]]
    constexpr auto back() const -> const input_t& {
        return ( _data.back() );
    }

    [[nodiscard]] CONST FORCE_INLINE constexpr auto data() -> input_t* {
        return ( _data.data() );
    }

    [[nodiscard]]
    constexpr auto data() const -> const input_t* {
        return ( _data.data() );
    }

private:
    fcb::forwardCircularBuffer_t< input_t, N > _data;
};

template < size_t N >
using inputBuffer_t = inputBuffer< N >;

} // namespace input
