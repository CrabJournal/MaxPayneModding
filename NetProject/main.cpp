#include <windows.h>
#include <stdio.h>
#include "../Patcher/x86opcodes.h"
#include "../MaxPayne.Client.Integration/main.h"

#include <math.h>

#pragma comment(linker,"/merge:.rdata=.text")
//#pragma comment(linker,"/merge:.data=.text")

union Address {
	void* ptr;
	int i;
};
struct X_CharacterProperties {
	char unknown1[2];
	char activation_group[3]	;			//  +0x2 
	bool is_getting_damage;				//  +0x5
	bool triggers_damage_animation;		//  +0x6
	bool is_dead;						//  +0x7
	char unknown2;
	bool is_wounded;						//  +0x9
	bool is_aiming;						//  +0xA
	char unknown3[0x34];		
	bool is_airborne;					// +0x3F
	bool is_falling;						// +0x40
	bool play_landing_animation;			// +0x41
	char unknown4;						
	bool shoot_dodge_allowed	;			// +0x43
	bool can_crossfade_to_shoot_dodge;	// +0x44
	bool can_end_shoot_dodge;			// +0x45
	bool is_shootdodging;				// +0x46
	char unknown5[0x20];	
	float cam_dist;				//idk	// +0x67
	float unknownfloat1;					// +0x6B
	char unknown6[5];
	float crouching_ratio;				// +0x74
	bool use_twohanded_animations;		// +0x78
	char unknown7;
	bool has_weapon_changed;				// +0x7A
	int current_weapon_ID;				// +0x7B
	int next_weapon_ID;					// +0x7F
	char unknown8[2];
	float health;						// +0x85
	bool send_OnActivate_messages;		// +0x89
	float shooting_frequency_multiplier;// +0x8A
	float AI_aiming_speed;				// +0x8E
	float AI_turn_left_right_speed;		// +0x92
	float AI_aim_up_down_speed;			// +0x96
	float AI_aiming_speed_cone;			// +0x9A
	float AI_visual_perceiving_radius;	// +0x9E
	float AI_general_perceiving_radius;	// +0xA2
	float AI_activation_reaction_time;	// +0xA6
	char unknown9[4];
	bool AI_active;						// +0xAE
	bool ally_aiming;					// +0xAF	
	bool enemy_aiming;					// +0xB0
	bool update_required;				// +0xB1
	char unknown10[0x15];
	char selected_weapon_ID;				// +0xC7
};
struct vector3 {
	float x, y, z;
};
struct X_Character {
	char unknown1[0x4E];
	float matrix[3][3];			// +0x4E
	vector3 pos;					// +0x72
	void* unk0x7E;
	char unknown2[0x1AC];
	// CE - bool flag0xCE;
	// A6 - bool flag0XA6;
	X_CharacterProperties prop;	// +0x22E
};
/*
	X_Character + 0x72 (HookedPtr + 0x24) = coords (vector3)
	X_Character + 0x22E = X_CharacterProperties
	*(X_Character + 0x718) = model
	*(X_Character + 0x53C) = mesh
	*(model + 4) = skin name
	*(model + 0x14) = Skeleton name
*/
/*

	HookedPtr - 0x4E = X_Character ??

	HookedPtr - 8 =	room ptr

	float matrix[] = HookedPtr;

	X_Character + 0x72 (HookedPtr + 0x24) = coords (vector3)
	X_Character + 0x22E = X_CharacterProperties


	0x06D74F0, 00698EC0, 00729610, 00743D60 - ðåâåðñèòü íå ïåðåðåâåðñèòü

	âûçûâàåì ìåòîä (_thiscall, íî ìîæíî èñïîëüçîâàòü êàê ôóíêöèþ _fastcall (this, ìóñîð, ...) [âðîäå])
	ïî àäðåñó 0x00729ED0([this = (void)*(X_Character + 0x398)], (int??)*(X_Character + 0x728))
	âîçâðàùàåò óêàçàòåëü íà îáúåêò X_GlobalPainkillerSettings??

	*(int*)X_GlobalPainkillerSettings = painkillers_amount

*/
struct node {
	node *prev;
	node *next;
	int number;
	int two; // type?
	X_Character *character;
};

