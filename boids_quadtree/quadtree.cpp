#include "quadtree.h"

// Hàm tính khoảng cách bình phương giữa hai điểm
float distSqrd(sf::Vector2f p1, sf::Vector2f p2) {
    return (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);  // trả về khoảng cách bình phương
}

// Hàm kiểm tra va chạm giữa hai hình chữ nhật
bool collideRectAndRect(rectByCenter rect1, rectByCenter rect2) {
    float tlx1 = rect1.center.x - rect1.radius.x; // Điểm trên bên trái của hình chữ nhật 1
    float tly1 = rect1.center.y - rect1.radius.y;
    float brx1 = rect1.center.x + rect1.radius.x; // Điểm dưới bên phải của hình chữ nhật 1
    float bry1 = rect1.center.y + rect1.radius.y;

    float tlx2 = rect2.center.x - rect2.radius.x; // Điểm trên bên trái của hình chữ nhật 2
    float tly2 = rect2.center.y - rect2.radius.y;
    float brx2 = rect2.center.x + rect2.radius.x; // Điểm dưới bên phải của hình chữ nhật 2
    float bry2 = rect2.center.y + rect2.radius.y;

    // Kiểm tra va chạm
    if ((tlx1 <= brx2) && (brx1 >= tlx2) && (tly1 <= bry2) && (bry1 >= tly2)) {
        return true; // Có va chạm
    }
    return false; // Không va chạm
}

// Hàm giải phóng bộ nhớ cho QuadTree
void QuadTree::del() {
    // Nếu có vùng con, gọi đệ quy để giải phóng bộ nhớ
    if (northWest != nullptr) {
        northWest->del();
        northEast->del();
        southEast->del();
        southWest->del();
    }
    // Giải phóng bộ nhớ cho các vùng con
    delete northWest;
    delete northEast;
    delete southEast;
    delete southWest;

    points.clear(); // Xóa danh sách điểm
}

// Hàm hiển thị QuadTree trên cửa sổ
void QuadTree::display(sf::RenderWindow &window) {
    sf::RectangleShape rect; // Tạo hình chữ nhật để hiển thị
    rect.setOutlineColor(sf::Color(150, 150, 150)); // Màu viền
    rect.setOutlineThickness(1); // Độ dày viền
    rect.setFillColor(sf::Color(0, 255, 0, 0)); // Màu nền trong suốt

    // Tính toán vị trí và kích thước của hình chữ nhật
    sf::Vector2f pos = sf::Vector2f(boundary.center.x - boundary.radius.x, boundary.center.y - boundary.radius.y);
    rect.setPosition(pos); // Thiết lập vị trí
    rect.setSize(sf::Vector2f(boundary.radius.x * 2, boundary.radius.y * 2)); // Thiết lập kích thước
    window.draw(rect); // Vẽ hình chữ nhật

    // Kiểm tra và đệ quy hiển thị các vùng con
    if (northWest == nullptr) {
        return; // Nếu không có vùng con thì kết thúc
    } else {
        // Hiển thị các vùng con
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
    r.radius = sf::Vector2f(boundary.radius.x / 2, boundary.radius.y / 2); // Bán kính của vùng con
    r.center.x = boundary.center.x - boundary.radius.x / 2; // Tâm của vùng con
    r.center.y = boundary.center.y - boundary.radius.y / 2;
    northWest = new QuadTree(r); // Tạo vùng tây bắc

    // Thiết lập các vùng con khác
    r.center.x = boundary.center.x + boundary.radius.x / 2;
    r.center.y = boundary.center.y - boundary.radius.y / 2;
    northEast = new QuadTree(r); // Tạo vùng đông bắc

    r.center.x = boundary.center.x + boundary.radius.x / 2;
    r.center.y = boundary.center.y + boundary.radius.y / 2;
    southEast = new QuadTree(r); // Tạo vùng đông nam

    r.center.x = boundary.center.x - boundary.radius.x / 2;
    r.center.y = boundary.center.y + boundary.radius.y / 2;
    southWest = new QuadTree(r); // Tạo vùng tây nam
}

// Hàm thêm một boid vào QuadTree
bool QuadTree::insert(Boid* p) {
    // Kiểm tra con trỏ NULL
    if (this == nullptr || p == nullptr) {
        return false; // Điểm không hợp lệ
    }

    // Kiểm tra xem điểm có nằm trong QuadTree hay không
    if ((abs(boundary.center.x - p->position.x) >= boundary.radius.x) ||
        (abs(boundary.center.y - p->position.y) >= boundary.radius.y)) {
        return false; // Điểm không thuộc về QuadTree
        }

    // Nếu còn chỗ trống trong QuadTree, thêm điểm
    if (points.size() < QT_NODE_CAPACITY && northWest == nullptr) {
        points.push_back(p); // Thêm điểm vào danh sách
        return true; // Thêm thành công
    }

    // Nếu không, chia nhỏ QuadTree và thêm điểm vào
    if (northWest == nullptr) {
        subdivide(); // Chia nhỏ
        // Kiểm tra lại các điểm có đang được thêm đúng vào vùng con không
        for (size_t i = 0; i < points.size(); i++) {
            if (!insert(points[i])) {
                // std::cerr << "Lỗi khi thêm lại boid vào vùng con!" << std::endl;
                return false; // Thêm thất bại
            }
        }

        points.clear(); // Xóa danh sách điểm sau khi thêm
    }

    // Thêm điểm vào các vùng con
    if (northWest->insert(p)) return true;
    if (northEast->insert(p)) return true;
    if (southWest->insert(p)) return true;
    if (southEast->insert(p)) return true;

    return false; // Không thể thêm điểm (không nên xảy ra)
}

// Hàm truy vấn các boid trong một hình tròn
void QuadTree::queryRangeCircle(rectByCenter range, std::vector<Boid>& pointsInRange) {
    // Bỏ qua nếu QuadTree không liên quan
    if (!collideRectAndRect(range, boundary)) {
        return;
    }

    // Nếu không có vùng con, trả về danh sách điểm trong nút này
    if (northWest == nullptr) {
        for (int p = 0; p < points.size(); p++) {
            if ((abs(points.at(p)->position.x - range.center.x) <= range.radius.x) &&
                (abs(points.at(p)->position.y - range.center.y) <= range.radius.y)) {

                // Trong hình chữ nhật, kiểm tra nếu nằm trong hình tròn
                if (distSqrd(points[p]->position, range.center)
                    <= range.radius.x * range.radius.x + range.radius.y * range.radius.y) {
                    pointsInRange.push_back(*points.at(p)); // Thêm điểm vào kết quả
                }
            }
        }
        return; // Kết thúc hàm
    }

    // Nếu có vùng con, truy vấn các vùng con
    northWest->queryRangeCircle(range, pointsInRange);
    northEast->queryRangeCircle(range, pointsInRange);
    southEast->queryRangeCircle(range, pointsInRange);
    southWest->queryRangeCircle(range, pointsInRange);
    return; // Kết thúc hàm
}