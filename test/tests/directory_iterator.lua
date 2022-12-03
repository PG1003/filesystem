local test = require( "test" )
local fs   = require( "filesystem" )

local function _current_test_path( str )
    return tostring( fs.current_path():append( str ):make_preferred() )
end

local _test_paths = {
    [ _current_test_path( "test/tests/foo/baz" ) ] = 0,
    [ _current_test_path( "test/tests/foo/baz/file.txt" ) ] = 1,
    [ _current_test_path( "test/tests/foo/bar" ) ] = 0,
    [ _current_test_path( "test/tests/foo/bar/file.txt" ) ] = 1,
    [ _current_test_path( "test/tests/foo/bar/buz" ) ] = 1,
    [ _current_test_path( "test/tests/foo/bar/buz/plik.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/fil.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/tiedosto.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/lime.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/bestand.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/Datei.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/bar/buz/file.txt" ) ] = 2,
    [ _current_test_path( "test/tests/foo/file.txt" ) ] = 0
}

local function _directory_iterator()
    local t = {}
    for e in fs.directory( _current_test_path( "test/tests/foo" ) ) do
        t[ tostring( e ) ] = true
    end

    for p, d in pairs( _test_paths ) do
        if d == 0 then
            test.is_true( t[ p ] )
        end
    end
end

local function _directory_iterator_with_options()
    local t = {}
    local options = fs.directory_options.skip_permission_denied | fs.directory_options.follow_directory_symlink
    for e in fs.directory( _current_test_path( "test/tests/foo" ), options ) do
        t[ tostring( e ) ] = true
    end

    for p, d in pairs( _test_paths ) do
        if d == 0 then
            test.is_true( t[ p ] )
        end
    end
end

local function _recursive_directory_iterator()
    local t = {}
    for s, e in fs.recursive_directory( _current_test_path( "test/tests/foo" ) ) do
        t[ e ] = s:depth()
    end

    for p, d in pairs( t ) do
        local expected_depth = _test_paths[ tostring( p ) ]
        test.is_same( expected_depth, d )
    end
end

local tests =
{
    directory_iterator              = _directory_iterator,
    directory_iterator_with_options = _directory_iterator_with_options,
    recursive_directory_iterator    = _recursive_directory_iterator
}

return tests
