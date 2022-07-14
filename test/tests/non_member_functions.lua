local test = require( "test" )
local fs   = require( "filesystem" )

local function _current_test_path( str )
    return tostring( fs.current_path():append( str ):make_preferred() )
end

local function _absolute()
    local p1 = fs.path( "." )
    local p2 = fs.absolute( p1 )

    test.is_true( fs.equivalent( p1, p2 ) )
end

local function _canonical()
    local p1 = fs.path( "." )
    local p2 = fs.canonical( p1 )

    test.is_true( fs.equivalent( p1, p2 ) )
end

local function _weakly_canonical()
    local p1 = fs.path( "." )
    local p2 = fs.weakly_canonical( p1 )

    test.is_true( fs.equivalent( p1, p2 ) )

    local p3 = fs.path( "./foo" )
    local p4 = fs.weakly_canonical( p1 )    -- Should not emit an error
end

local function _relative()
    local p1 = fs.current_path()
    local p2 = fs.relative( p1 )
    local p3 = fs.relative( p1, p2 )

    test.is_same( p2, fs.path( "." ) )
    test.is_same( p2, p2 )
end

local function _proximate()
    local p1 = fs.current_path()
    local p2 = fs.proximate( p1 )
    local p3 = fs.proximate( p1, p2 )

    test.is_same( p2, fs.path( "." ) )
    test.is_same( p2, p2 )
end

local function _copy()
    local src = "./test/tests/foo"
    local dst = fs.path( "./test/tests/bar" )
    
    fs.copy( src, dst )
    test.is_true( fs.exists( fs.path( dst ):append( "file.txt" ) ) )

    fs.remove_all( dst )

    fs.copy( src, dst, fs.copy_options.recursive | fs.copy_options.directories_only | fs.copy_options.overwrite_existing )
    test.is_true( fs.exists( fs.path( dst ):append( "bar/buz" ) ) )

    fs.remove_all( dst )
end

local function _copy_file()
    local src = "./test/tests/foo/file.txt"
    local dst = fs.path( "./test/tests/file.txt" )

    test.is_true( fs.copy_file( src, dst ) )
    test.is_true( fs.exists( dst ) )

    test.is_false( fs.copy_file( src, dst, fs.copy_options.skip_existing ) )

    fs.remove( dst )
end

local function _create_copy_read_symlink_status()
    local src = fs.path( "./test/tests/foo/file.txt" )
    local dst = "./test/tests/file.txt"
    local cpy = "./test/tests/copy.txt"

    fs.create_symlink( src, dst )
    test.is_true( fs.is_symlink( dst ) )

    fs.copy_symlink( dst, cpy );
    test.is_true( fs.is_symlink( cpy ) )

    local target = fs.read_symlink( dst )
    test.is_same( src, target )

    local is_ok, err = pcall( fs.read_symlink, src )
    test.is_false( is_ok )

    local perm, t = fs.symlink_status( dst )
    test.is_same( t, fs.file_type.symlink )

    fs.remove( dst )
    fs.remove( cpy )
end

local function _create_directory()
    local dir = "./test/tests/bar/"

    test.is_true( fs.create_directory( dir ) )
    test.is_true( fs.exists( dir ) )

    fs.remove( dir )
    test.is_false( fs.exists( dir ) )
end

local function _create_directories_remove_all()
    local dir = _current_test_path( "test/tests/bar/baz" )

    test.is_true( fs.create_directories( dir ) )
    test.is_true( fs.exists( dir ) )

    fs.remove_all( _current_test_path( "./test/tests/bar" ) )
    test.is_false( fs.exists( dir ) )
end

local function _create_hard_link_and_count()
    local src = "./test/tests/foo/file.txt"
    local dst = "./test/tests/file.txt"

    test.is_same( fs.hard_link_count( src ), 1 )

    fs.create_hard_link( src, dst )
    test.is_true( fs.exists( dst ) )
    test.is_same( fs.hard_link_count( src ), 2 )

    fs.remove( dst )
    test.is_false( fs.exists( dst ) )
end

local function _create_directory_symlink()
    local src = "./test/tests/foo"
    local dst = "./test/tests/bar"

    fs.create_directory_symlink( src, dst )
    test.is_true( fs.is_symlink( dst ) )

    fs.remove( dst )
end

local function _current_path()
    local p1 = fs.current_path()

    fs.current_path( p1 )
    local p2 = fs.current_path()

    test.is_same( p1, p2 )
end

local function _equivalent()
    local p1 = fs.path( _current_test_path( "test/tests/foo/file.txt" ) )
    local p2 = _current_test_path( "././test/../test/tests/foo/file.txt" )

    test.is_true( fs.equivalent( p1, p2 ) )
    test.is_false( fs.equivalent( p1, _current_test_path( "test/tests/foo/bar" ) ) )
