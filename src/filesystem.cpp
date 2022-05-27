// See LICENSE for the Copyright Notice

#include <lua.hpp>
#include <filesystem>
#include <string_view>
#include <utility>
#include <cassert>

#include <iostream>

#if defined( _WIN32 )
# define EXPORT __declspec( dllexport )
#else
# define EXPORT
#endif

#ifdef __has_cpp_attribute
# if __has_cpp_attribute( unlikely )
#  define PG_UNLIKELY [[unlikely]]
# endif
#endif
#ifndef PG_UNLIKELY
# define PG_UNLIKELY
#endif

#define BEGIN_TRY try {
#define CATCH_BAD_ALLOC } catch( const std::bad_alloc & e ){ lua_settop( L, 0 ); lua_pushstring( L, e.what() ); return lua_error( L );
#define CATCH_FILESYSTEM_ERROR } catch( const std::filesystem::filesystem_error & e ){ lua_settop( L, 0 ); lua_pushstring( L, e.what() ); return lua_error( L );
#define END_TRY } catch( ... ) { return luaL_error( L, "filesystem error" ); }

#define BEGIN_FUNCTION( NAME )\
static int NAME( lua_State * const L ) noexcept\
{

#define END_FUNCTION    }

#define BEGIN_PROTECTED_FUNCTION( NAME )\
static int NAME( lua_State * const L ) noexcept\
{\
    BEGIN_TRY

#define END_PROTECTED_FUNCTION\
    END_TRY\
}

namespace pg
{

using path_iterator                = std::pair< std::filesystem::path::iterator, const std::filesystem::path::iterator >;
using directory_iterator           = std::pair< std::filesystem::directory_iterator, const std::filesystem::directory_iterator >;
using recursive_directory_iterator = std::pair< std::filesystem::recursive_directory_iterator, const std::filesystem::recursive_directory_iterator >;

static constexpr const char path_meta_traits[]                         = "path.filesystem";
static constexpr const char path_iterator_meta_traits[]                = "itereator.path.filesystem";
static constexpr const char directory_iterator_meta_traits[]           = "directory_iterator.filesystem";
static constexpr const char recursive_directory_iterator_meta_traits[] = "recursive_directory_iterator.filesystem";
static constexpr const char directory_entry_meta_traits[]              = "directory_entry.path.filesystem";
static constexpr const char directory_options_meta_traits[]            = "directory_options.path.filesystem";
static constexpr const char copy_options_meta_traits[]                 = "copy_options.filesystem";
static constexpr const char perms_meta_traits[]                        = "perms.filesystem";
static constexpr const char perm_options_meta_traits[]                 = "perm_options.filesystem";
static constexpr const char file_type_meta_traits[]                    = "file_type.filesystem";
static constexpr const char file_time_type_meta_traits[]               = "file_time_type.filesystem";

template< typename >
struct meta_traits {};

template<>
struct meta_traits< std::filesystem::path >
{
    static constexpr auto       id     = path_meta_traits;
    static constexpr const char name[] = "path";
};

template<>
struct meta_traits< path_iterator >
{
    static constexpr auto       id     = path_iterator_meta_traits;
    static constexpr const char name[] = "path iterator state";
};

template<>
struct meta_traits< directory_iterator >
{
    static constexpr auto       id     = directory_iterator_meta_traits;
    static constexpr const char name[] = "directory iterator state";
};

template<>
struct meta_traits< recursive_directory_iterator >
{
    static constexpr auto       id     = recursive_directory_iterator_meta_traits;
    static constexpr const char name[] = "recursive directory iterator state";
};

template<>
struct meta_traits< std::filesystem::directory_entry >
{
    static constexpr auto       id     = directory_entry_meta_traits;
    static constexpr const char name[] = "directory entry";
};

template<>
struct meta_traits< std::filesystem::directory_options >
{
    static constexpr auto       id     = directory_options_meta_traits;
    static constexpr const char name[] = "directory options";
};

template<>
struct meta_traits< std::filesystem::copy_options >
{
    static constexpr auto       id     = copy_options_meta_traits;
    static constexpr const char name[] = "copy options";
};

template<>
struct meta_traits< std::filesystem::perms >
{
    static constexpr auto       id     = perms_meta_traits;
    static constexpr const char name[] = "permissions";
};

template<>
struct meta_traits< std::filesystem::perm_options >
{
    static constexpr auto       id     = perm_options_meta_traits;
    static constexpr const char name[] = "permission options";
};

template<>
struct meta_traits< std::filesystem::file_type >
{
    static constexpr auto       id     = file_type_meta_traits;
    static constexpr const char name[] = "file type";
};

template<>
struct meta_traits< std::filesystem::file_time_type >
{
    static constexpr auto       id     = file_time_type_meta_traits;
    static constexpr const char name[] = "file time type";
};

template< typename T >
T * test_user_data( lua_State * const L, int index ) noexcept
{
    return static_cast< T * >( luaL_testudata( L, index, meta_traits< T >::id ) );
}

template< typename T >
T & check_user_data_arg( lua_State * L, int arg, const char * tname = meta_traits< T >::name ) noexcept
{
    auto path = pg::test_user_data< T >( L, arg );
    if( !path ) PG_UNLIKELY
    {
        luaL_typeerror( L, arg, tname );
    }

    return *path;
}

template< typename T >
T & to_user_data( lua_State * const L, int index ) noexcept
{
    return *static_cast< T * >( lua_touserdata( L, index ) );
}

auto to_string_view( lua_State * const L, int index )
{
    size_t len = 0;
    auto   str = lua_tolstring( L, index, &len );

    return std::string_view( str, len );
}

template< typename T, typename ...A >
T & new_user_data( lua_State * const L, A&&... args )
{
    lua_checkstack( L, 2 );

    auto buffer    = lua_newuserdatauv( L, sizeof( T ), 0 );
    auto user_data = new( buffer ) T( std::forward< A >( args )... );

    luaL_getmetatable( L, meta_traits< T >::id );
    lua_setmetatable( L, -2 );

    return *user_data;
}

template< typename T, typename ...A >
int return_new_user_data( lua_State * const L, A&&... args )
{
    lua_checkstack( L, 2 );

    auto buffer = lua_newuserdatauv( L, sizeof( T ), 0 );
    new( buffer ) T( std::forward< A >( args )... );

    luaL_getmetatable( L, meta_traits< T >::id );
    lua_setmetatable( L, -2 );

    return 1;
}

int return_boolean( lua_State * const L, bool value ) noexcept
{
    lua_pushboolean( L, value );

    return 1;
}

int return_nil( lua_State * const L ) noexcept
{
    lua_pushnil( L );

    return 1;
}

int return_nothing( lua_State * const /* L */ ) noexcept
{
    return 0;
}

int return_integer( lua_State * const L, lua_Integer n ) noexcept
{
    lua_pushinteger( L, n );

    return 1;
}

int return_number( lua_State * const L, lua_Number n ) noexcept
{
    lua_pushnumber( L, n );

    return 1;
}

template< typename T >
void set_table_field( lua_State * const L, const char * const key, const T value ) noexcept
{
    pg::new_user_data< T >( L, value );
    lua_setfield( L, -2, key );
}

template< typename E >
struct enum_flags
{
    BEGIN_FUNCTION( eq )
        const auto & left  = pg::check_user_data_arg< E >( L, 1 );
        const auto & right = pg::check_user_data_arg< E >( L, 2 );

        return pg::return_boolean( L, left == right );
    END_FUNCTION

    BEGIN_FUNCTION( band )
        const auto & left  = pg::check_user_data_arg< E >( L, 1 );
        const auto & right = pg::check_user_data_arg< E >( L, 2 );

        return pg::return_new_user_data< E >( L, left & right );
    END_FUNCTION

    BEGIN_FUNCTION( bor )
        const auto & left  = pg::check_user_data_arg< E >( L, 1 );
        const auto & right = pg::check_user_data_arg< E >( L, 2 );

        return pg::return_new_user_data< E >( L, left | right );
    END_FUNCTION

    BEGIN_FUNCTION( bxor )
        const auto & left  = pg::check_user_data_arg< E >( L, 1 );
        const auto & right = pg::check_user_data_arg< E >( L, 2 );

        return pg::return_new_user_data< E >( L, left ^ right );
    END_FUNCTION

    BEGIN_FUNCTION( bnot )
        const auto & self  = pg::check_user_data_arg< E >( L, 1 );

        return pg::return_new_user_data< E >( L, ~self );
    END_FUNCTION

    static constexpr const luaL_Reg operators[ 6 ] =
    {
        { "__eq",   eq },
        { "__band", band },
        { "__bor",  bor },
        { "__bxor", bxor },
        { "__bnot", bnot },
        { NULL,     NULL }
    };

    static constexpr const luaL_Reg * methods = nullptr;
};

template< typename E >
struct enumeration
{
    BEGIN_FUNCTION( eq )
        const auto & left  = pg::check_user_data_arg< E >( L, 1 );
        const auto & right = pg::check_user_data_arg< E >( L, 2 );

        return pg::return_boolean( L, left == right );
    END_FUNCTION

    static constexpr const luaL_Reg operators[ 2 ] =
    {
        { "__eq",   eq },
        { NULL,     NULL }
    };

    static constexpr const luaL_Reg * methods = nullptr;
};

}

BEGIN_FUNCTION( path_to_string )
    const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 1 );
    const auto & str  = path.string();

    lua_settop( L, 0 );
    lua_pushlstring( L, str.c_str(), str.size() );

    return 1;
