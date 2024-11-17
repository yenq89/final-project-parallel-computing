cmake_minimum_required(VERSION 3.29)
project(boids_master)

set(CMAKE_CXX_STANDARD 17)

# Khai báo thư viện SFML
set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR "C:/Users/HAI YEN/SFML/SFML-2.6.1/lib/cmake/SFML")
set(CMAKE_PREFIX_PATH "C:/Users/HAI YEN/SFML/SFML-2.6.1")

# Tìm thư viện SFML
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

# Thêm thư mục chứa tệp header của SFML
include_directories("C:/Users/HAI YEN/SFML/SFML-2.6.1/include")



# Đường dẫn thủ công cho các file MPI
include_directories("C:/Program Files (x86)/Microsoft MPI/Include")
link_directories("C:/Program Files (x86)/Microsoft MPI/Lib/x64")

# Thêm executable
add_executable(test test/test.cpp)  # Đổi `main.cpp` thành tên file nguồn chính của bạn

# Liên kết với thư viện msmpi.lib của MPI
target_link_libraries(test "msmpi.lib")

add_executable(boids_quadtree
        boids_quadtree/boid_quadtree.cpp
        boids_quadtree/boid_quadtree.h
        boids_quadtree/Main.cpp
        boids_quadtree/quadtree.cpp
        boids_quadtree/quadtree.h
)
add_executable(boids
        boids/Boid.cpp
        boids/Boid.h
        boids/Main.cpp
)

add_executable(boids_parallel
        boids_parallel/Boid.cpp
        boids_parallel/Boid.h
        boids_parallel/Main.cpp
)

add_executable(boids_quadtree_parallel
        boids_quadtree_parallel/boid_quadtree.cpp
        boids_quadtree_parallel/boid_quadtree.h
        boids_quadtree_parallel/quadtree.cpp
        boids_quadtree_parallel/quadtree.h
        boids_quadtree_parallel/Main.cpp
)
# Liên kết với thư viện msmpi.lib của MPI

target_link_libraries(boids_parallel "msmpi.lib")

target_link_libraries(boids_quadtree_parallel "msmpi.lib")

target_link_libraries(boids_quadtree sfml-system sfml-window sfml-graphics)

target_link_libraries(boids sfml-system sfml-window sfml-graphics)

target_link_libraries(boids_parallel sfml-system sfml-window sfml-graphics)

target_link_libraries(boids_quadtree_parallel sfml-system sfml-window sfml-graphics)

# Thêm cờ -fopenmp vào khi biên dịch
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

# Thêm cờ -fopenmp vào khi liên kết
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fopenmp")

