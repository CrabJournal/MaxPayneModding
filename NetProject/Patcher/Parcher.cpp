#include <windows.h>
#include <stdio.h>

const unsigned char call_ptr_imm4_opcode[] = {0xFF, 0x15};
const unsigned char push_imm4_opcode		= 0x68;
const unsigned char jmp_imm4_opcode		= 0xe9;

const int code_RVA_RAW_diff	= 0xA00;
const int code_VA_RVA_diff	= 0x400000;
const int code_VA_RAW_diff	= code_RVA_RAW_diff + code_VA_RVA_diff;

const int entry_RAW			= 0x36F104;
const int end_of_code_RAW	= 0x3C863C;
const int AoEP_offset_PE		= 0xA8;
const int impLoadLibrary_VA	= 0x7CA05C;
const int dll_name_offs_VA	= end_of_code_RAW + 16 + code_VA_RAW_diff;
const int sleep_patch_RAW	= end_of_code_RAW + 36; // с выравниванием по 16 байт

inline void set_push_imm4(unsigned char *buf, int imm4){
		buf[0] = push_imm4_opcode;
		*(int*)(buf+1) = imm4;
}
inline void set_call_ptr_imm4(unsigned char *buf, int addr){
		*(WORD*)(buf) = *(WORD*)call_ptr_imm4_opcode;
		*(int*)(buf+2) = addr;
}
inline void set_jmp_imm4(unsigned char *buf, int dst_eip_diff){
		buf[0] = jmp_imm4_opcode;
		*(int*)(buf+1) = dst_eip_diff;
}

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
		// TODO
		break;
	}
	default:
		break;
	}
	goto again;
}