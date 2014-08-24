
#ifndef __FONTRENDERER_H__
#define __FONTRENDERER_H__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <pugiconfig.hpp>
#include <pugixml.hpp>
#include <memory>
#include "Texture.h"


struct Letter
{
	int x = 0, y = 0;
	int w = 0, h = 0;
	int pre = 0, post = 0;
	bool empty = true;
};


class FontRenderer
{
private:
	FontRenderer()
	{
		m_color = glm::vec4( 1.0f );
        lineHeight = 1.5f;
	}

public:
	static const int max_letters = 256;

	static std::shared_ptr<FontRenderer> LoadFont(const char* file)
	{
		std::shared_ptr<FontRenderer> renderer( new FontRenderer );
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file(file);
		if( !result ) {
			SDL_Log("Font error: %s", result.description());
			return nullptr;
		}
		pugi::xml_node root = doc.child("font");
		if( root.empty() ) {
			SDL_Log("Font error: can't find root <font> tag in file %s", file);
			return nullptr;
		}

		int lettersCount = 0;
		renderer->m_letterHeight = 0;

		for( pugi::xml_node node = root.child("letter"); node; node = node.next_sibling("letter") ) {

			std::string codestr = node.text().as_string();
			if( codestr.empty() )
				continue;

			int code = 0;
			try {
				code = std::stoi("0" + codestr, nullptr, 16);
				if( code < 0 || code >= max_letters )
					continue;
			} catch (...) {
				continue;
			}
			Letter a;
			a.x = node.attribute("x").as_int();
			a.y = node.attribute("y").as_int();
			a.w = node.attribute("w").as_int();
			a.h = node.attribute("h").as_int();
			a.pre = node.attribute("pre").as_int();
			a.post = node.attribute("post").as_int();
			a.empty = false;

			renderer->m_letters[code] = a;
			renderer->m_letterHeight = std::max( (float)a.h, renderer->m_letterHeight );
			lettersCount++;
		}
		if( lettersCount < 1 ) {
			renderer->m_texture = Texture::CreateBlank(16, 16, GL_NEAREST, GL_RGB, GL_RGB8);
			return renderer;
		}
		std::string bitmap = root.attribute("bitmap").as_string();
		if( bitmap.empty() ) {
			renderer->m_texture = Texture::CreateBlank(16, 16, GL_NEAREST, GL_RGB, GL_RGB8);
			return renderer;
		}

		std::string path = file;
		std::string::size_type backSlash = path.find_last_of("\\/");
		if( backSlash < path.size() ) {
			path = path.substr(0, backSlash);
			path += "/" + bitmap;
		}
		else path = bitmap;
		renderer->m_texture = Texture::LoadPicture(path.c_str(), GL_NEAREST, false);

		return renderer;
	}

	void RenderText(float xstart, float ystart, const char* text)
	{
		float xpos = xstart;
		float ypos = ystart;

		glPushAttrib(GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_texture->Bind();
		glColor4f( m_color.r, m_color.g, m_color.b, m_color.a );

		glBegin(GL_QUADS);
		while( *text )
		{
			if( *text == '\n' )
			{
				xpos = xstart;
				ypos -= m_letterHeight * lineHeight;
			}
			else
			{
				unsigned i = (unsigned)(*text);
				if( i > max_letters-1 )
					i = '?';
				if( m_letters[i].empty )
					i = '?';

				if( m_letters[i].empty )
					continue;

				xpos += (float)m_letters[i].pre;
				RenderLetter(xpos, ypos, m_letters[i]);
				xpos += (float)m_letters[i].w + (float)m_letters[i].post;
			}
			++text;
		}
		glEnd();
		m_texture->Unbind();
		glPopAttrib(); //GL_COLOR_BUFFER_BIT
	}

	void SetColor(float r, float g, float b, float alpha)
	{
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_color[3] = alpha;
	}

    float GetHeight() const { return m_letterHeight; }
    float GetLineHeight() const { return lineHeight; }

private:
	void RenderLetter(float x, float y, Letter& a)
	{
		float tw = (float)m_texture->Width();
		float th = (float)m_texture->Height();
		
		glTexCoord2f( a.x/tw, (a.y + a.h)/th );
		glVertex2f(x, y);
			
		glTexCoord2f( (a.x + a.w)/tw, (a.y + a.h)/th );
		glVertex2f(x + a.w, y);
			
		glTexCoord2f( (a.x + a.w)/tw, a.y/th );
		glVertex2f(x + a.w, y + a.h);
			
		glTexCoord2f(a.x/tw, a.y/th);
		glVertex2f(x, y + a.h);
	}

private:
	glm::vec4 m_color;
	std::shared_ptr<Texture> m_texture;
	Letter m_letters[max_letters];
	float m_letterHeight;
    float lineHeight;
};


#endif //__FONTRENDERER_H__
