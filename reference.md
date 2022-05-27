# filesystem for Lua - Reference

This reference is based on the [cppreference.com](https://en.cppreference.com/w/cpp/filesystem) filesystem library documentation.
It has been adapted and simplified for this filesystem Lua module.

## Contents

[absolute](#absolute-p-)  
[canonical](#canonical-p-)  
[copy](#copy-from-to-copy_options-)  
[copy_file](#copy_file-from-to-copy_options_)  
[copy_options](#copy_options)  
[current_path](#current_path-p-)  
[directory](#directory_p_directory_options_)  
[directory_entry](#directory_entry-p-)  
[directory_entry:assign](#directory_entryassign-p-)  
[directory_entry:replace_filename](#directory_entryreplace_filename-p-)  
[directory_entry:refresh](#directory_entryrefresh)  
[directory_entry:path](#directory_entrypath)  
[directory_entry:exists](#directory_entryexists)  
[directory_entry:is_block_file](#directory_entryis_block_file)  
[directory_entry:is_character_file](#directory_entryis_character_file)  
[directory_entry:is_directory](#directory_entryis_directory)  
[directory_entry:is_fifo](#directory_entryis_fifo)  
[directory_entry:is_other](#directory_entryis_other)  
[directory_entry:is_regular_file](#directory_entryis_regular_file)  
[directory_entry:is_socket](#directory_entryis_socket)  
[directory_entry:is_symlink](#directory_entryis_symlink)  
[directory_entry:file_size](#directory_entryfile_size)  
[directory_entry:hard_link_count](#directory_entryhard_link_count)  
[directory_entry:last_write_time](#directory_entrylast_write_time)  
[directory_entry:status](#directory_entrystatus)  
[directory_entry:symlink_status](#directory_entrysymlink_status)  
[directory_options](#directory-p-options-)  
[file_type](#file_type)  
[permissions](#permissions-p-perms-perm_options-)  
[perms](#perms)  
[perm_options](#perm_options)  
[path](#path-p-)  
[path:append](#pathappend-p-)  
[path:clear](#pathclear)  
[path:compare](#pathcompare_p_)  
[path:concat](#pathconcat-p-)  
[path:elements](#pathelements)  
[path:empty](#pathempty)  
[path:extension](#pathextension)  
[path:fileiname](#pathfilename)  
[path:has_extension](#pathhas_extension)  
[path:has_filename](#pathhas_filename)  
[path:has_parent_path](#pathhas_parent_path)  
[path:has_relative_path](#pathhas_relative_path)  
[path:has_root_directory](#pathhas_root_directory)  
[path:has_root_name](#pathhas_root_name)  
[path:has_root_path](#pathhas_root_path)  
[path:has_stem](#pathhas_stem)  
[path:is_absolute](#pathis_absolute)  
[path:is_relative](#pathis_relative)  
[path:lexically_normal](#pathlexically_normal)  
[path:lexically_proximate](#pathlexically_proximate-base-)  
[path:lexically_relative](#pathlexically_relative-base-)  
[path:make_preferred](#pathmake_preferred)  
[path:parent_path](#pathparent_path)  
[path:relative_path](#pathrelative_path)  
[path:remove_filename](#pathremove_filename)  
[path:root_directory](#pathroot_directory)  
[path:root_name](#pathroot_name)  
[path:root_path](#pathroot_path)  
[path:replace_extension](#pathreplace_extension-ext-)  
[path:replace_filename](#pathreplace_filename-repl-)  
[path:stem](#pathstem)  
[proximate](#proximate-p-base-)  
[recursive_directory](#recursive_directory-p-directory_options-)  
[recursive_directory_iterator_state](#recursive_directory_iterator_state)  
[recursive_directory_iterator_state:depth](#recursive_directory_iterator_statedepth)  
[recursive_directory_iterator_state:disable_recurion_pending](#recursive_directory_iterator_statedisable_recurion_pending)  
[recursive_directory_iterator_state:options](#recursive_directory_iterator_stateoptions)  
[recursive_directory_iterator_state:recursion_pending](#recursive_directory_iterator_staterecursion_pending)  
[recursive_directory_iterator_state:pop](#recursive_directory_iterator_statepop)  
[relative](#relative-p-base-)  
[weakly_canonical](#weakly_canonical-p-)  

### `absolute( p )`

Returns a path object with a absolute reference to the same file system location as `p`.

### `canonical( p )`

Converts path `p` to a canonical absolute path, i.e. an absolute path that has no dot, dot-dot elements or symbolic links in its generic format representation.
If `p` is not an absolute path, the function behaves as if it is first made absolute by [#absolute](#absolute) function.
The path `p` must exist.

### `copy( from, to, [copy_options] )`

Copies the file or directory `from` to file or directory `to`, using the options indicated by `copy_options`.

### `copy_file( from, to, [copy_options] )`

Copies a single file from `from` to `to`, using the options indicated by `copy_options`.

### `copy_options`

`copy_options` has members that are constants which are used to control the behavior of the [copy](#copy-from-to-copy_options-) and [copy_file](#copy_file-from-to-copy_options-) functions.
Its members support binary operators to combine, mask or check the options.

You can combine only one option from each option group below.
For example the result a copy with the options `skip_existing` and `overwrite_existing` combined is undefined but `overwrite_existing` and `skip_symlinks` is valid.

#### Options controlling [copy_file](#copy_file-from-to-copy_options-) when the file already exists

| Option               | Meaning |
|----------------------|---------|
| `none`               | Report an error (default behavior) |
| `skip_existing`      | Keep the existing file, without reporting an error |
| `overwrite_existing` | Replace the existing file |
| `update_existing`    | Replace the existing file only if it is older than the file being copied |

#### Options controlling the effects of [copy](#copy-from-to-copy_options-) on subdirectories

| Option      | Meaning |
|-------------|---------|
| `none`      | Skip subdirectories (default behavior) |
| `recursive` | Recursively copy subdirectories and their content |

#### Options controlling the effects of [copy](#copy-from-to-copy_options-) on symbolic links

| Option          | Meaning |
|-----------------|---------|
| `none`          | Follow symlinks (default behavior) |
| `copy_symlinks` | Copy symlinks as symlinks, not as the files they point to |
| `skip_symlinks` | Ignore symlinks |

#### Options controlling the kind of copying [copy](#copy-from-to-copy_options-) does

| Option              | Meaning |
|---------------------|---------|
| `none`              | Copy file content (default behavior) |
| `directories_only`  | Copy the directory structure, but do not copy any non-directory files |
| `create_symlinks`   | Instead of creating copies of files, create symlinks pointing to the originals. Note: the source path must be an absolute path unless the destination path is in the current directory. |
| `create_hard_links` | Instead of creating copies of files, create hardlinks that resolve to the same files as the originals |

### `current_path( [p] )`



### `directory_entry( [p] )`

Creates a new directory_entry object from `p`.
`p` can be a string or a path object.
A default empty directory_entry object is created when called without parameters.

### `directory_entry:assign( p )`

Assigns contents to the `directory_entry` object.
`p` can be a string or a path object.

### `directory_entry:replace_filename( p )`

Replaces the filename with extention by `p` of the `directory_entry` object and calls refresh to update the cached attributes.
`p` can be a string or a path object.

### `directory_entry:refresh()`

Update the cached status of the filesystem entry to which the `directory_entry` is referring to.

### `directory_entry:path()`

Returns a path object to the entry it refers to.

### `directory_entry:exists()`

Tests if the `directory_entry` refers an existing entry on the filesystem.

### `directory_entry:is_block_file()`

Tests if `directory_entry` refers to block device.

### `directory_entry:is_character_file()`

Tests if `directory_entry` refers to a character device.

### `directory_entry:is_directory()`

Tests if `directory_entry` refers to a directory.

### `directory_entry:is_fifo()`

Tests if `directory_entry` refers to a named pipe.

### `directory_entry:is_other()`

Tests if `directory_entry` refers to an other file.

### `directory_entry:is_regular_file()`

Tests if `directory_entry` refers to a regular file.

### `directory_entry:is_socket()`

Tests if  `directory_entry` refers to a named IPC socket.

### `directory_entry:is_symlink()`

Tests if `directory_entry` refers to a symbolic link.

### `directory_entry:file_size()`

Returns the file size of the file to which `directory_entry` refers.

### `directory_entry:hard_link_count()`

Returns the number of hard links referring to the file to which `directory_entry` refers.

### `directory_entry:last_write_time()`

Returns the (cached) time of the last data modification of the file to which `directory_entry` refers.

### `directory_entry:status()`

Returns a `perms` and a `file_type` of the file to which `directory_entry` refers.

### `directory_entry:symlink_status()`

Returns a `perms` and a `file_type` of the symbolic link to which `directory_entry` refers.

### `directory( p, [directory_options] )`

Enables iteration over the entries in a directory by using a generic for-loop.
The default for `directory_options` is `fs.directory_options.none`.
`p` can be a path object or a string.

``` lua
local fs = require( filesystem )

for entry in fs.directory( "my_directory", fs.directory_options.skip_permission_denied ) do
    print( entry:path():filename() )
end
```

In this example `entry` is a [directory_entry](#directory_entry) object.

### `directory_options`

`directory_options` has members that are constants which are used to control the behavior of the [directory](#directory_p__directory_options__) and [recursive_directory](#recursive_directory_p__directory_options__) functions.
Its members support binary operators to combine, mask or check the options.

| Option                   | Meaning |
|--------------------------|---------|
| none                     | Skip directory symlinks, permission denied is error |
| follow_directory_symlink | Follow rather than skip directory symlinks |
| skip_permission_denied   | Skip directories that would otherwise result in permission denied errors|

### `file_type`

`file_type` has members that are constants which are used to identify the type of the entry on the filesystem.

The supported file types are;

| Type        | Meaning |
|-------------|---------|
| `none`      | Indicates that the file status has not been evaluated yet, or an error occurred when evaluating it |
| `not_found` | Indicates that the file was not found (this is not considered an error) |
| `regular`   | A regular file |
| `directory` | A directory |
| `symlink`   | A symbolic link |
| `block`     | A block special file |
| `character` | A character special file |
| `fifo`      | A FIFO (also known as pipe) file |
| `socket`    | A socket file |
| `unknown`   | The file exists but its type could not be determined |

Depending on the implementation and platform the `file_type` returned by functions may nothold a value that is not listed.

### `permissions( p, perms, [perm_options] )`

Changes the permissions of the entry `p` refers to.
`perms` are the permissions that are applied as specified by `perm_options`.
By default `perms` will replace the current permissions when `perm_options` is not provided.

### `perms`

`perms` holds members that are constants which represents file access permissions.
Its members support binary operators to combine, mask or check permissions.

| Permission     | Meaning |
|----------------|---------|
| `none`         | No permission bits are set |
| `owner_read`   | File owner has read permission |
| `owner_write`  | File owner has write permission |
| `owner_exec`   | File owner has execute/search permission |
| `owner_all`    | File owner has read, write, and execute/search permissions. Equivalent to `owner_read | owner_write | owner_exec`. |
| `group_read`   | The file's user group has read permission |
| `group_write`  | The file's user group has write permission |
| `group_exec`   | The file's user group has execute/search permission |
| `group_all`    | The file's user group has read, write, and execute/search permissions. Equivalent to `group_read | group_write | group_exec`. |
| `others_read`  | Other users have read permission |
| `others_write` | Other users have write permission |
| `others_exec`  | Other users have execute/search permission |
| `others_all`   | Other users have read, write, and execute/search permissions. Equivalent to `others_read | others_write | others_exec`. |
| `all`          | Users have read, write, and execute/search permissions. Equivalent to `owner_all | group_all | others_all`. |
| `set_uid`      | Set user ID to file owner user ID on execution |
| `set_gid`      | Set group ID to file's user group ID on execution |
| `sticky_bit`   | Implementation-defined meaning, but POSIX XSI specifies that when set on a directory, only file owners may delete files even if the directory is writeable to others (used with /tmp) |
| `mask`         | All valid permission bits. Equivalent to `all | set_uid | set_gid | sticky_bit`. |

### `perm_options`

`perm_options` holds constants that control the behavior of the function [permissions](#permissions-p-perms-perm_options-).
The options support binary operators to combine, mask or check options.

| Option     | Meaning |
|------------|---------|
| `replace`  | Permissions will be completely replaced by the argument to permissions() (default behavior) |
| `add`      | Permissions will be replaced by the bitwise OR of the argument and the current permissions |
| `remove`   | Permissions will be replaced by the bitwise AND of the negated argument and current permissions |
| `nofollow` | Permissions will be changed on the symlink itself, rather than on the file it resolves to |

### `path( [p] )`

Creates a new path object from `p`.
`p` can be a string or another path object.
A default empty path object is created when called without parameters.

### `path:append( p )`

Appends another path object or string `p` to `path` with a directory separator.
Returns the `path` object itself.

### `path:clear()`

Erases the contents of `path`.

### `path:compare( p )`

Compares the lexical representations of `path` and `p` lexicographically.
`p` can be a string or another path object.

### `path:concat( p )`

Concatenates another path object or string `p` to `path`.
Returns the `path` object itself.

### `path:elements()`

Returns a function and a state to iterate through the elements of `path`
Typically you will use this function with a generic for-loop, for example;

```lua
for element in your_path:elements() do
    print( element )
end
```

### `path:empty()`

Check if `path` is empty.

### `path:extension()`

Returns a path object with the file extension component of `path`.

### `path:filename()`

Returns a path object with the filename component of `path`.

### `path:has_extension()`

Checks if [`path:root_path()`](#pathroot_path) will return an emty result.

### `path:has_filename()`

Checks if [`path:has_filename()`](#pathfilename) will return an emty result.

### `path:has_parent_path()`

Checks if [`path:parent_path()`](#pathparent_path) will return an emty result.

### `path:has_relative_path()`

Checks if [`path:relative_path()`](#pathrelative_path) will return an emty result.

### `path:has_root_directory()`

Checks if [`path:root_directory()`](#pathroot_directory) will return an emty result.

### `path:has_root_name()`

Checks if [`path:root_name()`](#pathroot_name) will return an emty result.

### `path:has_root_path()`

Checks if [`path:root_path()`](#pathroot_path) will return an emty result.

### `path:has_stem()`

Checks if [`path:stem()`](#pathstem) will return an emty result.

### `path:is_absolute()`

Returns true when `path` is an absolute path.

### `path:is_relative()`

Returns true when `path` is a relative path.

### `path:lexically_normal()`

Retuns the normal form of `path`.

### `path:lexically_proximate( base )`

Retuns the proximate form of `path`.
This function calls [path:lexically_relative](#pathlexically_relative-base-) and if this call results in an empty path then the value `path` is returned.

### `path:lexically_relative( base )`

Returns a path that is made relative to `base`.

### `path:make_preferred()`

Converts directory separators of `path` to preferred directory separator.
Returns the `path` object itself.

### `path:parent_path()`

Returns a path object with the parent path of `path`.

### `path:relative_path()`

Returns a path object with a path relative to the root path.

### `path:remove_filename()`

Removes the filename component from `path`.
Returns the `path` object itself.

### `path:root_directory()`

Returns a path object with the root directory of `path` or a new empty path object in case `path` does not include a root directory.

### `path:root_name()`

Returns a path object with the root name of `path` or a new empty path object in case `path` does not include a root name.

### `path:root_path()`

Returns a path object with the root path of `path` or a new empty path object in case `path` does not include a root name.

### `path:replace_extension( [ext] )`

Replaces the extension with `ext` or removes the extension when `ext` is not provided.
`ext` can be a path object or a string.
Returns the `path` object itself.

### `path:replace_filename( repl )`

Replaces the filename component of `path` by `repl`.
`repl` can be a path object or a string.
Returns the `path` object itself.

### `path:stem()`

Returns the filename identified by the generic-format path stripped of its extension.

### `proximate( p, [base] )`

Returns a path which is `p` that is relative to base.
Tries to resolve symlinks and normalizes with [weakly_canonical](#weakly_canonical-p-) and [path:lexically_proximate](#pathlexically_proximate-base-) for `p` and `base` before other processing.
Default for `base` when it's not provided is the result of [current_path](#current_path-p-).

### `recursive_directory( p, [directory_options] )`

Enables recusive iteration over entries in a directory and its subdirectories by using a generic for-loop.
The default for `directory_options` is `fs.directory_options.none`.
`p` can be a path object or a string.

``` lua
local fs = require( filesystem )

for state, entry in fs.recursive_directory( "my_directory" ) do
    print( state:depth() )
    print( entry )
end
```

In this example `state` is a [recursive_directory_iterator_state](#recursive_directory_iterator_state) object that let you control the recursion.
`entry` is a [directory_entry](#directory_entry) object.

### `recursive_directory_iterator_state`

An object that controls the recursive direcotry iteration

### `recursive_directory_iterator_state:depth()`

Returns the number of directories from the starting directory to the currently iterated directory, i.e. the current depth of the directory hierarchy.
The starting directory has depth of 0, its subdirectories have depth 1, etc.

### `recursive_directory_iterator_state:disable_recurion_pending()`

Disables recursion to the currently referred subdirectory, if any.

### `recursive_directory_iterator_state:options()`

Returns the options set at start of the directory iteration that affect the iteration.

### `recursive_directory_iterator_state:recursion_pending()`

Returns true if the next iteration will cause the directory that is currently referred will be iterated into.

### `recursive_directory_iterator_state:pop()`

Moves one level up in the directory hierarchy.

### `relative( p, [base] )`

Returns a path which is `p` that is relative to base.
Resolves symlinks and normalizes both `p` and `base` before other processing.
Default for `base` when it's not provided is the result of [current_path](#current_path-p-).

### `weakly_canonical( p )`

Returns a path composed by results of calling [canonical](#canonical-p-) for the leading elements of `p` that exist (as determined by [status](#status-p-)), followed by the elements of `p` that do not exist.
