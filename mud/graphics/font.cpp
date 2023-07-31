#include "font.hpp"

#include <freetype/ft2build.h>
#include <freetype/ftbitmap.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftmm.h>
#include <vector>

#include "utils/logger.hpp"

namespace mud
{
	struct FontCharacterGlyphInternal
	{
		FontCharacterGlyph glyph;
		uint16_t bufferPitch;
		uint16_t bufferRows;
		uint8_t * buffer;
		char32_t charcode;

		bool operator<(const FontCharacterGlyphInternal & rhs)
		{
			if (bufferRows == rhs.bufferRows)
				return bufferPitch < rhs.bufferPitch;
			return bufferRows < rhs.bufferRows;
		}
	};

	bool checkFTError(FT_Error error, const std::string & errorMessage)
	{
		if (error == FT_Err_Ok)
			return true;

		log(LogLevel::Error, fmt::format("{0}: {1}\n", errorMessage, FT_Error_String(error)), "FreeType");\
		return false;
	}

	std::string FontFamily::getFontStyleString(FontStyle style)
	{
		switch (style)
		{
			case FontStyle::Regular:
				return "Regular";
			case FontStyle::Bold:
				return "Bold";
			case FontStyle::Italic:
				return "Italic";
			case FontStyle::BoldItalic:
				return "Bold Italic";
			default:
				return "Font Style String Unsupported";
		}
	}

	FontFamily::FontFamily()
	{ }

	FontFamily::~FontFamily()
	{
		for (auto & pair : m_fontFaces)
			delete pair.second;
	}

	const std::string & FontFamily::getTypeFaceName() const
	{
		return m_typeFaceName;
	}

	const FontFace * FontFamily::getFontFace() const
	{
		if (m_fontFaces.empty())
			return nullptr;
		return m_fontFaces.begin()->second;
	}

	const FontFace * FontFamily::getFontFace(FontStyle style) const
	{
		auto iter = m_fontFaces.find(style);

		if (iter == m_fontFaces.end())
			return nullptr;

		return iter->second;
	}

