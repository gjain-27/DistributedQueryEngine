find_package(libpqxx CONFIG REQUIRED)

add_executable(DataGenerator
	main.cpp
)

target_compile_features(DataGenerator PRIVATE cxx_std_20)

target_link_libraries(DataGenerator PRIVATE
	libpqxx::pqxx
)