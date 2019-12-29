#include "Utility.h"

std::vector<std::string> Utility::split(const std::string& str, char delimiter)
{
	std::vector<std::string> result;

	size_t pos1 = 0;
	size_t pos2 = 0;

	while (pos1 < str.size())
	{
		pos2 = str.find(delimiter, pos1);

		if (pos2 == std::string::npos)
		{
			result.push_back(str.substr(pos1));
			break;
		}
		else
		{
			result.push_back(str.substr(pos1, pos2 - pos1));
		}

		pos1 = pos2 + 1;
	}

	if (result.empty())
	{
		result.push_back("");
	}

	return result;
}

std::string& Utility::replace(std::string& str, const std::string& before, const std::string& after)
{
	size_t index = 0;
	while (true)
	{
		index = str.find(before, index);
		if (index == std::string::npos) break;
		str.replace(index, before.size(), after);
		index += after.size();
	}

	return str;
}

Color Utility::styleSheetToColor(const QString& styleSheet) 
{
	QStringRef name(&styleSheet, styleSheet.indexOf('#'), 7);
	QColor color = QColor(name.toString());
	return Color(color.red() / 255.0, color.green() / 255.0, color.blue() / 255.0);
}

QColor Utility::styleSheetToQColor(const QString& styleSheet) 
{
	QStringRef name(&styleSheet, styleSheet.indexOf('#'), 7);
	return QColor(name.toString());
}

QColor Utility::colorToQColor(const Color& color) 
{
	return QColor(color.m_red * 255, color.m_green * 255, color.m_blue * 255);
}