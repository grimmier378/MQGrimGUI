#pragma once
#include <vector>
#include "imgui.h"
#include "imgui_internal.h"
#include "mq/base/String.h"

/**
* @namespace ImGuiTheme
* 
* @brief Contains functions and for chaniging the ImGui window themes.
*		 As well as a picker for selecting the theme to apply.
*/
namespace ImGuiTheme
{

	constexpr int ThemeCount = 11;

#pragma region Themes Set Functions

	inline void DrawThemeDefault(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeGrape(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0168f, 0.0022f, 0.0474f, 0.9400f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0630f, 0.0059f, 0.1564f, 0.0000f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.0543f, 0.0119f, 0.1564f, 0.9400f);
		colors[ImGuiCol_Border] = ImVec4(0.2946f, 0.1528f, 0.3981f, 0.5000f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.2631f, 0.1600f, 0.4800f, 0.5400f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4306f, 0.2600f, 0.9800f, 0.4000f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.5334f, 0.2600f, 0.9800f, 0.6700f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.1031f, 0.0037f, 0.1943f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.3544f, 0.1600f, 0.4800f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0788f, 0.0133f, 0.1857f, 0.5100f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.1400f, 0.1400f, 0.1400f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4585f, 0.1383f, 0.6209f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4136f, 0.2610f, 0.6635f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6160f, 0.1153f, 0.8389f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.6994f, 0.5758f, 0.9283f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.5965f, 0.2400f, 0.8800f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5516f, 0.2600f, 0.9800f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.2313f, 0.1024f, 0.7204f, 0.4000f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.5739f, 0.2600f, 0.9800f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.4792f, 0.0600f, 0.9800f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.5699f, 0.2600f, 0.9800f, 0.3100f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.6428f, 0.2600f, 0.9800f, 0.8000f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.5739f, 0.2600f, 0.9800f, 1.00f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7500f, 0.3588f, 0.1000f, 0.7800f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.7500f, 0.3033f, 0.1000f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2259f, 0.1795f, 0.2559f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.70f);   // Background of odd rows
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.20f, 0.22f, 0.27f, 0.80f);   // Background of even rows
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeRed(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_TextDisabled] = ImVec4(0.4976f, 0.3929f, 0.2830f, 1.0000f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.0800f, 0.0800f, 0.0800f, 0.9400f);
		colors[ImGuiCol_Border] = ImVec4(0.9621f, 0.0593f, 0.0593f, 0.5000f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.2489f, 0.2403f, 0.2300f, 0.5400f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.9800f, 0.6900f, 0.2600f, 0.4000f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.9800f, 0.2600f, 0.2600f, 0.6700f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.0000f, 0.0000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.0549f, 0.0537f, 0.0532f, 1.0000f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.5100f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.1400f, 0.1400f, 0.1400f, 1.0000f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0200f, 0.0200f, 0.0200f, 0.5300f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3100f, 0.3100f, 0.3100f, 1.0000f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4100f, 0.4100f, 0.4100f, 1.0000f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5100f, 0.5100f, 0.5100f, 1.0000f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.9800f, 0.5262f, 0.2600f, 1.0000f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.8800f, 0.4584f, 0.2400f, 1.0000f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9800f, 0.5466f, 0.2600f, 1.0000f);
		colors[ImGuiCol_Button] = ImVec4(0.6709f, 0.1897f, 0.2445f, 0.4000f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.0000f, 0.5823f, 0.0000f, 1.0000f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.9800f, 0.4001f, 0.0600f, 1.0000f);
		colors[ImGuiCol_Header] = ImVec4(0.9800f, 0.5466f, 0.2600f, 0.3100f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.9800f, 0.6900f, 0.2600f, 0.8000f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.9800f, 0.7309f, 0.2600f, 1.0000f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7500f, 0.3588f, 0.1000f, 0.7800f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.7500f, 0.3033f, 0.1000f, 1.0000f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.9479f, 0.5534f, 0.0898f, 0.2000f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.9800f, 0.5466f, 0.2600f, 0.6700f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9800f, 0.6081f, 0.2600f, 0.9500f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.70f); colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.2000f, 0.2000f, 0.2000f, 1.0000f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.6100f, 0.6100f, 0.6100f, 1.0000f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0000f, 0.4300f, 0.3500f, 1.0000f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.9000f, 0.7000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0000f, 0.6000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0759f, 0.0748f, 0.0737f, 1.0000f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(1.0000f, 0.0000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(1.0000f, 0.0000f, 0.0000f, 1.0000f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.0000f, 0.0000f, 0.0000f, 0.0000f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0000f, 1.0000f, 1.0000f, 0.0600f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.9800f, 0.7309f, 0.2600f, 0.3500f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.0000f, 1.0000f, 0.0000f, 0.9000f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.9145f, 0.0921f, 0.2133f, 1.0000f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0000f, 1.0000f, 1.0000f, 0.7000f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8000f, 0.8000f, 0.8000f, 0.2000f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8000f, 0.8000f, 0.8000f, 0.3500f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}
	}

