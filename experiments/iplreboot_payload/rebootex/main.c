#include <pspsdk.h>

int (* Ipl_Payload)(void *a0, void *a1, void *a2, void *a3) = (void *)0x88400000;

int Reboot_Entry()
{
	return Ipl_Payload(0x88000000, 0x02000000, 0, 0);
}


