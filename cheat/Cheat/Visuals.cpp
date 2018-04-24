#include "Visuals.h"
#include "../Core/Math.h"
#include "../Core/Draw.h"
#include "../Core/Font.h"
#include "../Core/options.h"

#include <iomanip>
#include <sstream>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include "../Core/Hooks.h"

void Visuals::PaintTraverse()
{
	Overlay();

	if (g_LocalPlayer && g_EngineClient->IsInGame())
	{
		SniperCrosshair();
		SniperCrosshair();
		Radar();
		NightMode();

		if (g_Options.VIS_ESP)
		{
			for (int i = 1; i <= g_GlobalVars->maxClients; i++)
			{
				auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
				if (!pEntity) continue;
				if (pEntity == g_LocalPlayer) continue;
				if (pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;
				if (pEntity->IsDormant() || !pEntity->IsAlive()) continue;

				Vector max = pEntity->GetCollideable()->OBBMaxs();
				Vector top3D, pos3D;
				Vector2D top, pos;
				pos3D = pEntity->m_vecOrigin();
				top3D = pEntity->m_fFlags() & FL_DUCKING ? pos3D + Vector(0, 0, 56.f) : pos3D + Vector(0, 0, 72.f);

				if (!Math::WorldToScreen(pos3D, pos) || !Math::WorldToScreen(top3D, top))
					continue;

				float height = (pos.y - top.y);
				float width = height / 4.f;

				if (g_Options.VIS_ESP_Box) ESP_Box(top.x, top.y, width, height, pEntity);
				if (g_Options.VIS_ESP_Name) ESP_Name(top.x, top.y, width, height, pEntity);
				if (g_Options.VIS_ESP_Health) ESP_Health(top.x, top.y, width, height, pEntity);

			}
		}
	}
}

void Visuals::Radar()
{
	if (!g_Options.VIS_ESP_Radar)
		return;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		if (i == g_EngineClient->GetLocalPlayer())
			continue;

		auto pPlayer = reinterpret_cast< C_BasePlayer* >(g_EntityList->GetClientEntity(i));
		if (!pPlayer)
			continue;

		pPlayer->m_bSpotted() = true;
	}
}

bool done = false;

