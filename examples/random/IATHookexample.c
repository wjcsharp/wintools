void APIHook(LPSTR szModuleName, LPSTR szProcName, DWORD dwNewFunc)
{
	DWORD dwOrgFunc = (DWORD)GetProcAddress(GetModuleHandle(szModuleName), szProcName);
	HMODULE hModule = GetModuleHandle(NULL);
	PIMAGE_DOS_HEADER pidh = (PIMAGE_DOS_HEADER)hModule;
	if(pidh->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}
	PIMAGE_NT_HEADERS pinh = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pidh->e_lfanew);
	if(pinh->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}
	PIMAGE_IMPORT_DESCRIPTOR piid = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)hModule + pinh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	while(piid->Name)
	{
		if(!stricmp((LPSTR)((PBYTE)hModule + piid->Name), szModuleName))
		{
			break;
		}
		piid++;
	}
	if(!piid->Name)
	{
		return;
	}
	PIMAGE_THUNK_DATA pitd = (PIMAGE_THUNK_DATA)((LPBYTE)hModule + piid->FirstThunk);
	while(pitd->u1.Function)
	{
		if(pitd->u1.Function == (LPDWORD)dwOrgFunc)
		{
			DWORD dwOldProtect;
			VirtualProtect(&pitd->u1.Function, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect);
			pitd->u1.Function = (LPDWORD)dwNewFunc;
			VirtualProtect(&pitd->u1.Function, sizeof(DWORD), dwOldProtect, NULL);
			break;
		}
		pitd++;
	}
}