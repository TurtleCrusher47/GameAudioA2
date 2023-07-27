/**
 CTexture
 Adapted from: https://www.mbsoftworks.sk/tutorials/opengl3/21-multilayered-terrain/
 Adapted By: Toh Da Jun
 Date: Sept 2021
 */
#include <windows.h>

#include <cstring>
#include <sstream>

using namespace std;

#include <gl/glew.h>

#include "texture.h"

//#include <FreeImage.h>
//
//#pragma comment(lib, "FreeImage.lib")

// Include ImageLoader
#include "System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CTexture::CTexture()
{
	bMipMapsGenerated = false;
}

/**
 @brief Destructor
 */
CTexture::~CTexture()
{
}

/**
 @brief Creates texture from provided data.
 @param a_iWidth Width of the texture
 @param a_iHeight Height of the texture
 @param format Format of data
 */
void CTexture::CreateEmptyTexture(int a_iWidth, int a_iHeight, GLenum format)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, NULL);

	glGenSamplers(1, &uiSampler);
}

/**
 @brief Creates texture from provided data.
 @param a_sPath - path to the texture
 @param format - format of data
 @param bGenerateMipMaps - whether to create mipmaps
 */
void CTexture::CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps)
{
	// Generate an OpenGL texture ID for this texture
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	if (format == GL_RGBA || format == GL_BGRA)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	// We must handle this because of internal format parameter
	else if (format == GL_RGB || format == GL_BGR)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	if (bGenerateMipMaps)glGenerateMipmap(GL_TEXTURE_2D);
	glGenSamplers(1, &uiSampler);

	sPath = "";
	bMipMapsGenerated = bGenerateMipMaps;
	iWidth = a_iWidth;
	iHeight = a_iHeight;
	iBPP = a_iBPP;
}

/**
 @brief Set sampler parameter
 @param parameter
 @param value
 */
void CTexture::SetSamplerParameter(GLenum parameter, GLenum value)
{
	glSamplerParameteri(uiSampler, parameter, value);
}

/**
 @brief Sets magnification and minification texture filter.
 @param tfMagnification - mag. filter, must be from ETextureFiltering enum
 @param tfMinification - min. filter, must be from ETextureFiltering enum
 */
