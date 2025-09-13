#pragma once

using slickdl_t = struct slickdl {
    slickdl() = default;
    slickdl( const slickdl& ) = default;
    slickdl( slickdl&& ) = default;
    ~slickdl() = default;
    auto operator=( const slickdl& ) -> slickdl& = default;
    auto operator=( slickdl&& ) -> slickdl& = default;
};
