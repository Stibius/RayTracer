
#include "Matrix.h"

Matrix::Matrix(const std::array<double, 16>& data)
{
	m_data = data;
}

Matrix::Matrix(double a00, double a01, double a02, double a03,
               double a10, double a11, double a12, double a13,
               double a20, double a21, double a22, double a23,
               double a30, double a31, double a32, double a33)
{
    m_data[0] = a00; m_data[1] = a01; m_data[2] = a02; m_data[3] = a03;
    m_data[4] = a10; m_data[5] = a11; m_data[6] = a12; m_data[7] = a13;
    m_data[8] = a20; m_data[9] = a21; m_data[10] = a22; m_data[11] = a23;
    m_data[12] = a30; m_data[13] = a31; m_data[14] = a32; m_data[15] = a33;
}

Matrix Matrix::createTranslation(const Vector& translation)
{
    Matrix result;
    result.m_data[0] = 1.0; result.m_data[1] = 0.0; result.m_data[2] = 0.0; result.m_data[3] = translation.m_x;
    result.m_data[4] = 0.0; result.m_data[5] = 1.0; result.m_data[6] = 0.0; result.m_data[7] = translation.m_y;
    result.m_data[8] = 0.0; result.m_data[9] = 0.0; result.m_data[10] = 1.0; result.m_data[11] = translation.m_z;
    result.m_data[12] = 0.0; result.m_data[13] = 0.0; result.m_data[14] = 0.0; result.m_data[15] = 1.0;

    return result;
}

Matrix Matrix::createScale(const Vector& factors)
{
    Matrix result;
    result.m_data[0] = factors.m_x; result.m_data[1] = 0.0; result.m_data[2] = 0.0; result.m_data[3] = 0.0;
    result.m_data[4] = 0.0; result.m_data[5] = factors.m_y; result.m_data[6] = 0.0; result.m_data[7] = 0.0;
    result.m_data[8] = 0.0; result.m_data[9] = 0.0; result.m_data[10] = factors.m_z; result.m_data[11] = 0.0;
    result.m_data[12] = 0.0; result.m_data[13] = 0.0; result.m_data[14] = 0.0; result.m_data[15] = 1.0;

    return result;
}

