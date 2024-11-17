#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <fstream> // Thêm thư viện để ghi vào file CSV

#include "quadtree.h"
#include "boid_quadtree.h"

int main() {
    bool pause = true; // Biến để kiểm soát trạng thái tạm dừng

    std::srand(std::time(nullptr)); // Khởi tạo hạt giống cho hàm rand

    // Tải bảng màu
    sf::Image tp;
    int windowWidth = 1920; // Chiều rộng của cửa sổ
    int windowHeight = 1080; // Chiều cao của cửa sổ

    std::vector<Boid> *boidsV = new std::vector<Boid>; // Khởi tạo vector chứa các boid
    for (int i = 0; i < 2048; i++) {
        Boid mt(sf::Vector2i(windowWidth, windowHeight)); // Tạo một boid mới với vị trí ngẫu nhiên
        boidsV->push_back(mt); // Thêm boid vào vector
    }

    sf::Clock deltaClock; // Để theo dõi thời gian
    sf::Time dt = deltaClock.restart(); // Khởi động đồng hồ và lưu thời gian delta

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Boids Simulation Using QuadTree"); // Tạo cửa sổ hiển thị
    const sf::Vector2i viewCenter = sf::Vector2i(70, 70); // Tọa độ trung tâm của cửa sổ
    window.setPosition(viewCenter); // Đặt vị trí cửa sổ
    window.clear(sf::Color::Black); // Xóa màn hình và đặt màu nền là đen

    // Tạo một lớp đen trong suốt để phủ lên màn hình
    sf::RectangleShape black;
    black.setSize(sf::Vector2f(windowWidth, windowHeight)); // Kích thước của lớp đen
    black.setFillColor(sf::Color(0, 0, 0, 30)); // Màu đen với độ trong suốt
    black.setPosition(sf::Vector2f(0, 0)); // Vị trí của lớp đen


    float fps = 0.0f; // Biến để lưu FPS
    int frameCount = 0; // Biến đếm số khung hình
    float elapsedTime = 0.0f; // Thời gian trôi qua trong mỗi giây
    float totalTime = 0.0f; // Tổng thời gian chương trình chạy

    // Mở file để ghi FPS vào CSV
    std::ofstream fpsFile("C:/Users/HAI YEN/CLionProjects/final_project/boids_quadtree/fps_result_quadtree.csv");
    fpsFile << "Time (s), FPS\n"; // Ghi tiêu đề cho các cột trong file CSV

    while (window.isOpen()) { // Vòng lặp chính của ứng dụng
        sf::Event event; // Biến để lưu trữ sự kiện
        rectByCenter screenRect; // Khung hiển thị
        screenRect.center = sf::Vector2f(windowWidth / 2, windowHeight / 2); // Tâm của khung
        screenRect.radius = sf::Vector2f(windowWidth / 2, windowHeight / 2); // Bán kính của khung
        QuadTree *bigQuad = new QuadTree(screenRect); // Tạo một QuadTree mới

        while (window.pollEvent(event)) { // Xử lý các sự kiện
            if (event.type == sf::Event::Closed) {
                window.close(); // Đóng cửa sổ nếu có sự kiện đóng
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            pause = false; // Bỏ tạm dừng khi nhấn phím Space
        }

        window.clear(); // Xóa màn hình
        window.draw(black); // Vẽ lớp đen lên cửa sổ

        for (int b = 0; b < boidsV->size(); b++) { // Chèn các boid vào QuadTree
            bigQuad->insert(&boidsV->at(b));
        }

        bigQuad->display(window); // Vẽ QuadTree (nếu cần)

        for (int b = 0; b < boidsV->size(); b++) {
            if (!pause) // Nếu không tạm dừng
                boidsV->at(b).update(window.getSize(), *bigQuad, dt); // Cập nhật vị trí boid
            boidsV->at(b).draw(window, 1); // Vẽ boid lên cửa sổ
        }

        // Tính toán FPS trung bình mỗi giây
        frameCount++;
        elapsedTime += dt.asSeconds(); // Cộng dồn thời gian

        if (elapsedTime >= 1.0f) {
            fps = frameCount / elapsedTime; // FPS trung bình trong 1 giây
            if (totalTime < 40.0f) {
                std::cout << "FPS: " << fps << std::endl; // In FPS ra console trong 40 giây đầu
            } else if (totalTime >= 40.0f && totalTime < 120.0f) {
                std::cout << "FPS: " << fps << std::endl; // In FPS ra console trong 80 giây tiếp theo
                fpsFile << totalTime << ", " << fps << "\n"; // Ghi FPS và thời gian vào file CSV
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

        window.display(); // Hiển thị cửa sổ
        bigQuad->del(); // Xóa QuadTree
        delete bigQuad; // Giải phóng bộ nhớ cho QuadTree
    }

    // Đóng file sau khi kết thúc
    fpsFile.close();

    return 0; // Kết thúc chương trình
}