END_FUNCTION

BEGIN_FUNCTION( path_gc )
    auto & self = pg::to_user_data< std::filesystem::path >( L, 1 );

    self.~path();

    return 0;
END_FUNCTION

#define PATH_COMPARE( NAME, OP )\
BEGIN_PROTECTED_FUNCTION( path_##NAME )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        auto p1 = pg::to_string_view( L, 1 );\
        if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            return pg::return_boolean( L, std::filesystem::path( p1 ) OP  pg::to_string_view( L, 2 ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            return pg::return_boolean( L, p1 OP p2 );\
        }\
    }\
    else\
    {\
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
        if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            return pg::return_boolean( L, p1 OP pg::to_string_view( L, 2 ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            return pg::return_boolean( L, p1 OP p2 );\
        }\
    }\
CATCH_BAD_ALLOC \
END_PROTECTED_FUNCTION

PATH_COMPARE( eq, == )
PATH_COMPARE( lt, < )
PATH_COMPARE( le, <= )

BEGIN_PROTECTED_FUNCTION( path_concat )
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        self.concat( pg::to_string_view( L, 2 ) );
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );

        self += path;
    }

    lua_settop( L, 1 );
    return 1;
CATCH_BAD_ALLOC
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( path_append )
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        self.append( pg::to_string_view( L, 2 ) );
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );

        self /= path;
    }

    lua_settop( L, 1 );
    return 1;
CATCH_BAD_ALLOC
END_PROTECTED_FUNCTION

static int path_clear( lua_State * const L ) noexcept
{
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    self.clear();

    return 0;
}

#define PATH_METHOD( METHOD )\
BEGIN_PROTECTED_FUNCTION( path_##METHOD )\
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );\
    self.METHOD();\
    return 1;\
END_PROTECTED_FUNCTION

PATH_METHOD( make_preferred )
PATH_METHOD( remove_filename )

BEGIN_PROTECTED_FUNCTION( path_replace_filename )
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        self.replace_filename( pg::to_string_view( L, 2 ) );
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );

        self.replace_filename( path );
    }

    lua_settop( L, 1 );
    return 1;
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( path_replace_extension )
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    const int type = lua_type( L, 2 );
    if( type == LUA_TSTRING )
    {
        self.replace_extension( pg::to_string_view( L, 2 ) );
    }
    else if( type == LUA_TNONE || type == LUA_TNIL )
    {
        self.replace_extension();
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path, string or nil" );

        self.replace_extension( path );
    }

    lua_settop( L, 1 );
    return 1;
END_PROTECTED_FUNCTION

#define PATH_METHOD_RETURN_NEW( METHOD )\
BEGIN_PROTECTED_FUNCTION( path_##METHOD )\
    const auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );\
    return pg::return_new_user_data< std::filesystem::path >( L, self.METHOD() );\
END_PROTECTED_FUNCTION

PATH_METHOD_RETURN_NEW( root_name )
PATH_METHOD_RETURN_NEW( root_directory )
PATH_METHOD_RETURN_NEW( root_path )
PATH_METHOD_RETURN_NEW( relative_path )
PATH_METHOD_RETURN_NEW( parent_path )
PATH_METHOD_RETURN_NEW( filename )
PATH_METHOD_RETURN_NEW( stem )
PATH_METHOD_RETURN_NEW( extension )

#define PATH_PROPERTY( METHOD )\
BEGIN_PROTECTED_FUNCTION( path_##METHOD )\
    const auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );\
    return pg::return_boolean( L, self.METHOD() );\
END_PROTECTED_FUNCTION

PATH_PROPERTY( empty )
PATH_PROPERTY( has_root_path )
PATH_PROPERTY( has_root_name )
PATH_PROPERTY( has_root_directory )
PATH_PROPERTY( has_relative_path )
PATH_PROPERTY( has_parent_path )
PATH_PROPERTY( has_filename )
PATH_PROPERTY( has_stem )
PATH_PROPERTY( has_extension )
PATH_PROPERTY( is_absolute )
PATH_PROPERTY( is_relative )

