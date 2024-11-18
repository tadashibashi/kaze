#include "common.h"
#include <string>
#include <string_view>
#include <fstream>

struct Brackets
{
    enum Enum
    {
        OpenParen = '(',
        CloseParen = ')',
        OpenBrace = '{',
        CloseBrace = '}',
        OpenBracket = '[',
        CloseBracket = ']',
    };
};

struct InAttributeType
{
    enum Enum
    {
        Position,
        Normal,
        Tangent,
        Bitangent,
        Color0,
        Color1,
        Color2,
        Color3,
        Indices,
        Weight,
        TexCoord0,
        TexCoord1,
        TexCoord2,
        TexCoord3,
        TexCoord4,
        TexCoord5,
        TexCoord6,
        TexCoord7,
        Count,
    };

    static const char *toBgfxName(const Enum value)
    {
        switch(value)
        {
        case Position: return "POSITION";
        case Normal: return "NORMAL";
        case Tangent: return "TANGENT";
        case Bitangent: return "BITANGENT";
        case Color0: return "COLOR0";
        case Color1: return "COLOR1";
        case Color2: return "COLOR2";
        case Color3: return "COLOR3";
        case Indices: return "INDICES";
        case Weight: return "WEIGHT";
        case TexCoord0: return "TEXCOORD0";
        case TexCoord1: return "TEXCOORD1";
        case TexCoord2: return "TEXCOORD2";
        case TexCoord3: return "TEXCOORD3";
        case TexCoord4: return "TEXCOORD4";
        case TexCoord5: return "TEXCOORD5";
        case TexCoord6: return "TEXCOORD6";
        case TexCoord7: return "TEXCOORD7";
        default: return "";
        }
    }

    static Enum fromText(const std::string_view name)
    {
        if (name.empty())
            return Count; // sentinel value

        switch(name[0])
        {
        case 'B':
            if (name == "Bitangent")
                return Bitangent;
            break;
        case 'C':
            if (name.length() == 6 && name.starts_with("Color"))
            {
                int index = name[5] - '0';
                if (index < 0 || index > 3)
                {
                    throw std::out_of_range("Color index is out of range: make sure to limit 4 Color attributes");
                }

                return static_cast<Enum>(Color0 + index);
            }
            break;

        case 'I':
            if (name == "Indices")
                return Indices;
            break;

        case 'N':
            if (name == "Normal")
                return Normal;
            break;

        case 'P':
            if (name == "Position")
                return Position;
            break;

        case 'T':
            if (name == "Tangent")
                return Tangent;
            if (name.length() == 9 && name.starts_with("TexCoord"))
            {
                int index = name[8] - '0';
                if (index < 0 || index > 7)
                {
                    throw std::out_of_range("TexCoord index is out of range: make sure to limit 8 TexCoord attributes");
                }

                return static_cast<Enum>(TexCoord0 + index);
            }
            break;

        case 'W':
            if (name == "Weight")
                return Weight;
            break;
        }

        return Count;
    }
};

// First check for attribute
struct StorageQualifier
{
    enum Enum
    {
        In, Out, Uniform, Count
    };


    static auto fromText(const std::string_view text)
    {
        if (text == "in")
            return In;
        if (text == "out")
            return Out;
        if (text == "uniform")
            return Uniform;
        return Count;
    }

    static const char *toText(const Enum value)
    {
        switch(value)
        {
        case In: return "in";
        case Out: return "out";
        default: return "";
        }
    }
};

struct InAttribute
{

};


int main(int argc, char *argv[]) {
    auto vertShader = R"(
in Position pos;
in Color    col;
in TexCoord uv;
)";
    return 0;
}
