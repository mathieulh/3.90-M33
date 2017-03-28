int func_f7cc(int u0, int u1, int u2)
{
	u8 structure[XX]; // less than 0x60

	structure[0xC] = u1; // 0x32
	structure[0xD] = u2; // 3
	structure[0xE] = u0; // 1
	structure[0xF] = u0 >> 8; // 0
	structure[0x10] = u0 >> 16; // 0
	structure[0x11] = u0 >> 26; // 0

	return sceSysconCmdExec(structure, 0);
}


int sceSysconResetDevice(int u0, int u1)
{
	s0 = u0;
	s1 = u1;

	if (u0 == 1)
	{
		if (u1 == 1)
		{
			func_f7cc(u0, 0x32, 3);
		}
	}
}

int sceSysconCmdExec(u8 *structure, u32 unk)
{
	if (sceKernelIsIntrContext())
		return 0x80000030;

	...;
}

int sceSysconCmdExecAsync(u8 *structure, u32 u1, u32 u2, u32 u3)
{
	s4 = u3;
	s2 = u1;
	s1 = u2;
	s0 = structure;
	
	if (g_x[0x30/4] != 0)
		return 0x80250003;

	if (u1 & 0x100)
	{
		...;
	}

	a2 = 0;

	for (i = 0; i < structure[0xD]; i++)
	{
		a2 += structure[0xC+i];
	}

	// a2 = 0x36 <- 0x32+0x3+1

	if (structure[0xD]+1 >= 0x10)
	{
		...;
	}

	// structure[0xF] = ~0x36 = 0xFFFFFFC9;
	structure[0xC+structure[0xD]] = ~a2;

	a1 = structure[0xD]+1;

	do
	{
		a1++;
		structure[a1+0xC] = 0xFF;
	} while (a1 < 0x10);

	// structure[0x11] = 0xFF;
	// structure[0x12] = 0xFF;
	// structure[0x13] = 0xFF;
	// structure[0x14] = 0xFF;
	// structure[0x15] = 0xFF;
	// structure[0x16] = 0xFF;
	// structure[0x17] = 0xFF;
	// structure[0x18] = 0xFF;
	// structure[0x19] = 0xFF;
	// structure[0x1A] = 0xFF
	// structure[0x1B] = 0xFF
	// structure[0x1C] = 0xFF

	for (i = 0; i < 0x10; i++)
		structure[0x1C+i] = 0xFF;

	s5 = sceKernelCpuSuspendIntr();

	structure[0/4] = 0;
	structure[4/4] = 0x10000 | (u1 & 0xFFFF); // 0x10000
	structure[8/4] = 0xFFFFFFFF;
	structure[0x2C/4] = u2; // 0
	structure[0x30/4] = gp;


}

int SuperFunc(u8 *structure)
{
	s0 = structure;
	
	if (DebugHandlers)
	{
		...;
	}

	sceGpioPortRead();
	sceGpioPortClear(8);

	while (_lw(0xbe58000c) & 4)
	{
		v1 = _lw(0xbe580008);
	}

	v0 = _lw(0xbe58000C);
	_sw(3, 0xbe580020);

	if (structure[4/4] & v0)
	{
		...;
	}
	a3 = structure[0xD]+1;

	if (a3 == 0)
	{
		...;
	}

	a2 = 0;
	a1 = structure;

	do
	{

		t1 = structure[0xC] << 8; 
		a0 = t1 | structure[0xD]; 
		a2 = a2 + 2;
		t0 = a2 < a3;

		v1 = _lw(0xbe58000C);
		structure += 2;

		_sw(a0, 0xbe580008);
	} while (t0);

	_sw(6, 0xbe580004);
	sceGpioPortSet(8);



}