BEGIN_PROTECTED_FUNCTION( path_lexically_relative )
    const auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        return pg::return_new_user_data< std::filesystem::path >( L, self.lexically_relative( pg::to_string_view( L, 2 ) ) );
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );

        return pg::return_new_user_data< std::filesystem::path >( L, self.lexically_relative( path ) );
    }
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( path_lexically_proximate )
    auto & self = pg::check_user_data_arg< std::filesystem::path >( L, 1 );

    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        return pg::return_new_user_data< std::filesystem::path >( L, self.lexically_proximate( pg::to_string_view( L, 2 ) ) );
    }
    else
    {
        const auto & path = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );

        return pg::return_new_user_data< std::filesystem::path >( L, self.lexically_proximate( path ) );
    }
END_PROTECTED_FUNCTION

PATH_METHOD_RETURN_NEW( lexically_normal )

BEGIN_FUNCTION( path_iterator_gc )
    auto self = &pg::to_user_data< pg::path_iterator >( L, 1 );

    self->~pair< std::filesystem::path::iterator, const std::filesystem::path::iterator >();

    return 0;
END_FUNCTION

struct path_iterator_state
{
    static constexpr luaL_Reg operators[] =
    {
        { "__gc", path_iterator_gc },
        { NULL,   NULL }
    };

    static constexpr const luaL_Reg * methods = nullptr;
};

BEGIN_PROTECTED_FUNCTION( next_path_element )
    auto & self = pg::check_user_data_arg< pg::path_iterator >( L, 1 );
    if( self.first == self.second )
    {
        return pg::return_nil( L );
    }

    return pg::return_new_user_data< std::filesystem::path >( L, *self.first++ );
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( path_elements )
    auto & self     = pg::check_user_data_arg< std::filesystem::path >( L, 1 );
    auto   it_begin = self.begin();
    auto   it_end   = self.end();

    lua_settop( L, 0 );
    lua_pushcfunction( L, next_path_element );
    pg::new_user_data< pg::path_iterator >( L, it_begin, it_end );

    return 2;
END_PROTECTED_FUNCTION

struct path
{
    static constexpr const luaL_Reg operators[] =
    {
        { "__tostring", path_to_string },
        { "__gc",       path_gc },
        { "__eq",       path_eq },
        { "__lt",       path_lt },
        { "__le",       path_le },
        { NULL,         NULL }
    };

    static constexpr const luaL_Reg methods[] =
    {
        { "concat",              path_concat },
        { "append",              path_append },
        { "clear",               path_clear },
        { "make_preferred",      path_make_preferred },
        { "remove_filename",     path_remove_filename },
        { "replace_filename",    path_replace_filename },
        { "replace_extension",   path_replace_extension },
        { "root_name",           path_root_name },
        { "root_directory",      path_root_directory },
        { "root_path",           path_root_path },
        { "relative_path",       path_relative_path },
        { "parent_path",         path_parent_path },
        { "filename",            path_filename },
        { "stem",                path_stem },
        { "extension",           path_extension },
        { "empty",               path_empty },
        { "has_root_path",       path_has_root_path },
        { "has_root_name",       path_has_root_name },
        { "has_root_directory",  path_has_root_directory },
        { "has_relative_path",   path_has_relative_path },
        { "has_parent_path",     path_has_parent_path },
        { "has_filename",        path_has_filename },
        { "has_stem",            path_has_stem },
        { "has_extension",       path_has_extension },
        { "is_absolute",         path_is_absolute },
        { "is_relative",         path_is_relative },
        { "lexically_normal",    path_lexically_normal },
        { "lexically_relative",  path_lexically_relative },
        { "lexically_proximate", path_lexically_proximate },
        { "elements",            path_elements },
        { NULL,                  NULL }
    };
};

BEGIN_FUNCTION( ft_gc )
    auto & self = pg::to_user_data< std::filesystem::file_time_type >( L, 1 );

    typedef std::filesystem::file_time_type file_time_type;
    self.~file_time_type();

    return 0;
END_FUNCTION

BEGIN_FUNCTION( ft_eq )
    const auto & left  = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 );

    return pg::return_boolean( L, left == right );
END_FUNCTION

BEGIN_FUNCTION( ft_lt )
    const auto & left  = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 );

    return pg::return_boolean( L, left < right );
END_FUNCTION

BEGIN_FUNCTION( ft_le )
    const auto & left  = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 );

    return pg::return_boolean( L, left <= right );
END_FUNCTION

BEGIN_FUNCTION( ft_add )
    using duration = std::filesystem::file_time_type::duration;
    using period   = std::filesystem::file_time_type::period;
    using rep      = std::filesystem::file_time_type::rep;

    const auto & self = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 1 );

    if( lua_isinteger( L, 2 ) )
    {
        const auto seconds = static_cast< rep >( lua_tointeger( L, 2 ) );
        const auto count   = seconds * period::den / period::num;

        return pg::return_new_user_data< std::filesystem::file_time_type >( L, self + duration( count ) );
    }
    else if( lua_isnumber( L, 2 ) )
    {
        const auto seconds = lua_tonumber( L, 2 );
        const auto count   = static_cast< rep >( seconds * period::den / period::num );

        return pg::return_new_user_data< std::filesystem::file_time_type >( L, self + duration( count ) );
    }
    else
    {
        return luaL_typeerror( L, 2, "number or integer" );
    }
END_FUNCTION

BEGIN_FUNCTION( ft_sub )
    const auto & left  = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 );

    const auto count     = ( left - right ).count();
    const auto count_num = static_cast< lua_Number >( count * std::filesystem::file_time_type::period::num );
    const auto seconds   = count_num / std::filesystem::file_time_type::period::den;

    return pg::return_number( L, seconds );
END_FUNCTION

struct fs_file_time
{
    static constexpr const luaL_Reg operators[] =
    {
        { "__gc",       ft_gc },
        { "__eq",       ft_eq },
        { "__lt",       ft_lt },
        { "__le",       ft_le },
        { "__add",      ft_add },
        { "__sub",      ft_sub },
        { NULL,         NULL }
    };

    static constexpr const luaL_Reg * methods = nullptr;
};

BEGIN_FUNCTION( de_to_string )
    const auto & path = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 ).path();
    const auto & str  = path.string();

    lua_settop( L, 0 );
    lua_pushlstring( L, str.c_str(), str.size() );

    return 1;
END_FUNCTION

