# backend/api-service/CMakeLists.txt
add_executable(ApiService src/main.cpp)

target_link_libraries(ApiService PRIVATE 
    Common 
    Drogon::Drogon
)