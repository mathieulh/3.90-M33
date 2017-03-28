/*
 * PSPLINK
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPLINK root for details.
 *
 * main.c - PSPLINK USB HostFS main code
 *
 * Copyright (c) 2006 James F <tyranid@gmail.com>
 *
 * $HeadURL: svn://svn.ps2dev.org/psp/trunk/psplink/usbhostfs/main.c $
 * $Id: main.c 1952 2006-06-25 15:57:15Z tyranid $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspkdebug.h>
#include <pspsdk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pspusb.h>
#include <pspusbbus.h>
#include "usbhostfs.h"
#include "usbasync.h"
#include "usbkprintf.h"

int psplinkSetK1(int k1);

PSP_MODULE_INFO(MODULE_NAME, PSP_MODULE_KERNEL, 1, 1);

/* Main USB event flags */
enum UsbEvents
{
	USB_EVENT_ATTACH = 1,
	USB_EVENT_DETACH = 2,
	USB_EVENT_ASYNC  = 4,
	USB_EVENT_CONNECT = 8,
	USB_EVENT_ALL = 0xFFFFFFFF
};

/* USB transfer event flags */
enum UsbTransEvents
{
	USB_TRANSEVENT_BULKOUT_DONE = 1,
	USB_TRANSEVENT_BULKIN_DONE = 2,
};

/* Main USB thread id */
static SceUID g_thid = -1;
/* Main USB event flag */
static SceUID g_mainevent = -1;
/* Main USB transfer event flag */
static SceUID g_transevent = -1;
/* Asynchronous input event flag */
static SceUID g_asyncevent = -1;
/* Main USB semaphore */
static SceUID g_mainsema   = -1;
/* Static bulkin request structure */
static struct UsbdDeviceReq g_bulkin_req;
/* Static bulkout request structure */
static struct UsbdDeviceReq g_bulkout_req;
/* Async request */
static struct UsbdDeviceReq g_async_req;
/* Indicates we have a connection to the PC */
static int g_connected = 0;
/* Buffers for async data */
static struct AsyncEndpoint *g_async_chan[MAX_ASYNC_CHANNELS];

/* HI-Speed device descriptor */
struct DeviceDescriptor devdesc_hi = 
{
	.bLength = 18, 
	.bDescriptorType = 0x01, 
	.bcdUSB = 0x200, 
	.bDeviceClass = 0, 
	.bDeviceSubClass = 0, 
	.bDeviceProtocol = 0, 
	.bMaxPacketSize = 64, 
	.idVendor = 0, 
	.idProduct = 0, 
	.bcdDevice = 0x100, 
	.iManufacturer = 0, 
	.iProduct = 0, 
	.iSerialNumber = 0,
	.bNumConfigurations = 1
};

/* Hi-Speed configuration descriptor */
struct ConfigDescriptor confdesc_hi = 
{
	.bLength = 9, 
	.bDescriptorType = 2, 
	.wTotalLength = (9+9+(3*7)), 
	.bNumInterfaces = 1, 
	.bConfigurationValue = 1, 
	.iConfiguration = 0, 
	.bmAttributes = 0xC0, 
	.bMaxPower = 0
};

/* Hi-Speed interface descriptor */
struct InterfaceDescriptor interdesc_hi = 
{
	.bLength = 9, 
	.bDescriptorType = 4, 
	.bInterfaceNumber = 0, 
	.bAlternateSetting = 0, 
	.bNumEndpoints = 3, 
	.bInterfaceClass = 0xFF, 
	.bInterfaceSubClass = 0x1, 
	.bInterfaceProtocol = 0xFF, 
	.iInterface = 1
};

/* Hi-Speed endpoint descriptors */
struct EndpointDescriptor endpdesc_hi[3] = 
{
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 0x81, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 512, 
		.bInterval = 0 
	},
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 2, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 512, 
		.bInterval = 0 
	},
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 3, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 512, 
		.bInterval = 0 
	},
};

/* Full-Speed device descriptor */
struct DeviceDescriptor devdesc_full = 
{
	.bLength = 18, 
	.bDescriptorType = 0x01, 
	.bcdUSB = 0x200, 
	.bDeviceClass = 0, 
	.bDeviceSubClass = 0, 
	.bDeviceProtocol = 0, 
	.bMaxPacketSize = 64, 
	.idVendor = 0, 
	.idProduct = 0, 
	.bcdDevice = 0x100, 
	.iManufacturer = 0, 
	.iProduct = 0, 
	.iSerialNumber = 0,
	.bNumConfigurations = 1
};

