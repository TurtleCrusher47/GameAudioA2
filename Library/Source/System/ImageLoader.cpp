/**
 CImageLoader
 @brief A class to load an image into system memory and also graphics card
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "ImageLoader.h"

#include <iostream>
using namespace std;

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// STB Image
#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include <includes/stb_image.h>
#include "filesystem.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CImageLoader::CImageLoader(void)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CImageLoader::~CImageLoader(void)
{
}

/**
@brief Init Initialise this instance
*/ 
bool CImageLoader::Init(void)
{
	return false;
}

/**
 @brief Load an image and return as unsigned char*
 @param filename A const char* storing the name of the image file
 @param width A int variable passed in by reference. It stores the width of the image.
 @param height A int variable passed in by reference. It stores the width of the image.
 @param nrChannels A int variable passed in by reference. It stores the number of channels in the image.
 @param bInvert A const bool 
 */
unsigned char * CImageLoader::Load(const char* filename, int& width, int& height, int& nrChannels, const bool bInvert)
{
	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(bInvert);

	// load image, create texture and generate mipmaps
	unsigned char *data = stbi_load(FileSystem::getPath(filename).c_str(),
									&width, &height, &nrChannels, 0);

	// Return the data but you must remember to free up the memory of the file data read in
	return data;
}

/**
 @brief Load an image into the graphics card and return its ID.
 @param filename A const char* storing the name of the image file
 @param bInvert A const bool
 */
unsigned int CImageLoader::LoadTextureGetID(const char* filename,  const bool bInvert)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	int nrChannels = 0;

	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(bInvert);

	// load image, create texture and generate mipmaps
	unsigned char* data = stbi_load(FileSystem::getPath(filename).c_str(),
		&image_width, &image_height, &nrChannels, 0);

	if (data == NULL)
	{
		cout << "CImageLoader::LoadTextureGetID(): Unable to load " << FileSystem::getPath(filename).c_str() << endl;
		return 0;
	}

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	if (nrChannels == 1)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, image_width, image_height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	else if (nrChannels == 3)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	else if (nrChannels == 4)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	// Generate mipmaps
	glGenerateMipmap(GL_TEXTURE_2D);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	return image_texture;
}

