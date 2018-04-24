#pragma once

struct SkinInfo_t {
	int weapon_skin_id;
	int weapon_seed;
	int weapon_stat_trak;
	float weapon_wear;

	SkinInfo_t(int skin = 0, int seed = 0, int stattrak = -1, float wear = 0.000000001f) {
		weapon_skin_id = skin;
		weapon_seed = seed;
		weapon_stat_trak = stattrak;
		weapon_wear = wear;
	}
};

#include <string>
#include "../SDK/Misc/Color.h"

#define OPTION(type, var, val) type var = val

class Config
{
public:
#pragma region Visuals
	//float CHAMS[3] = { 1.5f, 1.5f, 0.15f };
    float CHAMS[3] = { 0.000f, 0.749f, 1.000f };
    
	SkinInfo_t skins[5035];
	bool menu_open;
	OPTION(bool, VIS_Chams, true);
    OPTION(bool, chams_player_wireframe, false);
    OPTION(bool, chams_player_flat, false);
    OPTION(Color, color_esp_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_esp_ally_occluded, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_occluded, Color(255, 0, 0));
	OPTION(bool, VIS_ESP, false);
	OPTION(bool, VIS_ESP_Box, false);
	OPTION(bool, VIS_ESP_Bone, false);
	OPTION(bool, VIS_ESP_Name, false);
	OPTION(bool, VIS_ESP_Health, false);
	OPTION(bool, VIS_ESP_Radar, true);
	OPTION(bool, VIS_ESP_NightMode, false);

	OPTION(bool, VIS_ESP_SniperCrosshair, true);
#pragma endregion
#pragma region Misc

	int MISC_ChamsType;
	OPTION(bool, MISC_Bunnyhop, false);
	OPTION(bool, MISC_AutoAccept, true);
	OPTION(bool, MISC_RankReveal, true);
	OPTION(bool, MISC_Backtrack, true);
	OPTION(int, MISC_ViewmodelFOV, 5);
	OPTION(std::wstring, MISC_ClanTag, L"");
#pragma endregion
};

extern Config g_Options;
