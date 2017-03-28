#include <pspsdk.h>
#include <pspuser.h>
#include <psppaf.h>

#include "videoiso.h"

VideoIso cache[32];
VideoIso iso;
u8  referenced[32];
int cachechanged;
char buf[192];

static int ReadCache()
{
	SceUID fd;
	int i;
	
	paf_memset(cache, 0, sizeof(VideoIso)*32);
	paf_memset(referenced, 0, sizeof(referenced));

	for (i = 0; i < 0x10; i++)
	{
		fd = sceIoOpen("ms0:/PSP/SYSTEM/isocachev.bin", PSP_O_RDONLY, 0);

		if (fd >= 0)
			break;
	}

	if (i == 0x10)
		return -1;

	sceIoRead(fd, cache, sizeof(VideoIso)*32);	
	sceIoClose(fd);

	return 0;
}

static int IsCached(char *file, ScePspDateTime *mtime)
{
	int i;
	
	for (i = 0; i < 32; i++)
	{
		if (cache[i].filename[0] != 0)
		{
			if (paf_memcmp(cache[i].filename, file, paf_strlen(file)+1) == 0)
			{
				if (paf_memcmp(mtime, &cache[i].mtime, sizeof(ScePspDateTime)) == 0)
				{
					referenced[i] = 1;
					return 1;
				}
			}
		}
		else
			break;
	}

	return 0;
}

static int Cache(VideoIso *videoiso)
{
	int i;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].filename[0] == 0)
		{
			referenced[i] = 1;
			paf_memcpy(&cache[i], videoiso, sizeof(VideoIso));
			cachechanged = 1;
			return 1;
		}
	}

	return 0;
}

static int CheckIso(VideoIso *videoiso)
{
	int i;

	paf_snprintf(buf, sizeof(buf), "ms0:/ISO/VIDEO/%s", videoiso->filename);
	videoiso->disctype = 0;

	SceUID fd = sceIoOpen(buf, PSP_O_RDONLY, 0);
	if (fd < 0)
		return 0;

	sceIoLseek(fd, 0x8000, PSP_SEEK_SET);
	if (sceIoRead(fd, buf, 0x90) != 0x90)
	{
		sceIoClose(fd);
		return 0;
	}
	
	if (paf_memcmp(buf+1, "CD001", 5) != 0)
	{
		sceIoClose(fd);
		return 0;
	}

	// Seek to path table, and read it
	sceIoLseek(fd, (*(u32 *)&buf[0x8C]) * 0x800, PSP_SEEK_SET);
	sceIoRead(fd, buf, sizeof(buf));
	sceIoClose(fd);

	char *p = buf;

	for (i = 0; i < 4; i++)
	{
		if (p[0] == 9)
		{
			if (paf_memcmp(p+8, "UMD_VIDEO", 9) == 0)
			{
				videoiso->disctype |= 0x20;
			}
			else if (paf_memcmp(p+8, "UMD_AUDIO", 9) == 0)
			{
				videoiso->disctype |= 0x40;
			}
		}

		p += (8 + p[0] + (p[0]&1));
	}

	if (!videoiso->disctype)
		return 0;

	return 1;
}

static int SaveCache(int *n)
{
	SceUID fd;
	int i;

	*n = 0;

	for (i = 0; i < 32; i++)
	{
		if (cache[i].filename[0] != 0 && !referenced[i])
		{
			cachechanged = 1;
			paf_memset(&cache[i], 0, sizeof(VideoIso));			
		}
	}	
	
	// Move items to left if needed

	for (i = 0; i < 32; i++)
	{
		if (cache[i].filename[0] == 0)
		{
			int j;

			for (j = i+1; j < 32; j++)
			{
				if (cache[j].filename[0] != 0)
				{
					paf_memcpy(&cache[i], &cache[j], sizeof(VideoIso));
					paf_memset(&cache[j], 0, sizeof(VideoIso));
					*n = *n+1;
					cachechanged = 1;
					break;
				}
			}
		}
		else
		{
			*n = *n+1;
		}
	}

	if (!cachechanged)
		return 0;

	cachechanged = 0;

	for (i = 0; i < 0x10; i++)
	{
		fd = sceIoOpen("ms0:/PSP/SYSTEM/isocachev.bin", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);

		if (fd >= 0)
			break;
	}

	if (i == 0x10)
		return -1;

	if (*n)
		sceIoWrite(fd, cache, sizeof(VideoIso)*(*n));
	sceIoClose(fd);

	return 0;
}

void GetVideoIsos(VideoIso **isos, int *n)
{
	SceUID dfd = sceIoDopen("ms0:/ISO/VIDEO");
	SceIoDirent dir;

	if (dfd < 0)
	{
		isos = NULL;
		*n = 0;
		
		return;
	}

	ReadCache();

	paf_memset(&dir, 0, sizeof(SceIoDirent));
	while (sceIoDread(dfd, &dir) > 0)
	{
		if (!FIO_S_ISDIR(dir.d_stat.st_mode))
		{
			if (!IsCached(dir.d_name, &dir.d_stat.st_mtime))
			{
				paf_strcpy(iso.filename, dir.d_name);
				if (CheckIso(&iso))
				{
					paf_memcpy(&iso.mtime, &dir.d_stat.st_mtime, sizeof(ScePspDateTime));
					Cache(&iso);
				}
			}			
		}
	}

	sceIoDclose(dfd);
	SaveCache(n);

	*isos = cache;
}
