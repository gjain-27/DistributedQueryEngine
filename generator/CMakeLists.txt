find_package(libpqxx CONFIG REQUIRED)

add_executable(DataGenerator
	src/main.cpp
)

target_compile_features(DataGenerator PRIVATE cxx_std_20)

target_link_libraries(DataGenerator PRIVATE
	DQE::Common
	libpqxx::pqxx
)

target_include_directories(DataGenerator PRIVATE
	"include"
)