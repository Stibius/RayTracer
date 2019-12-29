#pragma once

#include <QColor>
#include <QString>

#include <vector>

#include "Color.h"

class Utility
{
public:

	static std::vector<std::string> split(const std::string& str, char delimiter);

	static std::string& replace(std::string &str, const std::string& before, const std::string& after);

	static Color styleSheetToColor(const QString& styleSheet);

	static QColor styleSheetToQColor(const QString& styleSheet);

	static QColor colorToQColor(const Color& color);
};
