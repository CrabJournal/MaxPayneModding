.model flat

EXTERN ?FromAsm@@YGXPAX@Z:near

.data
FunP DD 0

.code

_IGiveUCharacter PROC

	pushad
	push esi
	call ?FromAsm@@YGXPAX@Z
	popad
	mov eax, 0642ED0h
	jmp eax

_IGiveUCharacter ENDP

END