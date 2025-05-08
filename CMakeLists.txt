cmake_minimum_required(VERSION 3.10)
project(SnakeGame)

# Thiết lập C++17
set(CMAKE_CXX_STANDARD 17)

# Tìm thư viện SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Thêm thư mục header
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    include/  # Thêm thư mục header của bạn
)

# Tạo thư mục assets nếu chưa có
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/fonts)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/textures)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/assets/sounds)

# Thêm tất cả file .cpp từ src/
file(GLOB SOURCES "src/*.cpp")

# Tạo file thực thi
add_executable(SnakeGame ${SOURCES})

# Liên kết thư viện SDL2
target_link_libraries(SnakeGame
    ${SDL2_LIBRARIES}
    ${SDL2_TTF_LIBRARIES}
    ${SDL2_IMAGE_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Copy assets vào thư mục build (tuỳ chọn)
add_custom_command(
    TARGET SnakeGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_BINARY_DIR}/assets
)
