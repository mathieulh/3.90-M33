

#include <stdio.h>
#include "BootConfig.h"

int main(int argc, char *argv[])
{
	CBootConfig config;

	if(argc < 4)
	{
		printf("Invalid number of parameters\n");
		printf("\nUsage: bootcnf.exe -i[b][t] InputFilename -o[b][t] OutputFilename\n");
		printf("	Use [b] for binary input/output and [t] for text based input and output\n");
		return 0;
	}

	bool inputBinary = false;
	bool outputBinary = false;
	char *inputFilename = 0;
	char *outputFilename = 0;
	int argi;

	for (argi = 1; argi < argc; argi++)
	{
		if (argv[argi][0] == '-')
		{
			if (strlen(argv[argi]) != 3)
			{
				printf("Invalid usage.\n");
				return -1;
			}
			
			switch (argv[argi][1])
			{
				case 'i':
				{
					if(argv[argi][2] == 'b')
					{
						inputBinary = true;
					}
					else if(argv[argi][2] == 't')
					{
						inputBinary = false;
					}
					else
					{
						printf("Invalid parameter\n");
						return -1;
					}
					argi++;
					inputFilename = argv[argi];
				}
			}
			
			switch (argv[argi][1])
			{
				case 'o':
				{
					if(argv[argi][2] == 'b')
					{
						outputBinary = true;
					}
					else if(argv[argi][2] == 't')
					{
						outputBinary = false;
					}
					else
					{
						printf("Invalid parameter\n");
						return -1;
					}
					argi++;
					outputFilename = argv[argi];
				}
			}
			
		}
	}

	int rc = 0;

	if(inputBinary == true)
	{
		rc = config.ParseBinary(inputFilename);
	}
	else
	{
		rc = config.ParseText(inputFilename);
	}

	if(rc < 0)
	{
		printf("Error parsing file %s\n", inputFilename);
		return -1;
	}

	if(outputBinary == true)
	{
		config.WriteBinary(outputFilename);
	}
	else
	{
		config.WriteText(outputFilename);
	}

	return 0;
}
