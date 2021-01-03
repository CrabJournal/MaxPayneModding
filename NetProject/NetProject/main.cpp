#include <windows.h>
#include <stdio.h>
//#include <math.h>

#pragma comment(linker,"/merge:.rdata=.text")
//#pragma comment(linker,"/merge:.data=.text")

struct vector3 {
	float x, y, z;
};
union Address {
	void* ptr;
	int i;
};

void *HookedPtr = nullptr;

extern "C" void _cdecl IGiveUCharacter();
extern "C" void _stdcall SetFromAsm(void *FromAsmPtr); // вынужденый костыль

void _stdcall FromAsm(void *character) {
	vector3 *pos = (vector3*)(((float*)character)+9);
	if (HookedPtr == nullptr) {
		char buf[64];
		sprintf(buf, "x = %f, y = %f, z = %f\n", pos->x, pos->y, pos->z);
		MessageBoxA(nullptr, buf, "Hooked!", MB_OK);
	}
	HookedPtr = character;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpReserved ) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		// setting hooks
		SetFromAsm(FromAsm);
		Address HookPlace;
		HookPlace.i = 0x0504403;
		DWORD old;
		VirtualProtect(HookPlace.ptr, 5, PAGE_EXECUTE_READWRITE, &old);
		*(void**)HookPlace.ptr = (void*)((char*)IGiveUCharacter - ((char*)HookPlace.ptr + 4));
	}
    return TRUE;
}