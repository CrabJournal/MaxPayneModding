.model flat

.data
FunP DD 0


.code

_IGiveUCharacter PROC

	pushad
	push esi
	call dword ptr [FunP]
	popad
	mov eax, 0642ED0h
	jmp eax

_IGiveUCharacter ENDP


_SetFromAsm@4 PROC

	mov eax, dword ptr [esp + 4]
	mov dword ptr [FunP], eax
	ret 4

_SetFromAsm@4 ENDP

END