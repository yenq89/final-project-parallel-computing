#include <vector>
#include <SFML/Graphics.hpp>
#include <cmath>
#include "Boid.h"
#include <omp.h>
#include <openacc.h>  // Thêm OpenACC

Boid::Boid() : position(0, 0), velocity(0, 0), acceleration(0, 0) {
}

// Hàm tính khoảng cách giữa hai điểm
float dist(sf::Vector2f p1, sf::Vector2f p2) {
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

// Hàm chuẩn hóa vector
sf::Vector2f normalize(sf::Vector2f vect) {
    float d = dist(sf::Vector2f(0, 0), vect) + 0.0000001;
    return sf::Vector2f(vect.x / d, vect.y / d);
}

// Hàm vẽ boid lên cửa sổ
void Boid::draw(sf::RenderWindow &window, float zoom) {
    sf::CircleShape c;
    c.setFillColor(color);
    c.setPosition(sf::Vector2f(position.x - size, position.y - size));
    c.setRadius(this->size);
    window.draw(c);
}
std::vector<Boid> Boid::findNearPoints(const std::vector<Boid> &others, float radius) {
    std::vector<Boid> nearPoints;

#pragma omp parallel
    {
        std::vector<Boid> localNearPoints;
#pragma omp parallel for schedule(auto)
        for (int i = 0; i < others.size(); i++) {
            const auto &other = others[i];
            float distanceSquared = (position.x - other.position.x) * (position.x - other.position.x) +
                                    (position.y - other.position.y) * (position.y - other.position.y);
            if (distanceSquared < radius * radius) {
                localNearPoints.push_back(other);
            }
        }
#pragma omp critical
        nearPoints.insert(nearPoints.end(), localNearPoints.begin(), localNearPoints.end());
    }
    return nearPoints;
}

sf::Vector2f Boid::alignment(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 20);
    sf::Vector2f steering(0, 0);

    if (nearPoints.size() > 0) {
#pragma acc parallel loop reduction(+:steering) copyin(nearPoints[:nearPoints.size()])
        for (int p = 0; p < nearPoints.size(); p++) {
            steering.x += nearPoints[p].velocity.x;
            steering.y += nearPoints[p].velocity.y;
        }

        steering.x /= nearPoints.size();
        steering.y /= nearPoints.size();
        steering = normalize(steering);
        steering.x *= speed;
        steering.y *= speed;
        steering.x -= velocity.x;
        steering.y -= velocity.y;
    }
    return steering;
}

sf::Vector2f Boid::cohesion(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 8.5);
    sf::Vector2f steering(0, 0);

    if (nearPoints.size() > 0) {
#pragma acc parallel loop reduction(+:steering) copyin(nearPoints[:nearPoints.size()])
        for (int p = 0; p < nearPoints.size(); p++) {
            steering.x += nearPoints[p].position.x;
            steering.y += nearPoints[p].position.y;
        }

        steering.x /= nearPoints.size();
        steering.y /= nearPoints.size();
        steering.x -= position.x;
        steering.y -= position.y;
        steering = normalize(steering);
        steering.x *= speed;
        steering.y *= speed;
        steering.x -= velocity.x;
        steering.y -= velocity.y;
    }
    return steering;
}

sf::Vector2f Boid::separation(const std::vector<Boid> &others) {
    std::vector<Boid> nearPoints = findNearPoints(others, this->size * 4.0);
    sf::Vector2f steering(0, 0);

    if (nearPoints.size() > 0) {
#pragma acc parallel loop reduction(+:steering) copyin(nearPoints[:nearPoints.size()])
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
    return steering;
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
    sf::Vector2f a;
    a = alignment(others);
    acceleration.x += a.x * 0.5;
    acceleration.y += a.y * 0.5;
    a = cohesion(others);
    acceleration.x += a.x * 0.05;
    acceleration.y += a.y * 0.05;
    a = separation(others);
    acceleration.x += a.x * 1;
    acceleration.y += a.y * 1;

    velocity.x += acceleration.x / 50;
    velocity.y += acceleration.y / 50;
    velocity = normalize(velocity);

    position.x += velocity.x * speed * dt.asMilliseconds() / 1000;
    position.y += velocity.y * speed * dt.asMilliseconds() / 1000;
}
