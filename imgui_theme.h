#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>

#pragma region Themes Tables

struct ThemeColor
{
	ImGuiCol property;
	ImVec4 color;
};

struct ThemeStyle
{
	ImGuiStyleVar property;
	std::variant<ImVec2, float, int> value;
};

struct Theme
{
	std::vector<ThemeColor> colors;
	std::vector<ThemeStyle> styles;
};

// Themes
std::unordered_map<const char*, Theme> themes = { 
	{"Halloween", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.4976, 0.3929, 0.2830, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_ChildBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0800, 0.0800, 0.0800, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.9621, 0.4700, 0.0593, 1.0000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.2489, 0.2403, 0.2300, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800, 0.5876, 0.2600, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549, 0.0537, 0.0532, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000, 0.0000, 0.0000, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400, 0.1400, 0.1400, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100, 0.3100, 0.3100, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100, 0.4100, 0.4100, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100, 0.5100, 0.5100, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.5334, 0.9800, 0.2600, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800, 0.4584, 0.2400, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800, 0.5466, 0.2600, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.8009, 0.3196, 0.0835, 0.7867)},
			{ImGuiCol_ButtonHovered, ImVec4(0.8531, 0.3154, 0.0000, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800, 0.4001, 0.0600, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.1185, 0.0657, 0.0163, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800, 0.7309, 0.2600, 1.0000)},
			{ImGuiCol_Separator, ImVec4(1.0000, 0.2844, 0.0000, 0.5000)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500, 0.3588, 0.1000, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500, 0.3033, 0.1000, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479, 0.5534, 0.0898, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800, 0.5466, 0.2600, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800, 0.6081, 0.2600, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.0000, 0.0000, 0.0000, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.6962, 0.2820, 0.0000, 0.6872)},
			{ImGuiCol_TabActive, ImVec4(0.6160, 0.2719, 0.0936, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332, 0.0295, 0.0215, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.9409, 0.6306, 0.4486, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800, 0.5876, 0.2600, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.6100, 0.6100, 0.6100, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759, 0.0748, 0.0737, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000, 0.4557, 0.0000, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000, 0.4557, 0.0000, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000, 1.0000, 1.0000, 0.0600)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800, 0.7309, 0.2600, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800, 0.5876, 0.2600, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
	{"Burnt", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.4976, 0.3929, 0.2830, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_ChildBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0800, 0.0800, 0.0800, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.9621, 0.4700, 0.0593, 0.5000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.2489, 0.2403, 0.2300, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800, 0.5876, 0.2600, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549, 0.0537, 0.0532, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000, 0.0000, 0.0000, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400, 0.1400, 0.1400, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100, 0.3100, 0.3100, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100, 0.4100, 0.4100, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100, 0.5100, 0.5100, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.9800, 0.5262, 0.2600, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800, 0.4584, 0.2400, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800, 0.5466, 0.2600, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.6709, 0.3480, 0.1897, 0.4000)},
			{ImGuiCol_ButtonHovered, ImVec4(1.0000, 0.5823, 0.0000, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800, 0.4001, 0.0600, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.9800, 0.5466, 0.2600, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800, 0.7309, 0.2600, 1.0000)},
			{ImGuiCol_Separator, ImVec4(1.0000, 0.2844, 0.0000, 0.5000)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500, 0.3588, 0.1000, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500, 0.3033, 0.1000, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479, 0.5534, 0.0898, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800, 0.5466, 0.2600, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800, 0.6081, 0.2600, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.0000, 0.0000, 0.0000, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.6962, 0.2820, 0.0000, 0.6872)},
			{ImGuiCol_TabActive, ImVec4(0.6160, 0.2719, 0.0936, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332, 0.0295, 0.0215, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3602, 0.1633, 0.0478, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800, 0.5876, 0.2600, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.6100, 0.6100, 0.6100, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759, 0.0748, 0.0737, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000, 0.4557, 0.0000, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000, 0.4557, 0.0000, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000, 1.0000, 1.0000, 0.0600)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800, 0.7309, 0.2600, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800, 0.5876, 0.2600, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
	{"Lime", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.3629, 0.3614, 0.3598, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0170, 0.1327, 0.0258, 1.0000)},
			{ImGuiCol_ChildBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0800, 0.0800, 0.0800, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.3759, 0.9621, 0.0593, 0.5000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.3378, 0.6209, 0.3266, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.6968, 0.9800, 0.2600, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.6763, 0.9800, 0.2600, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.0400, 0.0400, 0.0400, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000, 0.0000, 0.0000, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400, 0.1400, 0.1400, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.0702, 0.5118, 0.0388, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.2043, 0.5877, 0.1504, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.3558, 0.7204, 0.1332, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.2600, 0.9800, 0.3555, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.2643, 0.8800, 0.2400, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.2600, 0.9800, 0.3351, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.1765, 0.5213, 0.0519, 0.4000)},
			{ImGuiCol_ButtonHovered, ImVec4(0.1028, 0.2038, 0.0715, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.6181, 0.9800, 0.0600, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.4716, 0.9800, 0.2600, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.3692, 0.9800, 0.2600, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.5330, 0.9800, 0.2600, 1.0000)},
			{ImGuiCol_Separator, ImVec4(0.1544, 0.9052, 0.2114, 0.5000)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.2171, 0.7500, 0.1000, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.1986, 0.7500, 0.1000, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.0980, 0.9479, 0.0898, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.2668, 0.9800, 0.2600, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.0800, 0.8957, 0.0722, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.0566, 0.1422, 0.0505, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.2442, 0.6962, 0.0000, 0.6872)},
			{ImGuiCol_TabActive, ImVec4(0.0435, 0.0711, 0.0057, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0217, 0.0332, 0.0215, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.0551, 0.0758, 0.0543, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.4511, 0.9800, 0.2600, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.6100, 0.6100, 0.6100, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.1900, 0.1900, 0.2000, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.3100, 0.3100, 0.3500, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2300, 0.2300, 0.2500, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000, 1.0000, 1.0000, 0.0600)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.6763, 0.9800, 0.2600, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(0.4076, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800, 0.5876, 0.2600, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
	{"MonoChrome", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.5000, 0.5000, 0.5000, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0600, 0.0600, 0.0600, 0.8483)},
			{ImGuiCol_ChildBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0800, 0.0800, 0.0800, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.4300, 0.4300, 0.5000, 0.5000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.3638, 0.3681, 0.3744, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.6360, 0.6551, 0.6777, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.5544, 0.5870, 0.6256, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.0400, 0.0400, 0.0400, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.2483, 0.2533, 0.2607, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000, 0.0000, 0.0000, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.2938, 0.2841, 0.2841, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100, 0.3100, 0.3100, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100, 0.4100, 0.4100, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100, 0.5100, 0.5100, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.9624, 0.9753, 0.9905, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.6400, 0.6482, 0.6588, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.7441, 0.7593, 0.7773, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.3757, 0.3816, 0.3886, 0.4000)},
			{ImGuiCol_ButtonHovered, ImVec4(0.0358, 0.0389, 0.0427, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.4852, 0.4988, 0.5118, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.4373, 0.4437, 0.5213, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.6591, 0.6742, 0.6919, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.4564, 0.5231, 0.6019, 1.0000)},
			{ImGuiCol_Separator, ImVec4(0.5811, 0.5811, 0.6256, 0.5000)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.2862, 0.3072, 0.3318, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.6026, 0.6285, 0.6588, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.5166, 0.5296, 0.5450, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.5360, 0.5575, 0.5829, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.3998, 0.4099, 0.4218, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.4924, 0.5107, 0.5355, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.6302, 0.6780, 0.7346, 0.8000)},
			{ImGuiCol_TabActive, ImVec4(0.5229, 0.5284, 0.5355, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0700, 0.1000, 0.1500, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3355, 0.3481, 0.3649, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.4063, 0.4156, 0.4265, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.8910, 0.8106, 0.5447, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.1900, 0.1900, 0.2000, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.3100, 0.3100, 0.3500, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2300, 0.2300, 0.2500, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000, 1.0000, 1.0000, 0.0600)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.5590, 0.5678, 0.5782, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.7066, 0.7368, 0.7725, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
	{"Default", {
		{  // Colors
		},
		{  // Styles
		}
	}},
	{"Grape", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.5000, 0.5000, 0.5000, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0168, 0.0022, 0.0474, 0.9400)},
			{ImGuiCol_ChildBg, ImVec4(0.0630, 0.0059, 0.1564, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0543, 0.0119, 0.1564, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.2946, 0.1528, 0.3981, 0.5000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.2631, 0.1600, 0.4800, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.4306, 0.2600, 0.9800, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.5334, 0.2600, 0.9800, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.1031, 0.0037, 0.1943, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.3544, 0.1600, 0.4800, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0788, 0.0133, 0.1857, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400, 0.1400, 0.1400, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.4585, 0.1383, 0.6209, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4136, 0.2610, 0.6635, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.6160, 0.1153, 0.8389, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.6994, 0.5758, 0.9283, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.5965, 0.2400, 0.8800, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.5516, 0.2600, 0.9800, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.2313, 0.1024, 0.7204, 0.4000)},
			{ImGuiCol_ButtonHovered, ImVec4(0.5739, 0.2600, 0.9800, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.4792, 0.0600, 0.9800, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.5699, 0.2600, 0.9800, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.6428, 0.2600, 0.9800, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.5739, 0.2600, 0.9800, 1.0000)},
			{ImGuiCol_Separator, ImVec4(0.3223, 0.0000, 1.0000, 0.8246)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.3797, 0.1000, 0.7500, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.3649, 0.1000, 0.7500, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.5739, 0.2600, 0.9800, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.4970, 0.2600, 0.9800, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.5699, 0.2600, 0.9800, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.2872, 0.1472, 0.4929, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.5330, 0.2600, 0.9800, 0.8000)},
			{ImGuiCol_TabActive, ImVec4(0.4144, 0.1743, 0.9194, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0981, 0.0700, 0.1500, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3509, 0.1853, 0.5213, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.6792, 0.2600, 0.9800, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.6100, 0.6100, 0.6100, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.2259, 0.1795, 0.2559, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.2120, 0.0954, 0.2796, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2061, 0.1294, 0.3175, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0788, 0.0261, 0.1896, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(0.4200, 0.2696, 0.4346, 0.8531)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.5516, 0.2600, 0.9800, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.2647, 0.2123, 0.2986, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
	{"Red", {
		{  // Colors
			{ImGuiCol_TextDisabled, ImVec4(0.4976, 0.3929, 0.2830, 1.0000)},
			{ImGuiCol_WindowBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_ChildBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_PopupBg, ImVec4(0.0800, 0.0800, 0.0800, 0.9400)},
			{ImGuiCol_Border, ImVec4(0.9621, 0.0593, 0.0593, 0.5000)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_FrameBg, ImVec4(0.2489, 0.2403, 0.2300, 0.5400)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.4000)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800, 0.2600, 0.2600, 0.6700)},
			{ImGuiCol_TitleBg, ImVec4(0.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549, 0.0537, 0.0532, 1.0000)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000, 0.0000, 0.0000, 0.5100)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400, 0.1400, 0.1400, 1.0000)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200, 0.0200, 0.0200, 0.5300)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100, 0.3100, 0.3100, 1.0000)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100, 0.4100, 0.4100, 1.0000)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100, 0.5100, 0.5100, 1.0000)},
			{ImGuiCol_CheckMark, ImVec4(0.9800, 0.5262, 0.2600, 1.0000)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800, 0.4584, 0.2400, 1.0000)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800, 0.5466, 0.2600, 1.0000)},
			{ImGuiCol_Button, ImVec4(0.6709, 0.1897, 0.2445, 0.4000)},
			{ImGuiCol_ButtonHovered, ImVec4(1.0000, 0.5823, 0.0000, 1.0000)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800, 0.4001, 0.0600, 1.0000)},
			{ImGuiCol_Header, ImVec4(0.9800, 0.5466, 0.2600, 0.3100)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800, 0.6900, 0.2600, 0.8000)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800, 0.7309, 0.2600, 1.0000)},
			{ImGuiCol_Separator, ImVec4(1.0000, 0.0000, 0.1247, 0.5000)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500, 0.3588, 0.1000, 0.7800)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500, 0.3033, 0.1000, 1.0000)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479, 0.5534, 0.0898, 0.2000)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800, 0.5466, 0.2600, 0.6700)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800, 0.6081, 0.2600, 0.9500)},
			{ImGuiCol_Tab, ImVec4(0.0000, 0.0000, 0.0000, 0.8600)},
			{ImGuiCol_TabHovered, ImVec4(0.8026, 0.0696, 0.1400, 0.6303)},
			{ImGuiCol_TabActive, ImVec4(0.6160, 0.0936, 0.1769, 1.0000)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332, 0.0295, 0.0215, 0.9700)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3602, 0.1633, 0.0478, 1.0000)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800, 0.5876, 0.2600, 0.7000)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000, 0.2000, 0.2000, 1.0000)},
			{ImGuiCol_PlotLines, ImVec4(0.6100, 0.6100, 0.6100, 1.0000)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000, 0.4300, 0.3500, 1.0000)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000, 0.7000, 0.0000, 1.0000)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000, 0.6000, 0.0000, 1.0000)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759, 0.0748, 0.0737, 1.0000)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000, 0.0000, 0.0000, 1.0000)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000, 0.0000, 0.0000, 0.0000)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000, 1.0000, 1.0000, 0.0600)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800, 0.7309, 0.2600, 0.3500)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000, 1.0000, 0.0000, 0.9000)},
			{ImGuiCol_NavHighlight, ImVec4(0.9145, 0.0921, 0.2133, 1.0000)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000, 1.0000, 1.0000, 0.7000)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.2000)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000, 0.8000, 0.8000, 0.3500)},
		},
		{  // Styles
		}
	}},
};

