const unsigned char call_ptr_imm4_opcode[] = {0xFF, 0x15};
const unsigned char call_imm4_opcode		= 0xE8;
const unsigned char jmp_imm4_opcode		= 0xE9;
const unsigned char push_imm4_opcode		= 0x68;
const unsigned char push_eax_opcode		= 0x50;
const unsigned char pushad_opcode		= 0x60;
const unsigned char popad_opcode			= 0x61;
const unsigned char xor_reg_reg_opcode	= 0x31;
const unsigned char eax_eax_code			= 0xC0;
const unsigned char mov_al_imm_opcode	= 0xB0;

inline void set_push_imm4(void *buf, int imm4){
		((unsigned char*)buf)[0] = push_imm4_opcode;
		*(int*)(((unsigned char*)buf)+1) = imm4;
}
inline void set_call_ptr_imm4(void *buf, int addr){
		*(WORD*)(buf) = *(WORD*)call_ptr_imm4_opcode;
		*(int*)(((unsigned char*)buf)+2) = addr;
}
inline void set_jmp_imm4(void *buf, int dst_eip_diff){
		((unsigned char*)buf)[0] = jmp_imm4_opcode;
		*(int*)(((unsigned char*)buf)+1) = dst_eip_diff;
}
inline void set_call_imm4(void *buf, int dst_eip_diff){
		((unsigned char*)buf)[0] = call_imm4_opcode;
		*(int*)(((unsigned char*)buf)+1) = dst_eip_diff;
}