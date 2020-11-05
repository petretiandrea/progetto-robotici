require("utilities")
require "libs.lua-collections"
local l = require("libs.lambda")
local pprint = require('libs.pprint')

local argos = {}

-- localizing global functions to increase performance
local bool_to_int = bool_to_int
local collect = collect
local math_ceil = math.ceil

local PROXIMITY_SENSORS_COUNT = 24
local PROXIMITY_SENSORS = collect(range(1, PROXIMITY_SENSORS_COUNT))
local GROUND_SENSORS_COUNT = 4
local GROUND_SENSORS = collect(range(1, GROUND_SENSORS_COUNT))

---@param output boolean
---@param speed number
---@return number
local function convert_output(output, speed)
    return bool_to_int(output)*speed
end

---@param boolean_outputs boolean[]
---@param speed number
function argos.move_robot_by_booleans(boolean_outputs, speed)
    robot.wheels.set_velocity(convert_output(boolean_outputs[1], speed), convert_output(boolean_outputs[2], speed))
end

---@param value_count_to_return number
---@return number[]
function argos.get_proximity_values(value_count_to_return)
    local sensor_values =
        PROXIMITY_SENSORS:mapValues(function (input_index) return robot.proximity[input_index].value end)
    if(value_count_to_return == PROXIMITY_SENSORS_COUNT) then return sensor_values:all() end

    return sensor_values:grouped(PROXIMITY_SENSORS_COUNT / value_count_to_return)
            :mapValues(function (group) return collect(group):max() end):all()
end


---@param values number[]
---@param threshold number
---@param invert_values boolean
---@return boolean[]
function argos.sensor_values_to_booleans(values, threshold, invert_values)
    return collect(values):mapValues(function (proximity)
        if(invert_values) then
            return proximity <= threshold
        else
            return proximity > threshold
        end
    end):all()
end

-- https://www.argos-sim.info/plow2015/
function argos.get_ground_values(value_count_to_return)
    return GROUND_SENSORS:mapValues(function (index) return robot.motor_ground[index].value end):all()
end

return argos