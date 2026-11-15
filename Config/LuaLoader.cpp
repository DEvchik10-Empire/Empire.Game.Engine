#include "LuaLoader.hpp"

namespace Config
{
    Loader::Loader()
    {  
        State = luaL_newstate();
        luaL_openlibs(State);
    }
    
    Loader::~Loader()
    {
        lua_close(State);
    }
}