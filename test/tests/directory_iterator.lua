local test = require( "test" )
local fs   = require( "filesystem" )


local _test_paths = {
    ["./test/tests/foo/baz"] = 0,
    ["./test/tests/foo/baz/file.txt"] = 1,
    ["./test/tests/foo/bar"] = 0,
    ["./test/tests/foo/bar/buz"] = 1,
    ["./test/tests/foo/bar/buz/plik.txt"] = 2,
    ["./test/tests/foo/bar/buz/fil.txt"] = 2,
    ["./test/tests/foo/bar/buz/tiedosto.txt"] = 2,
    ["./test/tests/foo/bar/buz/lime.txt"] = 2,
    ["./test/tests/foo/bar/buz/bestand.txt"] = 2,
    ["./test/tests/foo/bar/buz/Datei.txt"] = 2,
    ["./test/tests/foo/bar/buz/file.txt"] = 2,
    ["./test/tests/foo/file.txt"] = 0
}

local function _directory_iterator()
    local t = {}
    for e in fs.directory( "./test/tests/foo" ) do
        print( e )
        t[ tostring( e ) ] = true
    end

    local count = 0
    for p, d in pairs( _test_paths ) do
        if d == 0 then
            count = count + 1
            test.is_true( t[ p ] )
        end
    end
end

local function _recursive_directory_iterator()
    local t = {}
    for s, e in fs.recursive_directory( "./test/tests/foo" ) do
        t[ e ] = s:depth()
    end

    for p, d in pairs( t ) do
        local expected_depth = _test_paths[ tostring( p ) ]
        test.is_same( expected_depth, d )
    end
end

local tests =
{
    directory_iterator           = _directory_iterator,
    recursive_directory_iterator = _recursive_directory_iterator
}

return tests