/* Full-Speed configuration descriptor */
struct ConfigDescriptor confdesc_full = 
{
	.bLength = 9, 
	.bDescriptorType = 2, 
	.wTotalLength = (9+9+(3*7)), 
	.bNumInterfaces = 1, 
	.bConfigurationValue = 1, 
	.iConfiguration = 0, 
	.bmAttributes = 0xC0, 
	.bMaxPower = 0
};

/* Full-Speed interface descriptor */
struct InterfaceDescriptor interdesc_full = 
{
	.bLength = 9, 
	.bDescriptorType = 4, 
	.bInterfaceNumber = 0, 
	.bAlternateSetting = 0, 
	.bNumEndpoints = 3, 
	.bInterfaceClass = 0xFF, 
	.bInterfaceSubClass = 0x1, 
	.bInterfaceProtocol = 0xFF, 
	.iInterface = 1
};

/* Full-Speed endpoint descriptors */
struct EndpointDescriptor endpdesc_full[3] = 
{
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 0x81, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 64, 
		.bInterval = 0 
	},
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 2, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 64, 
		.bInterval = 0 
	},
	{
		.bLength = 7, 
		.bDescriptorType = 5, 
		.bEndpointAddress = 3, 
		.bmAttributes = 2, 
		.wMaxPacketSize = 64, 
		.bInterval = 0 
	},
};

/* String descriptor */
unsigned char strp[] = 
{
	0x8, 0x3, '<', 0, '>', 0, 0, 0
};

/* Endpoint blocks */
struct UsbEndpoint endp[4] = {
	{ 0, 0, 0 },
	{ 1, 0, 0 },
	{ 2, 0, 0 },
	{ 3, 0, 0 },
};

/* Intefaces */
struct UsbInterface intp = {
	0xFFFFFFFF, 0, 1,
};

/* Device request */
int usb_request(int arg1, int arg2, struct DeviceRequest *req)
{
	DEBUG_PRINTF("func24 a1 %08X, a2 %08X, a3 %p\n", arg1, arg2, req);
	DEBUG_PRINTF("ReqType %d, Request %d, Value %x, Index %x, Length %x\n", 
			req->bmRequestType, req->bRequest, req->wValue, req->wIndex,
			req->wLength);
	return 0;
}

/* Unknown callback */
int func28(int arg1, int arg2, int arg3)
{
	DEBUG_PRINTF("func28 a1 %08X, a2 %08X, a3 %08X\n", arg1, arg2, arg3);
	return 0;
}

/* Attach callback, speed 1=full, 2=hi  */
int usb_attach(int speed, void *arg2, void *arg3)
{
	DEBUG_PRINTF("usb_attach: speed %d, a2 %p, a3 %p\n", speed, arg2, arg3);
	sceKernelSetEventFlag(g_mainevent, USB_EVENT_ATTACH);

	return 0;
}

/* Detach callback */
int usb_detach(int arg1, int arg2, int arg3)
{
	DEBUG_PRINTF("usb_detach: a1 %08X, a2 %08X, a3 %08X\n", arg1, arg2, arg3);
	sceKernelSetEventFlag(g_mainevent, USB_EVENT_DETACH);

	return 0;
}

/* Forward define the driver structure */
extern struct UsbDriver g_driver;

/* USB data structures for hi and full speed endpoints */
struct UsbData usbdata[2];

/* Callback for when a bulkin request is done */
int bulkin_req_done(struct UsbdDeviceReq *req, int arg2, int arg3)
{
	DEBUG_PRINTF("bulkin_req_done:\n");
	DEBUG_PRINTF("size %08X, unkc %08X, recvsize %08X\n", req->size, req->unkc, req->recvsize);
	DEBUG_PRINTF("retcode %08X, unk1c %08X, arg %p\n", req->retcode, req->unk1c, req->arg);
	sceKernelSetEventFlag(g_transevent, USB_TRANSEVENT_BULKIN_DONE);
	return 0;
}

/* Callback for when a bulkout request is done */
int bulkout_req_done(struct UsbdDeviceReq *req, int arg2, int arg3)
{
	DEBUG_PRINTF("bulkout_req_done:\n");
	DEBUG_PRINTF("size %08X, unkc %08X, recvsize %08X\n", req->size, req->unkc, req->recvsize);
	DEBUG_PRINTF("retcode %08X, unk1c %08X, arg %p\n", req->retcode, req->unk1c, req->arg);
	sceKernelSetEventFlag(g_transevent, USB_TRANSEVENT_BULKOUT_DONE);
	return 0;
}

