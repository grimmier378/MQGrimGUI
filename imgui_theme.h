#pragma once
#include "imgui.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>

#pragma region Themes Sets

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
std::unordered_map<std::string, Theme> themeSet = {
	{"Halloween", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.4976f, 0.3929f, 0.2830f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_ChildBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.9621f, 0.4700f, 0.0593f, 1.0000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.2489f, 0.2403f, 0.2300f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800f, 0.5876f, 0.2600f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549f, 0.0537f, 0.0532f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100f, 0.3100f, 0.3100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100f, 0.4100f, 0.4100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100f, 0.5100f, 0.5100f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.5334f, 0.9800f, 0.2600f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800f, 0.4584f, 0.2400f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800f, 0.5466f, 0.2600f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.8009f, 0.3196f, 0.0835f, 0.7867f)},
			{ImGuiCol_ButtonHovered, ImVec4(0.8531f, 0.3154f, 0.0000f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800f, 0.4001f, 0.0600f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.1185f, 0.0657f, 0.0163f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800f, 0.7309f, 0.2600f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(1.0000f, 0.2844f, 0.0000f, 0.5000f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500f, 0.3588f, 0.1000f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500f, 0.3033f, 0.1000f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479f, 0.5534f, 0.0898f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800f, 0.5466f, 0.2600f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800f, 0.6081f, 0.2600f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.6962f, 0.2820f, 0.0000f, 0.6872f)},
			{ImGuiCol_TabActive, ImVec4(0.6160f, 0.2719f, 0.0936f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332f, 0.0295f, 0.0215f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.9409f, 0.6306f, 0.4486f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800f, 0.5876f, 0.2600f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759f, 0.0748f, 0.0737f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000f, 0.4557f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000f, 0.4557f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800f, 0.7309f, 0.2600f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800f, 0.5876f, 0.2600f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
	{"Burnt", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.4976f, 0.3929f, 0.2830f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_ChildBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.9621f, 0.4700f, 0.0593f, 0.5000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.2489f, 0.2403f, 0.2300f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800f, 0.5876f, 0.2600f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549f, 0.0537f, 0.0532f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100f, 0.3100f, 0.3100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100f, 0.4100f, 0.4100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100f, 0.5100f, 0.5100f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.9800f, 0.5262f, 0.2600f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800f, 0.4584f, 0.2400f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800f, 0.5466f, 0.2600f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.6709f, 0.3480f, 0.1897f, 0.4000f)},
			{ImGuiCol_ButtonHovered, ImVec4(1.0000f, 0.5823f, 0.0000f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800f, 0.4001f, 0.0600f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.9800f, 0.5466f, 0.2600f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800f, 0.7309f, 0.2600f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(1.0000f, 0.2844f, 0.0000f, 0.5000f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500f, 0.3588f, 0.1000f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500f, 0.3033f, 0.1000f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479f, 0.5534f, 0.0898f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800f, 0.5466f, 0.2600f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800f, 0.6081f, 0.2600f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.6962f, 0.2820f, 0.0000f, 0.6872f)},
			{ImGuiCol_TabActive, ImVec4(0.6160f, 0.2719f, 0.0936f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332f, 0.0295f, 0.0215f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3602f, 0.1633f, 0.0478f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800f, 0.5876f, 0.2600f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759f, 0.0748f, 0.0737f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000f, 0.4557f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000f, 0.4557f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800f, 0.7309f, 0.2600f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800f, 0.5876f, 0.2600f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
	{"Lime", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.3629f, 0.3614f, 0.3598f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0170f, 0.1327f, 0.0258f, 1.0000f)},
			{ImGuiCol_ChildBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.3759f, 0.9621f, 0.0593f, 0.5000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.3378f, 0.6209f, 0.3266f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.6968f, 0.9800f, 0.2600f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.6763f, 0.9800f, 0.2600f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.0400f, 0.0400f, 0.0400f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.0702f, 0.5118f, 0.0388f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.2043f, 0.5877f, 0.1504f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.3558f, 0.7204f, 0.1332f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.2600f, 0.9800f, 0.3555f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.2643f, 0.8800f, 0.2400f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.2600f, 0.9800f, 0.3351f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.1765f, 0.5213f, 0.0519f, 0.4000f)},
			{ImGuiCol_ButtonHovered, ImVec4(0.1028f, 0.2038f, 0.0715f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.6181f, 0.9800f, 0.0600f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.4716f, 0.9800f, 0.2600f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.3692f, 0.9800f, 0.2600f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.5330f, 0.9800f, 0.2600f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(0.1544f, 0.9052f, 0.2114f, 0.5000f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.2171f, 0.7500f, 0.1000f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.1986f, 0.7500f, 0.1000f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.0980f, 0.9479f, 0.0898f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.2668f, 0.9800f, 0.2600f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.0800f, 0.8957f, 0.0722f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.0566f, 0.1422f, 0.0505f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.2442f, 0.6962f, 0.0000f, 0.6872f)},
			{ImGuiCol_TabActive, ImVec4(0.0435f, 0.0711f, 0.0057f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0217f, 0.0332f, 0.0215f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.0551f, 0.0758f, 0.0543f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.4511f, 0.9800f, 0.2600f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.1900f, 0.1900f, 0.2000f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.3100f, 0.3100f, 0.3500f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2300f, 0.2300f, 0.2500f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.6763f, 0.9800f, 0.2600f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(0.4076f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.9800f, 0.5876f, 0.2600f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
	{"MonoChrome", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.5000f, 0.5000f, 0.5000f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0600f, 0.0600f, 0.0600f, 0.8483f)},
			{ImGuiCol_ChildBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.4300f, 0.4300f, 0.5000f, 0.5000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.3638f, 0.3681f, 0.3744f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.6360f, 0.6551f, 0.6777f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.5544f, 0.5870f, 0.6256f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.0400f, 0.0400f, 0.0400f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.2483f, 0.2533f, 0.2607f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.2938f, 0.2841f, 0.2841f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100f, 0.3100f, 0.3100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100f, 0.4100f, 0.4100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100f, 0.5100f, 0.5100f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.9624f, 0.9753f, 0.9905f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.6400f, 0.6482f, 0.6588f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.7441f, 0.7593f, 0.7773f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.3757f, 0.3816f, 0.3886f, 0.4000f)},
			{ImGuiCol_ButtonHovered, ImVec4(0.0358f, 0.0389f, 0.0427f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.4852f, 0.4988f, 0.5118f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.4373f, 0.4437f, 0.5213f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.6591f, 0.6742f, 0.6919f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.4564f, 0.5231f, 0.6019f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(0.5811f, 0.5811f, 0.6256f, 0.5000f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.2862f, 0.3072f, 0.3318f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.6026f, 0.6285f, 0.6588f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.5166f, 0.5296f, 0.5450f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.5360f, 0.5575f, 0.5829f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.3998f, 0.4099f, 0.4218f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.4924f, 0.5107f, 0.5355f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.6302f, 0.6780f, 0.7346f, 0.8000f)},
			{ImGuiCol_TabActive, ImVec4(0.5229f, 0.5284f, 0.5355f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0700f, 0.1000f, 0.1500f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3355f, 0.3481f, 0.3649f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.4063f, 0.4156f, 0.4265f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.8910f, 0.8106f, 0.5447f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.1900f, 0.1900f, 0.2000f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.3100f, 0.3100f, 0.3500f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2300f, 0.2300f, 0.2500f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.5590f, 0.5678f, 0.5782f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.7066f, 0.7368f, 0.7725f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
	{"Default", {
		{  
		},
		{ 
		}
	}},
	{"Grape", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.5000f, 0.5000f, 0.5000f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0168f, 0.0022f, 0.0474f, 0.9400f)},
			{ImGuiCol_ChildBg, ImVec4(0.0630f, 0.0059f, 0.1564f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0543f, 0.0119f, 0.1564f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.2946f, 0.1528f, 0.3981f, 0.5000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.2631f, 0.1600f, 0.4800f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.4306f, 0.2600f, 0.9800f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.5334f, 0.2600f, 0.9800f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.1031f, 0.0037f, 0.1943f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.3544f, 0.1600f, 0.4800f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0788f, 0.0133f, 0.1857f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.4585f, 0.1383f, 0.6209f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4136f, 0.2610f, 0.6635f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.6160f, 0.1153f, 0.8389f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.6994f, 0.5758f, 0.9283f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.5965f, 0.2400f, 0.8800f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.5516f, 0.2600f, 0.9800f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.2313f, 0.1024f, 0.7204f, 0.4000f)},
			{ImGuiCol_ButtonHovered, ImVec4(0.5739f, 0.2600f, 0.9800f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.4792f, 0.0600f, 0.9800f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.5699f, 0.2600f, 0.9800f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.6428f, 0.2600f, 0.9800f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.5739f, 0.2600f, 0.9800f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(0.3223f, 0.0000f, 1.0000f, 0.8246f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.3797f, 0.1000f, 0.7500f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.3649f, 0.1000f, 0.7500f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.5739f, 0.2600f, 0.9800f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.4970f, 0.2600f, 0.9800f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.5699f, 0.2600f, 0.9800f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.2872f, 0.1472f, 0.4929f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.5330f, 0.2600f, 0.9800f, 0.8000f)},
			{ImGuiCol_TabActive, ImVec4(0.4144f, 0.1743f, 0.9194f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0981f, 0.0700f, 0.1500f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3509f, 0.1853f, 0.5213f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.6792f, 0.2600f, 0.9800f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.2259f, 0.1795f, 0.2559f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(0.2120f, 0.0954f, 0.2796f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(0.2061f, 0.1294f, 0.3175f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0788f, 0.0261f, 0.1896f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(0.4200f, 0.2696f, 0.4346f, 0.8531f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.5516f, 0.2600f, 0.9800f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.2647f, 0.2123f, 0.2986f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
	{"Red", {
		{  
			{ImGuiCol_TextDisabled, ImVec4(0.4976f, 0.3929f, 0.2830f, 1.0000f)},
			{ImGuiCol_WindowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_ChildBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_PopupBg, ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f)},
			{ImGuiCol_Border, ImVec4(0.9621f, 0.0593f, 0.0593f, 0.5000f)},
			{ImGuiCol_BorderShadow, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_FrameBg, ImVec4(0.2489f, 0.2403f, 0.2300f, 0.5400f)},
			{ImGuiCol_FrameBgHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.4000f)},
			{ImGuiCol_FrameBgActive, ImVec4(0.9800f, 0.2600f, 0.2600f, 0.6700f)},
			{ImGuiCol_TitleBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TitleBgActive, ImVec4(0.0549f, 0.0537f, 0.0532f, 1.0000f)},
			{ImGuiCol_TitleBgCollapsed, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f)},
			{ImGuiCol_MenuBarBg, ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f)},
			{ImGuiCol_ScrollbarBg, ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f)},
			{ImGuiCol_ScrollbarGrab, ImVec4(0.3100f, 0.3100f, 0.3100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabHovered, ImVec4(0.4100f, 0.4100f, 0.4100f, 1.0000f)},
			{ImGuiCol_ScrollbarGrabActive, ImVec4(0.5100f, 0.5100f, 0.5100f, 1.0000f)},
			{ImGuiCol_CheckMark, ImVec4(0.9800f, 0.5262f, 0.2600f, 1.0000f)},
			{ImGuiCol_SliderGrab, ImVec4(0.8800f, 0.4584f, 0.2400f, 1.0000f)},
			{ImGuiCol_SliderGrabActive, ImVec4(0.9800f, 0.5466f, 0.2600f, 1.0000f)},
			{ImGuiCol_Button, ImVec4(0.6709f, 0.1897f, 0.2445f, 0.4000f)},
			{ImGuiCol_ButtonHovered, ImVec4(1.0000f, 0.5823f, 0.0000f, 1.0000f)},
			{ImGuiCol_ButtonActive, ImVec4(0.9800f, 0.4001f, 0.0600f, 1.0000f)},
			{ImGuiCol_Header, ImVec4(0.9800f, 0.5466f, 0.2600f, 0.3100f)},
			{ImGuiCol_HeaderHovered, ImVec4(0.9800f, 0.6900f, 0.2600f, 0.8000f)},
			{ImGuiCol_HeaderActive, ImVec4(0.9800f, 0.7309f, 0.2600f, 1.0000f)},
			{ImGuiCol_Separator, ImVec4(1.0000f, 0.0000f, 0.1247f, 0.5000f)},
			{ImGuiCol_SeparatorHovered, ImVec4(0.7500f, 0.3588f, 0.1000f, 0.7800f)},
			{ImGuiCol_SeparatorActive, ImVec4(0.7500f, 0.3033f, 0.1000f, 1.0000f)},
			{ImGuiCol_ResizeGrip, ImVec4(0.9479f, 0.5534f, 0.0898f, 0.2000f)},
			{ImGuiCol_ResizeGripHovered, ImVec4(0.9800f, 0.5466f, 0.2600f, 0.6700f)},
			{ImGuiCol_ResizeGripActive, ImVec4(0.9800f, 0.6081f, 0.2600f, 0.9500f)},
			{ImGuiCol_Tab, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.8600f)},
			{ImGuiCol_TabHovered, ImVec4(0.8026f, 0.0696f, 0.1400f, 0.6303f)},
			{ImGuiCol_TabActive, ImVec4(0.6160f, 0.0936f, 0.1769f, 1.0000f)},
			{ImGuiCol_TabUnfocused, ImVec4(0.0332f, 0.0295f, 0.0215f, 0.9700f)},
			{ImGuiCol_TabUnfocusedActive, ImVec4(0.3602f, 0.1633f, 0.0478f, 1.0000f)},
			{ImGuiCol_DockingPreview, ImVec4(0.9800f, 0.5876f, 0.2600f, 0.7000f)},
			{ImGuiCol_DockingEmptyBg, ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f)},
			{ImGuiCol_PlotLines, ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f)},
			{ImGuiCol_PlotLinesHovered, ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f)},
			{ImGuiCol_PlotHistogram, ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f)},
			{ImGuiCol_PlotHistogramHovered, ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableHeaderBg, ImVec4(0.0759f, 0.0748f, 0.0737f, 1.0000f)},
			{ImGuiCol_TableBorderStrong, ImVec4(1.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableBorderLight, ImVec4(1.0000f, 0.0000f, 0.0000f, 1.0000f)},
			{ImGuiCol_TableRowBg, ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f)},
			{ImGuiCol_TableRowBgAlt, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f)},
			{ImGuiCol_TextSelectedBg, ImVec4(0.9800f, 0.7309f, 0.2600f, 0.3500f)},
			{ImGuiCol_DragDropTarget, ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f)},
			{ImGuiCol_NavHighlight, ImVec4(0.9145f, 0.0921f, 0.2133f, 1.0000f)},
			{ImGuiCol_NavWindowingHighlight, ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f)},
			{ImGuiCol_NavWindowingDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f)},
			{ImGuiCol_ModalWindowDimBg, ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f)},
		},
		{ 
		}
	}},
};

