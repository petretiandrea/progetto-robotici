require("utilities")
require "libs.lua-collections"
local l = require("libs.lambda")
local class = require 'libs.middleclass'

local copy = copy
local collect = collect
local bool_to_int = bofunction bool_to_int(value)
return value and 1 or 0
endol_to_int
local index_of = index_of

---@class BooleanNetwork
---@field public boolean_functions boolean[][]
---@field public overridden_output_functions boolean[][]|nil
---@field public connection_matrix number[][]
---@field public node_states boolean[]
---@field public input_nodes number[]
---@field public output_nodes number[]
BooleanNetwork = class('BooleanNetwork')

---@param node_index number
---@param nodes_input_count number
---@param self_loops boolean
---@return number[]
local function get_node_input_indexes(node_index, node_indexes, nodes_input_count, self_loops)
  local exclusions = my_if(self_loops, {}, {node_index}) ---@type number[]
  return random_extract_except(nodes_input_count, node_indexes, exclusions)
end

---@param self_loops boolean
---@param node_count number
---@param nodes_input_count number
local function create_random_connection_matrix(self_loops, node_count, nodes_input_count)
  local node_indexes = range(1, node_count)
  local inputs_calculator =
  function(node_index) return get_node_input_indexes(node_index, node_indexes, nodes_input_count, self_loops) end
  return collect(node_indexes):mapValues(inputs_calculator):all()
end

local function check_options(options)
  if options["node_count"] == nil or options["nodes_input_count"] == nil or options["network_inputs_count"] == nil or
          options["network_outputs_count"] == nil or options["bias"] == nil then
    error("missing one or more boolean network options in 'options' table")
  end
  if options["nodes_input_count"] > options["node_count"] or
          options["network_inputs_count"] + options["network_outputs_count"] > options["node_count"] or
          options["bias"] < 0 or options["bias"] > 1 or
          not is_boolean(options["self_loops"]) or not is_boolean(options["override_output_nodes_bias"]) then
    error("network options are nonsensical (for example network_outputs_count > node_count, etc)")
  end
end

local function extract_input_output_nodes(options)
  local input_output_nodes_count = options["network_inputs_count"]+options["network_outputs_count"] ---@type number
  local extracted_nodes = random_extract(input_output_nodes_count, 1, options["node_count"])
  local results = collect(extracted_nodes):splitAt(options["network_inputs_count"]):all()
  return results[1], results[2] -- inputs and outputs
end

---@param node_count number
---@param function_length number
local function make_output_functions(node_count, function_length) -- creates boolean functions for output nodes
  return collect(range(1, node_count)):mapValues(function (_) -- half of the boolean values will be 1
    local array_with_false = create_array(function_length, l("(_) -> false"))
    local half_array_with_trues = create_array(function_length /2, l("(_) -> true"))
    local indexes_to_flip = collect(range(1, function_length)):random(function_length /2):all()
    return replace_in_array(array_with_false, indexes_to_flip, half_array_with_trues)
  end):all()
end

--[[ Creates a new random BN. Example: in 'options', put node_count=100, nodes_input_count=3, bias=0.5,
 network_inputs_count=24, network_outputs_count=2, self_loops=true --]]
---@param options table<string, number|boolean>
---@return BooleanNetwork
function BooleanNetwork:initialize(options)
  check_options(options)
  self.node_count = options["node_count"]
  self.function_length = 2^options["nodes_input_count"]
  self.boolean_functions = create_matrix(options["node_count"], self.function_length,
          function(_, _) return biased_random_boolean(options["bias"]) end)
  self.connection_matrix =
  create_random_connection_matrix(options["self_loops"], options["node_count"], options["nodes_input_count"])
  self.node_states = create_array(options["node_count"], l("(_) -> math.random() <= 0.5")) -- fills the network
  self.input_nodes, self.output_nodes = extract_input_output_nodes(options)
  local output_functions = make_output_functions(#self.output_nodes, self.function_length)
  self.overridden_output_functions = my_if(options["override_output_nodes_bias"], output_functions, nil)
end

---@param inputs boolean[]
function BooleanNetwork:force_input_values(inputs)
  local inputs_count = #inputs
  for network_input_index = 1, inputs_count do
    self.node_states[self.input_nodes[network_input_index]] = inputs[network_input_index]
  end
end

function BooleanNetwork:update_boolean_functions(boolean_functions_array)
  self.boolean_functions = create_matrix_from_array(self.node_count, boolean_functions_array)
end

--- Updates the provided node, given the index of the node and the network. It returns the updated node value.
---@param network BooleanNetwork
local function get_updated_node_value(node, network, previous_network_state)
  local node_input_values = {}
  local inputs_count = #network.connection_matrix[node]
  for input_node_index = 1,inputs_count do
    node_input_values[input_node_index] = previous_network_state[network.connection_matrix[node][input_node_index]]
  end
  local boolean_table_column = collect(node_input_values) -- 1 + input1*1 + input2*2 + input3*4 + input4*8 + ...
          :zipWithIndex():mapValues(function (x) return bool_to_int(x[1])*(2^(x[2]-1)) end):sum() + 1
  if(network:has_overridden_output_bias() and collect(network.output_nodes):hasValue(node)) then
    return network.overridden_output_functions[index_of(network.output_nodes, node)][boolean_table_column]
  end
  return network.boolean_functions[node][boolean_table_column]
end

--- Updates the network to the next state and returns the state of the output nodes.
---@return boolean[]
function BooleanNetwork:update_and_get_outputs()
  local previous_network_state = copy(self.node_states) -- this makes the network update synchronous
  local node_count = self:get_node_count()
  for node_index = 1,node_count do
    self.node_states[node_index] = get_updated_node_value(node_index, self, previous_network_state)
  end
  return collect(self.output_nodes)
          :mapValues(function (node_index) return self.node_states[node_index] end):all()
end

--- Returns whether the network has been created with override_output_nodes_bias set to true
---@return boolean
function BooleanNetwork:has_overridden_output_bias()
  return self.overridden_output_functions ~= nil
end

--- Returns the amount of inputs for each node. This expects the network to have constant inputs count for each node.
---@return number
function BooleanNetwork:get_nodes_input_count()
  return #self.connection_matrix[1]
end

--- Returns the amount of nodes present in the network.
---@return number
function BooleanNetwork:get_node_count()
  return #self.node_states
end