#pragma endregion

#pragma region Theme Functions

int PushTheme(const char* themeName) {
	int colorCount = 0;
	int styleCount = 0;

	auto themeIter = themes.find(themeName);
	if (themeIter != themes.end()) {
		const Theme& theme = themeIter->second;
		for (const auto& color : theme.colors) {
			ImGui::PushStyleColor(color.property, color.color);
			++colorCount;
		}
		for (const auto& style : theme.styles) {
			if (std::holds_alternative<ImVec2>(style.value)) {
				ImGui::PushStyleVar(style.property, std::get<ImVec2>(style.value));
			}
			else if (std::holds_alternative<float>(style.value)) {
				ImGui::PushStyleVar(style.property, std::get<float>(style.value));
			}
			else if (std::holds_alternative<int>(style.value)) {
				ImGui::PushStyleVar(style.property, static_cast<float>(std::get<int>(style.value)));
			}
			++styleCount;
		}
	}

	return colorCount + (styleCount << 16);
}

void PopTheme(int popCounts)
{
	int colorCount = popCounts & 0xFFFF;
	int styleCount = popCounts >> 16;
	ImGui::PopStyleColor(colorCount);
	ImGui::PopStyleVar(styleCount);
}

const char* DrawThemePicker(const char* currentTheme)
{
	std::vector<const char*> themeNames;
	for (const auto& theme : themes) {
		themeNames.push_back(theme.first);
	}

	static int selectedThemeIndex = 0;
	for (size_t i = 0; i < themeNames.size(); ++i) {
		if (strcmp(currentTheme, themeNames[i]) == 0) {
			selectedThemeIndex = static_cast<int>(i);
			break;
		}
	}

	if (ImGui::Combo("Theme Picker", &selectedThemeIndex, themeNames.data(), themeNames.size())) {
		return themeNames[selectedThemeIndex];
	}

	return currentTheme;
}

#pragma endregion
