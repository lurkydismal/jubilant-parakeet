#pragma once

#include <SDL3/SDL_render.h>

#include <algorithm>
#include <initializer_list>

#include "state.hpp"

namespace object {

using object_t = struct object {
    // TODO: Improve
    using position_t = struct position {
        float x;
        float y;
    };

    object() = delete;
    object( const object& ) = default;
    object( object&& ) = default;
    ~object() = default;

    constexpr object(
        std::span< const std::pair< std::string, state::state_t > > _states ) {
        _stateNames.reserve( _states.size() );
        this->_states.reserve( _states.size() );

        for ( const auto& [ l_name, l_state ] : _states ) {
            _stateNames.emplace_back( l_name );
            this->_states.emplace_back( l_state );
        }
    }

    constexpr object(
        std::initializer_list< std::pair< std::string, state::state_t > >
            _states )
        : object( std::span( _states ) ) {}

    auto operator=( const object& ) -> object& = default;
    auto operator=( object&& ) -> object& = default;

    [[nodiscard]] constexpr auto currentState() const -> const state::state_t& {
        return ( _states.at( _currentStateIndex ) );
    }

    constexpr void step() { _currentState().step(); }

    constexpr void move( float _x, float _y ) {
        stdfunc::assert( _x );
        stdfunc::assert( _y );

        _position.x =
            std::clamp( ( _position.x + _x ), _minPosition.x, _maxPosition.x );
        _position.y =
            std::clamp( ( _position.y + _y ), _minPosition.y, _maxPosition.y );
    }

    void render( const boxes::box_t& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes ) const;

    void render( const boxes::box_t& _cameraBoxCoordinates,
                 bool _doDrawBoxes,
                 bool _doFillBoxes,
                 double _angle,
                 SDL_FlipMode _flipMode ) const;

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

    position_t _position{};
    position_t _minPosition{};
    position_t _maxPosition{};
};

} // namespace object
