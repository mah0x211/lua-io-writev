local testcase = require('testcase')
local assert = require('assert')
local writev = require('io.writev')
local pipe = require('os.pipe')

function testcase.writev()
    local r, w, err = pipe(true)
    assert.is_nil(err)
    -- calculate the capacity of the pipe
    local cap = 0
    repeat
        local n, _, again = w:write(string.rep('a', 512))
        cap = cap + n
    until again
    r:read(cap)

    -- test that write specified strings to a file
    local n, again, remain
    n, err, again, remain = writev(w:fd(), 'hello', nil, ' writev ', 'world')
    assert.is_nil(err)
    assert.is_nil(again)
    assert.is_nil(remain)
    assert.equal(n, 18)
    assert.equal(r:read(), 'hello writev world')

    -- test that return again=true even if a little extra capacity is available.
    assert(w:write(string.rep('a', cap - 11)))
    n, err, again, remain = writev(w:fd(), 'hello', ' writev ', 'world')
    assert.is_nil(err)
    assert.is_true(again)
    assert.is_nil(remain)
    assert.equal(n, 0)

    -- test that return nil if peer closed
    r:close()
    n, err, again, remain = writev(w:fd(), 'hello', ' writev ', 'world')
    assert.is_nil(err)
    assert.is_nil(again)
    assert.is_nil(n)
    assert.is_nil(remain)

    -- test that throws an error if invalid file descriptor
    n, err, again, remain = writev(123456789, 'hello', ' writev ', 'world')
    assert.match(err, 'EBADF')
    assert.is_nil(n)
    assert.is_nil(again)
    assert.is_nil(remain)

    -- test that throws an error if no string argument specified
    err = assert.throws(writev, w:fd())
    assert.match(err, '#2 .+string expected, got no value', false)

    -- test that throws an error if invalid string argument
    err = assert.throws(writev, w:fd(), 'hello', 123, 'world')
    assert.match(err, '#3 .+string expected, got number', false)
end

function testcase.writev_to_file()
    local f = assert(io.tmpfile())

    -- test that write to a file
    local n, err, again = writev(f, 'hello', ' writev ', 'world')
    assert.is_nil(err)
    assert.is_nil(again)
    assert.equal(n, 18)
    f:seek('set')
    assert.equal(f:read(), 'hello writev world')
end
