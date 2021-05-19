#include <windows.h>
#include <stdio.h>
#include "x86opcodes.h"

const int code_RVA_RAW_diff	= 0xA00;
const int code_VA_RVA_diff	= 0x400000;
const int code_VA_RAW_diff	= code_RVA_RAW_diff + code_VA_RVA_diff;

const int entry_RAW			= 0x36F104;
const int end_of_code_RAW	= 0x3C863C;
const int AoEP_offset_PE		= 0xA8;
const int impLoadLibrary_VA	= 0x7CA05C;
const int impSleep_VA		= 0x7CA074;
const int dll_name_offs_VA	= end_of_code_RAW + 16 + code_VA_RAW_diff;

int main (){
	
	HANDLE hFile = CreateFileA("MaxPayne.exe", GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == NULL){
		printf("Error, mb here's no MaxPayne.exe\n");
		system("pause>nul");
		return 1;
	}
	again:
	printf("Max Payne 1.05 patcher\n"
			"Please backup MaxPayne.exe before patching, game must be closed\n"
			"1) Exit\n"
			"2) patch LoadLibrary\n"
			"3) patch Sleep in main loop\n");
	char code;
	DWORD written;
	unsigned char buf[64];
	scanf("%c", &code);
	switch (code)
	{
	case '1': {
		CloseHandle(hFile);
		return 0;
		break;
	}
	case '2': {
		// RVA адрес новой точки входа в PE заголовке
		*(int*)(buf) = end_of_code_RAW + code_RVA_RAW_diff;
		SetFilePointer(hFile, AoEP_offset_PE, 0, FILE_BEGIN);
		WriteFile(hFile, buf, 4, &written, NULL);

		//	push dll name offset
		set_push_imm4(buf, dll_name_offs_VA);

		// call [LoadLibrary]
		set_call_ptr_imm4(buf+5, impLoadLibrary_VA);
		
		// jmp EntryPoint
		set_jmp_imm4(buf + 11, entry_RAW - (end_of_code_RAW + 16));

		printf("Enter dll name: ");
		scanf("%s", buf+16);

		SetFilePointer(hFile, end_of_code_RAW, 0, FILE_BEGIN);
		WriteFile(hFile, buf, 16 + strlen((char*)buf+16), &written, NULL);
		system("cls");
		printf("LoadLibrary patched\n");
		break;
	}
	case '3': {
		const int src_VA = 0x40BD00;
		const int dst_VA = 0x40C020;
		const int sleep_patch_RAW = end_of_code_RAW + 36; // с выравниванием по 16 байт
		
		*(int*)(buf) = (sleep_patch_RAW + code_VA_RAW_diff) - (src_VA + 4);
		SetFilePointer(hFile, src_VA - code_VA_RAW_diff, 0, FILE_BEGIN);
		WriteFile(hFile, buf, 4, &written, NULL);

		// pushad
		buf[0] = pushad_opcode;

		// xor eax, eax
		buf[1] = xor_reg_reg_opcode;
		buf[2] = eax_eax_code;

		// mov al, delay
		buf[3] = mov_al_imm_opcode;
		printf("Enter +delay ms: ");
		scanf("%d", buf + 4);

		// push eax
		buf[5] = push_eax_opcode;

		// call [Sleep]
		set_call_ptr_imm4(buf + 6, impSleep_VA);

		// popad
		buf[12] = popad_opcode;

		// jmp dst
		set_jmp_imm4(buf + 13, dst_VA - (sleep_patch_RAW + code_VA_RAW_diff + 18));

		SetFilePointer(hFile, sleep_patch_RAW, 0, FILE_BEGIN);
		WriteFile(hFile, buf, 18, &written, NULL);

		system("cls");
		printf("Sleep patched\n");

		break;
	}
	default:
		break;
	}
	goto again;
}