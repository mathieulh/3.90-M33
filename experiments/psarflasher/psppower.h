/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * psppower.h - Prototypes for the scePower library.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 *
 * $Id: psppower.h 1949 2006-06-21 18:27:35Z tyranid $
 */
#ifndef __POWER_H__
#define __POWER_H__

#include <pspkerneltypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Power callback flags
 */
 /*indicates the power switch it pushed, putting the unit into suspend mode*/
#define PSP_POWER_CB_POWER_SWITCH	0x80000000
/*indicates the hold switch is on*/
#define PSP_POWER_CB_HOLD_SWITCH	0x40000000
/*what is standby mode?*/
#define PSP_POWER_CB_STANDBY		0x00080000
/*indicates the resume process has been completed (only seems to be triggered when another event happens)*/
#define PSP_POWER_CB_RESUME_COMPLETE	0x00040000
/*indicates the unit is resuming from suspend mode*/
#define PSP_POWER_CB_RESUMING		0x00020000
/*indicates the unit is suspending, seems to occur due to inactivity*/
#define PSP_POWER_CB_SUSPENDING		0x00010000
/*indicates the unit is plugged into an AC outlet*/
#define PSP_POWER_CB_AC_POWER		0x00001000
/*indicates the battery charge level is low*/
#define PSP_POWER_CB_BATTERY_LOW	0x00000100
/*indicates there is a battery present in the unit*/
#define PSP_POWER_CB_BATTERY_EXIST	0x00000080
/*unknown*/
#define PSP_POWER_CB_BATTPOWER		0x0000007F

/**
 * Power Callback Function Definition
 *
 * @param unknown - unknown function, appears to cycle between 1,2 and 3
 * @param powerInfo - combination of PSP_POWER_CB_ flags
 */
typedef void (*powerCallback_t)(int unknown, int powerInfo);

/**
 * Register Power Callback Function
 *
 * @param slot - slot of the callback in the list
 * @param cbid - callback id from calling sceKernelCreateCallback
 */
int scePowerRegisterCallback(int slot, SceUID cbid);

/**
 * Check if unit is plugged in
 */
int scePowerIsPowerOnline(void);

/**
 * Check if a battery is present
 */
int scePowerIsBatteryExist(void);

/**
 * Check if the battery is charging
 */
int scePowerIsBatteryCharging(void);

/**
 * Get the status of the battery charging
 */
int scePowerGetBatteryChargingStatus(void);

/**
 * Check if the battery is low
 */
int scePowerIsLowBattery(void);

/**
 * Get battery life as integer percent
 * @return battery charge percentage
 */
int scePowerGetBatteryLifePercent(void);

/**
 * Get battery life as time
 */
int scePowerGetBatteryLifeTime(void);

/**
 * Get temperature of the battery
 */
int scePowerGetBatteryTemp(void);

/**
 * unknown? - crashes PSP in usermode
 */
int scePowerGetBatteryElec(void);

/**
 * Get battery volt level
 */
int scePowerGetBatteryVolt(void);

/**
 * Set CPU Frequency
 * @param cpufreq - new CPU frequency, valid values are 1 - 333
 */
int scePowerSetCpuClockFrequency(int cpufreq);

/**
 * Set Bus Frequency
 * @param busfreq - new BUS frequency, valid values are 1 - 167
 */
int scePowerSetBusClockFrequency(int busfreq);

/**
 * Alias for scePowerGetCpuClockFrequencyInt
 * @returns frequency as int
 */
int scePowerGetCpuClockFrequency(void);

/**
 * Get CPU Frequency as Integer
 * @returns frequency as int
 */
int scePowerGetCpuClockFrequencyInt(void);

/**
 * Get CPU Frequency as Float
 * @returns frequency as float
 */
float scePowerGetCpuClockFrequencyFloat(void);

/**
 * Alias for scePowerGetBusClockFrequencyInt
 * @returns frequency as int
 */
int scePowerGetBusClockFrequency(void);

/**
 * Get Bus fequency as Integer
 * @returns frequency as int
 */
int scePowerGetBusClockFrequencyInt(void);

/**
 * Get Bus frequency as Float
 * @returns frequency as float
 */
float scePowerGetBusClockFrequencyFloat(void);

/**
 * Set Clock Frequencies
 *
 * NOTE: Please use scePowerSetBusClockFrequency and
 * scePowerSetCpuClockFrequency instead of this function
 * for clock <= 222 and bus <= 111.
 *
 * @param cpufreq - cpu frequency, valid from 1-333
 * @param ramfreq - ram frequency, valid from 1-333
 * @param busfreq - bus frequency, valid from 1-166
 */
int scePowerSetClockFrequency(int cpufreq, int ramfreq, int busfreq);

/**
 * Lock power switch
 *
 * Note: if the power switch is toggled while locked
 * it will fire immediately after being unlocked.
 *
 * @param unknown - pass 0
 */
int scePowerLock(int unknown);

/**
 * Unlock power switch
 *
 * @param unknown - pass 0
 */
int scePowerUnlock(int unknown);

/**
 * Generate a power tick, preventing unit from 
 * powering off and turning off display.
 *
 * @param unknown - pass 0
 */
int scePowerTick(int unknown);

/**
 * Get Idle timer
 *
 */
int scePowerGetIdleTimer(void);

/**
 * Enable Idle timer
 *
 * @param unknown - pass 0
 */
int scePowerIdleTimerEnable(int unknown);

/**
 * Disable Idle timer
 *
 * @param unknown - pass 0
 */
int scePowerIdleTimerDisable(int unknown);

/**
 * Request the PSP to go into standby
 *
 * @return 0 always
 */
int scePowerRequestStandby(void);

/**
 * Request the PSP to go into suspend
 *
 * @return 0 always
 */
int scePowerRequestSuspend(void);


/**
 * Coldresets the psp. (only available in firmware 2.00 and higher)
 *
 * @param unk - unknown, pass 0
 * @returns ???
*/
int scePower_0442D852(int unk);


#ifdef __cplusplus
}
#endif

#endif