	bool FontFamily::fromFile(const std::string & filepath)
	{
		FT_Library ftLibrary;

		if (!checkFTError(FT_Init_FreeType(&ftLibrary), "Failed to initialise FreeType library"))
			return false;

		FT_Face ftFace;

		//if (!checkFTError(FT_New_Face(ftLibrary, filepath.c_str(), -1, &ftFace), fmt::format("Failed to read font file '{0}'", filepath)))
		if (!checkFTError(FT_New_Face(ftLibrary, filepath.c_str(), -1, &ftFace), "Failed to read font file "))
			return false;

		FT_Long numFaces = ftFace->num_faces;

		FT_Done_Face(ftFace);

		for (FT_Long idx = 0; idx < numFaces; ++idx)
		{
			if (!checkFTError(FT_New_Face(ftLibrary, filepath.c_str(), idx, &ftFace), fmt::format("Failed to get face {0} from font file '{1}'", idx, filepath)))
				continue;

			const uint32_t fontFaceTextureWidth = 512;
			uint32_t fontFaceTextureHeight = 512;
			float pt = 16.0f;
			const float fontUnitScale = 1.0f / 64;

			if (!checkFTError(FT_Set_Char_Size(ftFace, 0, static_cast<FT_F26Dot6>(pt * 64), 0, 0), fmt::format("Failed to set character glyph size for font face (loaded from file '{0}') to {1}", filepath, pt)))
				continue;

			if (!checkFTError(FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE), fmt::format("Failed to select 'Unicode' character encoding for font face (loaded from file '{0}')", filepath)))
				continue;

			if (idx == 0)
			{
				if (ftFace->family_name != nullptr)
					m_typeFaceName = std::string(ftFace->family_name);
				else
				{
					const char * postscriptName = FT_Get_Postscript_Name(ftFace);

					if (postscriptName != nullptr)
						m_typeFaceName = std::string(postscriptName);
					else
						m_typeFaceName = "TYPE FACE NAME NOT FOUND";
				}
			}

			FontStyle fontStyle = FontStyle::Regular;

			if ((ftFace->style_flags & FT_STYLE_FLAG_BOLD & FT_STYLE_FLAG_ITALIC) == FT_STYLE_FLAG_BOLD & FT_STYLE_FLAG_ITALIC)
				fontStyle = FontStyle::BoldItalic;
			else if ((ftFace->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD)
				fontStyle = FontStyle::Bold;
			else if ((ftFace->style_flags & FT_STYLE_FLAG_BOLD) == FT_STYLE_FLAG_BOLD)
				fontStyle = FontStyle::Italic;

			FontFace & fontFace = *m_fontFaces.emplace(fontStyle, new FontFace{}).first->second;
			fontFace.style = fontStyle;

			const char32_t chars[] = U"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz()[]{}<>/\\\"`'^*¬-=+~_,.!?;:¦|£$€%&@# ";

			std::vector<FontCharacterGlyphInternal> tempFontCharacterGlyphs(sizeof(chars) / sizeof(char32_t));
			fontFace.characterGlyphs.reserve(tempFontCharacterGlyphs.size());

			for (auto it = tempFontCharacterGlyphs.begin(); it != tempFontCharacterGlyphs.end(); ++it)
			{
				const char32_t character = chars[it - tempFontCharacterGlyphs.begin()];

				if (!checkFTError(FT_Load_Char(ftFace, static_cast<FT_ULong>(character), FT_LOAD_RENDER), fmt::format("Failed to load character glyph for charcode '{0}' from font face loaded from file '{1}'", static_cast<uint32_t>(chars[idx]), filepath)))
					continue;

				const FT_GlyphSlot glyph = ftFace->glyph;

				FT_Bitmap convertedBitmap;
				FT_Bitmap_Init(&convertedBitmap);
				if (!checkFTError(FT_Bitmap_Convert(ftLibrary, &glyph->bitmap, &convertedBitmap, 1), fmt::format("Failed to convert glyph bitmap of font face loaded from file '{0}'", filepath)))
					continue;

				it->charcode = character;

				it->glyph.bearing = Vector2(static_cast<float>(glyph->metrics.horiBearingX), static_cast<float>(glyph->metrics.horiBearingY)) * fontUnitScale;
				it->glyph.size = Vector2(static_cast<float>(glyph->metrics.width), static_cast<float>(glyph->metrics.height)) * fontUnitScale;
				it->glyph.advance = static_cast<float>(glyph->advance.x) * fontUnitScale;

				it->bufferPitch = convertedBitmap.pitch;
				it->bufferRows = convertedBitmap.rows;
				it->buffer = new uint8_t[convertedBitmap.rows * convertedBitmap.pitch];
				memcpy(it->buffer, convertedBitmap.buffer, convertedBitmap.rows * convertedBitmap.pitch);
			}

			std::sort(tempFontCharacterGlyphs.begin(), tempFontCharacterGlyphs.end());

			uint8_t * fontFaceTextureData = new uint8_t[fontFaceTextureWidth * fontFaceTextureHeight];
			Vector2i nextGlyphBitmapPosition;
			uint32_t nextGlyphRowY = tempFontCharacterGlyphs.back().bufferRows;

			for (auto rit = tempFontCharacterGlyphs.rbegin(); rit != tempFontCharacterGlyphs.rend(); ++rit)
			{
				if (nextGlyphBitmapPosition[0] + rit->bufferPitch >= fontFaceTextureWidth)
				{
					nextGlyphBitmapPosition[0] = 0;
					nextGlyphBitmapPosition[1] = nextGlyphRowY;
					nextGlyphRowY += rit->bufferRows;
				}

				rit->glyph.textureUVMin.x = static_cast<float>(nextGlyphBitmapPosition[0]) / fontFaceTextureWidth;
				rit->glyph.textureUVMin.y = static_cast<float>(nextGlyphBitmapPosition[1]) / fontFaceTextureHeight;
				rit->glyph.textureUVMax.x = rit->glyph.textureUVMin.x + (static_cast<float>(rit->bufferPitch) / fontFaceTextureWidth);
				rit->glyph.textureUVMax.y = rit->glyph.textureUVMin.y + (static_cast<float>(rit->bufferRows) / fontFaceTextureHeight);

				for (size_t row = 0; row < rit->bufferRows; ++row)
				{
					memcpy(
						fontFaceTextureData + (nextGlyphBitmapPosition[0] + ((nextGlyphBitmapPosition[1] + row) * fontFaceTextureWidth)),
						rit->buffer + (row * rit->bufferPitch),
						rit->bufferPitch);
				}

				delete[] rit->buffer;

				nextGlyphBitmapPosition[0] += rit->bufferPitch;

				fontFace.characterGlyphs[rit->charcode] = rit->glyph;
			}

			fontFace.texture.setData(fontFaceTextureData, fontFaceTextureWidth, fontFaceTextureHeight, 1);

			delete[] fontFaceTextureData;

			fontFace.lineHeight = static_cast<float>(ftFace->size->metrics.height) * fontUnitScale;
			fontFace.spaceWidth = static_cast<float>(ftFace->size->metrics.max_advance) * fontUnitScale;

			checkFTError(FT_Done_Face(ftFace), fmt::format("Error calling FT_Done_Face"));

			log(LogLevel::Trace, fmt::format("Loaded font face from file '{0}': name: '{1}', style: '{2}'\n", filepath, m_typeFaceName, FontFamily::getFontStyleString(fontStyle)), "Font");
		}

		return true;
	}