/* Callback for when a bulkout request is done */
int async_req_done(struct UsbdDeviceReq *req, int arg2, int arg3)
{
	DEBUG_PRINTF("async_req_done:\n");
	DEBUG_PRINTF("size %08X, unkc %08X, recvsize %08X\n", req->size, req->unkc, req->recvsize);
	DEBUG_PRINTF("retcode %08X, unk1c %08X, arg %p\n", req->retcode, req->unk1c, req->arg);
	sceKernelSetEventFlag(g_mainevent, USB_EVENT_ASYNC);
	return 0;
}

/* Setup a bulkin request */
int set_bulkin_req(void *data, int size)
{
	sceKernelDcacheWritebackRange(data, size);
	memset(&g_bulkin_req, 0, sizeof(g_bulkin_req));
	g_bulkin_req.endp = &endp[1];
	g_bulkin_req.data = data;
	g_bulkin_req.size = size;
	g_bulkin_req.func = bulkin_req_done;
	sceKernelClearEventFlag(g_transevent, ~USB_TRANSEVENT_BULKIN_DONE);
	return sceUsbbdReqSend(&g_bulkin_req);
}

/* Setup a bulkout request */
int set_bulkout_req(void *data, int size)
{
	u32 addr;
	u32 blockaddr;
	u32 topaddr;

	/* Ensure address is uncached */
	addr = (u32) data;
	blockaddr = (addr & ~63);
	topaddr = (addr + size + 63) & ~63;

	if(blockaddr != addr)
	{
		MODPRINTF("Error read data not cache aligned\n");
		return -1;
	}

	/* Invalidate range */
	sceKernelDcacheInvalidateRange((void*) blockaddr, topaddr - blockaddr);
	memset(&g_bulkout_req, 0, sizeof(g_bulkout_req));
	g_bulkout_req.endp = &endp[2];
	g_bulkout_req.data = (void *) addr;
	g_bulkout_req.size = size;
	g_bulkout_req.func = bulkout_req_done;
	sceKernelClearEventFlag(g_transevent, ~USB_TRANSEVENT_BULKOUT_DONE);
	return sceUsbbdReqRecv(&g_bulkout_req);
}

/* Read/Write buffer */
unsigned char tx_buf[64*1024] __attribute__((aligned(64)));

/* Read a block of data from the USB bus */
int read_data(void *data, int size)
{
	int nextsize = 0;
	int readlen = 0;
	int ret;
	u32 result;

	while(readlen < size)
	{
		nextsize = (size - readlen) > sizeof(tx_buf) ? sizeof(tx_buf) : (size - readlen);
		if(set_bulkout_req(tx_buf, nextsize) < 0)
		{
			return -1;
		}

		ret = sceKernelWaitEventFlag(g_transevent, USB_TRANSEVENT_BULKOUT_DONE, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &result, NULL);
		if(ret == 0)
		{
			if((g_bulkout_req.retcode == 0) && (g_bulkout_req.recvsize > 0))
			{
				readlen += g_bulkout_req.recvsize;
				memcpy(data, tx_buf, g_bulkout_req.recvsize);
				data += g_bulkout_req.recvsize;
			}
			else
			{
				DEBUG_PRINTF("Error in BULKOUT request %d, %d\n", g_bulkout_req.retcode, g_bulkout_req.recvsize);
				return -1;
			}
		}
		else
		{
			MODPRINTF("Error waiting for BULKOUT %08X\n", ret);
			return -1;
		}
	}

	return readlen;
}

