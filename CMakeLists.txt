project(ss-cli)

add_executable(ss-cli databasemanager.cpp database.cpp main.cpp)

target_link_libraries(ss-cli cryptopp)
