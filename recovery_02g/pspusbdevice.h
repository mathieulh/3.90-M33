#ifndef __PSPUSBDEVICE_H__
#define __PSPUSBDEVICE_H__

#define PSP_USBDEVICE_FLASH0	0
#define	PSP_USBDEVICE_FLASH1	1
#define PSP_USBDEVICE_FLASH2	2
#define PSP_USBDEVICE_FLASH3	3
#define PSP_USBDEVICE_UMD9660	4

int pspUsbDeviceSetDevice(u32 unit, int ronly, int unassign_mask);
int pspUsbDeviceFinishDevice();


#endif

