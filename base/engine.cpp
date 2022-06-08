#include "engine.h"
#include "common.h"
#include "./utilities.h"
#include "./core/interfaces.h"

static const luaL_Reg functions[] =
{
	//{ "SetViewAngles", EngineLua::lua_setLocalViewAngles},
	{ NULL, NULL }
};


extern "C" int luaopen_engine(lua_State * L)
{
	//lua_register(L, "libluatest", functions);
	return 0;
}