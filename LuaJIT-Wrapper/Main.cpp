#include "LuaWrapper.h"
#include <string>
#include <iostream>
#include <vector>
#include "Drawing.hpp"
#include "Client.h"
#pragma comment(lib, "wininet.lib")

DWORD ScriptContext;

using LuaWrapper::m_rL;
using LuaWrapper::m_L;

// no this is definitely not axon you shut ur mouth

void Execute(std::string Script) {
	Script = "spawn(function() script=Instance.new(\"LocalScript\") " + Script + "\r\nend)";
	if (luaL_loadbuffer(m_L, Script.c_str(), Script.size(), "@JIT"))
	{
		r_lua_getglobal(m_rL, "warn");
		r_lua_pushstring(m_rL, lua_tostring(m_L, -1));
		r_lua_pcall(m_rL, 1, 0, 0);
		return;
	}
	else
	{
		lua_pcall(m_L, 0, 0, 0);
	}
	UserDataGC(m_L);
}


DWORD WINAPI LuaPipe(PVOID lvpParameter)
{
	string WholeScript = "";
	HANDLE hPipe;
	char buffer[999999];
	DWORD dwRead;
	hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\LuaJIT"),
		PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_WAIT,
		1,
		999999,
		999999,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	while (hPipe != INVALID_HANDLE_VALUE)
	{
		if (ConnectNamedPipe(hPipe, NULL) != FALSE)
		{
			while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
			{
				buffer[dwRead] = '\0';
				try {
					try {
						WholeScript = WholeScript + buffer;
					}
					catch (...) {
					}
				}
				catch (std::exception e) {

				}
				catch (...) {

				}
			}
			Execute(WholeScript);

			WholeScript = "";
		}
		DisconnectNamedPipe(hPipe);
	}
}

void ReturnDataModel()
{
	static DWORD SC;
	SC = FindFirstClass(GetDMPad(), "ScriptContext");
	m_rL = (SC + 164) + *(DWORD*)(SC + 164);
	*(DWORD*)(*(DWORD*)(m_rL + 112) + 24) = 7;
}

void WrapGlobals()
{
	std::vector<const char*> Globals = {
	"game","Game","workspace","Workspace", "printidentity",
	"Axes","BrickColor","CFrame","Color3","ColorSequence","ColorSequenceKeypoint",
	"NumberRange","NumberSequence","NumberSequenceKeypoint","PhysicalProperties","Ray",
	"Rect","Region3","Region3int16","TweenInfo","UDim","UDim2","Vector2",
	"Vector2int16","Vector3","Vector3int16","Enum","Faces",
	"Instance","math","warn","typeof","type","print",
	"printidentity","ypcall","Wait","wait","delay","Delay","tick", "pcall", "spawn", "Spawn"
	};
	for (int i = 0; i < Globals.size(); i++)
	{
		r_lua_getglobal(m_rL, Globals[i]);
		LuaWrapper::Wrap(m_rL, m_L, -1);
		lua_setglobal(m_L, Globals[i]);
		r_lua_pop(m_rL, 1);
		std::cout << "WRAPPED GLOBAL: " << Globals[i] << std::endl;
	}
	printf("Globals wrapped!\n");
}

void main()
{
	ConsoleBypass("LuaJIT Wrapper");
	printf("Getting DataModel..\n");
	ReturnDataModel();
	printf("Setting LuaState..\n");
	m_L = luaL_newstate();
	LuaWrapper::VehHandlerpush();
	printf("Loading libraries..\n");
	luaL_openlibs(m_L);
	printf("Wrapping Globals..\n");
	WrapGlobals();
	DrawingAPI::InitiateDrawingAPI(m_L);
	lua_newtable(m_L);
	lua_setglobal(m_L, "_G");
	lua_setglobal(m_L, "shared");
	printf("Loaded Lua functions..\n");
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)LuaPipe, NULL, NULL, NULL);
	printf("Injected!");
	string player = GetPlayerName();
	std::cout << "Welcome, " + player << endl;
	SetClientName("Roblox Lua Just-In-Time Client");
}



BOOL APIENTRY DllMain(HMODULE Module, DWORD Reason, LPVOID) {
	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(Module);
		DWORD OldProtection;
		VirtualProtect(Module, 0x1000, PAGE_READWRITE, &OldProtection);
		ZeroMemory(Module, 0x1000);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
		break;
	}
	return TRUE;
}