int write_data(const void *data, int size)
{
	int nextsize = 0;
	int writelen = 0;
	int ret;
	u32 result;

	if((u32) data & 63)
	{
		while(writelen < size)
		{
			nextsize = (size - writelen) > sizeof(tx_buf) ? sizeof(tx_buf) : (size - writelen);
			memcpy(tx_buf, data, nextsize);
			set_bulkin_req(tx_buf, nextsize);
			/* TODO: Add a timeout to the event flag wait */
			ret = sceKernelWaitEventFlag(g_transevent, USB_TRANSEVENT_BULKIN_DONE, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &result, NULL);
			if(ret == 0)
			{
				if((g_bulkin_req.retcode == 0) && (g_bulkin_req.recvsize > 0))
				{
					writelen += g_bulkin_req.recvsize;
					data += g_bulkin_req.recvsize;
				}
				else
				{
					MODPRINTF("Error in BULKIN request %d\n", g_bulkin_req.retcode);
					return -1;
				}
			}
			else
			{
				MODPRINTF("Error waiting for BULKIN %08X\n", ret);
				return -1;
			}
		}
	}
	else
	{
		while(writelen < size)
		{
			nextsize = (size - writelen) > sizeof(tx_buf) ? sizeof(tx_buf) : (size - writelen);
			set_bulkin_req((char *) data, nextsize);
			/* TODO: Add a timeout to the event flag wait */
			ret = sceKernelWaitEventFlag(g_transevent, USB_TRANSEVENT_BULKIN_DONE, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &result, NULL);
			if(ret == 0)
			{
				if((g_bulkin_req.retcode == 0) && (g_bulkin_req.recvsize > 0))
				{
					writelen += g_bulkin_req.recvsize;
					data += g_bulkin_req.recvsize;
				}
				else
				{
					MODPRINTF("Error in BULKIN request %d\n", g_bulkin_req.retcode);
					return -1;
				}
			}
			else
			{
				MODPRINTF("Error waiting for BULKIN %08X\n", ret);
				return -1;
			}
		}
	}

	return writelen;
}

/* Exchange a HOSTFS command with the PC host */
int command_xchg(void *outcmd, int outcmdlen, void *incmd, int incmdlen, const void *outdata, 
		int outlen, void *indata, int inlen)
{
	struct HostFsCmd *cmd;
	struct HostFsCmd *resp;
	int ret = 0;
	int err = 0;

	/* TODO: Set timeout on semaphore */
	err = sceKernelWaitSema(g_mainsema, 1, NULL);
	if(err < 0)
	{
		MODPRINTF("Error waiting on xchg semaphore %08X\n", err);
		return 0;
	}

	do
	{
		cmd = (struct HostFsCmd *) outcmd;
		resp = (struct HostFsCmd *) incmd;

		if(outcmdlen > 0)
		{
			err = write_data(outcmd, outcmdlen);
			if(err != outcmdlen)
			{
				MODPRINTF("Error writing command %08X %d\n", cmd->command, err);
				break;
			}
		}

		if(outlen > 0)
		{
			err = write_data(outdata, outlen);
			if(err != outlen)
			{
				MODPRINTF("Error writing command data %08X, %d\n", cmd->command, err);
				break;
			}
		}

		if(incmdlen > 0)
		{
			err = read_data(incmd, incmdlen);
			if(err != incmdlen)
			{
				MODPRINTF("Error reading response for %08X %d\n", cmd->command, err);
				break;
			}

			if((resp->magic != HOSTFS_MAGIC) && (resp->command != cmd->command))
			{
				MODPRINTF("Invalid response packet magic: %08X, command: %08X\n", resp->magic, resp->command);
				break;
			}

			DEBUG_PRINTF("resp->magic %08X, resp->command %08X, resp->extralen %d\n", 
					resp->magic, resp->command, resp->extralen);

			/* TODO: Should add checks for inlen being less that extra len */
			if((resp->extralen > 0) && (inlen > 0))
			{
				err = read_data(indata, resp->extralen);
				if(err != resp->extralen)
				{
					MODPRINTF("Error reading input data %08X, %d\n", cmd->command, err);
					break;
				}
			}
		}

		ret = 1;
	}
	while(0);

	(void) sceKernelSignalSema(g_mainsema, 1);

	return ret;
}

/* Send an async write */
int send_async(void *data, int len)
{
	int ret = 0;
	int err = 0;

	/* TODO: Set timeout on semaphore */
	err = sceKernelWaitSema(g_mainsema, 1, NULL);
	if(err < 0)
	{
		MODPRINTF("Error waiting on xchg semaphore %08X\n", err);
		return 0;
	}

	do
	{
		if((data) && (len > 0))
		{
			err = write_data(data, len);
			if(err != len)
			{
				MODPRINTF("Error writing async command %d\n", err);
				break;
			}
		}

		ret = 1;
	}
	while(0);

	(void) sceKernelSignalSema(g_mainsema, 1);

	return ret;
}

/* Send the hello command, indicates we are here */
int send_hello_cmd(void)
{
	struct HostFsHelloCmd cmd;
	struct HostFsHelloResp resp;

	memset(&cmd, 0, sizeof(cmd));
	cmd.cmd.magic = HOSTFS_MAGIC;
	cmd.cmd.command = HOSTFS_CMD_HELLO;

	return command_xchg(&cmd, sizeof(cmd), &resp, sizeof(resp), NULL, 0, NULL, 0);
}

