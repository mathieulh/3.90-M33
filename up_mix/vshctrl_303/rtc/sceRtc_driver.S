	.set noreorder

#include "pspimport.s"

#ifdef F_sceRtc_driver_0000
	IMPORT_START	"sceRtc_driver",0x00010000
#endif
#ifdef F_sceRtc_driver_0001
	IMPORT_FUNC	"sceRtc_driver",0xC41C2853,sceRtcGetTickResolution
#endif
#ifdef F_sceRtc_driver_0002
	IMPORT_FUNC	"sceRtc_driver",0x3F7AD767,sceRtcGetCurrentTick
#endif
#ifdef F_sceRtc_driver_0003
	IMPORT_FUNC	"sceRtc_driver",0x029CA3B3,sceRtc_driver_029CA3B3
#endif
#ifdef F_sceRtc_driver_0004
	IMPORT_FUNC	"sceRtc_driver",0x4CFA57B0,sceRtcGetCurrentClock
#endif
#ifdef F_sceRtc_driver_0005
	IMPORT_FUNC	"sceRtc_driver",0xE7C27D1B,sceRtcGetCurrentClockLocalTime
#endif
#ifdef F_sceRtc_driver_0006
	IMPORT_FUNC	"sceRtc_driver",0x34885E0D,sceRtcConvertUtcToLocalTime
#endif
#ifdef F_sceRtc_driver_0007
	IMPORT_FUNC	"sceRtc_driver",0x779242A2,sceRtcConvertLocalTimeToUTC
#endif
#ifdef F_sceRtc_driver_0008
	IMPORT_FUNC	"sceRtc_driver",0x42307A17,sceRtcIsLeapYear
#endif
#ifdef F_sceRtc_driver_0009
	IMPORT_FUNC	"sceRtc_driver",0x05EF322C,sceRtcGetDaysInMonth
#endif
#ifdef F_sceRtc_driver_0010
	IMPORT_FUNC	"sceRtc_driver",0x57726BC1,sceRtcGetDayOfWeek
#endif
#ifdef F_sceRtc_driver_0011
	IMPORT_FUNC	"sceRtc_driver",0x4B1B5E82,sceRtcCheckValid
#endif
#ifdef F_sceRtc_driver_0012
	IMPORT_FUNC	"sceRtc_driver",0x3A807CC8,sceRtcSetTime_t
#endif
#ifdef F_sceRtc_driver_0013
	IMPORT_FUNC	"sceRtc_driver",0x27C4594C,sceRtcGetTime_t
#endif
#ifdef F_sceRtc_driver_0014
	IMPORT_FUNC	"sceRtc_driver",0xF006F264,sceRtcSetDosTime
#endif
#ifdef F_sceRtc_driver_0015
	IMPORT_FUNC	"sceRtc_driver",0x36075567,sceRtcGetDosTime
#endif
#ifdef F_sceRtc_driver_0016
	IMPORT_FUNC	"sceRtc_driver",0x7ACE4C04,sceRtcSetWin32FileTime
#endif
#ifdef F_sceRtc_driver_0017
	IMPORT_FUNC	"sceRtc_driver",0xCF561893,sceRtcGetWin32FileTime
#endif
#ifdef F_sceRtc_driver_0018
	IMPORT_FUNC	"sceRtc_driver",0x7ED29E40,sceRtcSetTick
#endif
#ifdef F_sceRtc_driver_0019
	IMPORT_FUNC	"sceRtc_driver",0x6FF40ACC,sceRtcGetTick
#endif
#ifdef F_sceRtc_driver_0020
	IMPORT_FUNC	"sceRtc_driver",0x9ED0AE87,sceRtcCompareTick
#endif
#ifdef F_sceRtc_driver_0021
	IMPORT_FUNC	"sceRtc_driver",0x44F45E05,sceRtcTickAddTicks
#endif
#ifdef F_sceRtc_driver_0022
	IMPORT_FUNC	"sceRtc_driver",0x26D25A5D,sceRtcTickAddMicroseconds
#endif
#ifdef F_sceRtc_driver_0023
	IMPORT_FUNC	"sceRtc_driver",0xF2A4AFE5,sceRtcTickAddSeconds
#endif
#ifdef F_sceRtc_driver_0024
	IMPORT_FUNC	"sceRtc_driver",0xE6605BCA,sceRtcTickAddMinutes
#endif
#ifdef F_sceRtc_driver_0025
	IMPORT_FUNC	"sceRtc_driver",0x26D7A24A,sceRtcTickAddHours
#endif
#ifdef F_sceRtc_driver_0026
	IMPORT_FUNC	"sceRtc_driver",0xE51B4B7A,sceRtcTickAddDays
#endif
#ifdef F_sceRtc_driver_0027
	IMPORT_FUNC	"sceRtc_driver",0xCF3A2CA8,sceRtcTickAddWeeks
#endif
#ifdef F_sceRtc_driver_0028
	IMPORT_FUNC	"sceRtc_driver",0xDBF74F1B,sceRtcTickAddMonths
#endif
#ifdef F_sceRtc_driver_0029
	IMPORT_FUNC	"sceRtc_driver",0x42842C77,sceRtcTickAddYears
#endif
#ifdef F_sceRtc_driver_0030
	IMPORT_FUNC	"sceRtc_driver",0xC663B3B9,sceRtcFormatRFC2822
#endif
#ifdef F_sceRtc_driver_0031
	IMPORT_FUNC	"sceRtc_driver",0x7DE6711B,sceRtcFormatRFC2822LocalTime
#endif
#ifdef F_sceRtc_driver_0032
	IMPORT_FUNC	"sceRtc_driver",0x0498FB3C,sceRtcFormatRFC3339
#endif
#ifdef F_sceRtc_driver_0033
	IMPORT_FUNC	"sceRtc_driver",0x27F98543,sceRtcFormatRFC3339LocalTime
#endif
#ifdef F_sceRtc_driver_0034
	IMPORT_FUNC	"sceRtc_driver",0xDFBC5F16,sceRtcParseDateTime
#endif
#ifdef F_sceRtc_driver_0035
	IMPORT_FUNC	"sceRtc_driver",0x28E1E988,sceRtcParseRFC3339
#endif


