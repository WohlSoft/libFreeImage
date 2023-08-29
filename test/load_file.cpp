#include <stdio.h>
#include <FreeImageLite.h>

static void s_fiDebug(FREE_IMAGE_FORMAT fif, const char *msg)
{
	fprintf(stderr, "FI <%u>: %s\n", (uint32_t)fif, msg);
	fflush(stderr);
}

// strangely undocumented import necessary to use the FreeImage handle functions
extern void SetDefaultIO(FreeImageIO *io);

int main(int argc, char** argv)
{
	int ret = 0;

	if(argc < 2)
	{
		fprintf(stderr, "Syntax: %s <path to file>\n", argv[0]);
		fflush(stderr);
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");
	if(!file)
	{
		fprintf(stderr, "fopen: Can't open file %s\n", argv[1]);
		fflush(stderr);
		return 1;
	}

	FreeImage_Initialise();
	FreeImage_SetOutputMessage(&s_fiDebug);

	FreeImageIO io;
	SetDefaultIO(&io);

	FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromHandle(&io, (fi_handle)file);

	if(formato == FIF_UNKNOWN)
	{
		fclose(file);
		fprintf(stderr, "FreeImage_GetFileTypeFromHandle: Can't recognise format %s\n", argv[1]);
		fflush(stderr);
		FreeImage_DeInitialise();
		return 0;
	}

	FIBITMAP *img = FreeImage_LoadFromHandle(formato, &io, (fi_handle)file);
	fclose(file);
	if(img)
	{
		fprintf(stderr, "Successfully loaded image: %s\n", argv[1]);
		fprintf(stderr, "W = %u\n", FreeImage_GetWidth(img));
		fprintf(stderr, "H = %u\n", FreeImage_GetHeight(img));
		fprintf(stderr, "B = %u\n", FreeImage_GetBPP(img));
		fflush(stderr);

		FreeImage_Unload(img);
		ret = 0;
	}
	else
	{
		fprintf(stderr, "FreeImage_LoadFromHandle: Can't load image file %s\n", argv[1]);
		fflush(stderr);
		ret = 1;
	}

	FreeImage_DeInitialise();

	return ret;
}
