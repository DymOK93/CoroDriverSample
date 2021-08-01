cmake_minimum_required (VERSION 3.8)
project ("CoroDriver")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(WDK REQUIRED)
find_package(KTL REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/sys")

set(
	BASIC_COMPILE_OPTIONS
		/MP	# Multiprocessor compilation
		/W4 # Warnings level
		/WX # Treat warnings as errors
)
set(
	RELEASE_COMPILE_OPTIONS
		/O2		# Max optimization level
		/Ob3	# Aggressive inlining
		/Oi     # Enable intrinsics
		/Ot     # Prefer fast code
		/GL
)
set(
	RELEASE_LINK_OPTIONS
		/OPT:REF # Eliminate unused code
		/LTCG
)

SET(TARGET_DRIVER Driver25)

wdk_add_driver(
	${TARGET_DRIVER}
		CUSTOM_ENTRY_POINT KtlDriverEntry
		EXTENDED_CPP_FEATURES
		WINVER 0x602

		"driver.h"
		"driver.cpp"
)
target_compile_options(
	${TARGET_DRIVER} PRIVATE
		${BASIC_COMPILE_OPTIONS}
		$<$<CONFIG:Release>:${RELEASE_COMPILE_OPTIONS}>
)
target_compile_definitions(
	${TARGET_DRIVER} PUBLIC
		KTL_VERSION
		KTL_CPP_DBG
		FMT_HEADER_ONLY
		FMT_EXCEPTIONS
		FMT_STATIC_THOUSANDS_SEPARATOR
		KTL_NO_CXX_STANDARD_LIBRARY 
		KTL_COROUTINES
		_CRT_SECURE_CPP_OVERLOAD_SECURE_NAMES=0  
)
target_link_options(
	${TARGET_DRIVER} PRIVATE
		$<$<CONFIG:Release>:${RELEASE_LINK_OPTIONS}>
)
target_include_directories(
	${TARGET_DRIVER} PUBLIC
		${KTL_INCLUDE_DIR}
		${KTL_MODULES_DIR}
		${KTL_RUNTIME_INCLUDE_DIR}
		${CMAKE_INCLUDE_CURRENT_DIR}
		${Boost_INCLUDE_DIR}
)

target_link_libraries(
	${TARGET_DRIVER}
		ktl::basic_runtime 
		ktl::cpp_runtime
)
wdk_sign_driver(
	${TARGET_DRIVER}
		${CERTIFICATE_NAME}
		CERTIFICATE_PATH ${CERTIFICATE_PATH}
		TIMESTAMP_SERVER http://timestamp.comodoca.com/authenticode
)
