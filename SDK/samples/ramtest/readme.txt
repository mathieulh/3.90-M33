This sample illustrate how to make use of large memory size in the slim.
Use PSP_LARGE_MEMORY = 1 in the makefile  and the size of user memory will become of 52 MB.
This requires an up to date pspsdk, revision 2333 (Novemeber, 1) or newer.

You can also customize your memory needs by using the function sctrlHENSetMemory and reiniting your program.