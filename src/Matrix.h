#pragma once

#include "Vector.h"

#include <array>

class Matrix
{
public:

    Matrix() = default;

    explicit Matrix(const std::array<double, 16>& data);

    Matrix(double a00, double a01, double a02, double a03,
           double a10, double a11, double a12, double a13,
           double a20, double a21, double a22, double a23,
           double a30, double a31, double a32, double a33);

	Matrix(const Matrix& other) = default;

	Matrix(Matrix&& other) = default;

	Matrix& operator=(const Matrix& other) = default;

	Matrix& operator=(Matrix&& other) = default;

	~Matrix() = default;

    static Matrix createTranslation(const Vector& translation);

    static Matrix createScale(const Vector& factors);

    bool inverse(Matrix& invMatrix) const;

    Vector operator*(const Vector& vector) const;

    Matrix operator*(const Matrix& matrix) const;

	const Matrix& operator*=(const Matrix& matrix);

private:

	std::array<double, 16> m_data = 
	{
		1.0, 0.0, 0.0, 0.0,
	    0.0, 1.0, 0.0, 0.0,
	    0.0, 0.0, 1.0, 0.0,
	    0.0, 0.0, 0.0, 1.0
	};
};
