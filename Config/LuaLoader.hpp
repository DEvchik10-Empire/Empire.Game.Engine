#pragma once

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

namespace Config
{
    class Loader
    {
        public:
            Loader();
            ~Loader();

            Loader(const Loader&) = delete;
            Loader& operator=(const Loader&) = delete; 

        private:
            lua_State* State;
    };
}