#include "player_t.h"

#include <SDL3/SDL_rect.h>

#include "log.h"
#include "stdfunc.h"

player_t player_t$create( void ) {
    player_t l_returnValue = DEFAULT_PLAYER;

    {
        l_returnValue.character = character_t$create();
        l_returnValue.inputBuffer = inputBuffer_t$create();
    }

    return ( l_returnValue );
}

bool player_t$destroy( player_t* restrict _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$destroy( &( _player->character ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Destroying player" );

            goto EXIT;
        }

        l_returnValue = inputBuffer_t$destroy( &( _player->inputBuffer ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Destroying input buffer" );

            goto EXIT;
        }

        _player->isGuardBroken = NULL;
        _player->healthPointsMax = NULL;
        _player->restorableHealthPointsMax = NULL;
        _player->guardPointsMax = NULL;
        _player->meterPointsMax = NULL;
        _player->healthPoints = NULL;
        _player->restorableHealthPoints = NULL;
        _player->guardPoints = NULL;
        _player->meterPoints = NULL;

        if ( LIKELY( _player->name ) ) {
            free( _player->name );
            _player->name = NULL;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

#if 0
bool player_t$state$add$fromPaths( player_t* restrict _player,
                                   SDL_Renderer* _renderer,
                                   char* restrict _boxesPath,
                                   char* const* restrict _animationPaths,
                                   bool _isActionable,
                                   bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesPath ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationPaths ) ||
         UNLIKELY( !arrayLength( _animationPaths ) ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$state$add$fromPaths(
            &( _player->character ), _renderer, _boxesPath, _animationPaths,
            _isActionable, _canLoop );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Adding player state from paths" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$state$add$fromGlob( player_t* restrict _player,
                                  SDL_Renderer* _renderer,
                                  const char* restrict _boxesGlob,
                                  const char* restrict _animationGlob,
                                  bool _isActionable,
                                  bool _canLoop ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_renderer ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_boxesGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_animationGlob ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$state$add$fromGlob(
            &( _player->character ), _renderer, _boxesGlob, _animationGlob,
            _isActionable, _canLoop );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Adding player state from glob" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$states$remove( player_t* restrict _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        FOR_ARRAY( state_t* const*, _player->character.states ) {
            l_returnValue =
                character_t$state$remove( &( _player->character ), *_element );

            if ( UNLIKELY( !l_returnValue ) ) {
                log$transaction$query( ( logLevel_t )error,
                                       "Removing state from player" );

                goto EXIT;
            }
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
#endif

bool player_t$load( player_t* restrict _player, SDL_Renderer* _renderer ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$load( &( _player->character ), _renderer );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Loading character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$unload( player_t* restrict _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$unload( &( _player->character ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Unloading character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$step( player_t* restrict _player ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$step( &( _player->character ) );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Stepping character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$render( const player_t* restrict _player,
                      const SDL_FRect* restrict _cameraRectangle,
                      bool _doDrawBoxes ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_cameraRectangle ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = character_t$render( &( _player->character ),
                                            _cameraRectangle, _doDrawBoxes );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error, "Rendering character" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool player_t$input$add( player_t* restrict _player,
                         const input_t* _input,
                         const size_t _frame ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_input ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue =
            inputBuffer_t$insert( &( _player->inputBuffer ), _input, _frame );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Adding input to player" );

            goto EXIT;
        }

        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

input_t** player_t$inputsSequences$get$withLimit( player_t* restrict _player,
                                                  const size_t _currentFrame,
                                                  const size_t _limitAmount ) {
    input_t** l_returnValue = NULL;

    if ( UNLIKELY( !_player ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    if ( UNLIKELY( !_limitAmount ) ) {
        log$transaction$query( ( logLevel_t )error, "Invalid argument" );

        goto EXIT;
    }

    {
        l_returnValue = inputBuffer_t$inputsSequence$get$withLimit(
            &( _player->inputBuffer ), _currentFrame, _limitAmount );

        if ( UNLIKELY( !l_returnValue ) ) {
            log$transaction$query( ( logLevel_t )error,
                                   "Getting player inputs" );

            goto EXIT;
        }
    }

EXIT:
    return ( l_returnValue );
}
