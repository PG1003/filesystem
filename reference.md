# filesystem for Lua - Reference

This reference is based on the [cppreference.com](https://en.cppreference.com/w/cpp/filesystem) filesystem library documentation.
It has been adapted and simplified for this filesystem Lua module.

## Contents

[copy](#copy_from_to__copy_options__)  
[copy_file](#copy_file_from_to__copy_options__)  
[copy_options](#copy_options)  
[directory](#directory_p__directory_options__)  
[directory_entry](#directory_entry__p__)  
[directory_entry:assign](#directory_entry_assign_p_)  
[directory_entry:replace_filename](#directory_entry_replace_filename_p_)  
[directory_entry:refresh](#directory_entry_refresh__)  
[directory_entry:path](#directory_entry_path__)  
[directory_entry:exists](#directory_entry_exists__)  
[directory_entry:is_block_file](#directory_entry_is_block_file__)  
[directory_entry:is_character_file](#directory_entry_is_character_file__)  
[directory_entry:is_directory](#directory_entry_is_directory__)  
[directory_entry:is_fifo](#directory_entry_is_fifo__)  
[directory_entry:is_other](#directory_entry_is_other__)  
[directory_entry:is_regular_file](#directory_entry_is_regular_file__)  
[directory_entry:is_socket](#directory_entry_is_socket__)  
[directory_entry:is_symlink](#directory_entry_is_symlink__)  
[directory_entry:file_size](#directory_entry_file_size__)  
[directory_entry:hard_link_count](#directory_entry_hard_link_count__)  
[directory_entry:last_write_time](#directory_entry_last_write_time__)  
[directory_entry:status](#directory_entry_status__)  
[directory_entry:symlink_status](#directory_entry_symlink_status__)  
[directory_options](#directory_options)  
[file_type](#file_type)  
[perms](#perms)  
[perm_options](#perm_options)  
[path](#path_p_)  
[path:append](#path_append__p__)  
[path:clear](#path_clear__)  
[path:compare](#path_compare_p_)  
[path:concat](#path_concat_p_)  
[path:elements](#path_elements__)  
[path:empty](#path_empty__)  
[path:extension](#path_extension__)  
[path:fileiname](#path_filename__)  
[path:has_extension](#path_has_extension__)  
[path:has_filename](#path_has_filename__)  
[path:has_parent_path](#path_has_parent_path__)  
[path:has_relative_path](#path_has_relative_path__)  
[path:has_root_directory](#path_has_root_directory__)  
[path:has_root_name](#path_has_root_name__)  
[path:has_root_path](#path_has_root_path__)  
[path:has_stem](#path_has_stem__)  
[path:is_absolute](#path_is_absolute__)  
[path:is_relative](#path_is_relative__)  
[path:lexically_normal](#path_lexically_normal__)  
[path:lexically_proximate](#path_lexically_proximate_base_)  
[path:lexically_relative](#path_lexically_relative_base_)  
[path:make_preferred](#path_make_preferred__)  
[path:parent_path](#path_parent_path__)  
[path:relative_path](#path_relative_path__)  
[path:remove_filename](#path_remove_filename__)  
[path:root_directory](#path_root_directory__)  
[path:root_name](#path_root_name__)  
[path:root_path](#path_root_path__)  
[path:replace_extension](#path_replace_extension__ext__)  
[path:replace_filename](#path_replace_filename_repl_)  
[path:stem](#path_stem__)  
[recursive_directory](#recursive_directory_p__directory_options__)  




### `copy( from, to, [copy_options] )`



### `copy_file( from, to, [copy_options] )`



### `copy_options`

`copy_options` has members that are constants which are used to control the behavior of the [copy](#copy_from_to__copy_options__) and [copy_file](#copy_file_from_to__copy_options__) functions.
Its members support binary operators to combine, mask or check the options.

#### Options controlling [copy_file](#copy_file_from_to__copy_options__) when the file already exists

| Option               | Meaning |
|----------------------|---------|
| `none`               | Report an error (default behavior) |
| `skip_existing`      | Keep the existing file, without reporting an error |
| `overwrite_existing` | Replace the existing file |
| `update_existing`    | Replace the existing file only if it is older than the file being copied |

#### Options controlling the effects of [copy](#copy_from_to__copy_options__) on subdirectories

| Option      | Meaning |
|-------------|---------|
| `none`      | Skip subdirectories (default behavior) |
| `recursive` | Recursively copy subdirectories and their content |

#### Options controlling the effects of [copy](#copy_from_to__copy_options__) on symbolic links

| Option          | Meaning |
|-----------------|---------|
| `none`          | Follow symlinks (default behavior) |
| `copy_symlinks` | Copy symlinks as symlinks, not as the files they point to |
| `skip_symlinks` | Ignore symlinks |

#### Options controlling the kind of copying [copy](#copy_from_to__copy_options__) does

| Option              | Meaning |
|---------------------|---------|
| `none`              | Copy file content (default behavior) |
| `directories_only`  | Copy the directory structure, but do not copy any non-directory files |
| `create_symlinks`   | Instead of creating copies of files, create symlinks pointing to the originals. Note: the source path must be an absolute path unless the destination path is in the current directory. |
| `create_hard_links` | Instead of creating copies of files, create hardlinks that resolve to the same files as the originals |

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



### `directory_options`

`directory_options` has members that are constants which are used to control the behavior of the [directory](#directory_p__directory_options__) and [recursive_directory](#recursive_directory_p__directory_options__) functions.
Its members support binary operators to combine, mask or check the options.

| Option                   | Meaning |
|--------------------------|---------|
| none                     | (Default) Skip directory symlinks, permission denied is error |
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

`perm_options` holds constants that control the behavior of the function [permissions](#permissions_p_perms__perm_options__).
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

Checks if [`path:root_path()`](#path_root_path__) will return an emty result.

### `path:has_filename()`

Checks if [`path:has_filename()`](#path_filename__) will return an emty result.

### `path:has_parent_path()`

Checks if [`path:parent_path()`](#path_parent_path__) will return an emty result.

### `path:has_relative_path()`

Checks if [`path:relative_path()`](#path_relative_path__) will return an emty result.

### `path:has_root_directory()`

Checks if [`path:root_directory()`](#path_root_directory__) will return an emty result.

### `path:has_root_name()`

Checks if [`path:root_name()`](#path_root_name__) will return an emty result.

### `path:has_root_path()`

Checks if [`path:root_path()`](#path_root_path__) will return an emty result.

### `path:has_stem()`

Checks if [`path:stem()`](#path_stem__) will return an emty result.

### `path:is_absolute()`

Returns true when `path` is an absolute path.

### `path:is_relative()`

Returns true when `path` is a relative path.

### `path:lexically_normal()`

Retuns the normal form of `path`.

### `path:lexically_proximate( base )`

Retuns the proximate form of `path`.
This function calls `path:lexically_relative( base )` and if this call results in an empty path then the value `path` is returned.

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

### `recursive_directory( p, [directory_options] )`
