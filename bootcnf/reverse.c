int Parse(u8 *config, int size, void *param2, void *param3, u8 *config, void *t1, void *t2)
{
	s5 = param3;
	s4 = param2;
	s3 = 0;

	s7 = config+config[0x30/4]; // filename table
	s1 = config+config[0x10/4]; // mode config
	s2 = config+config[0x20/4]; // module table
	nmodes = config[0x14/4]; // t3

	for (i = 0; i < nmodes; i++)
	{
		s1[0x0C/4] += s7; // 0x0C/4 must be probably from where to start the file table read

		if (param2[0x3C/4] == s1[8/4]) // == mode 2
		{
			fp = s1[4/4]; // == mode 1
			break;
		}

		s1 += 0x20; // iterate array
	}

	if (i == nmodes)
		error;

	if (s1[0x10/4] == 0) // true
	{
		if (s1[0x14/4] == 0) // true
		{
			if (s1[0x18/4] == 0)
			{
				if (s1[0x1C/4] == 0)
				{
					goto next;
				}
			}
		}
	}
	else
	{
		...;
	}

next:

	if (s1[0x0C/4] == 0);
		...;

	else
		continue code;

	strncpy(param3, s1[0x0C], 0x100); // part of file table, why?
	param2[0x38/4] = param3;

	s2 = s2 + s1[2/2]; // + 0. probably indicates from which module entry offset to start search
	v1 = s1[0/2]; // the size to load?
	
	param2[0x0C/4] = 0;
	s0 = param2[0x10/4];
	s3 = 0;

	t8 = (fp & s2[8/4]) & 0xFFF;
	s5 = 0x00FF0000; //
	s6 = 0x40000;

	if (t8) // then read this module
	{
		v0 = s2[8/4] & 0x00FF0000; // get upper byte, usually 0x80, 0 in the module anchor
	}
}