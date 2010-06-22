
/*
 * 
 * pedmp.c -- dump/examine PE images of given process
 *
 * Note: the dumped images are not probably runnable, but they can be easily
 * analyzed with static analysis tools.
 *
 * Jarkko Turkulainen jt[at]klake.org
 *
 *
 * TODO:
 * - check PEB corruption
 * - compare the RO segments in memory to images on disk
 *
 *
 */


#include <stdio.h>
#include <windows.h>


typedef BOOL (WINAPI *f_EnumProcessModules)(
	HANDLE hProcess,
	HMODULE* lphModule,
	DWORD cb,
	LPDWORD lpcbNeeded);


typedef DWORD (WINAPI *f_GetModuleBaseNameEx)(
	HANDLE hProcess,
	HMODULE hModule,
	LPTSTR lpFilename,
	DWORD nSize);


DWORD get_page_size() {
	SYSTEM_INFO sinfo;

	GetSystemInfo(&sinfo);
	return sinfo.dwPageSize;

}

unsigned char *generate_pe_dump(HANDLE handle, unsigned char *src, int *len) {
	PIMAGE_NT_HEADERS 	nt;
	PIMAGE_DOS_HEADER 	dos;
	PIMAGE_SECTION_HEADER	sect;
	DWORD			ret, i = 0;
	unsigned char		hdr[8192], *section;


	if (!ReadProcessMemory(handle, src, (LPVOID)hdr, sizeof(hdr), NULL)) {
		// printf("pedmp: cannot find image header\n");
		return NULL;
	}
	
	if (memcmp(hdr, "MZ", 2)) {
		// printf("pedmp: illegal DOS header\n");
		return NULL;
	}

	printf("\nimage at 0x%p\n", src);

	dos = (PIMAGE_DOS_HEADER)hdr;
	nt  = (PIMAGE_NT_HEADERS)(hdr + dos->e_lfanew);

	/*
	 * XXXXXXXXXXXXXX some modules need the additional page size.. 
	 *
	 */
	*len = nt->OptionalHeader.SizeOfImage + 
		nt->OptionalHeader.SizeOfHeaders + get_page_size();

	if (!(ret = (DWORD)VirtualAlloc(NULL,
			nt->OptionalHeader.SizeOfImage + 
			nt->OptionalHeader.SizeOfHeaders + get_page_size(),
			MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))) {
		printf("pedmp: cannot allocate memory\n");
		return NULL;
	}

	printf("dumping headers...\n", i);
	printf("  headers size    : 0x%p\n", nt->OptionalHeader.SizeOfHeaders);
	printf("  virtual address : 0x%p\n", src);
	printf("  image address   : 0x%p\n", 0);

	memcpy((char *)ret, hdr, nt->OptionalHeader.SizeOfHeaders);
	
	sect = IMAGE_FIRST_SECTION(nt);
	for (i = 0; i < nt->FileHeader.NumberOfSections; i++) {

		printf("dumping section %d...\n", i);
		printf("  section size    : 0x%p\n", sect[i].SizeOfRawData);
		printf("  virtual address : 0x%p\n", src + sect[i].VirtualAddress);
		printf("  image address   : 0x%p\n", ret + sect[i].PointerToRawData);
		

		if (!(section = VirtualAlloc(NULL, sect[i].Misc.VirtualSize,
				MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))) {
			printf("pedmp: cannot allocate memory\n");
			return NULL;
		}

		if (!ReadProcessMemory(handle, src + sect[i].VirtualAddress,
				section, sect[i].Misc.VirtualSize, NULL)) {
			printf("pedmp: cannot read image section\n");
			return NULL;
		}

		memcpy((char *)ret + sect[i].PointerToRawData, section,
			sect[i].Misc.VirtualSize);

		VirtualFree(section, 0, MEM_RELEASE);

	}

	return (unsigned char *)ret;

}

void write_pe_dump(char *outfile, unsigned char *pe, int len) {
	HANDLE hFile;
	int bytes, written;

	printf("writing to file %s...\n", outfile);
 
	hFile = CreateFile(outfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
                              
	if (hFile == INVALID_HANDLE_VALUE) { 
		printf("pedmp: cannot open file for writing\n");
		exit (1);
	}

	WriteFile(hFile, pe, len, &written, NULL);

	if (written != len) {
		printf("pedmp: problems with file writing\n");
		exit (1);
	}
	
	return;

}

char * get_module_name(HANDLE handle, DWORD address) {
	HMODULE modules[1024];
	DWORD i, bytes;
	char name[MAX_PATH], *ptr = NULL;
	f_EnumProcessModules m_EnumProcessModules;
	f_GetModuleBaseNameEx m_GetModuleBaseNameEx;

	if (!(m_EnumProcessModules = 
		(f_EnumProcessModules)GetProcAddress(LoadLibrary("psapi"), 
		"EnumProcessModules")))
		return NULL;

	if (!(m_GetModuleBaseNameEx = 
		(f_GetModuleBaseNameEx)GetProcAddress(LoadLibrary("psapi"), 
		"GetModuleBaseNameA")))
		return NULL;

	if (m_EnumProcessModules(handle, modules, sizeof(modules), &bytes)) {
		for (i = 0; i < (bytes / sizeof(HMODULE)); i++) {

			if (address == (DWORD)modules[i]) {
				if (m_GetModuleBaseNameEx(handle, modules[i], 
						name, sizeof(name))) {
					ptr = strdup(name);
				}
				break;

			}
		}
	}
	return ptr;
}


void dump_address(HANDLE handle, DWORD address, unsigned char *pe, int len, int pid) {
	char name[256], *modname;
	
	if (!(modname = get_module_name(handle, address))) {
		modname = strdup("unknown.dat");
		printf("\nWARNING: corrupted module list\n\n");

	}
	memset(name, 0, sizeof(name));
	sprintf(name, "%d-%p-%s.dat", pid, address, modname);
	free(modname);

	write_pe_dump(name, pe, len);

}



int main(int argc, char **argv) {
	HANDLE		handle;
	unsigned char	*pe;
	DWORD		psize, address = 0;
	int		i, len, first = 0, all = 0;

	if (argc < 2) {
		printf("\nUsage: pedmp <pid> [address]\n\n"
			"  address    => meaning\n"
			"  ------------------------------------------\n"
			"  *          => all modules\n"
			"  0x11223344 => module at address 0x11223344\n"
			"  [empty]    => first module found\n\n");
		exit (1);
	}

	if (argv[2]) {
		if (argv[2][0] != '*')
			address = strtoul(argv[2], NULL, 0);
		else 
			all = 1;
	}

	if ((handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, atoi(argv[1]))) == NULL) {
		printf("pedmp: cannot open process\n");
		exit (1);
	}

	if (address) {
		if ((pe = generate_pe_dump(handle, (unsigned char *)address, 
				&len)) == NULL) {
			printf("pedmp: cannot find image\n");
			exit (1);

		}
		dump_address(handle, address, pe, len, atoi(argv[1]));

	} else {
		
		psize = get_page_size();
		for (address = 0; address < (0x80000000 - psize); address += psize) {

			if (!first)
				first = 1;
			if ((pe = generate_pe_dump(handle, 
					(unsigned char *)address, &len)) != NULL) {
				dump_address(handle, address, pe, len, atoi(argv[1]));

				VirtualFree(pe, 0, MEM_RELEASE);

				if (!all && first)
					break;
			}
		}
	}
	
	return(0);

}


