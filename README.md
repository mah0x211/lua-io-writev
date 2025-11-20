# lua-io-writev

[![test](https://github.com/mah0x211/lua-io-writev/actions/workflows/test.yml/badge.svg)](https://github.com/mah0x211/lua-io-writev/actions/workflows/test.yml)
[![codecov](https://codecov.io/gh/mah0x211/lua-io-writev/branch/master/graph/badge.svg)](https://codecov.io/gh/mah0x211/lua-io-writev)

Writes data to a specified file descriptor.


## Installation

```
luarocks install io-writev
```

---

## Error Handling

the following functions return the `error` object created by https://github.com/mah0x211/lua-errno module.


## n, err, again, remain = writev( file, data )

writes multiple strings to a file handle or file descriptor using the `writev` system call for efficient vectorized I/O.

**NOTE**

if the non-blocking flag is not set in `file`, then writev function will block until the write operation is completed.


**Parameters**

- `file:file*|integer`: a file handle or a file descriptor.
- `data:table`: a table containing strings to be written to the file. `nil` entries are treated as empty strings.

**Returns**

- `n:integer`: number of bytes written.
- `err:any`: error object.
- `again:boolean`: `true` if the write operation is incomplete with `EAGAIN`, `EWOULDBLOCK` or `EINTR` error.
- `remain:table`: when `again` is `true`, contains the unwritten data:
  - If no bytes were written (`EAGAIN`/`EWOULDBLOCK`/`EINTR` immediately), returns the original `data` table.
  - If partial write occurred, returns a new table containing only the remaining data. The original `data` table is not modified.


## Usage

```lua
local dump = require('dump')
local writev = require('io.writev')
local f = assert(io.tmpfile())

-- write strings to a file
-- nil is treated as an empty string
local n, err, again, remain = writev(f, {'hello', ' writev ', nil, 'world'}) 
while again do
    -- wait until writable with select/poll/epoll...
    -- write remaining data
    n, err, again, remain = writev(f, remain) 
end
f:seek('set')
print(dump({
    n = n,
    err = err,
    again = again,
    remain = remain,
    content = f:read(),
}))
-- {
--     content = "hello writev world",
--     n = 18
-- }
```
