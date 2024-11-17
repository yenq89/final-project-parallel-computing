#include "quadtree.h"
#include <omp.h>

// Hàm tính khoảng cách bình phương giữa hai điểm
float distSqrd(sf::Vector2f p1, sf::Vector2f p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
}

// Hàm kiểm tra va chạm giữa hai hình chữ nhật
bool collideRectAndRect(rectByCenter rect1, rectByCenter rect2) {
    // Tính toán các điểm trên bên trái và dưới bên phải của hai hình chữ nhật
    float tlx1 = rect1.center.x - rect1.radius.x;
    float tly1 = rect1.center.y - rect1.radius.y;
    float brx1 = rect1.center.x + rect1.radius.x;
    float bry1 = rect1.center.y + rect1.radius.y;

    float tlx2 = rect2.center.x - rect2.radius.x;
    float tly2 = rect2.center.y - rect2.radius.y;
    float brx2 = rect2.center.x + rect2.radius.x;
    float bry2 = rect2.center.y + rect2.radius.y;

    // Kiểm tra va chạm
    return (tlx1 <= brx2) && (brx1 >= tlx2) && (tly1 <= bry2) && (bry1 >= tly2);
}

// Hàm giải phóng bộ nhớ cho QuadTree
void QuadTree::del() {
    // Giải phóng vùng con đệ quy
    if (northWest != nullptr) {
        northWest->del();
        northEast->del();
        southEast->del();
        southWest->del();
    }
    delete northWest;
    delete northEast;
    delete southEast;
    delete southWest;
    points.clear(); // Xóa danh sách điểm
}

// Hàm hiển thị QuadTree trên cửa sổ
void QuadTree::display(sf::RenderWindow &window) {
    sf::RectangleShape rect;
    rect.setOutlineColor(sf::Color(150, 150, 150));
    rect.setOutlineThickness(1);
    rect.setFillColor(sf::Color(0, 0, 0, 0));

    // Tính toán vị trí và kích thước của hình chữ nhật
    sf::Vector2f pos = sf::Vector2f(boundary.center.x - boundary.radius.x, boundary.center.y - boundary.radius.y);
    rect.setPosition(pos);
    rect.setSize(sf::Vector2f(boundary.radius.x * 2, boundary.radius.y * 2));
    window.draw(rect);

    // Đệ quy hiển thị các vùng con
    if (northWest == nullptr) return;
    else {
        northWest->display(window);
        northEast->display(window);
        southEast->display(window);
        southWest->display(window);
    }
}

// Hàm chia nhỏ QuadTree thành 4 phần
void QuadTree::subdivide() {
    // Chia hình chữ nhật thành 4 phần bằng nhau
    rectByCenter r;
    r.radius = sf::Vector2f(boundary.radius.x / 2, boundary.radius.y / 2);

    // Tạo các vùng con
    r.center.x = boundary.center.x - boundary.radius.x / 2;
    r.center.y = boundary.center.y - boundary.radius.y / 2;
    northWest = new QuadTree(r);

    r.center.x = boundary.center.x + boundary.radius.x / 2;
    northEast = new QuadTree(r);

    r.center.y = boundary.center.y + boundary.radius.y / 2;
    southEast = new QuadTree(r);

    r.center.x = boundary.center.x - boundary.radius.x / 2;
    southWest = new QuadTree(r);
}

// Hàm chèn điểm vào QuadTree với OpenMP và OpenACC
bool QuadTree::insert(Boid *p) {
    if (this == nullptr || p == nullptr) return false;

    // Kiểm tra xem điểm có nằm trong QuadTree hay không
    if ((abs(boundary.center.x - p->position.x) >= boundary.radius.x) ||
        (abs(boundary.center.y - p->position.y) >= boundary.radius.y)) {
        return false;
    }

    // Thêm điểm nếu còn chỗ trống trong QuadTree và chưa chia nhỏ
    if (points.size() < QT_NODE_CAPACITY && northWest == nullptr) {
        points.push_back(p);
        return true;
    }

    // Nếu đã đầy, chia nhỏ QuadTree
    if (northWest == nullptr) {
        subdivide();
        for (size_t i = 0; i < points.size(); i++) {
            if (!insert(points[i])) return false;
        }
        points.clear();
    }

    bool inserted = false;

#pragma omp parallel sections
    {
#pragma omp section
        if (northWest->insert(p)) inserted = true;

#pragma omp section
        if (northEast->insert(p)) inserted = true;

#pragma omp section
        if (southWest->insert(p)) inserted = true;

#pragma omp section
        if (southEast->insert(p)) inserted = true;
    }
    return inserted;
}

void QuadTree::queryRangeCircle(rectByCenter range, std::vector<Boid> &pointsInRange) {
    // Bỏ qua nếu QuadTree không liên quan
    if (!collideRectAndRect(range, boundary)) return;

    // Nếu không có vùng con, thêm điểm trong nút hiện tại
    if (northWest == nullptr) {
#pragma omp parallel for schedule(auto)
        for (int p = 0; p < points.size(); p++) {
            if ((abs(points.at(p)->position.x - range.center.x) <= range.radius.x) &&
                (abs(points.at(p)->position.y - range.center.y) <= range.radius.y)) {
                if (distSqrd(points[p]->position, range.center)
                    <= range.radius.x * range.radius.x + range.radius.y * range.radius.y) {
#pragma omp critical
                    pointsInRange.push_back(*points.at(p));
                    }
                }
        }
        return;
    }

    // Truy vấn các vùng con song song bằng OpenMP
#pragma omp parallel sections
    {
#pragma omp section
        northWest->queryRangeCircle(range, pointsInRange);

#pragma omp section
        northEast->queryRangeCircle(range, pointsInRange);

#pragma omp section
        southEast->queryRangeCircle(range, pointsInRange);

#pragma omp section
        southWest->queryRangeCircle(range, pointsInRange);
    }
}
