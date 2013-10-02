
#If olly is running, ESI is set to -1 (0FFFFFFFFh)

include 'H:\LIAM\FASM\INCLUDE\WIN32AX.inc'

.data
	ollyTitle db 'Rofl...', 0
	ollyMsgBo db 'Nice try... dipshit!',13d,10d,13d,10d'- illuz1oN', 0

.code
start:
	detectolly:
		xor eax,eax
		cmp esi, 0FFFFFFFFh
		jnz .ollyPresent
	; Contine your program here!

	.ollyPresent:
		xor eax,eax
		push MB_OK
		push ollyTitle
		push ollyMsgBo
		push 0
		call [MessageBox]
		push eax
		call [ExitProcess]
.end start