	bool FontFamily::deserialize(std::ifstream & file)
	{
		serialization_helpers::deserialize(file, m_typeFaceName);

		size_t numFontFaces = 0;
		serialization_helpers::deserialize(file, numFontFaces);

		for (size_t fontFaceIdx = 0; fontFaceIdx < numFontFaces; ++fontFaceIdx)
		{
			FontStyle fontStyle;
			serialization_helpers::deserialize(file, fontStyle);
			FontFace & fontFace = *m_fontFaces.emplace(fontStyle, new FontFace).first->second;
			serialization_helpers::deserialize(file, fontFace.lineHeight);
			serialization_helpers::deserialize(file, fontFace.spaceWidth);
			size_t numCharacterGlyphs = 0;
			serialization_helpers::deserialize(file, numCharacterGlyphs);

			for (size_t charGlyphIdx = 0; charGlyphIdx < numCharacterGlyphs; ++charGlyphIdx)
			{
				char32_t character = 0;
				serialization_helpers::deserialize(file, character);

				FontCharacterGlyph & glyph = fontFace.characterGlyphs.emplace(character, FontCharacterGlyph{}).first->second;
				serialization_helpers::deserialize(file, glyph);
			}

			fontFace.texture.deserialize(file);
		}

		return true;
	}

	bool FontFamily::serialize(std::ofstream & file) const
	{
		serialization_helpers::serialize(file, m_typeFaceName);
		serialization_helpers::serialize(file, m_fontFaces.size());

		for (const auto & fontFacePair : m_fontFaces)
		{
			const FontFace & fontFace = *fontFacePair.second;

			serialization_helpers::serialize(file, fontFacePair.first);
			serialization_helpers::serialize(file, fontFace.lineHeight);
			serialization_helpers::serialize(file, fontFace.spaceWidth);
			serialization_helpers::serialize(file, fontFace.characterGlyphs.size());

			for (const auto & characterGlyphPair : fontFace.characterGlyphs)
			{
				serialization_helpers::serialize(file, characterGlyphPair.first);
				serialization_helpers::serialize(file, characterGlyphPair.second);
			}

			fontFace.texture.serialize(file);
		}

		return true;
	}
}