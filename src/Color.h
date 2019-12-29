#pragma once

class Color
{

public:

    double m_red = 0.0;
    double m_green = 0.0;
    double m_blue = 0.0;

    Color() = default;

    Color(double red, double green, double blue);

	Color(const Color& other) = default;

	Color(Color&& other) = default;

	Color& operator=(const Color& other) = default;

	Color& operator=(Color&& other) = default;

	~Color() = default;

    void accumulate(const Color& color, double scale = 1.0);
};

Color operator+(const Color& lhs, const Color& rhs);

Color& operator+=(Color& lhs, const Color& rhs);


Color operator-(const Color& lhs, const Color& rhs);

Color& operator-=(Color& lhs, const Color& rhs);


Color operator*(const Color& lhs, const Color& rhs);

Color operator*(const Color& lhs, double rhs);

Color operator*(double lhs, const Color& rhs);

Color& operator*=(Color& lhs, const Color& rhs);

Color& operator*=(Color& lhs, double rhs);


Color operator/(const Color& lhs, const Color& rhs);

Color operator/(const Color& lhs, double rhs);

Color& operator/=(Color& lhs, const Color& rhs);

Color& operator/=(Color& lhs, double rhs);


bool operator==(const Color& lhs, const Color& rhs);

bool operator!=(const Color& lhs, const Color& rhs);


double distance(const Color& color1, const Color& color2);
