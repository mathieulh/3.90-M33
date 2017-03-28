New in 3.71 M33-3 sdk:

- Large user memory for the slim. Check the sample for usage.

- Functions sctrlSEGetConfigEx and sctrlSESetConfigEx have been added. 

 Although sctrlSEGetConfig and sctrlSESetConfig are kept for compatibility purposes, their usage 
 is not recommended, as there is not a size field in the structure, and the usage of said 
 functions could corrupt your program or the configuration in flash as new fields are added 
 in future SE/M33 versions.

- Some ModuleMgrForKernel functions names have been replaced with their true names.
