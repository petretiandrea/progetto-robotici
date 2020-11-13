//
// Created by Andrea Petreti on 13/11/2020.
//

#ifndef SWARM_GEN_FPLUS_FIX_H
#define SWARM_GEN_FPLUS_FIX_H

/** FPlus library and argos math library have a clash name with "Mod".
 *  This header file allow to include FPlus without conflict with argos.
 */

#ifdef Mod
#define DISABLE_MOD Mod
#undef Mod
#endif

#include <fplus/fplus.hpp>

#ifdef DISABLE_MOD
#define Mod DISABLE_MOD
#undef DISABLE_MOD
#endif

#endif //SWARM_GEN_FPLUS_FIX_H
