#pragma once
#include "LuaWrapper.h"

using LuaWrapper::m_L;
using LuaWrapper::m_rL;

int getrawmetatable(lua_State* L) {
	LuaWrapper::Unwrap(L, m_rL, 1);

	if (r_lua_getmetatable(m_rL, -1) == 0) {
		lua_pushnil(L);
		return 0;
	}
	LuaWrapper::Wrap(m_rL, L, -1);

	return 1;
}
