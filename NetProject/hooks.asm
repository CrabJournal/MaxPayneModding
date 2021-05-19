.model flat

EXTERN ?FromAsm@@YIXPAX@Z:near
EXTERN ?HookListC@@YIXPAUnode@@@Z:near

.code

_IGiveUCharacter PROC

	pushad
	mov ecx, esi
	call ?FromAsm@@YIXPAX@Z
	popad
	mov eax, 0642ED0h
	jmp eax

_IGiveUCharacter ENDP


_HookList PROC

	
	pushad		;edi, ebx

	mov ecx, edi
	mov edx, ebx
	call ?HookListC@@YIXPAUnode@@@Z

	popad
	mov eax, dword ptr [edi+4]		;from hooked place
	test eax, eax
	ret

_HookList ENDP

END