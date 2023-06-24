# This one is messy, it should be cleaned up eventually

file(MAKE_DIRECTORY ${GFS_BIN})

if(SSTV_ENABLED)
    message(STATUS "SSTV Enabled In Giraffe Flight System")
    add_compile_definitions(SSTV_ENABLED=1)
else()
    message(STATUS "SSTV Disabled In Giraffe Flight System")
    add_compile_definitions(SSTV_ENABLED=0)
endif()

set(EXE_NAME giraffe_flight_system)

# -------- GFS Executable ---------
add_executable( # see sources.cmake for source files
	${EXE_NAME}
		# main
		${GFS_SRC}/main.cpp
		${GFS_SRC}/flight_runner.cpp
	${gfs_sources}
)

target_include_directories(
	${EXE_NAME} PRIVATE
	${gfs_include_dirs}
)

# -------- Properties ---------
set_target_properties(
	${EXE_NAME} PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${GFS_BIN}
	OUTPUT_NAME gfs
)

# -------- First Party Libraries ---------
target_link_libraries(
	giraffe_flight_system 
	PRIVATE 
	BoosterSeat
	#MWAVlib
)

# -------- Third Party Libraries ---------
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(
	giraffe_flight_system PRIVATE 
	i2c
	Threads::Threads
	nlohmann_json::nlohmann_json
	${CURSES_LIBRARIES}
)