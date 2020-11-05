//
// Created by Andrea Petreti on 04/11/2020.
//

#include "LuaBoolNetController.h"

void LuaBoolNetController::UpdateFromGenome(int size, const bool functions[]) {
    auto state = realController->GetLuaState();

    lua_getglobal(state, LuaBoolNetController::GENOME_UPDATE_FUNCTION);
    arrayAsTable(state, functions, size);
    if(lua_pcall(state, 1, 0, 0) != 0) {
        std::cerr << "Error during call " << lua_tostring(state, -1) << std::endl;
    }
}


/* Methods that wrap lua controller */
LuaBoolNetController::LuaBoolNetController() {
    realController = new CLuaController();
}

LuaBoolNetController::~LuaBoolNetController() {
    delete realController;
}

void LuaBoolNetController::Init(TConfigurationNode &t_node) {
    realController->Init(t_node);
}

void LuaBoolNetController::ControlStep() {
    realController->ControlStep();
}

void LuaBoolNetController::Reset() {
    realController->Reset();
}

void LuaBoolNetController::Destroy() {
    realController->Destroy();
}

template<typename T> void LuaBoolNetController::arrayAsTable(lua_State *L, T* array, int size) {
    lua_newtable(L);
    for (size_t i = 0; i < size; i++) {
        lua_pushinteger(L, i + 1);
        lua_pushnumber(L, array[i]);
        lua_settable(L, -3);
    }
}
/*
template<typename T> void LuaBoolNetController::matrixAsTable(lua_State* L, T** matrix, int row, int column) {
    lua_newtable(L);
    for(size_t i = 0; i < row; i++) {
        lua_pushinteger(L, i + 1);
        arrayAsTable(L, matrix[i], column);
        lua_settable( L, -3 );
    }
}*/


REGISTER_CONTROLLER(LuaBoolNetController, "customlua")