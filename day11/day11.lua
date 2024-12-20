local tokenize = function(str, fmt, transform)
    local t = {}

    for token in str:gmatch(fmt) do
        table.insert(t, transform(token))
    end

    return t
end

local file = assert(io.open(arg[1], "r"))
local numbers = tokenize(file:read("a*"), "%d+", tonumber)
file:close()

local count_digits = function(nr)
    local count = 1
    local pow = 10
    while pow <= nr do
        pow = pow * 10
        count = count + 1
    end
    return count;
end

local split_number = function(nr, state)
    local count = count_digits(nr)
    if count % 2 ~= 0 then
        return false
    end

    local div = math.pow(10, count / 2)
    local bottom = nr % div;
    local upper = (nr - bottom) / div;
    state[1] = upper
    state[2] = bottom
    return true
end

local blink_map = {}
local split
split = function(nr, blinks)
    if blink_map[nr] and blink_map[nr][blinks] then
        return blink_map[nr][blinks]
    end

    if blinks == 0 then
        return 1
    end

    local number_split = {}
    local next_blinks = blinks - 1
    local result

    if nr == 0 then
        result = split(1, next_blinks)
    elseif split_number(nr, number_split) then
        result = split(number_split[1], next_blinks) + split(number_split[2], next_blinks)
    else
        result = split(nr * 2024, next_blinks)
    end

    blink_map[nr] = blink_map[nr] or {}
    blink_map[nr][blinks] = result

    return result
end

local max_blinks = 75
local total = 0

for _, nr in ipairs(numbers) do
    total = total + split(nr, max_blinks)
end

print(string.format("Part two: %d", total))