	inline void DrawThemeBurnt(bool doRounding = false)
	{

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.96f, 0.47f, 0.06f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.24f, 0.23f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.98f, 0.69f, 0.26f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.98f, 0.59f, 0.26f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.98f, 0.53f, 0.26f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.46f, 0.24f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98f, 0.55f, 0.26f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.67f, 0.35f, 0.19f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.58f, 0.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.98f, 0.40f, 0.06f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.98f, 0.55f, 0.26f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.98f, 0.69f, 0.26f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.98f, 0.73f, 0.26f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(1.00f, 0.28f, 0.00f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.36f, 0.10f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.30f, 0.10f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.95f, 0.55f, 0.09f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.98f, 0.55f, 0.26f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.98f, 0.61f, 0.26f, 0.95f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(1.00f, 0.46f, 0.00f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(1.00f, 0.46f, 0.00f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.98f, 0.73f, 0.26f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.91f, 0.09f, 0.21f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeMono(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.85f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.36f, 0.37f, 0.37f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.64f, 0.65f, 0.68f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.55f, 0.59f, 0.63f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.26f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.29f, 0.28f, 0.28f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.96f, 0.98f, 0.99f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.64f, 0.65f, 0.66f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.76f, 0.78f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.38f, 0.38f, 0.39f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.49f, 0.50f, 0.51f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.44f, 0.44f, 0.52f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.66f, 0.67f, 0.69f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.46f, 0.52f, 0.60f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.58f, 0.58f, 0.63f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.29f, 0.31f, 0.33f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.60f, 0.63f, 0.66f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.52f, 0.53f, 0.55f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.54f, 0.56f, 0.58f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.41f, 0.42f, 0.95f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.89f, 0.81f, 0.54f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.56f, 0.57f, 0.58f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.71f, 0.74f, 0.77f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
		
		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeLime(bool doRounding = false)
	{

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.03f, 0.01f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.03f, 0.01f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.01f, 0.08f, 0.03f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.48f, 0.98f, 0.54f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.09f, 0.54f, 0.66f, 0.16f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.15f, 0.71f, 0.46f, 0.75f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.34f, 0.86f, 0.40f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.09f, 0.02f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.25f, 0.05f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.01f, 0.07f, 0.02f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.15f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.66f, 0.28f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.12f, 0.78f, 0.23f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.16f, 0.67f, 0.14f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.86f, 0.40f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.88f, 0.65f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.98f, 0.64f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.98f, 0.42f, 0.40f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.10f, 0.56f, 0.28f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.14f, 0.56f, 0.33f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.25f, 1.00f, 0.12f, 0.19f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.52f, 0.11f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.52f, 0.11f, 0.80f);
		colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.76f, 0.25f, 0.82f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.15f, 0.76f, 0.25f, 0.82f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.15f, 0.76f, 0.25f, 0.82f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.66f, 0.28f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.12f, 0.78f, 0.23f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16f, 0.67f, 0.14f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.60f, 0.23f, 0.41f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.15f, 0.60f, 0.23f, 0.64f);
		colors[ImGuiCol_TabActive] = ImVec4(0.03f, 0.36f, 0.00f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.15f, 0.08f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.42f, 0.20f, 1.00f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.02f, 0.36f, 0.06f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.22f, 0.98f, 0.32f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.23f, 0.78f, 0.15f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.08f, 0.21f, 0.02f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.15f, 0.76f, 0.25f, 0.82f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.15f, 0.76f, 0.25f, 0.82f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.12f, 0.65f, 0.14f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.32f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.98f, 0.50f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeNight(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;
		colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.02f, 0.04f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.03f, 0.03f, 0.05f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.07f, 0.06f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
		colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.07f, 0.10f, 0.15f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.05f, 0.05f, 0.08f, 1.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.06f, 0.08f, 0.11f, 1.00f);
		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
		colors[ImGuiCol_DockingPreview] = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.70f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}

	}

	inline void DrawThemeCottonCandy(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(0.78f, 0.97f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.98f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.06f, 0.08f, 0.98f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.05f, 0.12f, 0.98f);
		colors[ImGuiCol_Border] = ImVec4(0.94f, 0.30f, 0.90f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.65f, 0.34f, 0.50f, 0.80f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.50f, 0.64f, 0.83f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.94f, 0.30f, 0.90f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.03f, 0.09f, 0.18f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.29f, 0.47f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.03f, 0.03f, 0.23f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.60f, 0.20f, 0.55f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.26f, 0.37f, 0.75f, 0.99f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.57f, 0.13f, 0.53f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 0.91f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.98f, 0.00f, 0.84f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.06f, 0.96f, 1.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.57f, 0.13f, 0.53f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.98f, 0.00f, 0.84f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.30f, 0.52f, 0.91f, 0.67f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.82f, 0.38f, 0.90f, 0.64f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.24f, 0.59f, 0.73f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.72f, 0.06f, 0.85f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.72f, 0.00f, 1.00f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.56f, 0.23f, 0.49f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.39f, 0.68f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.56f, 0.23f, 0.49f, 0.86f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.24f, 0.48f, 0.64f, 0.81f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.92f, 0.54f, 0.99f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.46f, 0.22f, 0.49f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.45f, 0.18f, 0.52f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.24f, 0.59f, 0.73f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.23f, 0.43f, 0.65f, 0.33f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.05f, 0.42f, 0.49f, 0.11f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.67f, 0.33f, 0.92f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		// Rounding
		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}
	}

	inline void DrawThemeBlackDevil(bool doRounding = false)
	{

		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.41f, 0.31f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.12f, 0.06f, 0.39f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.78f);
		colors[ImGuiCol_Border] = ImVec4(0.39f, 0.00f, 0.00f, 0.78f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.24f, 0.35f, 0.78f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.12f, 0.78f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.12f, 0.16f, 0.20f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.78f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.12f, 0.35f, 0.35f, 0.78f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.59f, 0.24f, 0.78f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.12f, 0.59f, 0.24f, 0.78f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.12f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.12f, 0.59f, 0.24f, 0.78f);
		colors[ImGuiCol_Button] = ImVec4(0.35f, 0.35f, 0.12f, 0.78f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.47f, 0.24f, 0.78f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.59f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_Header] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.12f, 0.35f, 0.35f, 0.78f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.12f, 0.59f, 0.24f, 0.78f);
		colors[ImGuiCol_Separator] = ImVec4(0.35f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.12f, 0.35f, 0.35f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.59f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.59f, 0.35f, 0.35f, 0.78f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.59f, 0.24f, 0.24f, 0.78f);
		colors[ImGuiCol_Tab] = ImVec4(0.35f, 0.35f, 0.12f, 0.78f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.47f, 0.24f, 0.78f);
		colors[ImGuiCol_TabActive] = ImVec4(0.59f, 0.35f, 0.24f, 0.78f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.06f, 0.12f, 0.16f, 0.78f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.59f, 0.35f, 0.35f, 0.78f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.78f, 0.39f, 0.78f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 0.78f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.35f, 0.39f, 0.78f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.20f, 0.59f, 0.59f, 0.78f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 0.78f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 0.78f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 0.78f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.68f, 0.98f, 0.26f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.41f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.98f, 0.59f, 0.26f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}
	}

	inline void DrawThemeMoonLight(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.27f, 0.32f, 0.45f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.10f, 0.12f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_Border] = ImVec4(0.16f, 0.17f, 0.19f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.17f, 0.19f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.17f, 0.19f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.16f, 0.17f, 0.19f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.12f, 0.14f, 0.15f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.97f, 1.00f, 0.50f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.97f, 1.00f, 0.50f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.80f, 0.50f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.18f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.14f, 0.16f, 0.21f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.13f, 0.15f, 0.19f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.16f, 0.18f, 0.25f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.16f, 0.18f, 0.25f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.97f, 1.00f, 0.50f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.09f, 0.10f, 1.00f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.27f, 0.57f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.52f, 0.60f, 0.70f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.04f, 0.98f, 0.98f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.88f, 0.80f, 0.56f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.05f, 0.06f, 0.07f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.12f, 0.13f, 0.15f, 1.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.10f, 0.11f, 0.12f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.50f, 0.51f, 1.00f, 1.00f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.27f, 0.29f, 1.00f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.50f, 0.51f, 1.00f, 1.00f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.20f, 0.18f, 0.55f, 0.50f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.18f, 0.55f, 0.50f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}
	}

