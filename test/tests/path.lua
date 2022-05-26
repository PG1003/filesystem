local test = require( "test" )
local fs   = require( "filesystem" )


local function _tostring()
    local p1 = fs.path( "/home/foo/" )

    test.is_same( tostring( p1 ), "/home/foo/" )
end

local function _concat()
    local p1 = fs.path( "/home/foo" )
    local p2 = fs.path( "bar" )

    p1:concat( p2 )

    test.is_same( p1, fs.path( "/home/foobar" ) )

    p1:concat( "baz" )

    test.is_same( p1, fs.path( "/home/foobarbaz" ) )
end

local function _append()
    local p1 = fs.path( "/home/foo" )
    local p2 = fs.path( "bar" )

    p1:append( p2 )

    test.is_same( p1, fs.path( "/home/foo/bar" ) )

    p1:append( "baz" )

    test.is_same( p1, fs.path( "/home/foo/bar/baz" ) )
end

local function _eq()
    local p1 = fs.path( "/home/foo/" )
    local p2 = fs.path( "/home/foo/" )
    local p3 = fs.path( "/home/bar/" )

    test.is_true( p1 == p2 )
    test.is_false( p1 == p3 )
end

local function _lt()
    local p1 = fs.path( "/home/foo/" )
    local p2 = fs.path( "/home/foo/" )
    local p3 = fs.path( "/home/bar/" )

    test.is_false( p1 < p2 )
    test.is_false( p1 < p3 )
    test.is_true( p3 < p1 )
end

local function _le()
    local p1 = fs.path( "/home/foo/" )
    local p2 = fs.path( "/home/foo/" )
    local p3 = fs.path( "/home/bar/" )

    test.is_true( p1 <= p2 )
    test.is_false( p1 <= p3 )
    test.is_true( p3 <= p1 )
end

local function _clear()
    local p = fs.path( "/home/foo/" )

    test.is_false( p:empty() )

    p:clear()

    test.is_true( p:empty() )
end

local function _make_preferred()
    local p = fs.path( "\\home\\foo\\" )

    -- make_preferred does noting on unix like systems...
    test.is_same( p:make_preferred(), fs.path("\\home\\foo\\" ) )
end

local function _remove_filename()
    local p = fs.path( "/home/foo/bar" )

    test.is_true( p:has_filename() )

    test.is_false( p:remove_filename():has_filename() )
end

local function _replace_filename()
    local p1 = fs.path( "/home/foo" )
    local p2 = fs.path( "baz" )

    p1:replace_filename( "bar" )

    test.is_same( p1, fs.path( "/home/bar" ) )

    test.is_same( p1:replace_filename( p2 ), fs.path( "/home/baz" ) )
end

local function _replace_extension()
    local p1 = fs.path( "/home/foo.bar" )
    local p2 = fs.path( "buz" )

    p1:replace_extension( "baz" )

    test.is_same( p1, fs.path( "/home/foo.baz" ) )

    p1:replace_extension( p2 )

    test.is_same( p1, fs.path( "/home/foo.buz" ) )

    test.is_same( p1:replace_extension(), fs.path( "/home/foo" ) )
end

local function _root_name()
    local p1 = fs.path()
    local p2 = fs.path( "/home" )

    local p3 = p1:root_name()

    test.is_same( p3, fs.path() )

    local p4 = p2:root_name()

    test.is_same( p4, fs.path( "" ) )
end

local function _root_directory()
    local p1 = fs.path()
    local p2 = fs.path( "/home/" )

    local p3 = p1:root_directory()

    test.is_same( p3, fs.path() )

    local p4 = p2:root_directory()

    test.is_same( p4, fs.path( "/" ) )
end

local function _root_path()
    local p1 = fs.path()
    local p2 = fs.path( "/home/" )

    local p3 = p1:root_path()

    test.is_same( p3, fs.path() )

    local p4 = p2:root_path()

    test.is_same( p4, fs.path( "/" ) )
end

local function _relative_path()
    local p1 = fs.path()
    local p2 = fs.path( "/home/" )

    local p3 = p1:relative_path()

    test.is_same( p3, fs.path() )

    local p4 = p2:relative_path()

    test.is_same( p4, fs.path( "home/" ) )
end

local function _parent_path()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/bar" )

    local p3 = p1:parent_path()

    test.is_same( p3, fs.path() )

    local p4 = p2:parent_path()

    test.is_same( p4, fs.path( "/home/foo" ) )
end

local function _filename()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/bar.baz" )

    local p3 = p1:filename()

    test.is_same( p3, fs.path() )

    local p4 = p2:filename()

    test.is_same( p4, fs.path( "bar.baz" ) )
end

local function _stem()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/bar.baz" )

    local p3 = p1:stem()

    test.is_same( p3, fs.path() )

    local p4 = p2:stem()

    test.is_same( p4, fs.path( "bar" ) )
end

local function _extension()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/bar.baz" )

    local p3 = p1:extension()

    test.is_same( p3, fs.path() )

    local p4 = p2:extension()

    test.is_same( p4, fs.path( ".baz" ) )
end

local function _empty()
    local p1 = fs.path()
    local p2 = fs.path( "" )
    local p3 = fs.path( p1 )
    local p4 = fs.path( "/home/foo/" )
    local p5 = fs.path( p4 )

    test.is_true( p1:empty() )
    test.is_true( p2:empty() )
    test.is_true( p3:empty() )
    test.is_false( p4:empty() )
    test.is_false( p5:empty() )
end

