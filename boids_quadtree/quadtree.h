#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include "boid_quadtree.h"

// Cấu trúc để định nghĩa hình chữ nhật bằng tâm và bán kính
struct rectByCenter {
	sf::Vector2f center; // Tâm của hình chữ nhật
	sf::Vector2f radius; // Bán kính của hình chữ nhật
};

// Lớp QuadTree
class QuadTree
{

public:
	// Hằng số đại diện cho số lượng điểm tối đa có thể lưu trữ trong một nút của quadtree
	// 8 hoặc 16 là một lựa chọn phổ biến
	const int QT_NODE_CAPACITY = 16;
	// Các con của quadtree
	QuadTree* northWest = nullptr; // Nút Tây Bắc
	QuadTree* northEast = nullptr; // Nút Đông Bắc
	QuadTree* southWest = nullptr; // Nút Tây Nam
	QuadTree* southEast = nullptr; // Nút Đông Nam
	// Định nghĩa vùng giới hạn cho quadtree (bán kính và tâm)
	rectByCenter boundary;
	// Danh sách các điểm trong nút này của quadtree
	std::vector <Boid*> points;

	// Phương thức khởi tạo cho QuadTree
	QuadTree(rectByCenter bd) { boundary = bd; }; // Khởi tạo với một vùng giới hạn

	// Các phương thức của QuadTree
	bool insert(Boid* p); // Thêm một boid vào quadtree
	void subdivide(); // Chia nhỏ quadtree thành 4 phần
	// void queryRangeRect(rectByCenter range, std::vector<Boid>& pointsInRange); // Truy vấn các điểm trong hình chữ nhật
	void queryRangeCircle(rectByCenter range, std::vector<Boid>& pointsInRange); // Truy vấn các điểm trong hình tròn
	void display(sf::RenderWindow&); // Hiển thị quadtree lên cửa sổ
	void del(); // Giải phóng bộ nhớ cho quadtree từ vị trí hiện tại đến các lá


};