/* Setup a async request */
int set_ayncreq(void *data, int size)
{
	u32 addr;
	u32 blockaddr;
	u32 topaddr;

	/* Ensure address is uncached */
	addr = (u32) data;
	blockaddr = (addr & ~63);
	topaddr = (addr + size + 63) & ~63;

	if(blockaddr != addr)
	{
		MODPRINTF("Error read data not cache aligned\n");
		return -1;
	}

	/* Invalidate range */
	sceKernelDcacheInvalidateRange((void*) blockaddr, topaddr - blockaddr);
	memset(&g_async_req, 0, sizeof(g_async_req));
	g_async_req.endp = &endp[3];
	g_async_req.data = (void *) addr;
	g_async_req.size = size;
	g_async_req.func = async_req_done;
	sceKernelClearEventFlag(g_mainevent, ~USB_EVENT_ASYNC);
	return sceUsbbdReqRecv(&g_async_req);
}

/* Call to ensure we are connected to the USB host */
int usb_connected(void)
{	
	return g_connected;
}

char async_data[512] __attribute__((aligned(64)));

void fill_async(void *async_data, int len)
{
	struct AsyncCommand *cmd;
	unsigned char *data;
	int sizeleft;
	int intc;

	if(len > sizeof(struct AsyncCommand))
	{
		len -= sizeof(struct AsyncCommand);
		data = async_data + sizeof(struct AsyncCommand);
		cmd = (struct AsyncCommand *) async_data;

		DEBUG_PRINTF("magic %08X, channel %d\n", cmd->magic, cmd->channel);
		intc = pspSdkDisableInterrupts();
		if((cmd->magic == ASYNC_MAGIC) && (cmd->channel >= 0) && (cmd->channel < MAX_ASYNC_CHANNELS) && (g_async_chan[cmd->channel]))
		{
			struct AsyncEndpoint *pEndp = g_async_chan[cmd->channel];
			sizeleft = len < (MAX_ASYNC_BUFFER - pEndp->size) ? len 
						: (MAX_ASYNC_BUFFER - pEndp->size);
			while(sizeleft > 0)
			{
				pEndp->buffer[pEndp->write_pos++] = *data++;
				pEndp->write_pos %= MAX_ASYNC_BUFFER;
				pEndp->size++;
				sizeleft--;
			}
			sceKernelSetEventFlag(g_asyncevent, (1 << cmd->channel));
			DEBUG_PRINTF("Async chan %d - read_pos %d - write_pos %d - size %d\n", cmd->channel, 
					pEndp->read_pos, pEndp->write_pos,
					pEndp->size);
		}
		else
		{
			MODPRINTF("Error in command header\n");
		}
		pspSdkEnableInterrupts(intc);
	}
}

int usbAsyncRegister(unsigned int chan, struct AsyncEndpoint *endp)
{
	int intc;
	int ret = -1;

	intc = pspSdkDisableInterrupts();
	do
	{
		if(endp == NULL)
		{
			break;
		}

		if(chan == ASYNC_ALLOC_CHAN)
		{
			int i;

			for(i = ASYNC_USER; i < MAX_ASYNC_CHANNELS; i++)
			{
				if(g_async_chan[i] == NULL)
				{
					chan = i;
					break;
				}
			}

			if(i == MAX_ASYNC_CHANNELS)
			{
				break;
			}
		}
		else
		{
			if((chan >= MAX_ASYNC_CHANNELS) || (g_async_chan[chan] != NULL))
			{
				break;
			}
		}

		g_async_chan[chan] = endp;
		usbAsyncFlush(chan);

		ret = chan;
	}
	while(0);
	pspSdkEnableInterrupts(intc);

	return ret;
}

int usbAsyncUnregister(unsigned int chan)
{
	int intc;
	int ret = -1;

	intc = pspSdkDisableInterrupts();
	do
	{
		if((chan >= MAX_ASYNC_CHANNELS) || (g_async_chan[chan] == NULL))
		{
			break;
		}

		g_async_chan[chan] = NULL;

		ret = 0;
	}
	while(0);
	pspSdkEnableInterrupts(intc);

	return ret;
}

