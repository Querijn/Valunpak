cmake_minimum_required(VERSION 3.8)
project(valunpak)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

function(ADD_SRC folder inc src)
	source_group(${folder} FILES ${inc} ${src})
	set(VALUNPAK_SOURCES ${VALUNPAK_SOURCES} ${inc} ${src} PARENT_SCOPE)
endfunction(ADD_SRC)

# Local includes
include_directories("ext" "inc" "src")

ADD_SRC(Crypto\\AES\\Rijndael		"ext/rijndael/rijndael.hpp"				"ext/rijndael/rijndael.cpp")
ADD_SRC(Crypto\\SHA1				"inc/valunpak/sha1.hpp"					"src/sha1.cpp")
ADD_SRC(Crypto\\AES					"inc/valunpak/aes.hpp"					"src/aes.cpp")
ADD_SRC(UnrealFile					"inc/valunpak/ue4_bin_file.hpp"			"src/ue4_bin_file.cpp")
ADD_SRC(BinFile						"inc/valunpak/bin_file.hpp"				"src/bin_file.cpp")
ADD_SRC(SigFile						"inc/valunpak/sig_file.hpp"				"src/sig_file.cpp")
ADD_SRC(PakFile						"inc/valunpak/pak_file.hpp"				"src/pak_file.cpp")
ADD_SRC(App							"inc/valunpak/config.hpp"				"src/main.cpp")

add_executable(valunpak ${VALUNPAK_SOURCES})
set_target_properties(valunpak PROPERTIES 
					VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
					CXX_STANDARD 17
					CXX_EXTENSIONS OFF)
