#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>  // Để ghi vào file
#include "boid.h"  // Bao gồm lớp Boid đã chỉnh sửa với hàm `update`

int main() {
    bool pause = true; // Biến kiểm soát trạng thái tạm dừng
    std::srand(std::time(nullptr)); // Khởi tạo hạt giống cho hàm rand

    int windowWidth = 1920; // Chiều rộng của cửa sổ
    int windowHeight = 1080; // Chiều cao của cửa sổ
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Boid Simulation"); // Tạo cửa sổ hiển thị

    // Khởi tạo danh sách các Boid
    std::vector<Boid> boids;
    for (int i = 0; i < 1024; i++) {
        Boid boid(sf::Vector2i(windowWidth, windowHeight)); // Tạo Boid mới với vị trí ngẫu nhiên
        boids.push_back(boid); // Thêm Boid vào danh sách
    }

    sf::Clock deltaClock; // Đồng hồ để theo dõi thời gian delta
    sf::Time dt = deltaClock.restart();

    // Tạo lớp phủ màu đen để làm nền trong suốt
    sf::RectangleShape blackOverlay;
    blackOverlay.setSize(sf::Vector2f(windowWidth, windowHeight));
    blackOverlay.setFillColor(sf::Color(0, 0, 0, 30));

    float fps = 0.0f; // Biến để lưu FPS
    int frameCount = 0; // Biến đếm số khung hình
    float elapsedTime = 0.0f; // Thời gian trôi qua trong mỗi giây
    float totalTime = 0.0f; // Tổng thời gian chương trình chạy

    // File để ghi FPS sau 40 giây
    std::ofstream fpsFile("C:/Users/HAI YEN/CLionProjects/final_project/boids/fps_result_naive.csv");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            pause = !pause; // Tạm dừng hoặc tiếp tục khi nhấn phím Space
        }

        window.clear(); // Xóa màn hình
        window.draw(blackOverlay); // Vẽ lớp phủ đen lên màn hình

        // Cập nhật và vẽ từng Boid
        for (auto& boid : boids) {
            if (!pause) {
                boid.update(window.getSize(), boids, dt);
            }
            boid.draw(window, 1); // Vẽ Boid lên cửa sổ
        }

        // Tính toán FPS trung bình mỗi giây
        frameCount++;
        elapsedTime += dt.asSeconds(); // Cộng dồn thời gian

        if (elapsedTime >= 1.0f) {
            fps = frameCount / elapsedTime; // FPS trung bình trong 1 giây

            if (totalTime < 40.0f) {
                std::cout << "FPS: " << fps << std::endl; // In FPS ra console trong 40 giây đầu
            } else {
                std::cout << "FPS: " << fps << std::endl; // In FPS ra console sau 40 giây
                fpsFile << fps << std::endl; // Ghi FPS vào file sau 40 giây
            }

            // Reset các biến sau mỗi giây
            elapsedTime = 0.0f;
            frameCount = 0;
        }

        dt = deltaClock.restart(); // Cập nhật thời gian delta
        totalTime += dt.asSeconds(); // Cập nhật tổng thời gian

        if (totalTime >= 120.0f) {
            break; // Kết thúc sau 120 giây
        }

        window.display(); // Hiển thị nội dung của cửa sổ
    }

    // Đóng file sau khi kết thúc
    fpsFile.close();

    return 0;
}