void Visuals::NightMode()
{
	if (g_Options.VIS_ESP_NightMode)
	{
		if (!done)
		{


			static auto sv_skyname = g_CVar->FindVar("sv_skyname");
			static auto r_DrawSpecificStaticProp = g_CVar->FindVar("r_DrawSpecificStaticProp");
			r_DrawSpecificStaticProp->SetValue(1);
			sv_skyname->SetValue("sky_csgo_night02");

			for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
			{
				IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

				if (!pMaterial)
					continue;

				const char* group = pMaterial->GetTextureGroupName();
				const char* name = pMaterial->GetName();

				if (strstr(group, "World textures"))
				{
					pMaterial->ColorModulate(0.10, 0.10, 0.10);
				}
				if (strstr(group, "StaticProp"))
				{
					pMaterial->ColorModulate(0.30, 0.30, 0.30);
				}
				if (strstr(name, "models/props/de_dust/palace_bigdome"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				}
				if (strstr(name, "models/props/de_dust/palace_pillars"))
				{
					pMaterial->ColorModulate(0.30, 0.30, 0.30);
				}

				if (strstr(group, "Particle textures"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
				}
				done = true;
			}

		}
	}
	else
	{
		if (done)
		{
			for (MaterialHandle_t i = g_MatSystem->FirstMaterial(); i != g_MatSystem->InvalidMaterial(); i = g_MatSystem->NextMaterial(i))
			{
				IMaterial *pMaterial = g_MatSystem->GetMaterial(i);

				if (!pMaterial)
					continue;

				const char* group = pMaterial->GetTextureGroupName();
				const char* name = pMaterial->GetName();

				if (strstr(group, "World textures"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(group, "StaticProp"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(name, "models/props/de_dust/palace_bigdome"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
				}
				if (strstr(name, "models/props/de_dust/palace_pillars"))
				{

					pMaterial->ColorModulate(1, 1, 1);
				}
				if (strstr(group, "Particle textures"))
				{
					pMaterial->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
				}
			}
			done = false;
		}
	}
}

void Visuals::DrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t &state, const ModelRenderInfo_t &info, matrix3x4_t* matrix)
{
	const auto model = info.pModel;
    static auto fnDME = Hooks::MDLRenderHook.get_original<Hooks::DrawModelExecute>(Index::DrawModelExecute);


	auto texturedColor = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL); //textured color chams
	auto flatColor = g_MatSystem->FindMaterial("simple_flat", TEXTURE_GROUP_MODEL); //flat color chams
	if (strstr(model->szName, "models/player") != nullptr)
	{
		auto pEntity = C_BasePlayer::GetPlayerByIndex(info.entity_index);

        const auto enemy = pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();

            const auto clr_front = enemy ? Color::Legitware : Color::Legitware;
            const auto clr_back = enemy ? Color::Red : Color::Red;

		if (pEntity
			&& !pEntity->IsDormant()
			&& pEntity->IsAlive()
			&& pEntity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{

			if (g_Options.VIS_Chams)
			{
					//texturedColor->ColorModulate(g_Options.CHAMS[0], g_Options.CHAMS[1], g_Options.CHAMS[2]);
					//texturedColor->AlphaModulate(1.f);
					//g_ModelRender->ForcedMaterialOverride(texturedColor);


                OverrideMaterial(true, g_Options.chams_player_flat, g_Options.chams_player_wireframe, false, clr_back);
                fnDME(g_ModelRender, ctx, state, info, matrix);
                OverrideMaterial(false, g_Options.chams_player_flat, g_Options.chams_player_wireframe, false, clr_front);
			}

		}
    }
	else
	{

	}
}

void Visuals::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba)
{
    IMaterial* material = nullptr;

    if(flat) {
        if(ignoreZ)
            material = materialFlatIgnoreZ;
        else
            material = materialFlat;
    } else {
        if(ignoreZ)
            material = materialRegularIgnoreZ;
        else
            material = materialRegular;
    }

    g_ModelRender->ForcedMaterialOverride(material);
}

void Visuals::Overlay()
{
	if (g_Options.VIS_ESP) Draw::Text(10, 8, L"ESP", Font::Get().Overlay, Color::White);
}

void Visuals::SniperCrosshair()
{
	if (!g_Options.VIS_ESP_SniperCrosshair)
		return;

	auto pWeapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!pWeapon) return;
	if (!pWeapon->IsSniper()) return;

	int ScreenWidth, ScreenHeight;
	g_EngineClient->GetScreenSize(ScreenWidth, ScreenHeight);

	auto Center = Vector2D(ScreenWidth / 2, ScreenHeight / 2);

	Draw::FilledCircle(Center, 10, 1.5f, Color::White);
	Draw::Circle(Center, 10, 1.5f, Color::Black);
}

void Visuals::ESP_Box(int x, int y, int w, int h, C_BasePlayer* pEntity)
{
	g_VGuiSurface->DrawSetColor(Color::Black);
	g_VGuiSurface->DrawFilledRectFade(x - w, y, x + w, y + h, 20, 100, false);
	Draw::Rectangle(x - w, y, x + w, y + h, Color::Black);
}

void Visuals::ESP_Name(int x, int y, int w, int h, C_BasePlayer* pEntity)
{
	wchar_t buf[80];

	player_info_t info = pEntity->GetPlayerInfo();

	if (MultiByteToWideChar(CP_UTF8, 0, info.szName, -1, buf, 80) > 0)
		Draw::Text(x, y - 7, buf, Font::Get().ESP, Color::White, true);
}

void Visuals::ESP_Health(int x, int y, int w, int h, C_BasePlayer* pEntity)
{
	// clamp it to 100
	int HealthValue = std::max(0, std::min(pEntity->m_iHealth(), 100));
	float HealthPerc = HealthValue / 100.f;
	float hp = pEntity->m_iHealth();
	float hue = ((6.0f * hp) / 5.0f) / 360.0f;
	Color clr = Color::FromHSB(hue, 1.0f, 1.0f);

	int barx = x - w;
	int bary = y + h + 3;
	int barw = w *= 2;
	int barh = 4;

	float Width = (w * HealthPerc);
	barw = (int)(Width);

	Vertex_t Verts[4];
	Verts[0].Init(Vector2D(barx, bary));
	Verts[1].Init(Vector2D(barx + w + 5, bary));
	Verts[2].Init(Vector2D(barx + w, bary + 5));
	Verts[3].Init(Vector2D(barx - 5, bary + 5));

	Draw::TexturedPolygon(4, Verts, Color(10, 10, 10, 255));
	Draw::PolyLine(Verts, 4, Color(255, 255, 255, 170));

	Verts[0].Init(Vector2D(barx + 1, bary + 1));
	Verts[1].Init(Vector2D(barx + barw + 5, bary + 1));
	Verts[2].Init(Vector2D(barx + barw, bary + 5));
	Verts[3].Init(Vector2D(barx - 4, bary + 5));

	Draw::TexturedPolygon(4, Verts, clr);

	Verts[0].Init(Vector2D(barx + 1, bary + 1));
	Verts[1].Init(Vector2D(barx + barw + 2, bary + 1));
	Verts[2].Init(Vector2D(barx + barw, bary + 2));
	Verts[3].Init(Vector2D(barx - 2, bary + 2));

	Draw::TexturedPolygon(4, Verts, Color(255, 255, 255, 40));
}