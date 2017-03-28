	.set noreorder

#include "pspimport.s"

// Build List
// sceDdrdb_driver_0000.o sceDdrdb_driver_0001.o sceDdrdb_driver_0002.o sceDdrdb_driver_0003.o sceDdrdb_driver_0004.o sceDdrdb_driver_0005.o sceDdrdb_driver_0006.o sceDdrdb_driver_0007.o sceDdrdb_driver_0008.o sceDdrdb_driver_0009.o 

#ifdef F_sceDdrdb_driver_0000
	IMPORT_START	"sceDdrdb_driver",0x00010011
#endif
#ifdef F_sceDdrdb_driver_0001
	IMPORT_FUNC	"sceDdrdb_driver",0x05D50F41,sceDdrdbEncrypt
#endif
#ifdef F_sceDdrdb_driver_0002
	IMPORT_FUNC	"sceDdrdb_driver",0x370F456A,sceDdrdbCertvry
#endif
#ifdef F_sceDdrdb_driver_0003
	IMPORT_FUNC	"sceDdrdb_driver",0x40CB752A,sceDdrdbHash
#endif
#ifdef F_sceDdrdb_driver_0004
	IMPORT_FUNC	"sceDdrdb_driver",0xB24E1391,sceDdrdbSiggen
#endif
#ifdef F_sceDdrdb_driver_0005
	IMPORT_FUNC	"sceDdrdb_driver",0xB33ACB44,sceDdrdbDecrypt
#endif
#ifdef F_sceDdrdb_driver_0006
	IMPORT_FUNC	"sceDdrdb_driver",0xB8218473,sceDdrdbPrngen
#endif
#ifdef F_sceDdrdb_driver_0007
	IMPORT_FUNC	"sceDdrdb_driver",0xE27CE4CB,sceDdrdbSigvry
#endif
#ifdef F_sceDdrdb_driver_0008
	IMPORT_FUNC	"sceDdrdb_driver",0xEC05300A,sceDdrdbMul2
#endif
#ifdef F_sceDdrdb_driver_0009
	IMPORT_FUNC	"sceDdrdb_driver",0xF970D54E,sceDdrdbMul1
#endif
