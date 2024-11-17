#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Boid.h"

// Hàm tính khoảng cách giữa hai điểm
float dist(sf::Vector2f p1, sf::Vector2f p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

sf::Vector2f normalize(sf::Vector2f vect) {
    float length = sqrt(vect.x * vect.x + vect.y * vect.y); // Tính độ dài vector
    if (length > 0.0000001f) {
        // Kiểm tra nếu độ dài không bằng 0 (để tránh chia cho 0)
        return sf::Vector2f(vect.x / length, vect.y / length);
    } else {
        return sf::Vector2f(0, 0); // Nếu độ dài là 0, trả về vector không đổi
    }
}


// Hàm vẽ boid lên cửa sổ
void Boid::draw(sf::RenderWindow &window, float zoom) {
    sf::CircleShape c;
    c.setFillColor(color); // Thiết lập màu cho boid
    c.setPosition(sf::Vector2f(position.x - size, position.y - size)); // Vị trí vẽ
    c.setRadius(this->size); // Kích thước boid
    window.draw(c); // Vẽ boid lên cửa sổ
}

std::vector<Boid> Boid::findNearPoints(const std::vector<Boid> &others, float radius) {
    std::vector<Boid> nearPoints; // Danh sách các boid lân cận

    for (const auto &other: others) {
        float distanceSquared = (position.x - other.position.x) * (position.x - other.position.x) +
                                (position.y - other.position.y) * (position.y - other.position.y);

        if (distanceSquared < radius * radius) {
            nearPoints.push_back(other);
        }
    }
    return nearPoints;
}

// Hàm tính toán lực định hướng (alignment) cho boid
sf::Vector2f Boid::alignment(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 20);

    sf::Vector2f steering(0, 0); // Vector điều chỉnh

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
sf::Vector2f Boid::cohesion(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 8.5);
    sf::Vector2f steering = sf::Vector2f(0, 0); // Vector điều chỉnh
    if (nearPoints.size() > 0) {
        for (int p = 0; p < nearPoints.size(); p++) {
            steering.x += nearPoints[p].position.x; // Cộng tọa độ x
            steering.y += nearPoints[p].position.y; // Cộng tọa độ y
        }
        steering.x /= nearPoints.size(); // Tính trung bình tọa độ x
        steering.y /= nearPoints.size(); // Tính trung bình tọa độ y

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

sf::Vector2f Boid::separation(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 4.0);

    sf::Vector2f steering = sf::Vector2f(0, 0); // Vector điều chỉnh

    if (nearPoints.size() > 0) {
        for (int p = 0; p < nearPoints.size(); p++) {
            sf::Vector2f diff;
            diff.x = position.x - nearPoints[p].position.x;
            diff.y = position.y - nearPoints[p].position.y;
            float d = (nearPoints[p].position.x - position.x) * (nearPoints[p].position.x - position.x) +
                      (nearPoints[p].position.y - position.y) * (nearPoints[p].position.y - position.y);
            if (d < 1) {
                diff.x *= 1 / (d + 0.0001);
                diff.y *= 1 / (d + 0.0001);
            }
            steering.x += diff.x;
            steering.y += diff.y;
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
void Boid::update(sf::Vector2u screenSize, const std::vector<Boid> &others, sf::Time dt) {
    // Khởi tạo gia tốc
    acceleration.x = 0;
    acceleration.y = 0;

    // Phản xạ khi va chạm với tường
    if (position.x < 0) {
        acceleration.x = 20; // Đẩy ra
    } else if (position.x > screenSize.x) {
        acceleration.x = -20; // Đẩy ra
    }
    if (position.y < 0) {
        acceleration.y = 20; // Đẩy ra
    } else if (position.y > screenSize.y) {
        acceleration.y = -20; // Đẩy ra
    }

    // Tính toán các lực (alignment, cohesion, separation) và cập nhật
    sf::Vector2f a;
    a = alignment(others);
    acceleration.x += a.x * 0.5; // Cộng lực điều chỉnh
    acceleration.y += a.y * 0.5; // Cộng lực điều chỉnh
    a = cohesion(others);
    acceleration.x += a.x * 0.05; // Cộng lực điều chỉnh
    acceleration.y += a.y * 0.05; // Cộng lực điều chỉnh
    a = separation(others);
    acceleration.x += a.x * 1; // Cộng lực điều chỉnh
    acceleration.y += a.y * 1; // Cộng lực điều chỉnh

    // Giảm ảnh hưởng của gia tốc lên vận tốc, sao cho tốc độ thay đổi 1 cách từ từ và mượt mà
    velocity.x += acceleration.x / 50;
    velocity.y += acceleration.y / 50;
    velocity = normalize(velocity); // Chuẩn hóa vận tốc

    position.x += velocity.x * speed * dt.asMilliseconds() / 1000; // Cập nhật vị trí x
    position.y += velocity.y * speed * dt.asMilliseconds() / 1000; // Cập nhật vị trí y
}
