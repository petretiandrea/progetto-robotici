//
// Created by Andrea Petreti on 04/11/2020.
//

#ifndef SWARM_GEN_LUABOOLNETCONTROLLER_H
#define SWARM_GEN_LUABOOLNETCONTROLLER_H

#include <argos3/core/wrappers/lua/lua_controller.h>

using namespace argos;

class LuaBoolNetController : public CCI_Controller {

public:
    const char* GENOME_UPDATE_FUNCTION = "updateFromGenome";
    LuaBoolNetController();
    ~LuaBoolNetController() override;
    void Init(TConfigurationNode& t_node) override;
    void ControlStep() override;
    void Reset() override;
    void Destroy() override;



    void UpdateFromGenome(int size, const bool functions[]);

private:
    CLuaController* realController;
    template<typename T> void arrayAsTable(lua_State *L, T* array, int size);
    //template<typename T> void matrixAsTable(lua_State* L, T** matrix, int row, int column);
};

#endif //SWARM_GEN_LUABOOLNETCONTROLLER_H
