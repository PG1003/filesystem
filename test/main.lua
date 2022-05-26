local short_src  = debug.getinfo( 1 ).short_src
local test_path  = string.gsub( short_src, "main.lua$", "?.lua" )
local tests_path = string.gsub( short_src, "main.lua$", "tests/?.lua" )

package.path     = package.path .. ";./".. test_path .. ";./".. tests_path

local build_path = string.gsub( short_src, "test/main.lua$", "build/?.so" )
package.cpath    = package.cpath .. ";./".. build_path

local test = require( "test" )

local available_test_modules =
{
    path                 = true,
    directory_iterator   = true,
    directory_entry      = true,
    non_member_functions = true
}


local function run_tests()
    if arg[ 1 ] then
        local test_modules = {}
        for i = 1, #arg do
            local module = arg[ i ]
            if available_test_modules[ module ] then
                test_modules[ module ] = true
            else
                print( "Module " .. arg[ i ] .. " not available." )
                return
            end
        end
        return test.run_test_modules( test_modules )
    end

    return test.run_test_modules( available_test_modules )
end

if run_tests() then
    print( "      _" )
    print( "     /(|" )
    print( "    (  :" )
    print( "   __\\  \\  _____" )
    print( " (____)  '|" )
    print( "(____)|   |" )
    print( " (____).__|" )
    print( "  (___)__.|_____" )
else
    print( ">>> tests failed!" )
end
