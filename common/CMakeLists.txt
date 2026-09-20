add_library(DQE_Common
	src/env_reader.cpp
	src/currency_manager.cpp
	src/thread_pool.cpp
)

target_compile_features(DQE_Common PRIVATE cxx_std_20)

target_include_directories(DQE_Common PUBLIC
	"include"
)

add_library(DQE::Common ALIAS DQE_Common)