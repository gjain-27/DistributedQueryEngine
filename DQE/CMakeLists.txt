add_executable(DQE
	src/main.cpp
	src/linear_query_engine.cpp
	src/threaded_query_engine.cpp
	src/distributed_query_engine.cpp
)
	
target_compile_features(DQE PRIVATE cxx_std_20)

target_link_libraries(DQE PRIVATE
	DQE::Common
	libpqxx::pqxx
)

target_include_directories(DQE PRIVATE
	"include"
)

target_compile_definitions(DQE PRIVATE
    DQE_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)