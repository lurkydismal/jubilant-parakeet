#pragma once

#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "slickdl/point.hpp"
#include "state.hpp"
#include "stddebug.hpp"

namespace object {

using object_t = struct object {
    object() = delete;
    object( const object& ) = delete;

    constexpr object(
        std::span< std::pair< std::string_view, state::state_t > > _states ) {
        _stateNames.reserve( _states.size() );
        this->_states.reserve( _states.size() );

        for ( auto&& [ _name, _state ] : _states ) {
            _stateNames.emplace_back( _name );
            this->_states.emplace_back( std::move( _state ) );
        }
    }

#if 0
    // FIX: Implement
    constexpr object(
        std::initializer_list< std::pair< std::string, state::state_t > >
            _states )
        : object( std::span( _states ) ) {}
#endif

    object( object&& ) = default;
    ~object() = default;

    auto operator=( const object& ) -> object& = default;
    auto operator=( object&& ) -> object& = default;

    [[nodiscard]] constexpr auto currentState() const -> const state::state_t& {
        return ( _states.at( _currentStateIndex ) );
    }

    constexpr void step() { _currentState().step(); }

    constexpr void move( float _offsetX, float _offsetY ) {
        stdfunc::assert( _offsetX );
        stdfunc::assert( _offsetY );

        _position.x = std::clamp( ( _position.x + _offsetX ), _minPosition.x,
                                  _maxPosition.x );
        _position.y = std::clamp( ( _position.y + _offsetY ), _minPosition.y,
                                  _maxPosition.y );
    }

    void render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes );

    void render( const slickdl::box_t< float >& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes,
                 double _angle,
                 slickdl::flip_t _flipMode,
                 const std::optional< slickdl::point_t< float > >& _center =
                     std::nullopt );

    // helpers
private:
    [[nodiscard]] constexpr auto _currentState() -> state::state_t& {
        return ( _states.at( _currentStateIndex ) );
    }

    [[nodiscard]] constexpr auto _currentState() const
        -> const state::state_t& {
        return ( _states.at( _currentStateIndex ) );
    }

    // Variables
private:
    std::vector< std::string > _stateNames;
    std::vector< state::state_t > _states;
    size_t _currentStateIndex{};

    slickdl::point_t< float > _position{};
    slickdl::point_t< float > _minPosition{};
    slickdl::point_t< float > _maxPosition{};
};

} // namespace object
