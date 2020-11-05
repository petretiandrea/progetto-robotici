require("utilities")
require "libs.lua-collections"
require("boolean-networks")
local argos = require("argos-robot")
--
local collect = collect
local my_if = my_if
local bool_to_int = bool_to_int
local active_network ---@type BooleanNetwork

local GROUND_SENSOR_COUNT = 4

local network_options = {
    ["node_count"] = 25,
    ["nodes_input_count"] = 3,
    ["bias"] = 0.1,
    ["network_inputs_count"] = 8 + GROUND_SENSOR_COUNT,
    ["network_outputs_count"] = 2,
    ["self_loops"] = false,
    ["override_output_nodes_bias"] = false
}

function init()
    -- math.randomseed(os.time)
    --math.randomseed(randomseed)
    active_network = BooleanNetwork(network_options)
end

--- @param network BooleanNetwork
--- @param speed number
--- @param proximity_threshold number
local function run_network(network, speed, proximity_threshold)
    local proximity_values = argos.get_proximity_values(#network.input_nodes - GROUND_SENSOR_COUNT)
    local ground_values = argos.get_ground_values(GROUND_SENSOR_COUNT)
    local inputs = argos.sensor_values_to_booleans(proximity_values, proximity_threshold)
            :addAll(argos.sensor_values_to_booleans(ground_values, 0.1, true))

    network:force_input_values(inputs)
    local network_outputs = collect(network:update_and_get_outputs()):all()
    argos.move_robot_by_booleans(network_outputs, speed)
end

function updateFromGenome(boolean_functions)
    active_network:update_boolean_functions(boolean_functions)
end

function step()
    run_network(active_network, 10, 0.1)
end

function destroy()

end

function reset()

end