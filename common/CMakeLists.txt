add_library(DQE_Common
	src/env_reader.cpp
	src/currency_manager.cpp
)

target_include_directories(DQE_Common PUBLIC
	"include"
)

add_library(DQE::Common ALIAS DQE_Common)