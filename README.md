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


## n, err, again, remain = writev( file, ... )

open the lua file handle from a pathname or descriptor of the file.

**NOTE**

if the non-blocking flag is not set in `file`, then writev function will block until the write operation is completed.


**Parameters**

- `file:file*|integer`: a file handle or a file descriptor.
- `...:string`: strings to be written to the file, one or more are required.

**Returns**

- `n:integer`: number of bytes written.
- `err:any`: error object.
- `again:boolean`: `true` if the write operation is incomplete with `EAGAIN` or `EWOULDBLOCK` error.
- `remain:string`: the remaining data that could not be written.


## Usage

```lua
local dump = require('dump')
local writev = require('io.writev')

-- write strings to a file
local f = assert(io.tmpfile())
local n, err, again, remain = writev(f, 'hello', ' writev ', nil, 'world') -- nil is treated as an empty string

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