int usbAsyncRead(unsigned int chan, unsigned char *data, int len)
{
	int ret;
	int intc;
	int i;
	int k1;

	k1 = psplinkSetK1(0);

	if((chan >= MAX_ASYNC_CHANNELS) || (g_async_chan[chan] == NULL))
	{
		return -1;
	}

	ret = sceKernelWaitEventFlag(g_asyncevent, 1 << chan, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, NULL, NULL);
	if(ret < 0)
	{
		return -1;
	}

	intc = pspSdkDisableInterrupts();
	len = len < g_async_chan[chan]->size ? len : g_async_chan[chan]->size;
	for(i = 0; i < len; i++)
	{
		data[i] = g_async_chan[chan]->buffer[g_async_chan[chan]->read_pos++];
		g_async_chan[chan]->read_pos %= MAX_ASYNC_BUFFER;
		g_async_chan[chan]->size--;
	}

	if(g_async_chan[chan]->size != 0)
	{
		sceKernelSetEventFlag(g_asyncevent, 1 << chan);
	}
	pspSdkEnableInterrupts(intc);

	psplinkSetK1(k1);

	return len;
}

void usbAsyncFlush(unsigned int chan)
{
	int intc;

	if((chan >= MAX_ASYNC_CHANNELS) || (g_async_chan[chan] == NULL))
	{
		return;
	}

	intc = pspSdkDisableInterrupts();
	g_async_chan[chan]->size = 0;
	g_async_chan[chan]->read_pos = 0;
	g_async_chan[chan]->write_pos = 0;
	sceKernelClearEventFlag(g_asyncevent, ~(1 << chan));
	pspSdkEnableInterrupts(intc);
}

int usbAsyncWrite(unsigned int chan, const void *data, int len)
{
	int ret = -1;
	char buffer[512];
	struct AsyncCommand *cmd;
	int written = 0;
	int k1;

	k1 = psplinkSetK1(0);

	do
	{
		if(!usb_connected())
		{
			DEBUG_PRINTF("Error PC side not connected\n");
			break;
		}

		cmd = (struct AsyncCommand *) buffer;
		cmd->magic = ASYNC_MAGIC;
		cmd->channel = chan;

		while(written < len)
		{
			int size;

			size = (len-written) > (sizeof(buffer)-sizeof(struct AsyncCommand)) ? (sizeof(buffer)-sizeof(struct AsyncCommand)) : (len-written);
			memcpy(&buffer[sizeof(struct AsyncCommand)], data+written, size);
			if(send_async(buffer, size + sizeof(struct AsyncCommand)))
			{
				written += size;
			}
			else
			{
				break;
			}
		}

		ret = written;
	}
	while(0);

	psplinkSetK1(k1);

	return ret;
}

int usbWriteBulkData(const void *data, int len)
{
	int ret = -1;
	int err;
	struct BulkCommand cmd;
	int k1;

	k1 = psplinkSetK1(0);

	do
	{
		if(!usb_connected())
		{
			DEBUG_PRINTF("Error PC side not connected\n");
			break;
		}

		if((len <= 0) || (len > HOSTFS_BULK_MAXWRITE))
		{
			MODPRINTF("Invalid length %d\n", len);
			break;
		}

		cmd.magic = BULK_MAGIC;
		cmd.size = len;

		/* TODO: Set timeout on semaphore */
		err = sceKernelWaitSema(g_mainsema, 1, NULL);
		if(err < 0)
		{
			MODPRINTF("Error waiting on xchg semaphore %08X\n", err);
			break;
		}

		do
		{
			err = write_data(&cmd, sizeof(cmd));
			if(err != sizeof(cmd))
			{
				MODPRINTF("Error writing bulk header %d\n", err);
				err = -1;
				break;
			}

			err = write_data(data, len);
			if(err != len)
			{
				MODPRINTF("Error writing bulk data %d\n", err);
				err = -1;
				break;
			}
		}
		while(0);

		(void) sceKernelSignalSema(g_mainsema, 1);
		if(err >= 0)
		{
			ret = len;
		}
	}
	while(0);

	psplinkSetK1(k1);

	return ret;
}

int usbWaitForConnect(void)
{
	int ret;

	while(g_mainevent < 0)
	{
		sceKernelDelayThread(100000);
	}

	ret = sceKernelWaitEventFlag(g_mainevent, USB_EVENT_CONNECT, PSP_EVENT_WAITOR, NULL, NULL);
	if(ret == 0)
	{
		return 1;
	}

	return 0;
}

