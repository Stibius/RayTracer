#include <cmath>

#include "Color.h"

Color::Color(double red, double green, double blue)
    : m_red(red)
	, m_green(green)
	, m_blue(blue)
{

}

void Color::accumulate(const Color& color, double scale)
{
    m_red += scale * color.m_red;
    m_green += scale * color.m_green;
    m_blue += scale * color.m_blue;
}

Color operator+(const Color& lhs, const Color& rhs)
{
	return Color(lhs.m_red + rhs.m_red, lhs.m_green + rhs.m_green, lhs.m_blue + rhs.m_blue);
}

Color& operator+=(Color& lhs, const Color& rhs)
{
	lhs.m_red += rhs.m_red;
	lhs.m_green += rhs.m_green;
	lhs.m_blue += rhs.m_blue;

	return lhs;
}

Color operator-(const Color& lhs, const Color& rhs)
{
	return Color(lhs.m_red - rhs.m_red, lhs.m_green - rhs.m_green, lhs.m_blue - rhs.m_blue);
}

Color& operator-=(Color& lhs, const Color& rhs)
{
	lhs.m_red -= rhs.m_red;
	lhs.m_green -= rhs.m_green;
	lhs.m_blue -= rhs.m_blue;

	return lhs;
}

Color operator*(const Color& lhs, const Color& rhs)
{
	return Color(lhs.m_red * rhs.m_red, lhs.m_green * rhs.m_green, lhs.m_blue * rhs.m_blue);
}

Color operator*(const Color& lhs, double rhs)
{
	return Color(lhs.m_red * rhs, lhs.m_green * rhs, lhs.m_blue * rhs);
}

Color operator*(double lhs, const Color& rhs)
{
	return Color(lhs * rhs.m_red, lhs * rhs.m_green, lhs * rhs.m_blue);
}

Color& operator*=(Color& lhs, const Color& rhs)
{
	lhs.m_red *= rhs.m_red;
	lhs.m_green *= rhs.m_green;
	lhs.m_blue *= rhs.m_blue;

	return lhs;
}

Color& operator*=(Color& lhs, double rhs)
{
	lhs.m_red *= rhs;
	lhs.m_green *= rhs;
	lhs.m_blue *= rhs;

	return lhs;
}

Color operator/(const Color& lhs, const Color& rhs)
{
	return Color(lhs.m_red / rhs.m_red, lhs.m_green / rhs.m_green, lhs.m_blue / rhs.m_blue);
}

Color operator/(const Color& lhs, double rhs)
{
	return Color(lhs.m_red / rhs, lhs.m_green / rhs, lhs.m_blue / rhs);
}

Color& operator/=(Color& lhs, const Color& rhs)
{
	lhs.m_red /= rhs.m_red;
	lhs.m_green /= rhs.m_green;
	lhs.m_blue /= rhs.m_blue;

	return lhs;
}

Color& operator/=(Color& lhs, double rhs)
{
	lhs.m_red /= rhs;
	lhs.m_green /= rhs;
	lhs.m_blue /= rhs;

	return lhs;
}

bool operator==(const Color & lhs, const Color & rhs)
{
	return (lhs.m_red == rhs.m_red && lhs.m_green == rhs.m_green && lhs.m_blue == rhs.m_blue);
}

bool operator!=(const Color & lhs, const Color & rhs)
{
	return !(lhs == rhs);
}

double distance(const Color& color1, const Color& color2)
{
	double rr = color1.m_red - color2.m_red;
	double gg = color1.m_green - color2.m_green;
	double bb = color1.m_blue - color2.m_blue;

	return sqrt(rr * rr + gg * gg + bb * bb);
}
