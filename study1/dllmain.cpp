#include "pch.h"
#include <vector>
#include "client.dll.hpp"
#include "offsets.hpp"



HMODULE SelfhModule;
const float on = 86400.f;
uintptr_t Client = 0;
HWND g_hWnd = nullptr;
inline bool nearstAim = false;
inline int AimPoint = 6;
inline bool Esp = false;
const int AimtoPoints[] = { 0,6,4,1 };
inline int m_Aimpoint = 1;
inline Vector3 headPos;
inline Vector3 chestPos;
inline Vector3 waistPos;
static float AimRange = 400.f;
static float Smooth = 1.0f;
static bool unLoadDLL = false;



inline void GetViewAngle(float& yaw, float& pith) {
    yaw = ReadMemory<float>(Client + client_dll::dwViewAngles);
    pith = ReadMemory<float>(Client + client_dll::dwViewAngles + 0x04);
}
inline void SetViewAngle(float yaw, float pith) {
    WriteMemory<float>(Client + client_dll::dwViewAngles, yaw);
    WriteMemory<float>(Client + client_dll::dwViewAngles + 0x04, pith);

}

DWORD64 GetPawnAddress(DWORD64 Controller)
{
	DWORD64 Pawn = ReadMemory<DWORD64>(Controller + 0x2A0);
	return Pawn;
}



bool WorldToScreen(DWORD64 m__Matrix, Vector3& WorldPos, Vector2& ScreenPos, float width, float heigh)
{
    float Matrix[4][4] = { 0 };
    if (m__Matrix)
    {
        memcpy(Matrix, (void*)m__Matrix, 64);
        //if !m__Matrix Crash
    }
    //ReadProcessMemory(GetCurrentProcess(), (void*)m__Matrix, Matrix, 64, NULL);
    //Matrix = ReadMemory<float*>(m__Matrix);
    float ViewW = Matrix[3][0] * WorldPos.x + Matrix[3][1] * WorldPos.y + Matrix[3][2] * WorldPos.z + Matrix[3][3];

    float Scaling = 1.f / ViewW;
    float View_width = width / 2;
    float View_heigh = heigh / 2;
    if (ViewW <= 0.01)
        return false;
    float Screen_X = View_width + (Matrix[0][0] * WorldPos.x + Matrix[0][1] * WorldPos.y + Matrix[0][2] * WorldPos.z + Matrix[0][3]) * Scaling * View_width;
    float Screen_Y = View_heigh - (Matrix[1][0] * WorldPos.x + Matrix[1][1] * WorldPos.y + Matrix[1][2] * WorldPos.z + Matrix[1][3]) * Scaling * View_heigh;


    ScreenPos.x = Screen_X;
    ScreenPos.y = Screen_Y;
    return true;
}
DWORD64 m_Matrix;

static BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
    const auto isMainWindow = [handle]() {
        return GetWindow(handle, GW_OWNER) == nullptr && IsWindowVisible(handle) && handle != GetConsoleWindow();
        };

    DWORD pID = 0;
    GetWindowThreadProcessId(handle, &pID);

    if (GetCurrentProcessId() != pID || !isMainWindow()) {
        return TRUE;
    }

    *reinterpret_cast<HWND*>(lParam) = handle;
    return FALSE;
}

float GetDistanceByPos(float width, float height, float PosX, float PosY)
{
    return sqrtf((width / 2 - PosX) * (width / 2 - PosX) + (height / 2 - PosY) * (height / 2 - PosY));
}

