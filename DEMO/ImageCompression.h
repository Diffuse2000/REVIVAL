#ifndef _IC_IMAGECOMPRESSION_H_INCLUDED
#define _IC_IMAGECOMPRESSION_H_INCLUDED
#include "Base/FDS_DECS.h"

void ImageCompressionTestCode();

class S3TC_coder
{
	Image *_source;
	byte *_stream;
	byte *_compressed;

	inline mword toR5G6B5(mword color)
	{
		return 
			// round-to-nearest: may overflow, so it is currently disabled
//			(((color+0x40000)>>8)&0xf800) + // red
//			(((color+0x200)>>10)&0x07e0) + // green
//			(((color+0x4)>>3)&0x1f); // blue
			// truncation
			((color>>8)&0xf800) + // red
			((color>>5)&0x07e0) + // green
			((color>>3)&0x1f); // blue
	}

	inline mword fromR5G6B5(mword color)
	{
		return 
			((color<<8) & 0xf80000) + // red
			((color<<5) & 0x00fc00) + // green
			((color<<3) & 0x0000f8); // blue
	}

	// computes distance in RGB space between two X8R8G8B8 pixels.
	mword rgbDistance(mword u, mword v);

	// encodes pixels using 3-color method applied using colors u, v.
	// computes comulative error.
	// all colors are formatted X8R8G8B8 
	// comulative error may overflow on more than 64K pixels.
	mword calcError3(dword *pixels, mword numPixels, dword u, dword v);

	// encodes pixels using 4-color method applied using colors u, v.
	// computes comulative error.
	// all colors are formatted X8R8G8B8 
	// comulative error may overflow on more than 64K pixels.
	mword calcError4(dword *pixels, mword numPixels, dword u, dword v);

	dword encode3(dword *pixels, mword numPixels, dword u, dword v);
	dword encode4(dword *pixels, mword numPixels, dword u, dword v);


public:
	S3TC_coder(): _source(NULL), _stream (NULL) {}
	~S3TC_coder() {delete [] _stream;}
	void encode(Image *I);
	void decode(Image *I);

	// reorders stream, then compresses it using the Burrows-Wheeler transform, Move-
	// to-front encoding, followed by an entropy encoder. This is also implemented in 
	// bZip2 and is a quite powerful and efficient lossless compression method.
	// Stream reordering is done in such a way that hopefully improves the compression
	// rate of the method. 
	void compress();
	void decompress();
};

// encodes image using the compression technology found in JPeg2000. It may not be
// strictly complient with the file format itself, however.
// LOL. forget it - this one's a whole project by itself.
class JP2_coder
{
	byte *_stream;
public:
	JP2_coder(): _stream (NULL) {}
	~JP2_coder() {delete [] _stream;}
	void encode(Image *I);
	void decode(Image *I);

};

// mipmaps and compresses source with S3TC. saves result to file 'mipFilename'. 
// delta is the source minus the decompressed output from mipFilename,
// [normalized to unsigned by adding 128 to each channel]. It is saved to deltaFilename
// delta is saved as a BMP, to be saved as a JP2 by an external program.
void ImageMIPS3TC(Image *source, char *mipFilename, char *deltaFilename);

#endif //_IC_IMAGECOMPRESSION_H_INCLUDED