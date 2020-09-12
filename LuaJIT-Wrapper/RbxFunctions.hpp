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

int getgenv(lua_State* L)
{
	lua_pushvalue(L, LUA_GLOBALSINDEX);
	LuaWrapper::Wrap(m_rL, L, -1);
}

int getrenv(lua_State* L)
{
	lua_pushvalue(L, LUA_REGISTRYINDEX);
	LuaWrapper::Wrap(m_rL, L, -1);
}

int getfenv(lua_State* L) {
	lua_pushvalue(m_L, LUA_GLOBALSINDEX);
	lua_xmove(m_L, L, 1);
	return 1;
}

int getsenv(lua_State* L) {
	lua_pushvalue(L, LUA_ENVIRONINDEX);
	LuaWrapper::Wrap(m_rL, L, -1);
	return 1;
}

int getgc(lua_State* L)
{
	void* UD = lua_touserdata(L, 1);
	lua_pushvalue(L, 1);
	lua_gettable(L, LUA_REGISTRYINDEX);
	if (!lua_isnil(L, -1)) {
		int Reference = lua_tointeger(L, -1);
		r_lua_pushnil(m_rL);
		r_lua_rawseti(m_rL, LUA_REGISTRYINDEX, Reference);
		r_lua_settop(m_rL, 0);
		return 1;
	}
	lua_pop(L, 1);
	return 0;
}

int GetHardwareID(lua_State* L) {
	HW_PROFILE_INFO hwProfileInfo;
	GetCurrentHwProfileA(&hwProfileInfo);
	string hwidWString = hwProfileInfo.szHwProfileGuid;
	string hwid(hwidWString.begin(), hwidWString.end());
	lua_pushstring(L, hwid.c_str());
	return 1;
}


void Register(lua_State* L)
{
	lua_register(L, "getrawmetatable", getrawmetatable);
	lua_register(L, "getgenv", getgenv);
	lua_register(L, "getrenv", getrenv);
	lua_register(L, "getfenv", getfenv);
	lua_register(L, "getsenv", getsenv);
	lua_register(L, "getmenv", getsenv);
	lua_register(L, "getgc", getgc);
	lua_register(L, "gethwid", GetHardwareID);
}