void myFuction()
{
    EnumWindows(::EnumWindowsCallback, reinterpret_cast<LPARAM>(&g_hWnd));
    Client = GetClientModule();
    while (!unLoadDLL)
    {
        if (GetAsyncKeyState(VK_END))
        {
			unLoadDLL = true;
		}
        if (GetAsyncKeyState(VK_F2))
        {
			Esp = !Esp;
            if (Esp)
            {
                Beep(2000, 200);
			}
			else
			{
				Beep(1000, 400);
            }
            while (GetAsyncKeyState(VK_F2))
            {

            }
		}
        if (GetAsyncKeyState(VK_F3))
        {
            if (m_Aimpoint == 0)
            {
                Beep(400, 200);
                m_Aimpoint = 1;
            }
            else if(m_Aimpoint == 1)
            {
                Beep(500, 200);
                m_Aimpoint = 2;
            }
            else if (m_Aimpoint == 2)
            {
                Beep(600, 200);
                m_Aimpoint = 0;
            }
            while (GetAsyncKeyState(VK_F3))
            {

            }
        }


        RECT rect;
        GetWindowRect(g_hWnd, &rect);
        float width = rect.right - rect.left;
        float height = rect.bottom - rect.top;
        m_Matrix = (DWORD64)Client + client_dll::dwViewMatrix;
        float dts;
        float maxd = 10000.f;
        DWORD64 AimPawn = 0;
        for (int i = 0; i < 32; i++)
        {
            DWORD64 m_localPlayer = ReadMemory<DWORD64>((DWORD64)Client + client_dll::dwLocalPlayerController);
            DWORD64 m_PlayerList = ReadMemory<DWORD64>((DWORD64)Client + client_dll::dwGameEntitySystem);
            if (!(m_PlayerList + 0x8 * ((i & 0x7FFF) >> 9) + 16))
                continue;
            DWORD64 n_PlayerList = ReadMemory<DWORD64>((m_PlayerList + 0x8 * ((i & 0x7FFF) >> 9) + 16));
            if (!n_PlayerList)
                continue;
            if (!(n_PlayerList + 120 * (i & 0x1FF)))
                continue;
            DWORD64 playercontroller = ReadMemory<DWORD64>(n_PlayerList + 120 * (i & 0x1FF));
            if (!playercontroller)
                continue;
            if (!(playercontroller + CCSPlayerController::m_hPlayerPawn))
                continue;
            DWORD64 m_PlayerPawn = ReadMemory<DWORD64>(playercontroller + CCSPlayerController::m_hPlayerPawn);
            if (!m_PlayerPawn)
                continue;
            if (!(m_PlayerList + 0x8 * ((m_PlayerPawn & 0x7FFF) >> 9) + 16))
                continue;
            DWORD64 n_PlayerPawn = ReadMemory<DWORD64>((m_PlayerList + 0x8 * ((m_PlayerPawn & 0x7FFF) >> 9) + 16));
            if (!n_PlayerPawn)
                continue;
            if (!(n_PlayerPawn + 120 * (m_PlayerPawn & 0x1FF)))
                continue;
            DWORD64 PlayerPawn = ReadMemory<DWORD64>(n_PlayerPawn + 120 * (m_PlayerPawn & 0x1FF));
            if (!PlayerPawn)
                continue;
            if (!(PlayerPawn + C_BaseEntity::m_pGameSceneNode))
                continue;
            DWORD64 GameSceneNode = ReadMemory<DWORD64>(PlayerPawn + C_BaseEntity::m_pGameSceneNode);
            if (!GameSceneNode)
                continue;
            if (!(GameSceneNode + 0x1E0)) //DynamicMeshDeformParams_t
                continue;
            DWORD64 BoneArray = ReadMemory<DWORD64>(GameSceneNode + 0x1E0); //DynamicMeshDeformParams_t
            if (!BoneArray)
                continue;
            int LocalTeam = ReadMemory<int>(m_localPlayer + CCSPlayerController::m_iPendingTeamNum);
            int EntityTeam = ReadMemory<int>(playercontroller + CCSPlayerController::m_iPendingTeamNum);
            if (LocalTeam == EntityTeam)
                continue;
            int EpHp = ReadMemory<int>(PlayerPawn + C_BaseEntity::m_iHealth);
            if (EpHp <= 0 || EpHp > 200)
                continue;
            
            if (!(BoneArray + 0xC0) || !(BoneArray + 0xC4) || !(BoneArray + 0xC8))
                continue;
            Vector3 Head = ReadMemory<Vector3>(BoneArray + 0xC0);
            Vector3 Chest = ReadMemory<Vector3>(BoneArray + 0x20);
            Vector3 Waist = ReadMemory<Vector3>(BoneArray + 0x80);
            if (!(WorldToScreen(m_Matrix, Head, headPos, width, height)))
            {
                continue;
            }
            if (!(WorldToScreen(m_Matrix, Chest, chestPos, width, height)))
            {
                continue;
            }
            if (!(WorldToScreen(m_Matrix, Waist, waistPos, width, height)))
            {
                continue;
            }
            if (Esp)
            {
                WriteMemory<float>(PlayerPawn + 0x1440, on);
            }
            else
            {
                WriteMemory<float>(PlayerPawn + 0x1440, 0.f);
			}

            if (!(BoneArray + (5 * 0x20)) || !(BoneArray + (5 * 0x20 + 0x4)) || !(BoneArray + (5 * 0x20 + 0x4 + 0x4)))
                continue;

            if (!nearstAim)
            {
                Vector3 tEmpAp;
                AimPoint = AimtoPoints[m_Aimpoint];
                if (AimPoint == 0)
                {
                    tEmpAp = chestPos;
                }
                else if (AimPoint == 6)
                {
                    tEmpAp = headPos;
                }
                else if (AimPoint == 4)
                {
                    tEmpAp = waistPos;
                }
                dts = GetDistanceByPos(width, height, tEmpAp.x, tEmpAp.y);
                if (dts < AimRange)
                {
                    if (dts < maxd)
                    {
                        maxd = dts;
                        AimPawn = PlayerPawn;
                    }
                    
                }
            }
            else
            {
                dts = GetDistanceByPos(width, height, headPos.x, headPos.y);
                if (dts < AimRange)
                {
                    if (dts < maxd)
                    {
                        maxd = dts;
                        AimPawn = PlayerPawn;
                        AimPoint = 6;
                    }
                }
                /*dts = GetDistanceByPos(width, height, chestPos.X, chestPos.Y);
                if (dts < AimRange)
                {
                    if (dts < maxd)
                    {
                        maxd = dts;
                        AimPawn = PlayerPawn;
                        AimPoint = 4;
                    }
                }
                dts = GetDistanceByPos(width, height, waistPos.X, waistPos.Y);
                if (dts < AimRange)
                {
                    if (dts < maxd)
                    {
                        maxd = dts;
                        AimPawn = PlayerPawn;
                        AimPoint = 0;
                    }
                }*/
            }

        }
        if ((GetAsyncKeyState(5) && AimPawn) || (GetAsyncKeyState(6) && AimPawn) || (GetAsyncKeyState(2) && AimPawn))
        {
            DWORD64 t_pGameSceneNode = ReadMemory<DWORD64>(AimPawn + C_BaseEntity::m_pGameSceneNode);
            DWORD64 t_BoneArray = ReadMemory<DWORD64>(t_pGameSceneNode + 0x1E0);//DynamicMeshDeformParams_t
            DWORD64 playerpawn = ReadMemory<DWORD64>((DWORD64)Client + client_dll::dwLocalPlayerPawn);
            if (t_BoneArray && AimPawn && playerpawn)
            {
                Vector3 hitBox = ReadMemory<Vector3>(t_BoneArray + (AimPoint * 0x20));
                if (hitBox.x != 0 && hitBox.y != 0 && hitBox.z != 0)
                {
                    float Yaw, Pitch, AX, AY;
                    float Distance, Norm;
                    Vector3 OppPos, LocalPos, AimPos;
                    hitBox.z -= 1.0f;
                    LocalPos = ReadMemory<Vector3>(playerpawn + C_CSPlayerPawnBase::m_vecLastClipCameraPos);
                    OppPos.x = hitBox.x - LocalPos.x;
                    OppPos.y = hitBox.y - LocalPos.y;
                    OppPos.z = hitBox.z - LocalPos.z;
                    GetViewAngle(AX, AY);
                    Distance = sqrt(pow(OppPos.x, 2) + pow(OppPos.y, 2));
                    Yaw = atan2f(OppPos.y, OppPos.x) * 57.295779513 - AY;
                    Pitch = -atan(OppPos.z / Distance) * 57.295779513 - AX;
                    Norm = sqrt(pow(Yaw, 2) + pow(Pitch, 2));
                    if (Norm <= 60.0f) {
                        if (ReadMemory<DWORD>(playerpawn + C_CSPlayerPawnBase::m_iShotsFired) > 0) {//这个1是RCSBullet 开火松开之前打出的子弹数量
                            RCS aimPunchCache = ReadMemory<RCS>(playerpawn + C_CSPlayerPawn::m_aimPunchCache);
                            Vector3 aimPunch = ReadMemory<Vector3>(aimPunchCache.B + (aimPunchCache.A - 1) * sizeof(Vector3));
                            Yaw = Yaw * Smooth + (AY - aimPunch.y * 2.0f) ;
                            Pitch = Pitch * Smooth + (AX - aimPunch.x * 2.0f) ;
                        }
                        else
                        {
                            Yaw = Yaw * Smooth + AY;
                            Pitch = Pitch * Smooth + AX;
                        }
                        if (Pitch < 90.f && Pitch > -90.f) {
                            SetViewAngle(Pitch, Yaw);
                        }
                    }
                }
            }
        }
    }
    if (SelfhModule)
    {
        FreeLibraryAndExitThread(SelfhModule, 0);
    }
    else
    {
		ExitThread(0);
	}

}

// DLL Entry Point
BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        SelfhModule = hModule;
        HANDLE CurrentThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)myFuction, NULL, NULL, NULL);
        if (CurrentThread)
        {
            CloseHandle(CurrentThread);
        }
    }
    return TRUE;
}

