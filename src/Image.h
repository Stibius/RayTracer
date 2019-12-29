#pragma once

#include "Color.h"

#include <vector>

class Image
{
private:

	int m_width;
	int m_height;
    std::vector<std::vector<Color>> m_data;
	std::vector<unsigned char> m_pixmap;

    double saturate(double x) const;

public:

    Image() = default;

    Image(int width, int height);

	Image(const Image& other) = default;

	Image(Image&& other) = default;

	Image& operator=(const Image& other) = default;

	Image& operator=(Image&& other) = default;

	~Image() = default;

    const unsigned char* getPixmap() const;

    int getWidth() const;

    int getHeight() const;

    void putPixel(int x, int y, const Color& color);

    const Color& getPixel(int x, int y) const;

    void resize(int width, int height);
};


