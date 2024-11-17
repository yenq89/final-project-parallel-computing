//
// Created by HAI YEN on 10/27/2024.
//
#pragma once
#include <SFML/Graphics.hpp> // Thư viện để sử dụng các chức năng đồ họa của SFML
#include <random> // Thư viện để sử dụng các hàm sinh số ngẫu nhiên
#include <ctime> // Thư viện để làm việc với thời gian

#ifndef BOID_H
#define BOID_H

class Boid {
public:
    sf::Vector2f position; // Vị trí của boid
    sf::Vector2f velocity; // Vận tốc của boid
    sf::Vector2f acceleration; // Gia tốc của boid
    sf::Color color; // Màu sắc của boid

    float speed; // Tốc độ của boid
    float size = 3; // Kích thước của boid

    // Hàm khởi tạo cho Boid
    explicit Boid(sf::Vector2i borders) {
        // Khởi tạo vị trí ngẫu nhiên trong giới hạn
        position = sf::Vector2f(rand() % borders.x, rand() % borders.y);
        // Khởi tạo vecto vận tốc ngẫu nhiên
        velocity = sf::Vector2f(random(15,-2), random(15,-2));
        // Khởi tạo vecto gia tốc ngẫu nhiên
        acceleration = sf::Vector2f(random(100,50), random(100,50));
        // Khởi tạo màu sắc ngẫu nhiên dựa trên vị trí
        color = sf::Color(0,225,0);
        speed = 100.0f; // Tốc độ đặt sẵn
    }


    static float random(int b,int c) {
        while (true) {
            float a = rand() % b+c;
            if(a != 0) {
                return a;
            }
        }
    }
    void draw(sf::RenderWindow& window, float zoom); // Vẽ boid lên cửa sổ
    std::vector<Boid> findNearPoints(const std::vector<Boid> &others, float radius);
    void update(sf::Vector2u screenSize, const std::vector<Boid>& others , sf::Time dt);
    // Các phương thức điều chỉnh hành vi của boid
    sf::Vector2f cohesion(const std::vector<Boid>& others);
    sf::Vector2f alignment(const std::vector<Boid>& others);
    sf::Vector2f separation(const std::vector<Boid>& others);
};
#endif //BOID_H
