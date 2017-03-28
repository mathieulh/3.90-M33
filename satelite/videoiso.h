#ifndef __VIDEOISO_H__
#define __VIDEOISO_H__

typedef struct VideoIso
{
	char filename[72];
	int  disctype;
	ScePspDateTime mtime;
} VideoIso;

void GetVideoIsos(VideoIso **isos, int *n);

#endif