node* HookedList = 0;
X_Character* Max = 0;
node *NPC = 0;

bool connected = 0;

extern "C" void _cdecl IGiveUCharacter();
extern "C" void _cdecl HookList();

void add_char(){
	node *current = HookedList;
	while (current->next->next) {
		if (current->character->prop.is_dead == 0) {
			NPC = current;
		}
		current = current->next;
	}
	NPC->character->prop.health = 0;
}

#define HOOKED_TO_MAX(H) ((X_Character*)((char*)(H) - 0x4E))
#define NORM_VEC2_OTHER_AXIS(X) (sqrt(1-X*X))
void _fastcall FromAsm(void *hooked) { 
	X_Character* lMax = HOOKED_TO_MAX(hooked);
	if (Max != lMax) {
		Max = lMax;
		add_char();
	}
	else {
		Max = lMax;
		if (connected == 0) {
			if (Max->prop.is_airborne) {
				connected = 1; 
				on_attach();
			}
		}
		else {
			player_info to_send;
			state_from_server state;
			to_send.x = Max->pos.x;
			to_send.y = Max->pos.y;
			to_send.z = Max->pos.z;
			to_send.rotation = acos(Max->matrix[0][0]);
			if (Max->matrix[0][2] < 0)
				to_send.rotation = -to_send.rotation;
			on_frame(to_send, state);
			
			if (state.players.size()) {
				remote_player* coop_player = &(state.players[0]);
				X_Character* coop_player_local = NPC->character;
				coop_player_local->pos.x = coop_player->x;
				coop_player_local->pos.y = coop_player->y;
				coop_player_local->pos.z = coop_player->z;

				float norm_vec_x = cos(coop_player->rotation);
				float norm_vec_y = NORM_VEC2_OTHER_AXIS(norm_vec_x);	//cos(coop_player.rotation);
				if (coop_player->rotation < 0)
					norm_vec_y = -norm_vec_y;
				coop_player_local->matrix[0][0] = norm_vec_x;
				coop_player_local->matrix[2][2] = norm_vec_x;
				coop_player_local->matrix[0][2] = norm_vec_y;
				coop_player_local->matrix[2][0] = -norm_vec_y;
				/*
				arccos(matrix[0]) = óãîë ïîâîðîòà ïî ãîðèçîíòó(òîëüêî ïîëó÷åíèå)
				óñòàíîâêà: íîðìàëü - âåêòîð vector2 v
				matrix[0] = v.x
				matrix[8] = v.x
				matrix[2] = v.y
				matrix[6] = -v.y
				*/
			}
		}
	}
}

int Handler(unsigned int code, struct _EXCEPTION_POINTERS* ep) { return EXCEPTION_EXECUTE_HANDLER; }
// hooks first
void _fastcall HookListC(node *list) {
	__try {
		if (NPC != 0) {
			NPC->character->prop.is_dead = 0;
			//NPC->character->prop.health = 0;
		}
	}
	__except (Handler(GetExceptionCode(), GetExceptionInformation())) {	}
	HookedList = list;
}

void PlaceCallJmpHook(int place, void* dst) {
	Address HookPlace;
	DWORD old;
	HookPlace.i = place;
	VirtualProtect(HookPlace.ptr, 4, PAGE_EXECUTE_READWRITE, &old);
	*(void**)HookPlace.ptr = (void*)((char*)dst - ((char*)HookPlace.ptr + 4));
}

inline void* GetAddress(int iaddr){
	Address addr;
	addr.i = iaddr;
	return addr.ptr;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved ) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		// setting hooks
		DWORD old;
		PlaceCallJmpHook(0x504403, IGiveUCharacter);

		void* listhookaddr = GetAddress(0x44D8A4);
		VirtualProtect(listhookaddr, 5, PAGE_EXECUTE_READWRITE, &old);
		set_call_imm4(listhookaddr, (char*)HookList - ((char*)listhookaddr + 5));
	}
	else {
		if (fdwReason == DLL_PROCESS_DETACH) {
			on_detach();
		}
	}
    return TRUE;
}