BEGIN_FUNCTION( de_gc )
    auto & self = pg::to_user_data< std::filesystem::directory_entry >( L, 1 );

    self.~directory_entry();

    return 0;
END_FUNCTION

BEGIN_FUNCTION( de_eq )
    const auto & left  = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 2 );

    return pg::return_boolean( L, left == right );
END_FUNCTION

BEGIN_FUNCTION( de_lt )
    const auto & left  = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 2 );

    return pg::return_boolean( L, left < right );
END_FUNCTION

BEGIN_FUNCTION( de_le )
    const auto & left  = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    const auto & right = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 2 );

    return pg::return_boolean( L, left <= right );
END_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_assign )
    auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        self.assign( pg::to_string_view( L, 2 ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
        self.assign( p );
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_replace_filename )
    auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    if( lua_type( L, 2 ) == LUA_TSTRING )
    {
        self.replace_filename( pg::to_string_view( L, 2 ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
        self.replace_filename( p );
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_refresh )
    auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    self.refresh();
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_path )
    const auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    return pg::return_new_user_data< std::filesystem::path >( L, self.path() );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

#define DE_CHECK_PROPERTY( PROPERTY )\
BEGIN_PROTECTED_FUNCTION( de_##PROPERTY )\
    const auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );\
    return pg::return_boolean( L, self.PROPERTY() );\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

DE_CHECK_PROPERTY( exists )
DE_CHECK_PROPERTY( is_block_file )
DE_CHECK_PROPERTY( is_character_file )
DE_CHECK_PROPERTY( is_directory )
DE_CHECK_PROPERTY( is_fifo )
DE_CHECK_PROPERTY( is_other )
DE_CHECK_PROPERTY( is_regular_file )
DE_CHECK_PROPERTY( is_socket )
DE_CHECK_PROPERTY( is_symlink )

BEGIN_PROTECTED_FUNCTION( de_file_size )
    const auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    return pg::return_integer( L, self.file_size() );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_hard_link_count )
    const auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    return pg::return_integer( L, self.hard_link_count() );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( de_last_write_time )
    const auto & self = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );
    return pg::return_new_user_data< std::filesystem::file_time_type >( L, self.last_write_time() );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

#define DE_X_STATUS( FUNCTION )\
BEGIN_PROTECTED_FUNCTION( de_##FUNCTION )\
    const auto & self        = pg::check_user_data_arg< std::filesystem::directory_entry >( L, 1 );\
    const auto   file_status = self.FUNCTION();\
    lua_settop( L, 0 );\
    pg::new_user_data< std::filesystem::perms >( L, file_status.permissions() );\
    pg::new_user_data< std::filesystem::file_type >( L, file_status.type() );\
    return 2;\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

DE_X_STATUS( status )
DE_X_STATUS( symlink_status )

struct directory_entry
{
    static constexpr const luaL_Reg operators[] =
    {
        { "__tostring", de_to_string },
        { "__gc",       de_gc },
        { "__eq",       de_eq },
        { "__lt",       de_lt },
        { "__le",       de_le },
        { NULL,         NULL }
    };

    static constexpr const luaL_Reg methods[] =
    {
        { "assign",             de_assign },
        { "replace_filename",   de_replace_filename },
        { "refresh",            de_refresh },
        { "path",               de_path },
        { "exists",             de_exists },
        { "is_block_file",      de_is_block_file },
        { "is_character_file",  de_is_character_file },
        { "is_directory",       de_is_directory },
        { "is_fifo",            de_is_fifo },
        { "is_other",           de_is_other },
        { "is_regular_file",    de_is_regular_file },
        { "is_socket",          de_is_socket },
        { "is_symlink",         de_is_symlink },
        { "file_size",          de_file_size },
        { "hard_link_count",    de_hard_link_count },
        { "last_write_time",    de_last_write_time },
        { "status",             de_status },
        { "symlink_status",     de_symlink_status },
        { NULL,                 NULL }
    };
};

using directory_options = pg::enum_flags< std::filesystem::directory_options >;

BEGIN_FUNCTION( directory_iterator_gc )
    auto self = &pg::to_user_data< pg::directory_iterator >( L, 1 );

    self->~pair< std::filesystem::directory_iterator, const std::filesystem::directory_iterator >();

    return 0;
END_FUNCTION

struct directory_iterator_state
{
    static constexpr const luaL_Reg operators[] =
    {
        { "__gc", directory_iterator_gc },
        { NULL,   NULL }
    };

    static constexpr const luaL_Reg * methods = nullptr;
};

BEGIN_PROTECTED_FUNCTION( next_directory_element )
    auto & self = pg::check_user_data_arg< pg::directory_iterator >( L, 1 );
    if( self.first == self.second )
    {
        return pg::return_nil( L );
    }

    return pg::return_new_user_data< std::filesystem::directory_entry >( L, *self.first++ );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_directory )
    std::filesystem::directory_iterator di;
    if( lua_gettop( L ) <= 1 )
    {
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
             di = std::filesystem::directory_iterator( pg::to_string_view( L, 1 ) );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );

            di = std::filesystem::directory_iterator( p );
        }
    }
    else
    {
        const auto options = pg::check_user_data_arg< std::filesystem::directory_options >( L, 2 );
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
             di = std::filesystem::directory_iterator( pg::to_string_view( L, 1 ), options );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );

             di = std::filesystem::directory_iterator( p, options );
        }
    }

    lua_settop( L, 0 );
    lua_pushcfunction( L, next_directory_element );
    pg::new_user_data< pg::directory_iterator >( L, std::move( di ), std::filesystem::directory_iterator() );
    return 2;
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_FUNCTION( rdi_gc )
    auto self = &pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );

    self->~pair< std::filesystem::recursive_directory_iterator, const std::filesystem::recursive_directory_iterator >();

    return 0;
END_FUNCTION

BEGIN_FUNCTION( rdi_options )
    const auto & self = pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );
    if( self.first == self.second )
    {
        return pg::return_nothing( L );
    }
    else
    {
        return pg::return_new_user_data< std::filesystem::directory_options >( L, self.first.options() );
    }
END_FUNCTION

BEGIN_FUNCTION( rdi_depth )
    const auto & self = pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );
    if( self.first == self.second )
    {
        return pg::return_nothing( L );
    }
    else
    {
        return pg::return_integer( L, self.first.depth() );
    }
END_FUNCTION

BEGIN_FUNCTION( rdi_recursion_pending )
    const auto & self = pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );
    if( self.first == self.second )
    {
        return pg::return_nothing( L );
    }
    else
    {
        return pg::return_boolean( L, self.first.recursion_pending() );
    }
END_FUNCTION

