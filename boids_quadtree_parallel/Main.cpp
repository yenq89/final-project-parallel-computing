#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <chrono>
#include <omp.h>
#include <fstream> // Thêm thư viện để làm việc với file

#include "quadtree.h"
#include "boid_quadtree.h"

int main() {
    int total_boids = 512;
    omp_set_num_threads(16); // Thiết lập số luồng OpenMP

    bool pause = true; // Biến để kiểm soát trạng thái tạm dừng

    std::srand(std::time(nullptr)); // Khởi tạo hạt giống cho hàm rand

    int windowWidth = 1920; // Chiều rộng của cửa sổ
    int windowHeight = 1080; // Chiều cao của cửa sổ

    // Khởi tạo danh sách boids
    std::vector<Boid> boids(total_boids); // Sử dụng vector có kích thước cố định cho 256 boid
    for (int i = 0; i < total_boids; i++) {
        boids[i] = Boid(sf::Vector2i(windowWidth, windowHeight)); // Tạo một boid mới với vị trí ngẫu nhiên
    }

    sf::Clock deltaClock; // Đồng hồ thời gian
    sf::Time dt = deltaClock.restart(); // Khởi động đồng hồ và lưu thời gian delta

    float fps = 0.0f; // Biến để lưu FPS
    int frameCount = 0; // Biến đếm số khung hình
    float elapsedTime = 0.0f; // Thời gian trôi qua
    float totalTime = 0.0f; // Thời gian tổng cộng đã trôi qua
    bool switchToFile = false; // Cờ kiểm tra khi nào bắt đầu ghi vào file

    // Mở file để ghi FPS sau 40 giây
    std::ofstream file;
    file.open("C:/Users/HAI YEN/CLionProjects/final_project/boids_quadtree_parallel/fps_result.csv");
    if (!file.is_open()) {
        std::cerr << "Không thể mở file để ghi!" << std::endl;
        return 1;
    }

    // Lớp đen phủ lên cửa sổ
    sf::RectangleShape blackOverlay;
    blackOverlay.setSize(sf::Vector2f(windowWidth, windowHeight));
    blackOverlay.setFillColor(sf::Color(0, 0, 0, 30));

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Boids Simulation Using QuadTree");
    // Tạo cửa sổ hiển thị
    const sf::Vector2i viewCenter = sf::Vector2i(70, 70); // Tọa độ trung tâm của cửa sổ
    window.setPosition(viewCenter); // Đặt vị trí cửa sổ
    window.clear(sf::Color::Black); // Xóa màn hình và đặt màu nền là đen

    while (window.isOpen()) {
        // Vòng lặp chính của ứng dụng
        sf::Event event; // Biến để lưu trữ sự kiện
        rectByCenter screenRect; // Khung hiển thị
        screenRect.center = sf::Vector2f(windowWidth / 2, windowHeight / 2); // Tâm của khung
        screenRect.radius = sf::Vector2f(windowWidth / 2, windowHeight / 2); // Bán kính của khung
        QuadTree *bigQuad = new QuadTree(screenRect); // Tạo một QuadTree mới

        while (window.pollEvent(event)) {
            // Xử lý các sự kiện
            if (event.type == sf::Event::Closed) {
                window.close(); // Đóng cửa sổ nếu có sự kiện đóng
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
            pause = false; // Bỏ tạm dừng khi nhấn phím Space
        }

        window.clear(); // Xóa màn hình
        window.draw(blackOverlay); // Vẽ lớp đen lên cửa sổ

        for (int b = 0; b < boids.size(); b++) {
            // Chèn các boid vào QuadTree
            bigQuad->insert(&boids.at(b));
        }
        bigQuad->display(window); // Vẽ QuadTree (nếu cần)

#pragma omp parallel for
        for (int b = 0; b < boids.size(); b++) {
            if (!pause) {
                // Nếu không tạm dừng
                boids.at(b).update(window.getSize(), *bigQuad, dt); // Cập nhật vị trí Boid
            }
        }

        // Lệnh vẽ vẫn sẽ được thực hiện trong luồng chính
        for (int b = 0; b < boids.size(); b++) {
            boids.at(b).draw(window, 1); // Vẽ Boid lên cửa sổ
        }


        // Tính toán FPS trung bình mỗi giây
        frameCount++;
        elapsedTime += dt.asSeconds(); // Cộng dồn thời gian
        totalTime += dt.asSeconds(); // Cộng dồn thời gian tổng cộng

        if (elapsedTime >= 1.0f) {
            fps = frameCount / elapsedTime; // FPS trung bình trong 1 giây

            // In FPS ra console trong 40 giây đầu
            if (totalTime <= 40.0f) {
                std::cout << "FPS: " << fps << std::endl;
            } else {
                // Sau 40 giây, in FPS ra console và ghi vào file
                std::cout << "FPS: " << fps << std::endl;
                if (!switchToFile) {
                    // Sau 40 giây, bắt đầu ghi vào file CSV
                    file << "FPS\n"; // Tiêu đề cột
                    switchToFile = true;
                }
                // Ghi FPS vào file
                file << fps << "\n";
            }

            // Reset các biến sau mỗi giây
            elapsedTime = 0.0f;
            frameCount = 0;
        }

        // Dừng khi đạt 120 giây
        if (totalTime >= 120.0f) {
            break;
        }

        dt = deltaClock.restart(); // Cập nhật thời gian delta
        window.display(); // Hiển thị cửa sổ
        bigQuad->del(); // Xóa QuadTree
        delete bigQuad; // Giải phóng bộ nhớ cho QuadTree
    }

    // Đóng file ghi FPS
    file.close();

    return 0; // Kết thúc chương trình
}