#pragma endregion

#pragma region Theme Functions

/**
* @brief Applies the Selected ThemeSet to the ImGui Window using ImGui::PushStyleColor and ImGui::PushStyleVar
* 
* @param const std::string& themeName The name of the ThemeSet to Load
* @return int The number of colors pushed + (the number of styles pushed << 16)
*/
int PushTheme(const std::string& themeName) {
	int colorCount = 0;
	int styleCount = 0;

	auto themeIter = themeSet.find(themeName);
	if (themeIter != themeSet.end()) {
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

/**
* @brief Pop a theme from the ImGui context
* @param int popCounts The number of colors pushed + (the number of styles pushed << 16)
*/
void PopTheme(int popCounts)
{
	int colorCount = popCounts & 0xFFFF;
	int styleCount = popCounts >> 16;
	ImGui::PopStyleColor(colorCount);
	ImGui::PopStyleVar(styleCount);
}


/**
* @brief Draw a combo box to select a theme
* @param const std::string& currentTheme The current theme name
* @param const char* winName The name of the window
* @return std::string The name of the selected theme
*/
std::string DrawThemePicker(const std::string& currentTheme, const char* winName) {
	std::vector<std::string> themeNames;
	for (const auto& theme : themeSet) {
		themeNames.push_back(theme.first);
	}

	static int selectedThemeIndex = 0;
	for (size_t i = 0; i < themeNames.size(); ++i) {
		if (currentTheme == themeNames[i]) {
			selectedThemeIndex = static_cast<int>(i);
			break;
		}
	}

	if (ImGui::Combo(winName, &selectedThemeIndex, [](void* data, int idx, const char** out_text) {
		const auto& themeNames = *static_cast<const std::vector<std::string>*>(data);
		*out_text = themeNames[idx].c_str();
		return true;
		}, static_cast<void*>(&themeNames), themeNames.size())) {
		return themeNames[selectedThemeIndex];
	}

	return currentTheme;
}

#pragma endregion
