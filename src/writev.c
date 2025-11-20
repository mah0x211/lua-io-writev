/**
 *  Copyright (C) 2024 Masatoshi Fukunaga
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */
#include <unistd.h>
// lua
#include <lauxhlib.h>
#include <lua_errno.h>

static int writev_lua(lua_State *L)
{
    int narg        = lua_gettop(L);
    int fd          = -1;
    size_t len      = 0;
    const char *str = NULL;
    ssize_t n       = 0;

    // check fd
    if (lauxh_isint(L, 1)) {
        fd = lauxh_checkint(L, 1);
    } else {
        fd = lauxh_fileno(L, 1);
    }

    // check strings
    if (narg < 2) {
        return lauxh_argerror(L, 2, "string expected, got no value");
    }
    for (int i = 2; i <= narg; i++) {
        if (lua_isnoneornil(L, i)) {
            lua_pushliteral(L, "");
            lua_replace(L, i);
        } else if (lua_type(L, i) != LUA_TSTRING) {
            return lauxh_argerror(L, i, "string expected, got %s",
                                  luaL_typename(L, i));
        }
    }
    // merge specified strings
    lua_concat(L, narg - 1);
    str = lua_tolstring(L, 2, &len);

    n = write(fd, str, len);
    if (n == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR) {
            // again
            lua_pushinteger(L, 0);
            lua_pushnil(L);
            lua_pushboolean(L, 1);
            lua_pushvalue(L, 2);
            return 4;
        } else if (errno == EPIPE) {
            // closed by peer
            return 0;
        }
        lua_pushnil(L);
        lua_errno_new(L, errno, "write");
        return 2;
    }

    lua_pushinteger(L, n);
    len -= (size_t)n;
    if (!len) {
        return 1;
    }
    lua_pushnil(L);
    lua_pushboolean(L, 1);
    lua_pushlstring(L, str + n, len);
    return 4;
}

LUALIB_API int luaopen_io_writev(lua_State *L)
{
    lua_errno_loadlib(L);
    lua_pushcfunction(L, writev_lua);
    return 1;
}
