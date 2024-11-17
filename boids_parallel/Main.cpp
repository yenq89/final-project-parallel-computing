#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <fstream>
#include <omp.h>

#include "boid.h" // Bao gồm lớp Boid đã chỉnh sửa với hàm `update`

int main() {
    int total_boids = 1024;  // Tổng số boid trong mô phỏng

    omp_set_num_threads(16); // Thiết lập số luồng OpenMP

    bool pause = true;
    std::srand(std::time(nullptr)); // Khởi tạo seed ngẫu nhiên

    int windowWidth = 1920;
    int windowHeight = 1080;

    // Tạo cửa sổ vẽ
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Boid Simulation With OpenMP");

    // Khởi tạo danh sách boids
    std::vector<Boid> boids(total_boids);
    for (int i = 0; i < total_boids; i++) {
        boids[i] = Boid(sf::Vector2i(windowWidth, windowHeight));
    }

    sf::Clock deltaClock;
    sf::Time dt = deltaClock.restart();
    float fps = 0.0f;
    int frameCount = 0;
    float elapsedTime = 0.0f;

    // Ghi FPS vào file
    std::ofstream file;
    file.open("C:/Users/HAI YEN/CLionProjects/final_project/boids_parallel/fps_result.csv");
    if (!file.is_open()) {
        std::cerr << "Can not open file" << std::endl;
        return 1;
    }

    sf::RectangleShape blackOverlay;
    blackOverlay.setSize(sf::Vector2f(windowWidth, windowHeight));
    blackOverlay.setFillColor(sf::Color(0, 0, 0, 30));

    // Khởi tạo đồng hồ cho thời gian chạy tổng thể
    sf::Clock runClock; // Đồng hồ chạy mô phỏng tổng thể

    // Vòng lặp chính
    while (window.isOpen()) {
        // Tính toán thời gian đã chạy
        float totalTime = runClock.getElapsedTime().asSeconds();
        if (totalTime >= 120.0f) { // Kết thúc sau 120 giây
            break;
        }

        // Xử lý sự kiện
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Dừng hoặc tiếp tục khi nhấn phím Space
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            pause = !pause;
        }

        // Cập nhật vị trí của boids (song song hóa với OpenMP)
#pragma omp parallel for schedule(auto)
        for (int i = 0; i < boids.size(); i++) {
            if (!pause) {
                boids[i].update(sf::Vector2u(windowWidth, windowHeight), boids, dt);
            }
        }

        // Vẽ tất cả boids lên cửa sổ
        window.clear();
        window.draw(blackOverlay);
        for (auto& boid : boids) {
            boid.draw(window, 1);
        }
        dt = deltaClock.restart();
        window.display();

        // Tính toán FPS và ghi vào file hoặc console
        frameCount++;
        elapsedTime += dt.asSeconds();
        if (elapsedTime >= 1.0f) {
            fps = frameCount / elapsedTime;

            // In FPS ra console
            std::cout << "FPS: " << fps << std::endl;

            // Ghi FPS vào file nếu đã qua 40 giây
            if (totalTime > 40.0f) {
                file << fps << "\n"; // Ghi FPS vào file CSV
            }

            // Reset các biến để tính FPS cho giây tiếp theo
            elapsedTime = 0.0f;
            frameCount = 0;
        }
    }

    // Đóng file ghi FPS
    if (runClock.getElapsedTime().asSeconds() > 40.0f) {
        file << "\n"; // Dòng mới sau khi kết thúc
        file.close();
    }

    return 0;
}
