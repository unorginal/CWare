#include <iostream>
#include <string>
#include "../dependencies/Lua542/include/lua.hpp"
#include "LuaStructs.h"
#include <string>

struct SavedLuaScript {
	lua_State* L;
	bool enabled;
	std::string fileName;
	const char* fileLocation;
	SavedLuaScript(lua_State* L, const char* fileLocation, std::string name, bool enabled = false) {
		this->L = L;
		this->fileLocation = fileLocation;
		this->fileName = name;
		this->enabled = false;
	}

};

inline std::vector<SavedLuaScript> activeLuaScripts = {};

namespace Functions {
	int lua_setCatwareClantag(lua_State* L);
	int lua_setCatwareUsername(lua_State* L);
}

namespace Lua {
	void exec(const char* code);
	void ExecFile(const char* filename, std::string name);
	bool CheckLua(lua_State* L, int r);
	void RefreshLua();
}