bool Matrix::inverse(Matrix& invMatrix) const
{
    double det;
    int i;

    invMatrix.m_data[0] = 
		m_data[5] * m_data[10] * m_data[15] -
        m_data[5]  * m_data[11] * m_data[14] -
        m_data[9]  * m_data[6]  * m_data[15] +
        m_data[9]  * m_data[7]  * m_data[14] +
        m_data[13] * m_data[6]  * m_data[11] -
        m_data[13] * m_data[7]  * m_data[10];

    invMatrix.m_data[4] = 
		-m_data[4] * m_data[10] * m_data[15] +
        m_data[4]  * m_data[11] * m_data[14] +
        m_data[8]  * m_data[6]  * m_data[15] -
        m_data[8]  * m_data[7]  * m_data[14] -
        m_data[12] * m_data[6]  * m_data[11] +
        m_data[12] * m_data[7]  * m_data[10];

    invMatrix.m_data[8] = 
		m_data[4] * m_data[9] * m_data[15] -
        m_data[4]  * m_data[11] * m_data[13] -
        m_data[8]  * m_data[5] * m_data[15] +
        m_data[8]  * m_data[7] * m_data[13] +
        m_data[12] * m_data[5] * m_data[11] -
        m_data[12] * m_data[7] * m_data[9];

    invMatrix.m_data[12] = 
		-m_data[4] * m_data[9] * m_data[14] +
        m_data[4]  * m_data[10] * m_data[13] +
        m_data[8]  * m_data[5] * m_data[14] -
        m_data[8]  * m_data[6] * m_data[13] -
        m_data[12] * m_data[5] * m_data[10] +
        m_data[12] * m_data[6] * m_data[9];

    invMatrix.m_data[1] = 
		-m_data[1] * m_data[10] * m_data[15] +
        m_data[1]  * m_data[11] * m_data[14] +
        m_data[9]  * m_data[2] * m_data[15] -
        m_data[9]  * m_data[3] * m_data[14] -
        m_data[13] * m_data[2] * m_data[11] +
        m_data[13] * m_data[3] * m_data[10];

    invMatrix.m_data[5] = 
		m_data[0] * m_data[10] * m_data[15] -
        m_data[0]  * m_data[11] * m_data[14] -
        m_data[8]  * m_data[2] * m_data[15] +
        m_data[8]  * m_data[3] * m_data[14] +
        m_data[12] * m_data[2] * m_data[11] -
        m_data[12] * m_data[3] * m_data[10];

    invMatrix.m_data[9] = 
		-m_data[0] * m_data[9] * m_data[15] +
        m_data[0]  * m_data[11] * m_data[13] +
        m_data[8]  * m_data[1] * m_data[15] -
        m_data[8]  * m_data[3] * m_data[13] -
        m_data[12] * m_data[1] * m_data[11] +
        m_data[12] * m_data[3] * m_data[9];

    invMatrix.m_data[13] = 
		m_data[0] * m_data[9] * m_data[14] -
        m_data[0]  * m_data[10] * m_data[13] -
        m_data[8]  * m_data[1] * m_data[14] +
        m_data[8]  * m_data[2] * m_data[13] +
        m_data[12] * m_data[1] * m_data[10] -
        m_data[12] * m_data[2] * m_data[9];

    invMatrix.m_data[2] = 
		m_data[1] * m_data[6] * m_data[15] -
        m_data[1]  * m_data[7] * m_data[14] -
        m_data[5]  * m_data[2] * m_data[15] +
        m_data[5]  * m_data[3] * m_data[14] +
        m_data[13] * m_data[2] * m_data[7] -
        m_data[13] * m_data[3] * m_data[6];

    invMatrix.m_data[6] = 
		-m_data[0] * m_data[6] * m_data[15] +
        m_data[0]  * m_data[7] * m_data[14] +
        m_data[4]  * m_data[2] * m_data[15] -
        m_data[4]  * m_data[3] * m_data[14] -
        m_data[12] * m_data[2] * m_data[7] +
        m_data[12] * m_data[3] * m_data[6];

    invMatrix.m_data[10] = 
		m_data[0] * m_data[5] * m_data[15] -
        m_data[0]  * m_data[7] * m_data[13] -
        m_data[4]  * m_data[1] * m_data[15] +
        m_data[4]  * m_data[3] * m_data[13] +
        m_data[12] * m_data[1] * m_data[7] -
        m_data[12] * m_data[3] * m_data[5];

    invMatrix.m_data[14] = 
		-m_data[0] * m_data[5] * m_data[14] +
        m_data[0]  * m_data[6] * m_data[13] +
        m_data[4]  * m_data[1] * m_data[14] -
        m_data[4]  * m_data[2] * m_data[13] -
        m_data[12] * m_data[1] * m_data[6] +
        m_data[12] * m_data[2] * m_data[5];

    invMatrix.m_data[3] = 
		-m_data[1] * m_data[6] * m_data[11] +
        m_data[1] * m_data[7] * m_data[10] +
        m_data[5] * m_data[2] * m_data[11] -
        m_data[5] * m_data[3] * m_data[10] -
        m_data[9] * m_data[2] * m_data[7] +
        m_data[9] * m_data[3] * m_data[6];

    invMatrix.m_data[7] = 
		m_data[0] * m_data[6] * m_data[11] -
        m_data[0] * m_data[7] * m_data[10] -
        m_data[4] * m_data[2] * m_data[11] +
        m_data[4] * m_data[3] * m_data[10] +
        m_data[8] * m_data[2] * m_data[7] -
        m_data[8] * m_data[3] * m_data[6];

    invMatrix.m_data[11] = 
		-m_data[0] * m_data[5] * m_data[11] +
        m_data[0] * m_data[7] * m_data[9] +
        m_data[4] * m_data[1] * m_data[11] -
        m_data[4] * m_data[3] * m_data[9] -
        m_data[8] * m_data[1] * m_data[7] +
        m_data[8] * m_data[3] * m_data[5];

    invMatrix.m_data[15] = 
		m_data[0] * m_data[5] * m_data[10] -
        m_data[0] * m_data[6] * m_data[9] -
        m_data[4] * m_data[1] * m_data[10] +
        m_data[4] * m_data[2] * m_data[9] +
        m_data[8] * m_data[1] * m_data[6] -
        m_data[8] * m_data[2] * m_data[5];

    det = m_data[0] * invMatrix.m_data[0] + m_data[1] * invMatrix.m_data[4] + m_data[2] * invMatrix.m_data[8] + m_data[3] * invMatrix.m_data[12];

	if (det == 0)
	{
		return false;
	}

    det = 1.0 / det;

	for (i = 0; i < 16; ++i)
	{
		invMatrix.m_data[i] = invMatrix.m_data[i] * det;
	}

    return true;
}

Vector Matrix::operator*(const Vector& vector) const
{
    return Vector(
		vector.m_x * m_data[0] + vector.m_y * m_data[1] + vector.m_z * m_data[2] + m_data[3],
        vector.m_x * m_data[4] + vector.m_y * m_data[5] + vector.m_z * m_data[6] + m_data[7],
        vector.m_x * m_data[8] + vector.m_y * m_data[9] + vector.m_z * m_data[10] + m_data[11]);
}

Matrix Matrix::operator*(const Matrix& matrix) const
{
    Matrix result;

    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            result.m_data[row * 4 + col] = 
				m_data[row * 4 + 0] * matrix.m_data[0 * 4 + col] +
                m_data[row * 4 + 1] * matrix.m_data[1 * 4 + col] +
                m_data[row * 4 + 2] * matrix.m_data[2 * 4 + col] +
                m_data[row * 4 + 3] * matrix.m_data[3 * 4 + col];
        }
    }

    return result;
}

const Matrix& Matrix::operator*=(const Matrix& matrix)
{
    *this = *this * matrix;

    return *this;
}
