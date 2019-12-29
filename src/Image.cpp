
#include "Image.h"

double Image::saturate(double x) const
{
    return (x > 1) ? 1 : ((x < 0) ? 0 : x);
}

Image::Image(int width, int height)
{
	resize(width, height);
}

const unsigned char* Image::getPixmap() const
{
    if (m_pixmap.empty()) return nullptr;

    return m_pixmap.data();
}

int Image::getWidth() const
{
	return m_width;
}

int Image::getHeight() const
{
	return m_height;
}

void Image::putPixel(int x, int y, const Color& color)
{
    if (x >= 0 && x < m_width && y >= 0 && y < m_height)
    {
		m_data[y][x] = color;

		int i = (m_width * y + x) * 4;

        m_pixmap[i + 3] = static_cast<unsigned char>(0xff);
        m_pixmap[i + 2] = static_cast<unsigned char>(0xff * saturate(color.m_red));
        m_pixmap[i + 1] = static_cast<unsigned char>(0xff * saturate(color.m_green));
        m_pixmap[i + 0] = static_cast<unsigned char>(0xff * saturate(color.m_blue));
    }
}

const Color& Image::getPixel(int x, int y) const
{
    return m_data[y][x];
}

void Image::resize(int width, int height)
{
	m_data.clear();
	m_pixmap.clear();

	m_width = width;
	m_height = height;
	m_data = std::vector<std::vector<Color>>(height, std::vector<Color>(width, Color()));
	m_pixmap = std::vector<unsigned char>(4 * width * height, 0);
}
