#pragma once
#include <iostream>

struct Vec2 {

	Vec2():
		x(0),y(0){};

	Vec2(float posX, float posY) :
		x(posX), y(posY) {};

	void debugPrint() {
		std::cout << x << ", " << y << '\n';
	}



	
	float x, y;
};