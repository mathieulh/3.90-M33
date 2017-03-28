#ifndef __IPL_UPDATE__
#define __IPL_UPDATE__

/**
 * Erases the first blocks of IPL to delete it.
 * @returns 0 on sucess
*/
int sceIplUpdateClearIpl();

/**
 * Writes an IPL
 *
 * @param buf - Buffer containing the ipl to write
 * @param size - The size of IPL * 
 * @returns < 0 on error
 * @note In 1.5X updaters, this funtion takes no params, 
 * because the ipl is inside the prx
*/
int sceIplUpdateSetIpl(void *buf, int size);


#endif

