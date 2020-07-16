cmake_minimum_required(VERSION 3.8)
project(valunpak)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

function(ADD_SRC folder inc src)
	source_group(${folder} FILES ${inc} ${src})
	set(VALUNPAK_SOURCES ${VALUNPAK_SOURCES} ${inc} ${src} PARENT_SCOPE)
endfunction(ADD_SRC)

# Local includes
include_directories("ext" "ext/s3tc" "inc" "src")

# Crypto files
ADD_SRC(Crypto\\AES\\Rijndael		"ext/rijndael/rijndael.hpp"				"ext/rijndael/rijndael.cpp")
ADD_SRC(Crypto\\SHA1				"inc/valunpak/sha1.hpp"					"src/sha1.cpp")
ADD_SRC(Crypto\\AES					"inc/valunpak/aes.hpp"					"src/aes.cpp")
ADD_SRC(Crypto\\FNV1				"inc/valunpak/fnv1.hpp"					"src/fnv1.cpp")

# Utility 
ADD_SRC(Utility\\Json				"inc/valunpak/json_writer.hpp"			"src/json_writer.cpp")
ADD_SRC(Utility\\VectorGetter		"inc/valunpak/vector_getter.hpp"		"")
ADD_SRC(Utility\\DXT				"ext/s3tc/s3tc.h"						"ext/s3tc/s3tc.cpp")

# Unreal files
ADD_SRC(Unreal\\Bin					"inc/valunpak/ue4_bin_file.hpp"			"src/ue4_bin_file.cpp")
ADD_SRC(Unreal\\Base				"inc/valunpak/ue4_base.hpp"				"src/ue4_base.cpp")
ADD_SRC(Unreal\\Uasset				"inc/valunpak/ue4_uasset.hpp"			"src/ue4_uasset.cpp")
ADD_SRC(Unreal\\Uobject				"inc/valunpak/ue4_uobject.hpp"			"src/ue4_uobject.cpp")
ADD_SRC(Unreal\\UTexture2D			"inc/valunpak/ue4_utexture2d.hpp"		"src/ue4_utexture2d.cpp")
ADD_SRC(Unreal\\UExp				"inc/valunpak/ue4_uexp.hpp"				"src/ue4_uexp.cpp")
ADD_SRC(Unreal\\BulkData			"inc/valunpak/ue4_bulkdata.hpp"			"src/ue4_bulkdata.cpp")

# High level UE files
ADD_SRC(BinFile						"inc/valunpak/bin_file.hpp"				"src/bin_file.cpp")
ADD_SRC(SigFile						"inc/valunpak/sig_file.hpp"				"src/sig_file.cpp")
ADD_SRC(Pak\\File					"inc/valunpak/pak_file.hpp"				"src/pak_file.cpp")
ADD_SRC(Pak\\FS						"inc/valunpak/pak_filesystem.hpp"		"src/pak_filesystem.cpp")

ADD_SRC(App							"inc/valunpak/config.hpp"				"src/main.cpp")

add_executable(valunpak ${VALUNPAK_SOURCES})
set_target_properties(valunpak PROPERTIES 
					VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
					CXX_STANDARD 17
					CXX_EXTENSIONS OFF)