end

local function _file_size_resize()
    local p = "./test/tests/foo/file.txt"

    local size = fs.file_size( p )
    test.is_true( math.type( size ) == "integer" )
    test.is_true( size >= 0 )

    local new_size = size + 42
    fs.resize_file( p, new_size )

    test.is_same( fs.file_size( p ), new_size )

    fs.resize_file( p, size )
end

local function _status_permissions()
    local p         = fs.path( "./test/tests/foo/file.txt" )
    local perm1, t1 = fs.status( p )

    test.is_same( t1, fs.file_type.regular )

    local new_perm = perm1 | fs.perms.owner_exec
    fs.permissions( p, new_perm )

    local perm2, t2 = fs.status( p )
    test.is_same( new_perm, perm2 )

    fs.permissions( p, fs.perms.owner_exec, fs.perm_options.remove )

    local perm3, t3 = fs.status( p )
    test.is_same( perm1, perm3 )
end

local function _rename()
    local p1 = fs.path( "./test/tests/foo/file.txt" )
    local p2 = "./test/tests/foo/elif.xtx"

    test.is_true( fs.exists( p1 ) )
    fs.rename( p1, p2 )
    test.is_true( fs.exists( p2 ) )
    fs.rename( p2, p1 )
    test.is_true( fs.exists( p1 ) )
end

local function _space()
    local p                         = fs.path( "./test/tests/foo" )
    local free, available, capacity = fs.space( p )

    test.is_not_nil( free )
    test.is_not_nil( available )
    test.is_not_nil( capacity )
end

local function _temp_directory_path()
    local tmp = fs.temp_directory_path()

    test.is_false( tmp:empty() )
end

local function _last_write_time()
    local str = _current_test_path( "test/tests/foo/file.txt" )
    local p   = fs.path( str )
    local ft1 = fs.last_write_time( str )
    local ft2 = fs.last_write_time( p )

    test.is_not_nil( ft1 )
    test.is_same( ft1, ft2 )

    local ft3 = fs.file_time_now()
    fs.last_write_time( p, ft3 )
    
    test.is_not_same( fs.last_write_time( p ), ft2 )
    test.is_same( fs.last_write_time( str ), ft3 )

    local ft4 = ft3 - 1
    test.is_same( ft3 - ft4, 1 )
end

local function _file_time_now()
    local ft1 = fs.file_time_now()

    local ft2 = fs.last_write_time( "./test/tests/foo/file.txt" )
    test.is_true( ft1 > ft2 )
    test.is_true( ft1 >= ft2 )
    test.is_false( ft1 < ft2 )
    test.is_false( ft1 <= ft2 )

    local diff = ft1 - ft2
    test.is_true( diff > 0 )
    test.is_same( ft1, ft2 + diff )
end

local function _is_xyz()
    local p1 = _current_test_path( "test/tests/foo/" )
    test.is_false( fs.is_block_file( p1 ) )
    test.is_false( fs.is_character_file( p1 ) )
    test.is_true( fs.is_directory( p1 ) )
    test.is_false( fs.is_fifo( p1 ) )
    test.is_false( fs.is_other( p1 ) )

    local p2 = _current_test_path( "./test/tests/foo/file.txt" )
    test.is_true( fs.is_regular_file( p2 ) )
    test.is_false( fs.is_socket( p2 ) )
    test.is_false( fs.is_symlink( p2 ) )
end

local function _enum_binary_operators()
    local all  = fs.perms.all
    local none = all & ~all

    test.is_same( none, fs.perms.none )
    test.is_same( none | all, fs.perms.all )
end

local tests =
{
    absolute                        = _absolute,
    canonical                       = _canonical,
    weakly_canonical                = _weakly_canonical,
    relative                        = _relative,
    proximate                       = _proximate,
    copy                            = _copy,
    copy_file                       = _copy_file,
    create_copy_read_symlink_status = _create_copy_read_symlink_status,
    create_directory                = _create_directory,
    create_directories_remove_all   = _create_directories_remove_all,
    create_hard_link_and_count      = _create_hard_link_and_count,
    create_directory_symlink        = _create_directory_symlink,
    current_path                    = _current_path,
    equivalent                      = _equivalent,
    file_size_resize                = _file_size_resize,
    permissions                     = _permissions,
    status_permissions              = _status_permissions,
    rename                          = _rename,
    space                           = _space,
    temp_directory_path             = _temp_directory_path,
    last_write_time                 = _last_write_time,
    file_time_now                   = _file_time_now,
    is_xyzz                         = _is_xyz,
    enum_binary_operators           = _enum_binary_operators
}

return tests
