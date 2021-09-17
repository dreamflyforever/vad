
cmake_minimum_required(VERSION 3.10.2)




include_directories(include)

add_library(
	vad
	SHARED
	native-lib.cpp
	wb_vad.c
	vad_api.c
	)

target_link_libraries(
                      vad 
                       log
		       )
