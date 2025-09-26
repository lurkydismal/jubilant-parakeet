#pragma once

#include <SDL3/SDL_render.h>

#include <gsl/pointers>
#include <optional>

namespace slickdl {

template < typename T >
concept is_int_or_float =
    ( std::is_same_v< T, int > || std::is_same_v< T, float > );

using renderer_t = gsl::not_null< SDL_Renderer* >;
using texture_t = gsl::not_null< SDL_Texture* >;

// The structure that defines a point
template < typename T >
    requires is_int_or_float< T >
struct point {
    T x, y;
};

template < typename T >
using point_t = point< T >;

// A rectangle, with the origin at the upper left
template < typename T >
    requires is_int_or_float< T >
struct rect {
    [[nodiscard]] constexpr auto operator<=>(
        const rect< T >& _rectangle ) const = default;

    template < typename T2 >
    [[nodiscard]] constexpr operator rect< T2 >() const {
        return ( rect< T2 >{
            static_cast< T2 >( x ),
            static_cast< T2 >( y ),
            static_cast< T2 >( width ),
            static_cast< T2 >( height ),
        } );
    }

    // Determine whether a point resides inside a rectangle
    //
    // A point is considered part of a rectangle if points x and y coordinates
    // are >= to the rectangles top left corner, and < the rectangles x +
    // width and y + height. So a 1x1 rectangle considers point ( 0, 0 ) as
    // "inside" and ( 0, 1 ) as not
    [[nodiscard]] constexpr auto contains( const point_t< T >& _point ) const
        -> bool {
        constexpr auto l_inRange1D = [ & ]( T _point, T _min, T _max ) -> auto {
            return ( ( _point >= _min ) && ( _point < _max ) );
        };

        return ( l_inRange1D( _point.x, x, ( x + width ) ) &&
                 l_inRange1D( _point.y, y, ( y + height ) ) );
    }

    // TODO: Description
    [[nodiscard]] constexpr auto contains( const rect< T >& _rectangle ) const
        -> bool {
        return ( ( x <= _rectangle.x ) && ( y <= _rectangle.y ) &&
                 ( ( x + width ) >= ( _rectangle.x + _rectangle.width ) ) &&
                 ( ( y + height ) >= ( _rectangle.y + _rectangle.height ) ) );
    }

    // Determine whether two rectangles intersect
    [[nodiscard]] constexpr auto intersects( const rect< T >& _rectangle ) const
        -> bool {
        return ( x < _rectangle.x + _rectangle.width ) &&
               ( x + width > _rectangle.x ) &&
               ( y < _rectangle.y + _rectangle.height ) &&
               ( y + height > _rectangle.y );
    }

    // Calculate the intersection of two rectangles
    [[nodiscard]] constexpr auto intersection(
        const rect< T >& _rectangle ) const -> std::optional< rect< T > > {
        std::optional< rect< T > > l_returnValue = std::nullopt;

        const rect< T > l_composite = composite( _rectangle );

        if ( ( l_composite.x < l_composite.width ) &&
             ( l_composite.y < l_composite.height ) ) {
            l_returnValue = l_composite;
        }

        return ( l_returnValue );
    }

    // Calculate the union of two rectangles.
    [[nodiscard]] constexpr auto composite( const rect< T >& _rectangle )
        -> rect< T > {
        constexpr auto l_right = []( const rect< T >& _rectangle ) -> T {
            return ( _rectangle.x + _rectangle.width );
        };

        constexpr auto l_bottom = []( const rect< T >& _rectangle ) -> T {
            return ( _rectangle.y + _rectangle.height );
        };

        T l_x1 = std::min( x, _rectangle.x );
        T l_y1 = std::min( y, _rectangle.y );
        T l_x2 = std::max( l_right( *this ), l_right( _rectangle ) );
        T l_y2 = std::max( l_bottom( *this ), l_bottom( _rectangle ) );

        return {
            l_x1,
            l_y1,
            ( l_x2 - l_x1 ),
            ( l_y2 - l_y1 ),
        };
    }

    // Determine whether a rectangle has no area
    //
    // A rectangle is considered "empty" for this function if
    // its width and/ or height are <= 0
    [[nodiscard]] constexpr auto empty() const -> bool {
        return ( !height || !height );
    }

