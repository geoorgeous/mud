#ifndef FONT_HPP
#define FONT_HPP

#include <string>
#include <unordered_map>

#include "graphics/texture.hpp"
#include "math/rect.hpp"
#include "math/vector.hpp"
#include "utils/asset_object.hpp"

namespace mud
{
	enum class FontStyle
	{
		Regular,
		Bold,
		Italic,
		BoldItalic
	};

	struct FontCharacterGlyph
	{
		Vector2 textureUVMin;
		Vector2 textureUVMax;
		Vector2 bearing;
		Vector2 size;
		float advance;
	};

	struct FontFace
	{
		FontStyle style;
		Texture texture;
		std::unordered_map<char32_t, FontCharacterGlyph> characterGlyphs;
		float lineHeight;
		float spaceWidth;
	};

	class FontFamily : public AssetObject<FontFamily>
	{
	public:

		static std::string getFontStyleString(FontStyle style);

		FontFamily();

		~FontFamily();

		const std::string & getTypeFaceName() const;

		const FontFace * getFontFace() const;

		const FontFace * getFontFace(FontStyle style) const;

		bool fromFile(const std::string & filepath);

		virtual bool deserialize(std::ifstream & file) override;

		virtual bool serialize(std::ofstream & file) const override;

	private:

		std::string m_typeFaceName;
		std::unordered_map<FontStyle, FontFace *> m_fontFaces;
	};

	template<>
	inline AssetObject<FontFamily>::AssetObject()
		: AssetObjectBase(AssetObjectType::FontFamily)
	{ }
}

#endif