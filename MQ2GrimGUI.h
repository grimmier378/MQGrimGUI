#pragma once
// MQ2GrimGUI.h
#include <string>
#include <imgui.h>
#include "main/datatypes/MQ2DataTypes.h"
#include "main/MQ2Main.h"

// Class Definitions


// Color utility functions

/**
* @fn CalculateProgressiveColor
*
* Function to calculate a color between two colors based on a value between 0 and 100
*
* @param minColor const ImVec4& Minimum color value
* @param maxColor const ImVec4& Maximum color value
* @param value int Value between 0 and 100 to calculate the color between minColor and maxColor
* @param midColor const ImVec4* Optional midColor value to calculate in two segments
* @param midValue int Optional midValue to split the value between minColor and maxColor
*
* @return ImVec4 color value
*/
static ImVec4 CalculateProgressiveColor(const ImVec4& minColor, const ImVec4& maxColor, int value, const ImVec4* midColor = nullptr, int midValue = 50)
{
	// Clamp value between 0 and 100
	value = std::max(0, std::min(100, value));

	float r, g, b, a;

	if (midColor)
	{
		// If midColor is provided, calculate in two segments
		if (value > midValue)
		{
			float proportion = static_cast<float>(value - midValue) / (100 - midValue);
			r = midColor->x + proportion * (maxColor.x - midColor->x);
			g = midColor->y + proportion * (maxColor.y - midColor->y);
			b = midColor->z + proportion * (maxColor.z - midColor->z);
			a = midColor->w + proportion * (maxColor.w - midColor->w);
		}
		else
		{
			float proportion = static_cast<float>(value) / midValue;
			r = minColor.x + proportion * (midColor->x - minColor.x);
			g = minColor.y + proportion * (midColor->y - minColor.y);
			b = minColor.z + proportion * (midColor->z - minColor.z);
			a = minColor.w + proportion * (midColor->w - minColor.w);
		}
	}
	else
	{
		// Calculate between minColor and maxColor
		float proportion = static_cast<float>(value) / 100;
		r = minColor.x + proportion * (maxColor.x - minColor.x);
		g = minColor.y + proportion * (maxColor.y - minColor.y);
		b = minColor.z + proportion * (maxColor.z - minColor.z);
		a = minColor.w + proportion * (maxColor.w - minColor.w);
	}

	return ImVec4(r, g, b, a);
}


/**
* @fn ColorToVec
*
* Function to convert a color name to an ImVec4 color value
*
* @param color_name const std::string& Color name to convert to ImVec4
*
* @return ImVec4 color value
*/
static ImVec4 ColorToVec(const std::string& color_name)
{
	std::string color = color_name;
	std::transform(color.begin(), color.end(), color.begin(), ::tolower);

	if (color == "red")          return ImVec4(0.9f, 0.1f, 0.1f, 1.0f);
	if (color == "pink2")        return ImVec4(0.976f, 0.518f, 0.844f, 1.0f);
	if (color == "pink")         return ImVec4(0.9f, 0.4f, 0.4f, 0.8f);
	if (color == "orange")       return ImVec4(0.78f, 0.20f, 0.05f, 0.8f);
	if (color == "tangarine")    return ImVec4(1.0f, 0.557f, 0.0f, 1.0f);
	if (color == "yellow")       return ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
	if (color == "yellow2")      return ImVec4(0.7f, 0.6f, 0.1f, 0.7f);
	if (color == "white")        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	if (color == "blue")         return ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	if (color == "softblue")     return ImVec4(0.37f, 0.704f, 1.0f, 1.0f);
	if (color == "light blue2")  return ImVec4(0.2f, 0.9f, 0.9f, 0.5f);
	if (color == "light blue")   return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	if (color == "teal")         return ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
	if (color == "green")        return ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
	if (color == "green2")       return ImVec4(0.01f, 0.56f, 0.001f, 1.0f);
	if (color == "grey")         return ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	if (color == "purple")       return ImVec4(0.8f, 0.0f, 1.0f, 1.0f);
	if (color == "purple2")      return ImVec4(0.46f, 0.204f, 1.0f, 1.0f);
	if (color == "btn_red")      return ImVec4(1.0f, 0.4f, 0.4f, 0.4f);
	if (color == "btn_green")    return ImVec4(0.4f, 1.0f, 0.4f, 0.4f);

	// Default White if no match
	return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}


/**
* @fn ConColorToVec
*
* Function to convert a CONCOLOR value to an ImVec4 color value
*
* @param color_code int CONCOLOR value to convert to ImVec4
* @return ImVec4 color value
*/
static ImVec4 ConColorToVec(int color_code)
{
	switch (color_code)
	{
	case 0x06: return ImVec4(0.6f, 0.6f, 0.6f, 1.0f); // CONCOLOR_GREY
	case 0x02: return ImVec4(0.0f, 1.0f, 0.0f, 1.0f); // CONCOLOR_GREEN
	case 0x12: return ImVec4(0.37f, 0.704f, 1.0f, 1.0f); // CONCOLOR_LIGHTBLUE
	case 0x04: return ImVec4(0.0f, 0.0f, 1.0f, 1.0f); // CONCOLOR_BLUE
	case 0x14: return ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // CONCOLOR_BLACK
	case 0x0a: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // CONCOLOR_WHITE
	case 0x0f: return ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // CONCOLOR_YELLOW
	case 0x0d: return ImVec4(0.9f, 0.1f, 0.1f, 1.0f); // CONCOLOR_RED

		// Default color if the color code doesn't match any known values
	default: return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

