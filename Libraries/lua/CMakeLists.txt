cmake_minimum_required(VERSION 3.16)

project(Lua
		LANGUAGES C
		VERSION 5.4.8
)

option(LUA_COMPILE_EXE ON)
option(LUA_STATIC ON)

file(GLOB LUA_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
file(GLOB LUA_HEADERS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

# Define executables
set(LUA_EXECUTABLE lua)
set(LUAC_EXECUTABLE luac)

# Separate main files
set(LUA_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/lua.c")
set(LUAC_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/luac.c")
list(REMOVE_ITEM LUA_SOURCES ${LUA_MAIN} ${LUAC_MAIN})

message(${LUA_SOURCES})
# Static and shared libraries
add_library(lua_static STATIC ${LUA_SOURCES} ${LUA_HEADERS})
add_library(lua_shared SHARED ${LUA_SOURCES} ${LUA_HEADERS})

target_compile_features(lua_static PRIVATE c_std_99)
target_compile_features(lua_shared PRIVATE c_std_99)

target_include_directories(lua_static PUBLIC src)
target_include_directories(lua_shared PUBLIC src)

if(LUA_STATIC)
	add_library(Lua ALIAS lua_static)
else()
	add_library(Lua ALIAS lua_shared)
endif()

# Executables
if(LUA_COMPILE_EXE)
	add_executable(${LUA_EXECUTABLE} ${LUA_MAIN})
	add_executable(${LUAC_EXECUTABLE} ${LUAC_MAIN})

	target_link_libraries(${LUA_EXECUTABLE} PRIVATE lua_static)
	target_link_libraries(${LUAC_EXECUTABLE} PRIVATE lua_static)
endif()