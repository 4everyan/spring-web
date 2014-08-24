
#include <string>
#include "Texture.h"
#include "Extensions.h"
#include "GraphicObjManager.h"



class TextureObject : public GraphicObject
{
private:
    void Release() override
    {
        glDeleteTextures(1, &handle);
        handle = 0;
    }
public:
    TextureObject() : handle(0) {}
    TextureObject(GLuint h) : handle(h) {}
    GLuint handle = 0;
};


class TextureBinder
{
public:
	TextureBinder(Texture& _texture): texture(_texture) {
		texture.Bind();
	}
	~TextureBinder() {
		texture.Unbind();
	}
private:
	Texture& texture;
};


std::shared_ptr<Texture> Texture::LoadPicture(const SDL_Surface* surface, GLenum filter, bool generateMipmap)
{
	std::shared_ptr<Texture> tex( new Texture );
	glGenTextures(1, &tex->handle);

    // регистрируем объект текстуры в менеджере
    // теперь нам не нужно заботиться об освобождении памяти
    std::shared_ptr<GraphicObject> texObject( new TextureObject(tex->handle) );
    GraphicObjManager::Instance()->RegisterObject(texObject);

    tex->Bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	if( generateMipmap && filter == GL_LINEAR )
		filter = GL_LINEAR_MIPMAP_LINEAR;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum format = surface->format->Amask ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
	if( generateMipmap )
		FramebufferExt::GenerateMipmap(GL_TEXTURE_2D);

	tex->width = surface->w;
	tex->height = surface->h;
    tex->Unbind();

	return tex;
}

std::shared_ptr<Texture> Texture::LoadPicture(const char* file, GLenum filter, bool generateMipmap)
{
	std::shared_ptr<SDL_Surface> surface( IMG_Load(file), SDL_FreeSurface );
	if( !surface ) {
		SDL_Log("Image file \"%s\" not found or have unsupported format!", file);
		return std::shared_ptr<Texture>( new Texture );
	}
	return Texture::LoadPicture( surface.get(), filter, generateMipmap );
}

std::shared_ptr<Texture> Texture::CreateBlank(GLuint width, GLuint height, GLenum filter, GLenum format, GLenum internalFormat)
{
	std::shared_ptr<Texture> tex( new Texture );
	glGenTextures(1, &tex->handle);

    // регистрируем объект текстуры в менеджере
    // теперь нам не нужно заботиться об освобождении памяти
    std::shared_ptr<GraphicObject> texObject(new TextureObject(tex->handle));
    GraphicObjManager::Instance()->RegisterObject(texObject);

    tex->Bind();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	if( filter == GL_LINEAR )
		filter = GL_LINEAR_MIPMAP_LINEAR;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

	tex->width = width;
	tex->height = height;
    tex->Unbind();

	return tex;
}

void Texture::SetSubData(GLint xoffset, GLint yoffset, GLuint width, GLuint height, GLenum format, GLenum type, GLvoid* pixels)
{
	TextureBinder binder{ *this };
    glTexSubImage2D(GL_TEXTURE_2D, 0, xoffset, yoffset, width, height, format, type, pixels);
}

void Texture::UpdatePixels(GLenum format, GLenum type, GLvoid* pixels)
{
    return SetSubData(0, 0, width, height, format, type, pixels);
}

void Texture::Bind()
{
	glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*) &previous);
	glBindTexture(GL_TEXTURE_2D, handle);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, previous);
}

void Texture::GenerateMipmap()
{
	TextureBinder binder{ *this };
	FramebufferExt::GenerateMipmap(GL_TEXTURE_2D);
}
