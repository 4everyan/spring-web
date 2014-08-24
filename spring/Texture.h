
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glew.h>
//#include <GL/glu.h>
#include <memory>



class Texture
{
private:
	Texture()
	{
		width = 0;
		height = 0;
		previous = 0;
		handle = 0;
	}
	Texture(const Texture&);
	void operator = (const Texture&);

public:
	static std::shared_ptr<Texture> LoadPicture(const SDL_Surface* surface, GLenum filter, bool generateMipmap = true);
	static std::shared_ptr<Texture> LoadPicture(const char* file, GLenum filter, bool generateMipmap = true);
	static std::shared_ptr<Texture> CreateBlank(GLuint width, GLuint height, GLenum filter, GLenum format, GLenum internalFormat);

    void SetSubData(GLint xoffset, GLint yoffset, GLuint width, GLuint height, GLenum format, GLenum type, GLvoid* pixels);
    void UpdatePixels(GLenum format, GLenum type, GLvoid* pixels);

	void Bind();
	void Unbind();
	void GenerateMipmap();

	GLuint Handle() const { return handle; }
	bool IsHandleValid() const { return handle > 0; }
	int Width() const { return width; }
	int Height() const { return height; }

private:
	GLuint handle;
	GLuint previous;
	int width;
	int height;
};

#endif //__TEXTURE_H__
