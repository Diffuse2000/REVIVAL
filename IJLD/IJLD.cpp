#include "..\FDS\FDS_DECS.h"
#include "ijl.h"

char *LoadJPEG(char *argv1,unsigned int SeekPos)
{
	// check if library requires initialization.
	JPEG_CORE_PROPERTIES jcprops;
	ijlInit(&jcprops);

	jcprops.JPGFile = argv1;

	//ASSERT(SeekPos == 0); // totally ignored, FDS always sends 0 anyhow

	// Verifies file exists/opened OK.
	if (ijlRead(&jcprops, IJL_JFILE_READPARAMS) != IJL_OK)
		return NULL;


	Word x, y;

	x = jcprops.JPGWidth;
	y = jcprops.JPGHeight;

	byte *buffer = new byte [x*y*3 + 4]; //24bit RGB image
	if (!buffer) return NULL;

	((Word *)buffer) [0] = x;
	((Word *)buffer) [1] = y;

	jcprops.DIBWidth = x;
	jcprops.DIBHeight = y;
	jcprops.DIBChannels = 3;
	jcprops.DIBColor = IJL_RGB;
	jcprops.DIBPadBytes = 0;
	jcprops.DIBBytes = buffer + 4;

	switch (jcprops.JPGChannels)
	{
		case 1: 
		{
			jcprops.JPGColor = IJL_G;
			break;
		}
		case 3:
		{
			jcprops.JPGColor = IJL_YCBCR;
			break;
		}
		default:
		{
			jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
			jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
			break;
		}
	}

	ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE);

	ijlFree(&jcprops);

	return (char *)buffer;
}
