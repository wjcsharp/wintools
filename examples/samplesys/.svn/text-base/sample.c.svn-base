#include <ddk/ntddk.h>

/*
 * DDKAPI is necessary:
 * it is defined as __attribute__((__stdcall__))
 * and this changes the calling convention for this function.
 * Without it the linker will fail to find _DriverEntry@8 and
 * will crash the kernel at unload time.
 */

void DDKAPI DriverUnload(PDRIVER_OBJECT driver)
{
        DbgPrint("driver unload");
        return;
}

NTSTATUS DDKAPI DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING registry)
{
        DbgPrint("driver entry");
        driver->DriverUnload = DriverUnload;
        return STATUS_SUCCESS;
}