/* USB thread to handle attach/detach */
int usb_thread(SceSize size, void *argp)
{
	int ret;
	u32 result;

	DEBUG_PRINTF("USB Thread Started\n");
	while(1)
	{
		ret = sceKernelWaitEventFlag(g_mainevent, USB_EVENT_ATTACH | USB_EVENT_DETACH | USB_EVENT_ASYNC
				, PSP_EVENT_WAITOR | PSP_EVENT_WAITCLEAR, &result, NULL);
		if(ret < 0)
		{
			DEBUG_PRINTF("Error waiting on event flag %08X\n", ret);
			sceKernelExitDeleteThread(0);
		}

		if(result & USB_EVENT_ASYNC)
		{
			DEBUG_PRINTF("Async Request Done %d %d\n", g_async_req.retcode, g_async_req.recvsize);
			if((g_async_req.retcode == 0) && (g_async_req.recvsize > 0))
			{
				fill_async(async_data, g_async_req.recvsize);
				set_ayncreq(async_data, sizeof(async_data));
			}
		}

		if(result & USB_EVENT_DETACH)
		{
			g_connected = 0;
			sceKernelClearEventFlag(g_mainevent, ~USB_EVENT_CONNECT);
			DEBUG_PRINTF("USB Detach occurred\n");
		}

		if(result & USB_EVENT_ATTACH)
		{
			uint32_t magic;
			g_connected = 0;
			sceKernelClearEventFlag(g_mainevent, ~USB_EVENT_CONNECT);
			DEBUG_PRINTF("USB Attach occurred\n");
			if(read_data(&magic, sizeof(magic)) == sizeof(magic))
			{
				if(magic == HOSTFS_MAGIC)
				{
					if(send_hello_cmd())
					{
						set_ayncreq(async_data, sizeof(async_data));
						g_connected = 1;
						sceKernelSetEventFlag(g_mainevent, USB_EVENT_CONNECT);
					}
				}
			}
		}
	}

	return 0;
}

/* USB start function */
int start_func(int size, void *p)
{
	int ret;

	DEBUG_PRINTF("Start Function %p\n", p);

	/* Fill in the descriptor tables */
	memset(usbdata, 0, sizeof(usbdata));

	memcpy(usbdata[0].devdesc, &devdesc_hi, sizeof(devdesc_hi));
	usbdata[0].config.pconfdesc = &usbdata[0].confdesc;
	usbdata[0].config.pinterfaces = &usbdata[0].interfaces;
	usbdata[0].config.pinterdesc = &usbdata[0].interdesc;
	usbdata[0].config.pendp = usbdata[0].endp;
	memcpy(usbdata[0].confdesc.desc, &confdesc_hi,  sizeof(confdesc_hi));
	usbdata[0].confdesc.pinterfaces = &usbdata[0].interfaces;
	usbdata[0].interfaces.pinterdesc[0] = &usbdata[0].interdesc;
	usbdata[0].interfaces.intcount = 1;
	memcpy(usbdata[0].interdesc.desc, &interdesc_hi, sizeof(interdesc_hi));
	usbdata[0].interdesc.pendp = usbdata[0].endp;
	memcpy(usbdata[0].endp[0].desc, &endpdesc_hi[0], sizeof(endpdesc_hi[0]));
	memcpy(usbdata[0].endp[1].desc, &endpdesc_hi[1], sizeof(endpdesc_hi[1]));
	memcpy(usbdata[0].endp[2].desc, &endpdesc_hi[2], sizeof(endpdesc_hi[2]));

	memcpy(usbdata[1].devdesc, &devdesc_full, sizeof(devdesc_full));
	usbdata[1].config.pconfdesc = &usbdata[1].confdesc;
	usbdata[1].config.pinterfaces = &usbdata[1].interfaces;
	usbdata[1].config.pinterdesc = &usbdata[1].interdesc;
	usbdata[1].config.pendp = usbdata[1].endp;
	memcpy(usbdata[1].confdesc.desc, &confdesc_full,  sizeof(confdesc_full));
	usbdata[1].confdesc.pinterfaces = &usbdata[1].interfaces;
	usbdata[1].interfaces.pinterdesc[0] = &usbdata[1].interdesc;
	usbdata[1].interfaces.intcount = 1;
	memcpy(usbdata[1].interdesc.desc, &interdesc_full, sizeof(interdesc_full));
	usbdata[1].interdesc.pendp = usbdata[1].endp;
	memcpy(usbdata[1].endp[0].desc, &endpdesc_full[0], sizeof(endpdesc_full[0]));
	memcpy(usbdata[1].endp[1].desc, &endpdesc_full[1], sizeof(endpdesc_full[1]));
	memcpy(usbdata[1].endp[2].desc, &endpdesc_full[2], sizeof(endpdesc_full[2]));

	g_driver.devp_hi = usbdata[0].devdesc;
	g_driver.confp_hi = &usbdata[0].config;
	g_driver.devp = usbdata[1].devdesc;
	g_driver.confp = &usbdata[1].config;

	DEBUG_PRINTF("pusbdata %p\n", &usbdata);

	memset(g_async_chan, 0, sizeof(g_async_chan));

	g_mainevent = sceKernelCreateEventFlag("USBEvent", 0x200, 0, NULL);
	if(g_mainevent < 0)
	{
		MODPRINTF("Couldn't create event flag %08X\n", g_mainevent);
		return -1;
	}

	g_transevent = sceKernelCreateEventFlag("USBEventTrans", 0x200, 0, NULL);
	if(g_transevent < 0)
	{
		MODPRINTF("Couldn't create trans event flag %08X\n", g_transevent);
		return -1;
	}

	g_asyncevent = sceKernelCreateEventFlag("USBEventAsync", 0x200, 0, NULL);
	if(g_asyncevent < 0)
	{
		MODPRINTF("Couldn't create async event flag %08X\n", g_asyncevent);
		return -1;
	}

	g_mainsema = sceKernelCreateSema("USBSemaphore", 0, 1, 1, NULL);
	if(g_mainsema < 0)
	{
		MODPRINTF("Couldn't create semaphore %08X\n", g_mainsema);
		return -1;
	}

	g_thid = sceKernelCreateThread("USBThread", usb_thread, 10, 0x10000, 0, NULL);
	if(g_thid < 0)
	{
		MODPRINTF("Couldn't create usb thread %08X\n", g_thid);
		return -1;
	}

	ret = hostfs_init();
	DEBUG_PRINTF("hostfs_init: %d\n", ret);

	if(sceKernelStartThread(g_thid, 0, NULL))
	{
		MODPRINTF("Couldn't start usb thread\n");
		return -1;
	}

	return 0;
}

