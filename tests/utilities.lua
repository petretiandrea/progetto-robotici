local pprint = require('libs.pprint')
local inspect = require("libs.inspect")
require("libs.lua-collections")

local setmetatable = setmetatable
local getmetatable = getmetatable
local type = type
local pairs = pairs
local collect = collect
local math_ceil = math.ceil
local math_floor = math.floor
local math_random = math.random

setmetatable(_G,{__index=function (_, key) -- gives an error when using undefined global variables
  error("Attempt to access undefined global variable "..tostring(key),2)
end })

---@return string
function one_line_serialize(table) -- does not serialize meta-methods nor class information
  local remove_all_metatables = function(item, path)
    if item ~="class" and path[#path] ~= inspect.METATABLE then return item end
  end
  return inspect(table, {newline="", indent="", process = remove_all_metatables})
end

local function cycle_aware_copy(t, cache) -- Code is from penlight library
  if type(t) ~= 'table' then return t end
  if cache[t] then return cache[t] end
  local res = {}
  cache[t] = res
  local mt = getmetatable(t)
  for k,v in pairs(t) do
    k = cycle_aware_copy(k, cache) ---@type table
    v = cycle_aware_copy(v, cache) ---@type table
    res[k] = v
  end
  setmetatable(res,mt)
  return res
end

---@param deep boolean
---@overload fun(table)
---@overload fun(table, deep: boolean)
function copy(table, deep) -- deep copy is off by default for performance reasons
  if(deep) then return cycle_aware_copy(table,{}) end
  return setmetatable(collect(table):clone():all(), getmetatable(table))
end

function clone_object(object)
  return copy(object, true)
end

-- Creates an array from start to end_value. For example, range(0, 10, 2) returns {0, 2, 4, 6, 8, 10}
---@param start number
---@param end_value number
---@param step number
---@return number[]
---@overload fun(start: number, end_value: number): number[]
---@overload fun(start: number, end_value: number, step: number): number[]
function range(start, end_value, step) -- ATTENTION: this is not lazy, big ranges will take up more memory
  step = step or 1
  local array = {}
  local size = 1
  for i = start,end_value,step do
    array[size] = i
    size = size + 1
  end
  return array
end

---@param length number
---@return any[]
function fill_array(length, value)
  return create_array(length, function(_) return value end)
end

---@param length number
---@param value_generator fun(index: number): any
---@return any[]
function create_array(length, value_generator)
  return collect(range(1, length)):mapValues(function (index) return value_generator(index) end):all()
end

---@param row_count number
---@param column_count number
---@param value_generator fun(row: number, column: number): any
---@return any[][]
function create_matrix(row_count, column_count, value_generator)
  local matrix = {} -- matrix of Boolean functions
  for row_index=1,row_count do
    matrix[row_index] = {}
    for column_index=1,column_count do
      matrix[row_index][column_index] = value_generator(row_index, column_index)
    end
  end
  return matrix
end

function create_matrix_from_array(row_count, array)
  local column_count = #array / row_count
  return create_matrix(row_count, column_count, function (row, col)
    return array[col + row * row_count]
  end)
end

---@param array any[]
---@param indexes_of_values_to_replace number[]
---@param new_values_array any[]
---@return any[]
function replace_in_array(array, indexes_of_values_to_replace, new_values_array)
  local newArray = copy(array)
  if(#indexes_of_values_to_replace ~= #new_values_array) then error("#indexes is different than #values") end
  if(#indexes_of_values_to_replace > #array) then error("#indexes is more than #array") end
  for new_value_index=1,#new_values_array do
    local index = indexes_of_values_to_replace[new_value_index]
    if(newArray[index] == nil) then
      error("Array contains nil at the specified index")
    end
    newArray[index] = new_values_array[new_value_index]
  end
  return newArray
end

---@param matrix any[][]
---@param positions_2D_to_change number[][]
---@param new_values_array any[]
---@return any[][]
function replace_in_matrix(matrix, positions_2D_to_change, new_values_array)
  local newMatrix = copy(matrix, true)
  if(#positions_2D_to_change ~= #new_values_array) then error("#indexes is different than #values") end
  for current_index=1,#new_values_array do
    if(#positions_2D_to_change[current_index] ~= 2) then error("each position should have 2 indexes") end
    local row_to_change = positions_2D_to_change[current_index][1]
    local column_to_change = positions_2D_to_change[current_index][2]
    if(newMatrix[row_to_change][column_to_change] == nil) then error("Matrix has nil at this index") end
    newMatrix[row_to_change][column_to_change] = new_values_array[current_index]
  end
  return newMatrix
end

---@param index_1D number
---@param matrix any[][]
---@return number[]
function from_1D_to_2D_index(index_1D, matrix)
  local column_count = #matrix[1]
  return {math_ceil(index_1D/column_count), (index_1D-1)%column_count + 1} -- returns row_index and column_index
end

---@param matrix any[][]
---@param positions_2D_1 number[][]
---@param positions_2D_2 number[][]
---@return any[][]
function swap_in_matrix(matrix, positions_2D_1, positions_2D_2) -- #positions_2D_1 has to be == #positions_2D_2
  local new_matrix = copy(matrix, true)
  if(#positions_2D_1 ~= #positions_2D_2) then error("#positions_1 is different than #positions_2") end
  for position_index=1, #positions_2D_1 do
    local position_1, position_2 = positions_2D_1[position_index], positions_2D_2[position_index]
    local temporary_value = new_matrix[position_1[1]][position_1[2]]
    new_matrix[position_1[1]][position_1[2]] = new_matrix[position_2[1]][position_2[2]]
    new_matrix[position_2[1]][position_2[2]] = temporary_value
  end
  return new_matrix
end

---@param matrix any[][]
---@return number
function get_fixed_matrix_cells_count(matrix) -- ATTENTION: this works only for matrices with fixed-sized rows
  return #matrix * #matrix[1]
end

---@param condition boolean
function my_if(condition, then_value, else_value) -- functional if. ATTENTION: the values are always evaluated!
  if condition then return then_value else return else_value end
end

---@return boolean
function is_boolean(value)
  return type(value) == "boolean"
end

---@param value number
---@return number
function round(value)
  return math_floor(value + 0.5)
end

---@param bias number
---@return boolean
function biased_random_boolean(bias) -- for example, bias = 0.8 means that the value will be true with 80% chance
  return math_random() <= bias
end

---@param minimum number
---@param maximum number
---@param prefer_low_values boolean
---@param bias number
---@return number
function skewed_random(minimum, maximum, prefer_low_values, bias) --bigger bias values result in more skewed numbers
  local final_bias = bias or 2
  if(final_bias < 1) then error("'bias' parameter in skewed_random function should be greater than 1") end
  local random = math_random()
  local skewed_random = my_if(prefer_low_values, random^final_bias, random^(1/final_bias))
  return minimum + (maximum - minimum) * skewed_random
end

-- extracts random numbers from a range of numbers. By default, it avoids repetition.
---@param extractions_count number
---@param range_minimum number
---@param range_maximum number
---@param allow_repetition boolean
---@return number[]
---@overload fun(extractions_count: number, range_minimum: number, range_maximum: number): number[]
---@overload fun(extractions_count: number, range_minimum: number, range_maximum: number, allow_repetition: boolean): number[]
function random_extract(extractions_count, range_minimum, range_maximum, allow_repetition)
  return collect(range(range_minimum, range_maximum)):random(extractions_count, allow_repetition):all()
end

---@param extractions_count number
---@param elements any[]
---@param exclusions any[]
---@param allow_repetition boolean
---@return any[]
---@overload fun(extractions_count: number, elements: any[], exclusions: any[]): any[]
---@overload fun(extractions_count: number, elements: any[], exclusions: any[], allow_repetition: boolean): any[]
function random_extract_except(extractions_count, elements, exclusions, allow_repetition)
  return collect(elements):diff(exclusions):random(extractions_count, allow_repetition):all()
end

---@param array any[]
---@param object any
---@return number
function index_of(array, object)
  if type(array) ~= "table" then error("table expected, got " .. type(array), 2) end

  for i = 1, #array do
    if object == array[i] then
      return i
    end
  end
end

---@param value boolean
---@return number
function bool_to_int(value)
  return value and 1 or 0
end