local function _has_root_path()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/" )

    test.is_false( p1:has_root_path() )
    test.is_true( p2:has_root_path() )
end

local function _has_root_name()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/" )

    test.is_false( p1:has_root_name() )
    test.is_false( p2:has_root_name() )
end

local function _has_root_directory()
    local p1 = fs.path()
    local p2 = fs.path( "/home/foo/" )

    test.is_false( p1:has_root_directory() )
    test.is_true( p2:has_root_directory() )
end

local function _has_relative_path()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )

    test.is_false( p1:has_relative_path() )
    test.is_false( p2:has_relative_path() )
    test.is_true( p3:has_relative_path() )
end

local function _has_parent_path()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )
    local p4 = fs.path( "foo/bar" )

    test.is_false( p1:has_parent_path() )
    test.is_true( p2:has_parent_path() )
    test.is_true( p3:has_parent_path() )
    test.is_true( p4:has_parent_path() )
end

local function _has_filename()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )
    local p4 = fs.path( "/home/foo" )
    local p5 = fs.path( "/home/foo/" )

    test.is_false( p1:has_filename() )
    test.is_false( p2:has_filename() )
    test.is_true( p3:has_filename() )
    test.is_true( p4:has_filename() )
    test.is_false( p5:has_filename() )
end

local function _has_stem()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )
    local p4 = fs.path( "/home/foo" )
    local p5 = fs.path( "/home/.foo" )
    local p6 = fs.path( "/home/foo.bar" )

    test.is_false( p1:has_stem() )
    test.is_false( p2:has_stem() )
    test.is_true( p3:has_stem() )
    test.is_true( p4:has_stem() )
    test.is_true( p5:has_stem() )
    test.is_true( p6:has_stem() )
end

local function _has_extension()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home/" )
    local p4 = fs.path( "/home/foo" )
    local p5 = fs.path( "/home/foo.bar" )

    test.is_false( p1:has_extension() )
    test.is_false( p2:has_extension() )
    test.is_false( p3:has_extension() )
    test.is_false( p4:has_extension() )
    test.is_true( p5:has_extension() )
end

local function _is_absolute()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )
    local p4 = fs.path( ".." )
    local p5 = fs.path( "foo/bar" )
    local p6 = fs.path( "../foo" )
    local p7 = fs.path( "/home/foo/../bar" )

    test.is_false( p1:is_absolute() )
    test.is_true( p2:is_absolute() )
    test.is_true( p3:is_absolute() )
    test.is_false( p4:is_absolute() )
    test.is_false( p5:is_absolute() )
    test.is_false( p6:is_absolute() )
    test.is_true( p7:is_absolute() )
end

local function _is_relative()
    local p1 = fs.path()
    local p2 = fs.path( "/" )
    local p3 = fs.path( "/home" )
    local p4 = fs.path( ".." )
    local p5 = fs.path( "foo/bar" )
    local p6 = fs.path( "../foo" )
    local p7 = fs.path( "/home/foo/../bar" )

    test.is_true( p1:is_relative() )
    test.is_false( p2:is_relative() )
    test.is_false( p3:is_relative() )
    test.is_true( p4:is_relative() )
    test.is_true( p5:is_relative() )
    test.is_true( p6:is_relative() )
    test.is_false( p7:is_relative() )
end

local function _lexically_normal()
    local p1 = fs.path( "/home/./foo/.." )
    local p2 = p1:lexically_normal()

    test.is_not_same( p1, p2 )
    test.is_same( p2, fs.path( "/home/" ) )
end

local function _lexically_relative()
    local p1 = fs.path( "/home/foo/bar" )
    local p2 = p1:lexically_relative( "/home/baz" )

    test.is_not_same( p1, p2 )
    test.is_same( p2, fs.path( "../foo/bar" ) )
end

local function _lexically_proximate()
    local p1 = fs.path( "home/foo" )
    local p2 = p1:lexically_proximate( "/home/foo" )

    test.is_same( p1, p2 )
end

local function _elements()
    local p = fs.path( "/home/foo" )

    local t = {}
    for e in p:elements() do
        t[ #t + 1 ] = e
    end

    test.is_same( #t, 3 )
    test.is_same( t[ 1 ], fs.path( "/" ) )
    test.is_same( t[ 2 ], fs.path( "home" ) )
    test.is_same( t[ 3 ], fs.path( "foo" ) )
end

local tests =
{
    tostring            = _tostring,
    concat              = _concat,
    append              = _append,
    eq                  = _eq,
    lt                  = _lt,
    le                  = _le,
    clear               = _clear,
    make_preferred      = _make_preferred,
    remove_filename     = _remove_filename,
    replace_filename    = _replace_filename,
    replace_extension   = _replace_extension,
    root_name           = _root_name,
    root_directory      = _root_directory,
    root_path           = _root_path,
    relative_path       = _relative_path,
    parent_path         = _parent_path,
    filename            = _filename,
    stem                = _stem,
    extension           = _extension,
    empty               = _empty,
    has_root_path       = _has_root_path,
    has_root_name       = _has_root_name,
    has_root_directory  = _has_root_directory,
    has_relative_path   = _has_relative_path,
    has_parent_path     = _has_parent_path,
    has_filename        = _has_filename,
    has_stem            = _has_stem,
    has_extension       = _has_extension,
    is_absolute         = _is_absolute,
    is_relative         = _is_relative,
    lexically_normal    = _lexically_normal,
    lexically_relative  = _lexically_relative,
    lexically_proximate = _lexically_proximate,
    elements            = _elements
}

return tests
