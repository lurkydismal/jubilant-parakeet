#include "config_t.h"

#include "stdfunc.h"

config_t config_t$create() {
    config_t l_returnValue = DEFAULT_CONFIG;

    return ( l_returnValue );
}

bool config_t$destroy( config_t* _config ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromString( config_t* _config, const char* _string ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_string ) || UNLIKELY( !__builtin_strlen( _string ) ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromAsset( config_t* _config, asset_t* _asset ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_asset ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$load$fromPath( config_t* _config, const char* _path ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    if ( UNLIKELY( !_path ) || UNLIKELY( !__builtin_strlen( _path ) ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}

bool config_t$unload( config_t* _config ) {
    bool l_returnValue = false;

    if ( UNLIKELY( !_config ) ) {
        goto EXIT;
    }

    {
        l_returnValue = true;
    }

EXIT:
    return ( l_returnValue );
}
