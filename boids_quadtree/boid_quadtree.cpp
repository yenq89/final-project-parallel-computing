#pragma once

#ifndef iosteam
#include <iostream>
#include <string.h>
#include <cmath>
#include <vector>
#include <SFML/Graphics.hpp>

#include "boid_quadtree.h"
#include "quadtree.h"
#endif

// Hàm tính khoảng cách giữa hai điểm
float dist(sf::Vector2f p1, sf::Vector2f p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

// Hàm chuẩn hóa vecto
sf::Vector2f normalize(sf::Vector2f vect) {
    float d = dist(sf::Vector2f(0, 0), vect) + 0.0000001; // Tránh chia cho 0
    return sf::Vector2f(vect.x / d, vect.y / d);
}

// Hàm vẽ boid lên cửa sổ
void Boid::draw(sf::RenderWindow& window, float zoom) {
    sf::CircleShape c;
    c.setFillColor(color); // Thiết lập màu cho boid
    c.setPosition(sf::Vector2f(position.x - size, position.y - size)); // Vị trí vẽ
    c.setRadius(this->size); // Kích thước boid
    window.draw(c); // Vẽ boid lên cửa sổ
}

// Hàm tính toán lực định hướng (alignment) cho boid
sf::Vector2f Boid::alignment(QuadTree quad) {
    std::vector<Boid> nearPoints; // Danh sách các boid lân cận
    rectByCenter range; // Khung tìm kiếm
    range.center = position; // Tâm của khung
    range.radius = sf::Vector2f(this->size * 20, this->size * 20); // Bán kính tìm kiếm
    quad.queryRangeCircle(range, nearPoints); // Tìm các boid trong khung

    sf::Vector2f steering = sf::Vector2f(0, 0); // Vector điều chỉnh

    if (nearPoints.size() > 0) {
        for (int p = 0; p < nearPoints.size(); p++) {
            steering.x += nearPoints[p].velocity.x; // Cộng vận tốc x
            steering.y += nearPoints[p].velocity.y; // Cộng vận tốc y
        }
        steering.x /= nearPoints.size(); // Tính trung bình
        steering.y /= nearPoints.size(); // Tính trung bình

        // Chuẩn hóa vector
        steering = normalize(steering);

        steering.x *= speed; // Điều chỉnh theo tốc độ
        steering.y *= speed; // Điều chỉnh theo tốc độ

        steering.x -= velocity.x; // Tính lực điều chỉnh
        steering.y -= velocity.y; // Tính lực điều chỉnh
    }
    return steering; // Trả về vector điều chỉnh
}

// Hàm tính toán lực gắn kết (cohesion) cho boid
sf::Vector2f Boid::cohesion(QuadTree quad) {
    std::vector<Boid> nearPoints; // Danh sách các boid lân cận
    rectByCenter range; // Khung tìm kiếm
    range.center = position; // Tâm của khung
    range.radius = sf::Vector2f(this->size * 8.5, this->size * 8.5); // Bán kính tìm kiếm
    quad.queryRangeCircle(range, nearPoints); // Tìm các boid trong khung

    sf::Vector2f avgPoint; // Điểm trung bình
    sf::Vector2f steering = sf::Vector2f(0, 0); // Vector điều chỉnh
    avgPoint.x = 0;
    avgPoint.y = 0;
    if (nearPoints.size() > 0) {
        for (int p = 0; p < nearPoints.size(); p++) {
            avgPoint.x += nearPoints[p].position.x; // Cộng tọa độ x
            avgPoint.y += nearPoints[p].position.y; // Cộng tọa độ y
        }
        avgPoint.x /= nearPoints.size(); // Tính trung bình tọa độ x
        avgPoint.y /= nearPoints.size(); // Tính trung bình tọa độ y

        // Chuẩn hóa vector
        steering = avgPoint;

        steering.x -= position.x; // Tính vector hướng tới điểm trung bình
        steering.y -= position.y; // Tính vector hướng tới điểm trung bình

        steering = normalize(steering); // Chuẩn hóa vector

        steering.x *= speed; // Điều chỉnh theo tốc độ
        steering.y *= speed; // Điều chỉnh theo tốc độ

        steering.x -= velocity.x; // Tính lực điều chỉnh
        steering.y -= velocity.y; // Tính lực điều chỉnh
    }
    return steering; // Trả về vector điều chỉnh
}

// Hàm tính toán lực phân tán (separation) cho boid
sf::Vector2f Boid::separation(QuadTree quad) {
    std::vector<Boid> nearPoints; // Danh sách các boid lân cận
    rectByCenter range; // Khung tìm kiếm
    range.center = position; // Tâm của khung
    range.radius = sf::Vector2f(this->size * 4, this->size * 4); // Bán kính tìm kiếm
    quad.queryRangeCircle(range, nearPoints); // Tìm các boid trong khung

    sf::Vector2f diff; // Vector phân tán
    sf::Vector2f steering = sf::Vector2f(0, 0); // Vector điều chỉnh

    if (nearPoints.size() > 0) {
        for (int p = 0; p < nearPoints.size(); p++) {
            diff.x = position.x - nearPoints[p].position.x; // Tính vector phân tán
            diff.y = position.y - nearPoints[p].position.y; // Tính vector phân tán

            // Tính khoảng cách bình phương
            float d = (nearPoints[p].position.x - position.x) * (nearPoints[p].position.x - position.x) +
                      (nearPoints[p].position.y - position.y) * (nearPoints[p].position.y - position.y);
            if (d < 1){
                diff.x *= 1 / (d + 0.0001); // Tránh va chạm
                diff.y *= 1 / (d + 0.0001); // Tránh va chạm
            }

            steering.x += diff.x; // Cộng vector phân tán vào lực điều chỉnh
            steering.y += diff.y; // Cộng vector phân tán vào lực điều chỉnh
        }
        steering.x /= nearPoints.size(); // Tính trung bình
        steering.y /= nearPoints.size(); // Tính trung bình

        steering = normalize(steering); // Chuẩn hóa vector
        steering.x *= speed; // Điều chỉnh theo tốc độ
        steering.y *= speed; // Điều chỉnh theo tốc độ

        steering.x -= velocity.x; // Tính lực điều chỉnh
        steering.y -= velocity.y; // Tính lực điều chỉnh
    }
    return steering; // Trả về vector điều chỉnh
}

// Hàm cập nhật vị trí của boid
void Boid::update(sf::Vector2u screenSize, QuadTree quad, sf::Time dt) {
    // Khởi tạo gia tốc
    acceleration.x = 0;
    acceleration.y = 0;

    // Phản xạ khi va chạm với tường
    if (position.x < 0) {
        acceleration.x = 20; // Đẩy ra
    }
    else if (position.x > screenSize.x) {
        acceleration.x = -20; // Đẩy ra
    }
    if (position.y < 0) {
        acceleration.y = 20; // Đẩy ra
    }
    else if (position.y > screenSize.y) {
        acceleration.y = -20; // Đẩy ra
    }

    // Tính toán các lực (alignment, cohesion, separation) và cập nhật
    sf::Vector2f a;
    a = alignment(quad);
    acceleration.x += a.x * 0.5; // Cộng lực điều chỉnh
    acceleration.y += a.y * 0.5; // Cộng lực điều chỉnh
    a = cohesion(quad);
    acceleration.x += a.x * 0.05; // Cộng lực điều chỉnh
    acceleration.y += a.y * 0.05; // Cộng lực điều chỉnh
    a = separation(quad);
    acceleration.x += a.x * 1; // Cộng lực điều chỉnh
    acceleration.y += a.y * 1; // Cộng lực điều chỉnh

    // Cập nhật vận tốc và vị trí
    velocity.x += acceleration.x / 50;
    velocity.y += acceleration.y / 50;
    velocity = normalize(velocity); // Chuẩn hóa vận tốc

    position.x += velocity.x * speed * dt.asMilliseconds() / 1000; // Cập nhật vị trí x
    position.y += velocity.y * speed * dt.asMilliseconds() / 1000; // Cập nhật vị trí y

}