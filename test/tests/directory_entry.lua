local test = require( "test" )
local fs   = require( "filesystem" )

local function _assign()
    local str = "/home/foo/bar.baz"
    local de1 = fs.directory_entry( str )
    local de2 = fs.directory_entry()

    test.is_not_same( de1, de2 )

    de2:assign( str )
    test.is_same( de1, de2 )

    de2:assign( fs.path( "/home/foo/bar.fuz" ) )
    test.is_not_same( de1, de2 )
end

local function _replace_filename()
    local de = fs.directory_entry( fs.path( "/home/foo/bar" ) )

    de:replace_filename( "baz" )
    test.is_same( de, fs.directory_entry( "/home/foo/baz" ) )

    de:replace_filename( fs.path( "buz" ) )
    test.is_same( de, fs.directory_entry( "/home/foo/buz" ) )
end

local function _refresh_exists()
    local src = "./test/tests/foo/file.txt"
    local dst = fs.path( "./test/tests/foo/file.xtx" )

    local de = fs.directory_entry( dst )
    test.is_false( de:exists() )

    test.is_true( fs.copy_file( src, dst ) )

    de:refresh()
    test.is_true( de:exists() )

    fs.remove( dst )
    test.is_true( de:exists() )
end

local function _path()
    local str = "/home/foo/bar"
    local de = fs.directory_entry( str )
    local p1 = de:path()
    local p2 = fs.path( str )

    test.is_same( p1, p2 )
end

local function _is_xyz()
    local de = fs.directory_entry( "./test/tests/foo/" )
    test.is_false( de:is_block_file() )
    test.is_false( de:is_character_file() )
    test.is_true( de:is_directory() )
    test.is_false( de:is_fifo() )
    test.is_false( de:is_other() )

    de:assign( "./test/tests/foo/file.txt" )
    test.is_true( de:is_regular_file() )
    test.is_false( de:is_socket() )
    test.is_false( de:is_symlink() )
end

local function _file_size()
    local de = fs.directory_entry( "./test/tests/foo/file.txt" )

    test.is_true( de:file_size() > 0 )
end

local function _hard_link_count()
    local de = fs.directory_entry( "./test/tests/foo/file.txt" )

    test.is_true( de:hard_link_count() > 0 )
end

local function _last_write_time()
    local de = fs.directory_entry( "./test/tests/foo/file.txt" )

    test.is_true( fs.file_time_now() > de:last_write_time() )
end

local function _status()
    local de = fs.directory_entry( "./test/tests/foo/file.txt" )

    local perms, file_type = de:status()

    test.is_true( ( perms & fs.perms.owner_read ) == fs.perms.owner_read )
    test.is_same( file_type, fs.file_type.regular )
end

local function _symlink_status()
    local src = fs.path( "./test/tests/foo/file.txt" ):make_preferred()
    local dst = fs.path( "./test/tests/foo/file.xtx" ):make_preferred()

    fs.create_symlink( src, dst )

    local de               = fs.directory_entry( dst )
    local perms, file_type = de:symlink_status()

    test.is_true( ( perms & fs.perms.owner_read ) == fs.perms.owner_read )
    test.is_same( file_type, fs.file_type.symlink )

    fs.remove( dst )
end

local tests =
{
    assign              = _assign,
    replace_filename    = _replace_filename,
    refresh_exists      = _refresh_exists,
    path                = _path,
    is_xyz              = _is_xyz,
    file_size           = _file_size,
    hard_link_count     = _hard_link_count,
    last_write_time     = _last_write_time,
    status              = _status,
    symlink_status      = _symlink_status
}

return tests
