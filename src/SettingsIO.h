#pragma once

#include <string>

class SettingsIO
{
public:

	static void saveSettings(const std::string& filename);

	static bool loadSettings(const std::string& filename);
};