void CTexture::SetFiltering(int a_tfMagnification, int a_tfMinification)
{
	glBindSampler(0, uiSampler);

	// Set magnification filter
	if (a_tfMagnification == TEXTURE_FILTER_MAG_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	else if (a_tfMagnification == TEXTURE_FILTER_MAG_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set minification filter
	if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_NEAREST_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_BILINEAR_MIPMAP)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	else if (a_tfMinification == TEXTURE_FILTER_MIN_TRILINEAR)
		glSamplerParameteri(uiSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	tfMinification = a_tfMinification;
	tfMagnification = a_tfMagnification;
}

/**
 @brief Loads texture from a file, supports most graphics formats.
 @param a_sPath - path to the texture
 @param bGenerateMipMaps - whether to create mipmaps
 */
bool CTexture::LoadTexture2D(string a_sPath, bool bGenerateMipMaps)
{
	int width = 0;
	int height = 0;
	int nrChannels = 0;
	unsigned char* data = CImageLoader::GetInstance()->Load(a_sPath.c_str(), width, height, nrChannels, false);

	GLenum format;
	if (nrChannels == 4)format = GL_RGBA;
	if (nrChannels == 3)format = GL_BGR;
	if (nrChannels == 1)format = GL_LUMINANCE;

	//cout << "***New : " << a_sPath.c_str() << "***" << endl;
	//cout << width << endl;
	//cout << height << endl;
	//cout << nrChannels * 8 << endl;
	//cout << format << endl;
	CreateFromData(data, width, height, nrChannels * 8, format, bGenerateMipMaps);

	// Free up the memory of the file data read in
	free(data);

	sPath = a_sPath;

	// These codes uses the FreeImage library to load the image files.

	//	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//	FIBITMAP* dib(0);

	//	fif = FreeImage_GetFileType(a_sPath.c_str(), 0); // Check the file signature and deduce its format

	//	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
	//		fif = FreeImage_GetFIFFromFilename(a_sPath.c_str());

	//	if (fif == FIF_UNKNOWN) // If still unknown, return failure
	//		return false;

	//	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
	//		dib = FreeImage_Load(fif, a_sPath.c_str());
	//	if (!dib)
	//		return false;

	//	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	//	// If somehow one of these failed (they shouldn't), return failure
	//	if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
	//		return false;

	//	GLenum format;
	//	if (FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
	//	if (FreeImage_GetBPP(dib) == 24)format = GL_BGR;
	//	if (FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;

	//	cout << "***Old : " << a_sPath.c_str() << "***" << endl;
	//	cout << FreeImage_GetWidth(dib) << endl;
	//	cout << FreeImage_GetHeight(dib) << endl;
	//	cout << FreeImage_GetBPP(dib) << endl;
	//	cout << format << endl;
	//	CreateFromData(bDataPointer, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), FreeImage_GetBPP(dib), format, bGenerateMipMaps);

	//	FreeImage_Unload(dib);

	//	sPath = a_sPath;

	return true; // Success
}

///**
// @brief Reload the texture.
// */
//bool CTexture::ReloadTexture()
//{
//	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
//	FIBITMAP* dib(0);
//
//	fif = FreeImage_GetFileType(sPath.c_str(), 0); // Check the file signature and deduce its format
//
//	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
//		fif = FreeImage_GetFIFFromFilename(sPath.c_str());
//
//	if (fif == FIF_UNKNOWN) // If still unknown, return failure
//		return false;
//
//	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
//		dib = FreeImage_Load(fif, sPath.c_str());
//	if (!dib)
//		return false;
//
//	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data
//
//	// If somehow one of these failed (they shouldn't), return failure
//	if (bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
//		return false;
//
//	GLenum format;
//	int bada = FreeImage_GetBPP(dib);
//	if (FreeImage_GetBPP(dib) == 32)format = GL_RGBA;
//	if (FreeImage_GetBPP(dib) == 24)format = GL_BGR;
//	if (FreeImage_GetBPP(dib) == 8)format = GL_LUMINANCE;
//
//	glBindTexture(GL_TEXTURE_2D, uiTexture);
//	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, iWidth, iHeight, format, GL_UNSIGNED_BYTE, bDataPointer);
//	if (bMipMapsGenerated)glGenerateMipmap(GL_TEXTURE_2D);
//
//	FreeImage_Unload(dib);
//
//	return true; // Success
//}

/**
 @brief Bind textures and samplers together
 @param iTextureUnit The texture unit to bind texture to
 */
void CTexture::BindTexture(const int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	glBindSampler(iTextureUnit, uiSampler);
}

/**
 @brief UnBind textures and samplers
 @param iTextureUnit The texture unit to unbind
 */
void CTexture::UnBindTexture(const int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindSampler(iTextureUnit, 0);
}

/**
 @brief Frees all memory used by texture.
 @param iTextureUnit The texture unit to unbind
 */
void CTexture::DeleteTexture(void)
{
	glDeleteSamplers(1, &uiSampler);
	glDeleteTextures(1, &uiTexture);
}

/**
 @brief Get Minification Filter.
 */
int CTexture::GetMinificationFilter(void) const
{
	return tfMinification;
}

/**
 @brief Get Magnification Filter.
 */
int CTexture::GetMagnificationFilter(void) const
{
	return tfMagnification;
}

/**
 @brief Get width.
 */
int CTexture::GetWidth(void) const
{
	return iWidth;
}

/**
 @brief Get height.
 */
int CTexture::GetHeight(void) const
{
	return iHeight;
}

/**
 @brief Get bits per pixel.
 */
int CTexture::GetBPP(void) const
{
	return iBPP;
}

/**
 @brief Get texture ID.
 */
unsigned int CTexture::GetTextureID(void) const
{
	return uiTexture;
}

/**
 @brief Get path of file containing this texture.
 */
string CTexture::GetPath(void) const
{
	return sPath;
}

/**
 @brief The array of textures for the terrain
 */
CTexture tTextures[NUMTEXTURES];

/**
 @brief Load the 5 textures for the terrain
 */
bool LoadAllTextures(void)
{
	// Load textures
	string sTextureNames[] = { "fungus.png", "sand_grass_02.png", "rock_2_4w.png", "sand.png", "path.png" };

	for (int i = 0; i < NUMTEXTURES; i++)
	{
		if (tTextures[i].LoadTexture2D("Image/Terrain/textures/" + sTextureNames[i], true) == false)
			return false;

		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}

	return true;
}