#include "TextRenderer.h"
#include "Camera.h"

#include <gtc\type_ptr.hpp>
#include <gtc\matrix_transform.hpp>


TextRenderer::~TextRenderer()
{
}

void TextRenderer::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, GLuint screenWidth, GLuint screenHeight, int FontSize)
{
	this->fontSize = FontSize;

	if (!initialized)
	{
		Initialize();
		initialized = true;
	}

	// Activate corresponding render state	
	TextShader::shader->Use();

	glUniformMatrix4fv(TextShader::shader->projectionID, 1, GL_FALSE, &Camera::activeCamera->OrthoProjection[0][0]);
	glUniform3f(TextShader::shader->colorID, color.x, color.y, color.z);
	glUniform1i(TextShader::shader->text, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};

		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::Initialize()
{
	// Initialize FreeType
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		printf("Failed to initialize FreeType Library");
	}

	// Load font face
	FT_Face face;
	if (FT_New_Face(ft, "Assets/Fonts/arial.ttf", 0, &face))
	{
		printf("Failed to load font");
	}

	// Set font size to extract
	FT_Set_Pixel_Sizes(face, 0, fontSize);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load font characters
	LoadCharacters(face);

	// Destroy FreeType objects
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	TextShader::shader = new TextShader("shaders\\textShader.VERTEXSHADER", "shaders\\textShader.FRAGMENTSHADER");
	CreateRenderData();
}

void TextRenderer::LoadCharacters(FT_Face face)
{
	// Disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			printf("Failed to load  Glyph");
			continue;
		}

		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Add to character map
		Character character =
		{
			texture,
			glm::ivec2((GLuint)face->glyph->bitmap.width, (GLuint)face->glyph->bitmap.rows),
			glm::ivec2((GLuint)face->glyph->bitmap_left, (GLuint)face->glyph->bitmap_top),
			(GLuint)face->glyph->advance.x
		};

		Characters.insert(std::pair<GLchar, Character>(c, character));

		// Unbind texture
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void TextRenderer::CreateRenderData()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}