	inline void DrawThemeEnemyMouse(bool doRounding = false)
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		ImVec4* colors = style->Colors;

		colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.16f, 0.24f, 0.22f, 0.60f);
		colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.21f, 0.73f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
		colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.43f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
		colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.50f, 0.50f, 0.33f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.50f, 0.50f, 0.47f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.70f, 0.70f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.35f, 0.57f, 0.86f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.41f, 0.68f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.71f, 0.54f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);

		if (doRounding)
		{
			style->FrameRounding = 9.0f;
			style->GrabRounding = 12.0f;
			style->PopupRounding = 10.0f;
			style->ScrollbarRounding = 12.0f;
			style->TabRounding = 10.0f;
			style->WindowRounding = 10.0f;
			style->ChildRounding = 8.0f;
		}
	}

#pragma endregion

	enum class Theme
	{
		Grape,
		Red,
		Burnt,
		Monochrome,
		Lime,
		Night,
		CottonCandy,
		BlackDevil,
		MoonLight,
		EnemyMouse,
		Default,
	};

	static const std::vector<char*> ThemeNames
	{
		"Grape",
		"Red",
		"Burnt",
		"Monochrome",
		"Lime",
		"Night",
		"Cotton Candy",
		"Black Devil",
		"Moon Light",
		"Enemy Mouse",
		"Default",
	};

	inline void (*ThemeFunctions[])(bool doRounding) = {
		DrawThemeGrape,
		DrawThemeRed,
		DrawThemeBurnt,
		DrawThemeMono,
		DrawThemeLime,
		DrawThemeNight,
		DrawThemeCottonCandy,
		DrawThemeBlackDevil,
		DrawThemeMoonLight,
		DrawThemeEnemyMouse,
		DrawThemeDefault,
	};


	/**
	* @brief Draw a theme picker combo box
	* @param int themeIdx The index of the current theme
	* @param const char* winName The name of the window
	* @return int The index of the selected theme
	*/
	inline int DrawThemePicker(int themeIdx, const char* winName)
	{
		if (themeIdx > ThemeCount - 1)
			themeIdx = ThemeCount - 1;

		int selectedThemeIndex = ThemeCount - 1;

		if (selectedThemeIndex != themeIdx)
			selectedThemeIndex = themeIdx;

		if (ImGui::BeginCombo(winName, ThemeNames[selectedThemeIndex]))
		{
			for (int i = 0; i < ThemeCount; ++i)
			{
				bool isSelected = (selectedThemeIndex == i);
				if (ImGui::Selectable(ThemeNames[i], isSelected))
					selectedThemeIndex = i;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		return selectedThemeIndex;
	}

	/**
	* @brief Apply a theme to the window and Backup the Original to replace when done
	*
	* @param int themeIdx The index of the theme to apply
	* @param bool doRounding Should the theme have rounding applied
	*
	* @return ImGuiStyle The old style of the window so you can replace it later
	*/
	inline ImGuiStyle ApplyTheme(int themeIdx, bool doRounding = false)
	{
		ImGuiStyle oldStyle = ImGui::GetStyle();

		if (themeIdx > ThemeCount - 1)
			themeIdx = ThemeCount - 1;

		if (ThemeFunctions[themeIdx])
			ThemeFunctions[themeIdx](doRounding);

		return oldStyle;
	}

	/**
	* @brief Reset the theme to the original style
	*
	* @param ImGuiStyle oldStyle The old style of the window
	*/
	inline void ResetTheme(ImGuiStyle& oldStyle)
	{
		ImGui::GetStyle() = oldStyle;
	}

}
