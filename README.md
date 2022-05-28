# filesystem

A filesystem Lua module based on the std::filesystem C++ standard library.

## Features

The filesystem module supports the same features as std::filesystem does.  
This includes the following;
* File operations such as copy, remove and renaming.
* Iterating (recursively) through direcotries.
* Create and remove symbolic links and hard links.
* Querying information about a file like permissions and type of a filesystem entity.
* Modifying permissions.

## Requirements

* Lua version 5.3 or 5.4
* C++17 or newer compatible C++ compiler

## Examples

### Path utilities
``` lua
local fs = require( "filesystem" )

local p        = fs.path( "/home/foo/bar.txt" )
local root_dir = p:root_directory()
local filename = p:filename()
local has_ext  = p:has_extension()

p:replace_filename( "buz.bak" )

print( root_dir )
print( filename )
print( has_ext )
print( p )
```
The output is:
>/  
>bar.txt  
>true  
>/home/foo/buz.bak  

### Copy file

``` lua
local fs = require( "filesystem" )

-- Create source path object
local src = fs.path( "/home/foo/bar.txt" )

-- Create a copy of the source path object
local dst = fs.path( str )

-- Change extension of the copied file to 'bak'
dst:replace_extension( "bak" )

-- Perform copy
fs.copy( src, dst )
```

### Iterating files in a directory

``` lua
local fs = require( "filesystem" )

-- Iterate (non-recursive) through the entries of the given directory
for e in fs.directory( "./test/tests/foo" ) do
    -- 'e' is an directory_entry object containing information about the
    -- currently visited entry in the directory.

    -- Don't show directories in the output.
    if not e:is_directory() then
        print( e )
    end
end
```

### Error handling

``` lua
local fs = require( "filesystem" )

-- Wrap one or more filesystem functions that could fail in a function
local function may_fail( src, dst )
    fs.create_symlink( src, dst )
end

-- Execute filesystem operations
local success, result = pcall( may_fail, "/home/foo/bar.txt", "/home/foo/baz.txt" )

-- No errors occured when 'success' is true. In this case 'result' contains
--   the return value of 'may_fail' which in this example returns nothing.
-- If 'success' is false then an error occured and 'result' an error message
--   when the error was caused by the filesystem module.

if not success then
    print( result )
end
```

## Building the filesystem Lua module

You can build the module by executing the following command in the project directory;
``` sh
make all
```

This library has only [one cpp-file](/src/filesystem.cpp) without external dependencies which makes easy to integrate it into other projects or your own build system.

## Tests

There are tests to validate the Lua API of the filesystem module.
However, these tests are written with a Linux OS in mind, so there is no guaranty that all test will pass on other platforms.

You can find these tests in the [tests](/test/tests) directory.

Execute the following command to run the tests;

``` sh
lua test/main.lua
```

Or using the makefile;

``` sh
make test
```