/* USB stop function */
int stop_func(int size, void *p)
{
	DEBUG_PRINTF("Stop function %p\n", p);

	if(g_thid >= 0)
	{
		/* Should really just signal for completion */
		sceKernelTerminateDeleteThread(g_thid);
		g_thid = -1;
	}

	if(g_mainevent >= 0)
	{
		sceKernelDeleteEventFlag(g_mainevent);
		g_mainevent = -1;
	}

	if(g_transevent >= 0)
	{
		sceKernelDeleteEventFlag(g_transevent);
		g_mainevent = -1;
	}

	if(g_asyncevent >= 0)
	{
		sceKernelDeleteEventFlag(g_asyncevent);
		g_asyncevent = -1;
	}

	if(g_mainsema >= 0)
	{
		sceKernelDeleteSema(g_mainsema);
		g_mainsema = -1;
	}

	return 0;
}

/* USB host driver */
struct UsbDriver g_driver = 
{
	HOSTFSDRIVER_NAME,
	4,
	endp,
	&intp,
	NULL, NULL, NULL, NULL,
	(struct StringDescriptor *) strp,
	usb_request, func28, usb_attach, usb_detach,
	0, 
	start_func,
	stop_func,
	NULL
};


/* Entry point */
int module_start(SceSize args, void *argp)
{
	int ret;

	ret = sceUsbbdRegister(&g_driver);
	memset(g_async_chan, 0, sizeof(g_async_chan));
	DEBUG_PRINTF("sceUsbbdRegister %08X\n", ret);
	DEBUG_PRINTF("g_driver 0x%p\n", &g_driver);
	MODPRINTF("USB HostFS Driver (c) TyRaNiD 2k6\n");

	sceUsbStart(PSP_USBBUS_DRIVERNAME, 0, 0);
	sceUsbStart("USBHostFSDriver", 0, 0);
	sceUsbActivate(0x1C9);

	installUsbKprintf();

	return 0;
}

/* Module stop entry */
int module_stop(SceSize args, void *argp)
{
	int ret;

	ret = sceUsbbdUnregister(&g_driver);
	DEBUG_PRINTF("sceUsbbdUnregister %08X\n", ret);
	hostfs_term();

	return 0;
}