    T x, y, width, height;
};

template < typename T >
using rect_t = rect< T >;

// Calculate a minimal rectangle enclosing a set of points.
//
// \param points an array of SDL_Point structures representing points to be
//               enclosed.
// \param count the number of structures in the `points` array.
// \param clip an SDL_Rect used for clipping or NULL to enclose all points.
// \param result an SDL_Rect structure filled in with the minimal enclosing
//               rectangle.
// \returns true if any points were enclosed or false if all the points were
//          outside of the clipping rectangle.
inline auto setRectEnclosingPoints( const SDL_Point* points,
                                    int count,
                                    const SDL_Rect* clip,
                                    SDL_Rect* result ) -> bool;

/**
 * Calculate the intersection of a rectangle and line segment.
 *
 * This function is used to clip a line segment to a rectangle. A line segment
 * contained entirely within the rectangle or that does not intersect will
 * remain unchanged. A line segment that crosses the rectangle at either or
 * both ends will be clipped to the boundary of the rectangle and the new
 * coordinates saved in `X1`, `Y1`, `X2`, and/or `Y2` as necessary.
 *
 * \param rect an SDL_Rect structure representing the rectangle to intersect.
 * \param X1 a pointer to the starting X-coordinate of the line.
 * \param Y1 a pointer to the starting Y-coordinate of the line.
 * \param X2 a pointer to the ending X-coordinate of the line.
 * \param Y2 a pointer to the ending Y-coordinate of the line.
 * \returns true if there is an intersection, false otherwise.
 */
inline auto getRectAndLineIntersection( const SDL_Rect* rect,
                                        int* X1,
                                        int* Y1,
                                        int* X2,
                                        int* Y2 ) -> bool;

/* SDL_FRect versions... */

/**
 * Determine whether a point resides inside a floating point rectangle.
 *
 * A point is considered part of a rectangle if both `p` and `r` are not NULL,
 * and `p`s x and y coordinates are >= to the rectangles top left corner,
 * and <= the rectangles x+w and y+h. So a 1x1 rectangle considers point
 * (0,0) and (0,1) as "inside" and (0,2) as not.
 *
 * Note that this is a forced-inline function in a header, and not a public
 * API function available in the SDL library (which is to say, the code is
 * embedded in the calling program and the linker and dynamic loader will not
 * be able to find this function inside SDL itself).
 *
 * \param p the point to test.
 * \param r the rectangle to test.
 * \returns true if `p` is contained by `r`, false otherwise.
 */
inline auto pointInRectFloat( const SDL_FPoint* p, const SDL_FRect* r )
    -> bool {
    return ( p && r && ( p->x >= r->x ) && ( p->x <= ( r->x + r->w ) ) &&
             ( p->y >= r->y ) && ( p->y <= ( r->y + r->h ) ) )
               ? true
               : false;
}

/**
 * Determine whether a floating point rectangle takes no space.
 *
 * A rectangle is considered "empty" for this function if `r` is NULL, or if
 * `r`s width and/or height are < 0.0f.
 *
 * Note that this is a forced-inline function in a header, and not a public
 * API function available in the SDL library (which is to say, the code is
 * embedded in the calling program and the linker and dynamic loader will not
 * be able to find this function inside SDL itself).
 *
 * \param r the rectangle to test.
 * \returns true if the rectangle is "empty", false otherwise.
 */
inline auto rectEmptyFloat( const SDL_FRect* r ) -> bool {
    return ( ( !r ) || ( r->w < 0.0f ) || ( r->h < 0.0f ) ) ? true : false;
}

/**
 * Determine whether two floating point rectangles are equal, within some
 * given epsilon.
 *
 * Rectangles are considered equal if both are not NULL and each of their x,
 * y, width and height are within `epsilon` of each other. If you don't know
 * what value to use for `epsilon`, you should call the SDL_RectsEqualFloat
 * function instead.
 *
 * Note that this is a forced-inline function in a header, and not a public
 * API function available in the SDL library (which is to say, the code is
 * embedded in the calling program and the linker and dynamic loader will not
 * be able to find this function inside SDL itself).
 *
 * \param a the first rectangle to test.
 * \param b the second rectangle to test.
 * \param epsilon the epsilon value for comparison.
 * \returns true if the rectangles are equal, false otherwise.
 */
inline auto rectsEqualEpsilon( const SDL_FRect* a,
                               const SDL_FRect* b,
                               float epsilon ) -> bool {
    return ( a && b &&
             ( ( a == b ) || ( ( SDL_fabsf( a->x - b->x ) <= epsilon ) &&
                               ( SDL_fabsf( a->y - b->y ) <= epsilon ) &&
                               ( SDL_fabsf( a->w - b->w ) <= epsilon ) &&
                               ( SDL_fabsf( a->h - b->h ) <= epsilon ) ) ) )
               ? true
               : false;
}

/**
 * Determine whether two floating point rectangles are equal, within a default
 * epsilon.
 *
 * Rectangles are considered equal if both are not NULL and each of their x,
 * y, width and height are within SDL_FLT_EPSILON of each other. This is often
 * a reasonable way to compare two floating point rectangles and deal with the
 * slight precision variations in floating point calculations that tend to pop
 * up.
 *
 * Note that this is a forced-inline function in a header, and not a public
 * API function available in the SDL library (which is to say, the code is
 * embedded in the calling program and the linker and dynamic loader will not
 * be able to find this function inside SDL itself).
 *
 * \param a the first rectangle to test.
 * \param b the second rectangle to test.
 * \returns true if the rectangles are equal, false otherwise.
 */
inline auto sectsEqualFloat( const SDL_FRect* a, const SDL_FRect* b ) -> bool {
    return rectsEqualEpsilon( a, b, SDL_FLT_EPSILON );
}

/**
 * Determine whether two rectangles intersect with float precision.
 *
 * If either pointer is NULL the function will return false.
 *
 * \param A an SDL_FRect structure representing the first rectangle.
 * \param B an SDL_FRect structure representing the second rectangle.
 * \returns true if there is an intersection, false otherwise.
 */
inline auto hasRectIntersectionFloat( const SDL_FRect* A, const SDL_FRect* B )
    -> bool;

/**
 * Calculate the intersection of two rectangles with float precision.
 *
 * If `result` is NULL then this function will return false.
 *
 * \param A an SDL_FRect structure representing the first rectangle.
 * \param B an SDL_FRect structure representing the second rectangle.
 * \param result an SDL_FRect structure filled in with the intersection of
 *               rectangles `A` and `B`.
 * \returns true if there is an intersection, false otherwise.
 */
inline auto getRectIntersectionFloat( const SDL_FRect* A,
                                      const SDL_FRect* B,
                                      SDL_FRect* result ) -> bool;

/**
 * Calculate the union of two rectangles with float precision.
 *
 * \param A an SDL_FRect structure representing the first rectangle.
 * \param B an SDL_FRect structure representing the second rectangle.
 * \param result an SDL_FRect structure filled in with the union of rectangles
 *               `A` and `B`.
 * \returns true on success or false on failure; call SDL_GetError() for more
 *          information.
 *
 * \since This function is available since SDL 3.2.0.
 */
inline auto getRectUnionFloat( const SDL_FRect* A,
                               const SDL_FRect* B,
                               SDL_FRect* result ) -> bool;

/**
 * Calculate a minimal rectangle enclosing a set of points with float
 * precision.
 *
 * If `clip` is not NULL then only points inside of the clipping rectangle are
 * considered.
 *
 * \param points an array of SDL_FPoint structures representing points to be
 *               enclosed.
 * \param count the number of structures in the `points` array.
 * \param clip an SDL_FRect used for clipping or NULL to enclose all points.
 * \param result an SDL_FRect structure filled in with the minimal enclosing
 *               rectangle.
 * \returns true if any points were enclosed or false if all the points were
 *          outside of the clipping rectangle.
 */
inline auto getRectEnclosingPointsFloat( const SDL_FPoint* points,
                                         int count,
                                         const SDL_FRect* clip,
                                         SDL_FRect* result ) -> bool;

/**
 * Calculate the intersection of a rectangle and line segment with float
 * precision.
 *
 * This function is used to clip a line segment to a rectangle. A line segment
 * contained entirely within the rectangle or that does not intersect will
 * remain unchanged. A line segment that crosses the rectangle at either or
 * both ends will be clipped to the boundary of the rectangle and the new
 * coordinates saved in `X1`, `Y1`, `X2`, and/or `Y2` as necessary.
 *
 * \param rect an SDL_FRect structure representing the rectangle to intersect.
 * \param X1 a pointer to the starting X-coordinate of the line.
 * \param Y1 a pointer to the starting Y-coordinate of the line.
 * \param X2 a pointer to the ending X-coordinate of the line.
 * \param Y2 a pointer to the ending Y-coordinate of the line.
 * \returns true if there is an intersection, false otherwise.
 */
inline auto getRectAndLineIntersectionFloat( const SDL_FRect* rect,
                                             float* X1,
                                             float* Y1,
                                             float* X2,
                                             float* Y2 ) -> bool;

} // namespace slickdl
