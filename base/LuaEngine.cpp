#include "LuaEngine.h"
#include "./global.h"
#include "./utilities.h"
#include "./core/interfaces.h"
#include "./core/variables.h"
#include "LuaStructs.h"
// Functions
int Functions::lua_setCatwareClantag(lua_State* L) {
	const char* clantag = lua_tostring(L, 1);
	U::SendClanTag(clantag, clantag);
	return 0;
}

int Functions::lua_setCatwareUsername(lua_State* L) {
	const char* name = lua_tostring(L, 1);
	U::SendName(name);
	return 0;
}



// Lua Stuff
bool Lua::CheckLua(lua_State* L, int r) {
	if (r != LUA_OK) {
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}

void Lua::RefreshLua() {
	for (const auto& it : std::filesystem::directory_iterator(C::GetWorkingPath().append(XorStr("Scripts"))))
	{
		if (it.path().filename().extension() == XorStr(".lua"))
		{
			std::filesystem::path LuaFilePath = std::filesystem::path(it.path().string()).string();
			std::filesystem::path LuaFileName = std::filesystem::path(it.path().filename().string()).string();
			Lua::ExecFile(LuaFilePath.string().c_str(), LuaFileName.string());
		}
	}
}

void Lua::exec(const char* code) {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	int r = luaL_dostring(L, code);
	if (r == LUA_OK) {
	}
	else{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
	}
	lua_close(L);
}

void Lua::ExecFile(const char* filename, std::string name) {
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	// Register our functions
	lua_register(L, "SetClanTag", Functions::lua_setCatwareClantag);
	lua_register(L, "SetLocalPlayerName", Functions::lua_setCatwareUsername);
	//lua_register(L, "EngineSetLocalViewAngles", Functions::lua_setLocalViewAngles);
	// Execution Code
	if (CheckLua(L, luaL_dofile(L, filename))) {
		// Get the main function sort of like int main () { } c++
		//lua_getglobal(L, "start");
		//lua_call(L, 0, 0);
		//lua_pop(L, 1);
		std::cout << name << std::endl;
		SavedLuaScript currentScript = SavedLuaScript(L, filename, name);
		activeLuaScripts.push_back(currentScript);
	}
	else {
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
	}
	//lua_close(L);
}