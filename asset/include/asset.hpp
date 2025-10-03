#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace asset {

namespace loader {

auto init() -> bool;
void quit();

} // namespace loader

using asset_t = std::optional< std::vector< std::byte > >;

namespace sync {

auto load( std::string_view _path ) -> asset_t;
auto save( std::span< const std::byte > _asset,
           std::string_view _path,
           bool _needAppend ) -> bool;

} // namespace sync

namespace async {

auto load( std::string_view _path ) -> asset_t;
auto save( std::span< const std::byte > _asset,
           std::string_view _path,
           bool _needAppend ) -> bool;

} // namespace async

} // namespace asset