BEGIN_PROTECTED_FUNCTION( rdi_pop )
    auto & self = pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );
    if( self.first != self.second )
    {
        self.first.pop();
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_FUNCTION( rdi_disable_recursion_pending )
    auto & self = pg::to_user_data< pg::recursive_directory_iterator >( L, 1 );
    if( self.first != self.second )
    {
        self.first.disable_recursion_pending();
    }
    return pg::return_nothing( L );
END_FUNCTION

struct recursive_directory_iterator_state
{
    static constexpr const luaL_Reg operators[] =
    {
        { "__gc", rdi_gc },
        { NULL,   NULL }
    };

    static constexpr const luaL_Reg methods[] =
    {
        { "options",                   rdi_options },
        { "depth",                     rdi_depth },
        { "recursion_pending",         rdi_recursion_pending },
        { "pop",                       rdi_pop },
        { "disable_recursion_pending", rdi_disable_recursion_pending },
        { NULL,   NULL }
    };
};

BEGIN_PROTECTED_FUNCTION( next_recursive_directory_element )
    auto & self = pg::check_user_data_arg< pg::recursive_directory_iterator >( L, 1 );
    if( self.first == self.second )
    {
        // Finished iteration in previous call
        return pg::return_nil( L );
    }
    else if( lua_type( L, 2 ) == LUA_TNIL )
    {
        // First call of next with the current state
        if( self.first == self.second )
        {
            // Nothing to iterate
            return pg::return_nil( L );
        }
    }
    else if( ++self.first == self.second )
    {
        // Finished iteration
        return pg::return_nil( L );
    }
    lua_settop( L, 1 );
    pg::new_user_data< std::filesystem::directory_entry >( L, *self.first );
    return 2;
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_recursive_directory )
    std::filesystem::recursive_directory_iterator rdi;
    if( lua_gettop( L ) <= 1 )
    {
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
             rdi = std::filesystem::recursive_directory_iterator( pg::to_string_view( L, 1 ) );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );

            rdi = std::filesystem::recursive_directory_iterator( p );
        }
    }
    else
    {
        const auto options = pg::check_user_data_arg< std::filesystem::directory_options >( L, 2 );
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
             rdi = std::filesystem::recursive_directory_iterator( pg::to_string_view( L, 1 ), options );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );

             rdi = std::filesystem::recursive_directory_iterator( p, options );
        }
    }

    lua_settop( L, 0 );
    lua_pushcfunction( L, next_recursive_directory_element );
    pg::new_user_data< pg::recursive_directory_iterator >( L, begin( rdi ), end( rdi ) );
    return 2;
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_make_directory_entry )
    const std::filesystem::path            * other_path = nullptr;
    const std::filesystem::directory_entry * other_de   = nullptr;

    const int type = lua_type( L, 1 );
    switch( type )
    {
    case LUA_TUSERDATA:
        if( other_path = pg::test_user_data< std::filesystem::path >( L, 1 ) ; other_path )
        {
            break;
        }
        if( other_de = pg::test_user_data< std::filesystem::directory_entry >( L, 1 ) ; other_de )
        {
            break;
        }
        [[fallthrough]];
    case LUA_TNUMBER:
    case LUA_TBOOLEAN:
    case LUA_TTABLE:
    case LUA_TFUNCTION:
    case LUA_TTHREAD:
    case LUA_TLIGHTUSERDATA:
        luaL_typeerror( L, 1, "directory entry, path, string or nil" );
    }

    if( type == LUA_TNIL || type == LUA_TNONE )
    {
        return pg::return_new_user_data< std::filesystem::directory_entry >( L );
    }
    else if( type == LUA_TSTRING )
    {
        return pg::return_new_user_data< std::filesystem::directory_entry >( L, pg::to_string_view( L, 1 ) );
    }

    assert( type == LUA_TUSERDATA );

    if( other_path )
    {
        return pg::return_new_user_data< std::filesystem::directory_entry >( L, *other_path );
    }
    else
    {
        assert( other_de );

        return pg::return_new_user_data< std::filesystem::directory_entry >( L, *other_de );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_make_path )
    const std::filesystem::path * other_path = nullptr;
    
    const int type = lua_type( L, 1 );
    switch( type )
    {
    case LUA_TUSERDATA:
        if( other_path = pg::test_user_data< std::filesystem::path >( L, 1 ) ; other_path )
        {
            break;
        }
        [[fallthrough]];
    case LUA_TNUMBER:
    case LUA_TBOOLEAN:
    case LUA_TTABLE:
    case LUA_TFUNCTION:
    case LUA_TTHREAD:
    case LUA_TLIGHTUSERDATA:
        luaL_typeerror( L, 1, "path, string or nil" );
    }

    if( type == LUA_TNIL || type == LUA_TNONE )
    {
        return pg::return_new_user_data< std::filesystem::path >( L );
    }
    else if( type == LUA_TSTRING )
    {
        return pg::return_new_user_data< std::filesystem::path >( L, pg::to_string_view( L, 1 ) );
    }
    else
    {
        assert( type == LUA_TUSERDATA );

        return pg::return_new_user_data< std::filesystem::path >( L, *other_path );
    }
END_PROTECTED_FUNCTION

#define FS_TRANSFORM_PATH( TRANSFORMATION )\
BEGIN_PROTECTED_FUNCTION( fs_##TRANSFORMATION )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORMATION( pg::to_string_view( L, 1 ) ) );\
    }\
    const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
    return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORMATION( p ) );\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

FS_TRANSFORM_PATH( absolute )
FS_TRANSFORM_PATH( canonical )
FS_TRANSFORM_PATH( weakly_canonical )

using copy_options = pg::enum_flags< std::filesystem::copy_options >;

BEGIN_PROTECTED_FUNCTION( fs_copy )
    const auto options = lua_gettop( L ) < 3 ? std::filesystem::copy_options::none
                                             : pg::check_user_data_arg< std::filesystem::copy_options >( L, 3 );
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        auto p1 = pg::to_string_view( L, 1 );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            std::filesystem::copy( p1, pg::to_string_view( L, 2 ), options );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            std::filesystem::copy( p1, p2, options );
        }
    }
    else
    {
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            std::filesystem::copy( p1, pg::to_string_view( L, 2 ), options );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            std::filesystem::copy( p1, p2, options );
        }
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

#define FS_RELATIVE_PROXIMATE( TRANSFORM )\
BEGIN_PROTECTED_FUNCTION( fs_##TRANSFORM )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        auto p1 = pg::to_string_view( L, 1 );\
        if( lua_gettop( L ) == 1 )\
        {\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1 ) );\
        }\
        else if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1, pg::to_string_view( L, 2 ) ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1, p2 ) );\
        }\
    }\
    else\
    {\
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
        if( lua_gettop( L ) == 1 )\
        {\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1 ) );\
        }\
        else if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1, pg::to_string_view( L, 2 ) ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::TRANSFORM( p1, p2 ) );\
        }\
    }\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

FS_RELATIVE_PROXIMATE( relative )
FS_RELATIVE_PROXIMATE( proximate )
    
BEGIN_PROTECTED_FUNCTION( fs_copy_file )
    const auto options = lua_gettop( L ) < 3 ? std::filesystem::copy_options::none
                                             : pg::check_user_data_arg< std::filesystem::copy_options >( L, 3 );
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        auto p1 = pg::to_string_view( L, 1 );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            return pg::return_boolean( L, std::filesystem::copy_file( p1, pg::to_string_view( L, 2 ), options ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            return pg::return_boolean( L, std::filesystem::copy_file( p1, p2, options ) );
        }
    }
    else
    {
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            return pg::return_boolean( L, std::filesystem::copy_file( p1, pg::to_string_view( L, 2 ), options ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            return pg::return_boolean( L, std::filesystem::copy_file( p1, p2, options ) );
        }
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

#define FS_X_LINK( FUNCTION )\
BEGIN_PROTECTED_FUNCTION( fs_##FUNCTION )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        auto p1 = pg::to_string_view( L, 1 );\
        if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            std::filesystem::FUNCTION( p1, pg::to_string_view( L, 2 ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            std::filesystem::FUNCTION( p1, p2 );\
        }\
    }\
    else\
    {\
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
        if( lua_type( L, 2 ) == LUA_TSTRING )\
        {\
            std::filesystem::FUNCTION( p1, pg::to_string_view( L, 2 ) );\
        }\
        else\
        {\
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );\
            std::filesystem::FUNCTION( p1, p2 );\
        }\
    }\
    return pg::return_nothing( L );\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

FS_X_LINK( copy_symlink )
FS_X_LINK( create_hard_link )
FS_X_LINK( create_symlink )
FS_X_LINK( create_directory_symlink )

BEGIN_PROTECTED_FUNCTION( fs_create_directory )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        if( lua_gettop( L ) > 1 )
        {
            if( lua_type( L, 2 ) == LUA_TSTRING )
            {
                return pg::return_boolean( L, std::filesystem::create_directory( pg::to_string_view( L, 1 ), pg::to_string_view( L, 2 ) ) );
            }
            else
            {
                const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
                return pg::return_boolean( L, std::filesystem::create_directory( pg::to_string_view( L, 1 ), p ) );
            }
        }
        else
        {
            return pg::return_boolean( L, std::filesystem::create_directory( pg::to_string_view( L, 1 ) ) );
        }
    }
    else
    {
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        if( lua_gettop( L ) > 1 )
        {
            if( lua_type( L, 2 ) == LUA_TSTRING )
            {
                return pg::return_boolean( L, std::filesystem::create_directory( p1, pg::to_string_view( L, 2 ) ) );
            }
            else
            {
                const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
                return pg::return_boolean( L, std::filesystem::create_directory( p1, p2 ) );
            }
        }
        else
        {
            return pg::return_boolean( L, std::filesystem::create_directory( p1 ) );
        }
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_create_directories )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_boolean( L, std::filesystem::create_directories( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        return pg::return_boolean( L, std::filesystem::create_directories( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_current_path )
    if( lua_gettop( L ) == 0 )
    {
        return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::current_path() );
    }

    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        std::filesystem::current_path( pg::to_string_view( L, 1 ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        std::filesystem::current_path( p );
    }

    lua_settop( L, 0 );
    return 0;
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_exists )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_boolean( L, std::filesystem::exists( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        return pg::return_boolean( L, std::filesystem::exists( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_equivalent )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        auto p1 = pg::to_string_view( L, 1 );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            return pg::return_boolean( L, std::filesystem::equivalent( p1, pg::to_string_view( L, 2 ) ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            return pg::return_boolean( L, std::filesystem::equivalent( p1, p2 ) );
        }
    }
    else
    {
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            return pg::return_boolean( L, std::filesystem::equivalent( p1, pg::to_string_view( L, 2 ) ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            return pg::return_boolean( L, std::filesystem::equivalent( p1, p2 ) );
        }
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_file_size )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_integer( L, std::filesystem::file_size( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
        return pg::return_integer( L, std::filesystem::file_size( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_hard_link_count )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_integer( L, std::filesystem::hard_link_count( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
        return pg::return_integer( L, std::filesystem::hard_link_count( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_last_write_time )
    if( lua_gettop( L ) <= 1 )
    {
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
            return pg::return_new_user_data< std::filesystem::file_time_type >( L, std::filesystem::last_write_time( pg::to_string_view( L, 1 ) ) );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
            return pg::return_new_user_data< std::filesystem::file_time_type >( L, std::filesystem::last_write_time( p ) );
        }
    }
    else
    {
        if( lua_type( L, 1 ) == LUA_TSTRING )
        {
            std::filesystem::last_write_time( pg::to_string_view( L, 1 ), pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 ) );
        }
        else
        {
            const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
            std::filesystem::last_write_time( p, pg::check_user_data_arg< std::filesystem::file_time_type >( L, 2 ) );
        }

        return pg::return_nothing( L );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_FUNCTION( fs_file_time_now )
        return pg::return_new_user_data< std::filesystem::file_time_type >( L, std::filesystem::file_time_type::clock::now() );
END_FUNCTION

using fs_perms        = pg::enum_flags< std::filesystem::perms >;
using fs_perm_options = pg::enum_flags< std::filesystem::perm_options >;

BEGIN_PROTECTED_FUNCTION( fs_permissions )
    const auto perms   = pg::check_user_data_arg< std::filesystem::perms >( L, 2 );
    const auto options = lua_gettop( L ) < 3 ? std::filesystem::perm_options::replace
                                             : pg::check_user_data_arg< std::filesystem::perm_options >( L, 3 );
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        std::filesystem::permissions( pg::to_string_view( L, 1 ), perms, options );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        std::filesystem::permissions( p, perms, options );
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_read_symlink )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::read_symlink( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::read_symlink( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_remove )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_boolean( L, std::filesystem::remove( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        return pg::return_boolean( L, std::filesystem::remove( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_remove_all )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        return pg::return_integer( L, std::filesystem::remove_all( pg::to_string_view( L, 1 ) ) );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        return pg::return_integer( L, std::filesystem::remove_all( p ) );
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_rename )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        auto p1 = pg::to_string_view( L, 1 );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            std::filesystem::rename( p1, pg::to_string_view( L, 2 ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            std::filesystem::rename( p1, p2 );
        }
    }
    else
    {
        const auto & p1 = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        if( lua_type( L, 2 ) == LUA_TSTRING )
        {
            std::filesystem::rename( p1, pg::to_string_view( L, 2 ) );
        }
        else
        {
            const auto & p2 = pg::check_user_data_arg< std::filesystem::path >( L, 2, "path or string" );
            std::filesystem::rename( p1, p2 );
        }
    }

    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_resize_file )
    const auto new_size = luaL_checkinteger( L, 2 );
    if( new_size < 0 ) PG_UNLIKELY
    {
        return luaL_error( L, "new file size cannot be negative" );
    }
    const auto new_size_unsigned = static_cast<  std::uintmax_t >( new_size );

    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        std::filesystem::resize_file( pg::to_string_view( L, 1 ), new_size_unsigned );
    }
    else
    {
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        std::filesystem::resize_file( p, new_size_unsigned );
    }
    return pg::return_nothing( L );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

BEGIN_PROTECTED_FUNCTION( fs_space )
    if( lua_type( L, 1 ) == LUA_TSTRING )
    {
        const auto info = std::filesystem::space( pg::to_string_view( L, 1 ) );

        lua_settop( L, 0 );
        lua_pushinteger( L, info.capacity );
        lua_pushinteger( L, info.free );
        lua_pushinteger( L, info.available );
        return 3;
    }
    else
    {
        const auto & p  = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );
        const auto info = std::filesystem::space( p );

        lua_settop( L, 0 );
        lua_pushinteger( L, info.free );
        lua_pushinteger( L, info.available );
        lua_pushinteger( L, info.capacity );
        return 3;
    }
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

using fs_file_type =  pg::enumeration< std::filesystem::file_type >;

#define FS_X_STATUS( FUNCTION )\
BEGIN_PROTECTED_FUNCTION( fs_##FUNCTION )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        const auto file_status = std::filesystem::FUNCTION( pg::to_string_view( L, 1 ) );\
        lua_settop( L, 0 );\
        pg::new_user_data< std::filesystem::perms >( L, file_status.permissions() );\
        pg::new_user_data< std::filesystem::file_type >( L, file_status.type() );\
        return 2;\
    }\
    else\
    {\
        const auto & p           = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
        const auto   file_status = std::filesystem::FUNCTION( p );\
        lua_settop( L, 0 );\
        pg::new_user_data< std::filesystem::perms >( L, file_status.permissions() );\
        pg::new_user_data< std::filesystem::file_type >( L, file_status.type() );\
        return 2;\
    }\
CATCH_BAD_ALLOC \
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

FS_X_STATUS( status )
FS_X_STATUS( symlink_status )

BEGIN_PROTECTED_FUNCTION( fs_temp_directory_path )
    return pg::return_new_user_data< std::filesystem::path >( L, std::filesystem::temp_directory_path() );
CATCH_BAD_ALLOC
CATCH_FILESYSTEM_ERROR
END_PROTECTED_FUNCTION

#define FS_CHECK_PATH_PROPERTY( PROPERTY )\
BEGIN_PROTECTED_FUNCTION( fs_##PROPERTY )\
    if( lua_type( L, 1 ) == LUA_TSTRING )\
    {\
        return pg::return_boolean( L, std::filesystem::PROPERTY( pg::to_string_view( L, 1 ) ) );\
    }\
    else\
    {\
        const auto & p = pg::check_user_data_arg< std::filesystem::path >( L, 1, "path or string" );\
        return pg::return_boolean( L, std::filesystem::PROPERTY( p ) );\
    }\
CATCH_FILESYSTEM_ERROR \
END_PROTECTED_FUNCTION

FS_CHECK_PATH_PROPERTY( is_block_file )
FS_CHECK_PATH_PROPERTY( is_character_file )
FS_CHECK_PATH_PROPERTY( is_directory )
FS_CHECK_PATH_PROPERTY( is_empty )
FS_CHECK_PATH_PROPERTY( is_fifo )
FS_CHECK_PATH_PROPERTY( is_other )
FS_CHECK_PATH_PROPERTY( is_regular_file )
FS_CHECK_PATH_PROPERTY( is_socket )
FS_CHECK_PATH_PROPERTY( is_symlink )

static constexpr const luaL_Reg fs_functions[] =
{
    { "directory",                  fs_directory },
    { "recursive_directory",        fs_recursive_directory },
    { "directory_entry",            fs_make_directory_entry },
    { "path",                       fs_make_path },
    { "absolute",                   fs_absolute },
    { "canonical",                  fs_canonical },
    { "weakly_canonical",           fs_weakly_canonical },
    { "relative",                   fs_relative },
    { "proximate",                  fs_proximate },
    { "copy",                       fs_copy },
    { "copy_file",                  fs_copy_file },
    { "copy_symlink",               fs_copy_symlink },
    { "create_directory",           fs_create_directory },
    { "create_directories",         fs_create_directories },
    { "create_hard_link",           fs_create_hard_link },
    { "create_symlink",             fs_create_symlink },
    { "create_directory_symlink",   fs_create_directory_symlink },
    { "current_path",               fs_current_path },
    { "exists",                     fs_exists },
    { "equivalent",                 fs_equivalent },
    { "file_size",                  fs_file_size },
    { "hard_link_count",            fs_hard_link_count },
    { "last_write_time",            fs_last_write_time },
    { "file_time_now",              fs_file_time_now },
    { "permissions",                fs_permissions },
    { "read_symlink",               fs_read_symlink },
    { "remove",                     fs_remove },
    { "remove_all",                 fs_remove_all },
    { "rename",                     fs_rename },
    { "resize_file",                fs_resize_file },
    { "space",                      fs_space },
    { "status",                     fs_status },
    { "symlink_status",             fs_symlink_status },
    { "temp_directory_path",        fs_temp_directory_path },
    { "is_block_file",              fs_is_block_file },
    { "is_character_file",          fs_is_character_file },
    { "is_directory",               fs_is_directory },
    { "is_empty",                   fs_is_empty },
    { "is_fifo",                    fs_is_fifo },
    { "is_other",                   fs_is_other },
    { "is_regular_file",            fs_is_regular_file },
    { "is_socket",                  fs_is_socket },
    { "is_symlink",                 fs_is_symlink },
    { NULL,                         NULL }
};

static void register_directory_options( lua_State * const L ) noexcept
{
    lua_createtable( L, 0, 3 );

    pg::set_table_field( L, "none",                     std::filesystem::directory_options::none );
    pg::set_table_field( L, "follow_directory_symlink", std::filesystem::directory_options::follow_directory_symlink );
    pg::set_table_field( L, "skip_permission_denied",   std::filesystem::directory_options::skip_permission_denied );

    lua_setfield( L, -2, "directory_options" );
}

static void register_copy_options( lua_State * const L ) noexcept
{
    lua_createtable( L, 0, 10 );

    pg::set_table_field( L, "none",                 std::filesystem::copy_options::none );
    pg::set_table_field( L, "skip_existing",        std::filesystem::copy_options::skip_existing );
    pg::set_table_field( L, "overwrite_existing",   std::filesystem::copy_options::overwrite_existing );
    pg::set_table_field( L, "update_existing",      std::filesystem::copy_options::update_existing );
    pg::set_table_field( L, "recursive",            std::filesystem::copy_options::recursive );
    pg::set_table_field( L, "copy_symlinks",        std::filesystem::copy_options::copy_symlinks );
    pg::set_table_field( L, "skip_symlinks",        std::filesystem::copy_options::skip_symlinks );
    pg::set_table_field( L, "directories_only",     std::filesystem::copy_options::directories_only );
    pg::set_table_field( L, "create_symlinks",      std::filesystem::copy_options::create_symlinks );
    pg::set_table_field( L, "create_hard_links",    std::filesystem::copy_options::create_hard_links );

    lua_setfield( L, -2, "copy_options" );
}

static void register_perms( lua_State * const L ) noexcept
{
    lua_createtable( L, 0, 18 );

    pg::set_table_field( L, "none",         std::filesystem::perms::none );
    pg::set_table_field( L, "owner_read",   std::filesystem::perms::owner_read );
    pg::set_table_field( L, "owner_write",  std::filesystem::perms::owner_write );
    pg::set_table_field( L, "owner_exec",   std::filesystem::perms::owner_exec );
    pg::set_table_field( L, "owner_all",    std::filesystem::perms::owner_all );
    pg::set_table_field( L, "group_read",   std::filesystem::perms::group_read );
    pg::set_table_field( L, "group_write",  std::filesystem::perms::group_write );
    pg::set_table_field( L, "group_exe",    std::filesystem::perms::group_exec );
    pg::set_table_field( L, "group_all",    std::filesystem::perms::group_all );
    pg::set_table_field( L, "others_read",  std::filesystem::perms::others_read );
    pg::set_table_field( L, "others_write", std::filesystem::perms::others_write );
    pg::set_table_field( L, "others_exec",  std::filesystem::perms::others_exec );
    pg::set_table_field( L, "others_all",   std::filesystem::perms::others_all );
    pg::set_table_field( L, "all",          std::filesystem::perms::all );
    pg::set_table_field( L, "set_uid",      std::filesystem::perms::set_uid );
    pg::set_table_field( L, "set_gid",      std::filesystem::perms::set_gid );
    pg::set_table_field( L, "sticky_bit",   std::filesystem::perms::sticky_bit );
    pg::set_table_field( L, "mask",         std::filesystem::perms::mask );

    lua_setfield( L, -2, "perms" );
}

static void register_perm_options( lua_State * const L ) noexcept
{
    lua_createtable( L, 0, 4 );

    pg::set_table_field( L, "replace",  std::filesystem::perm_options::replace );
    pg::set_table_field( L, "add",      std::filesystem::perm_options::add );
    pg::set_table_field( L, "remove",   std::filesystem::perm_options::remove );
    pg::set_table_field( L, "nofollow", std::filesystem::perm_options::nofollow );

    lua_setfield( L, -2, "perm_options" );
}

static void register_file_types( lua_State * const L ) noexcept
{
#if defined( _WIN32 )
    lua_createtable( L, 0, 11 );

    pg::set_table_field( L, "junction",  std::filesystem::file_type::junction );
#else
    lua_createtable( L, 0, 10 );
#endif

    pg::set_table_field( L, "none",      std::filesystem::file_type::none );
    pg::set_table_field( L, "not_found", std::filesystem::file_type::not_found );
    pg::set_table_field( L, "regular",   std::filesystem::file_type::regular );
    pg::set_table_field( L, "directory", std::filesystem::file_type::directory );
    pg::set_table_field( L, "symlink",   std::filesystem::file_type::symlink );
    pg::set_table_field( L, "block",     std::filesystem::file_type::block );
    pg::set_table_field( L, "character", std::filesystem::file_type::character );
    pg::set_table_field( L, "fifo",      std::filesystem::file_type::fifo );
    pg::set_table_field( L, "socket",    std::filesystem::file_type::socket );
    pg::set_table_field( L, "unknown",   std::filesystem::file_type::unknown );

    lua_setfield( L, -2, "file_type" );
}

static void register_metatable( lua_State * const L, const char * table_name, const luaL_Reg * operators, const luaL_Reg * methods )
{
    if( luaL_newmetatable( L, table_name ) )
    {
        if( operators )
        {
            luaL_setfuncs( L, operators, 0 );
        }

        if( methods )
        {
            lua_newtable( L );
            luaL_setfuncs( L, methods, 0 );

            lua_setfield( L, -2, "__index" );

            lua_pushboolean( L, 0 );
            lua_setfield( L, -2, "__metatable" );
        }

        lua_settop( L, 0 );

        return;
    }

    assert( !"metatable already registered" );
}

extern "C"
{

EXPORT int luaopen_filesystem( lua_State * const L ) noexcept
{
    register_metatable( L, pg::path_meta_traits,                         path::operators,                               path::methods );
    register_metatable( L, pg::path_iterator_meta_traits,                path_iterator_state::operators,                path_iterator_state::methods );
    register_metatable( L, pg::directory_iterator_meta_traits,           directory_iterator_state::operators,           directory_iterator_state::methods );
    register_metatable( L, pg::recursive_directory_iterator_meta_traits, recursive_directory_iterator_state::operators, recursive_directory_iterator_state::methods );
    register_metatable( L, pg::directory_entry_meta_traits,              directory_entry::operators,                    directory_entry::methods );
    register_metatable( L, pg::directory_options_meta_traits,            directory_options::operators,                  directory_options::methods );
    register_metatable( L, pg::copy_options_meta_traits,                 copy_options::operators,                       copy_options::methods );
    register_metatable( L, pg::perms_meta_traits,                        fs_perms::operators,                           fs_perms::methods );
    register_metatable( L, pg::perm_options_meta_traits,                 fs_perm_options::operators,                    fs_perm_options::methods );
    register_metatable( L, pg::file_type_meta_traits,                    fs_file_type::operators,                       fs_file_type::methods );
    register_metatable( L, pg::file_time_type_meta_traits,               fs_file_time::operators,                       fs_file_time::methods );

    luaL_checkversion( L );
    lua_newtable( L );
    luaL_setfuncs( L, fs_functions, 0 );

    register_directory_options( L );
    register_copy_options( L );
    register_perms( L );
    register_perm_options( L );
    register_file_types( L );

    return 1;
}

}
