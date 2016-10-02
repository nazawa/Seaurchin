#include "DxLibResource.h"

#include "Config.h"
#include "Debug.h"
#include "Setting.h"
#include "Misc.h"

using namespace std;

DxLibResource::DxLibResource()
{
}

DxLibResource::~DxLibResource()
{
}

// Font -----------------

VirtualFont::VirtualFont(int h, string name, int size)
{
    fontName = name;
    this->size = size;
    handle = h;
}

VirtualFont::~VirtualFont()
{
    DeleteFontToHandle(handle);
}

shared_ptr<VirtualFont> VirtualFont::Create(std::string name, int size)
{
    int h = CreateFontToHandle(name.c_str(), size, -1, DX_FONTTYPE_ANTIALIASING_4X4);
    return shared_ptr<VirtualFont>(new VirtualFont(h, name, size));
}

void VirtualFont::DrawRaw(const string & str, double x, double y)
{
    DrawStringFToHandle(x, y, str.c_str(), GetColor(255, 255, 255), handle);
}

// Image ------------------------

Image::Image(int handle)
{
    this->handle = handle;
}

Image::~Image()
{
    DeleteGraph(handle);
}

std::shared_ptr<Image> Image::LoadFromFile(std::string fileName)
{
    return shared_ptr<Image>(new Image(LoadGraph(fileName.c_str())));
}

std::shared_ptr<Image> Image::LoadFromMemory(void * buffer, int size)
{
    return shared_ptr<Image>(new Image(CreateGraphFromMem(buffer, size)));
}

// Font ------------------------------

Font::Font()
{
    for (int i = 0; i < 0x10000; i++) chars.push_back(nullptr);
}


Font::~Font()
{
    for (const auto& gi : chars) if (gi) delete gi;
}

int Font::DrawRawUTF8(const std::string & str, double x, double y)
{
    
    int ulen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wchar_t *wstr = new wchar_t[ulen];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wstr, ulen);
    double cx = 0;
    if (x != INT_MIN)
    {
        for (int i = 0; i < ulen - 1; i++)
        {
            auto ci = chars[wstr[i]];

            DrawRectGraphF(
                x + cx + ci->bearX, y + ci->bearY,
                ci->x, ci->y,
                ci->width, ci->height,
                images[ci->texture]->GetHandle(),
                TRUE, FALSE);

            cx += ci->wholeAdvance;
        }
    }
    else
    {
        for (int i = 0; i < ulen - 1; i++) cx += chars[wstr[i]]->wholeAdvance;
    }
    delete[] wstr;
    return cx;
}

std::shared_ptr<Font> Font::LoadFromFile(std::string fileName)
{
    shared_ptr<Font> ret(new Font());
    ifstream font(fileName, ios::in | ios::binary);

    FontDataHeader header;
    font.read((char*)&header, sizeof(FontDataHeader));
    ret->size = header.Size;

    for (int i = 0; i < header.GlyphCount; i++)
    {
        GlyphInfo *info = new GlyphInfo();
        font.read((char*)info, sizeof(GlyphInfo));
        ret->chars[info->letter] = info;
    }
    int size;
    for (int i = 0; i < header.ImageCount; i++)
    {
        font.read((char*)&size, sizeof(int));
        uint8_t *pngdata = new uint8_t[size];
        font.read((char*)pngdata, size);
        ret->images.push_back(Image::LoadFromMemory(pngdata, size));
        delete[] pngdata;
    }
    return ret;
}

void Font::CreateAndSave(std::string name, std::string save, int size, int texWidth, int texHeight)
{
    using namespace boost::filesystem;

    std::ofstream fontstream;
    ostringstream ss;
    FT_Library freetype;
    FT_Face face;
    FT_Error error;
    RectPacker packer;
    int dpi = 72;

    FT_Init_FreeType(&freetype);
    error = FT_New_Face(freetype, name.c_str(), 0, &face);
    if (error)
    {
        WriteDebugConsole("Can't Read Font File!\n");
        return;
    }

    FT_Size_RequestRec req;
    req.width = 0;
    req.height = (int)size * 64;
    req.horiResolution = 0;
    req.vertResolution = 0;
    req.type = FT_SIZE_REQUEST_TYPE_BBOX;
    FT_Request_Size(face, &req);
    FT_Select_Charmap(face, FT_ENCODING_UNICODE);

    //FT_Set_Char_Size(face, 0, size * 64, 0, dpi);
    int glyphs = face->num_glyphs;
    ss << "Found " << glyphs << " Glyphs in " << face->family_name << endl;
    WriteDebugConsole(ss.str().c_str());

    //書き込み開始
    path sfn = Setting::GetRootDirectory() / SU_DATA_DIR / SU_FONT_DIR / (save + ".sif");
    fontstream.open(sfn.string(), ios::out | ios::trunc | ios::binary);
    fontstream.seekp(sizeof(FontDataHeader));

    int imgidx = 0;
    uint8_t *bitmap = new uint8_t[texWidth * texHeight * 2];
    memset(bitmap, 0, texWidth * texHeight * 2);
    packer.Init(texWidth, texHeight, (int)size);

    int baseline = -face->size->metrics.descender >> 6;
    int all = 0;
    GlyphInfo ginfo;
    FT_GlyphSlot gslot;
    FT_UInt gidx;
    FT_ULong code;
    code = FT_Get_First_Char(face, &gidx);
    while (gidx != 0)
    {
        FT_Load_Glyph(face, gidx, FT_LOAD_DEFAULT);
        gslot = face->glyph;
        FT_Render_Glyph(gslot, FT_RENDER_MODE_NORMAL);
        ginfo.width = gslot->bitmap.width;
        ginfo.height = gslot->bitmap.rows;
        ginfo.wholeAdvance = gslot->metrics.horiAdvance >> 6;
        ginfo.bearX = gslot->metrics.horiBearingX >> 6;
        ginfo.bearY = size - (baseline + (gslot->metrics.horiBearingY >> 6));
        ginfo.letter = code;
        ginfo.texture = imgidx;
        ginfo.x = 0;
        ginfo.y = 0;
        if (ginfo.width * ginfo.height == 0)
        {
            //まさか' 'がグリフを持たないとは思わなかった(いや当たり前でしょ)
            fontstream.write((const char*)&ginfo, sizeof(GlyphInfo));
            all++;
            code = FT_Get_Next_Char(face, code, &gidx);
            continue;
        }

        RectPacker::Rect rect;
        rect = packer.Insert(ginfo.width, ginfo.height);
        if (rect.height == 0)
        {
            //先に現行のを保存する
            ss.str("");
            ss << "FontOutput" << imgidx << ".png";
            auto ffp = Setting::GetRootDirectory() / SU_DATA_DIR / SU_CACHE_DIR / ss.str();
            SaveFontTexture(bitmap, texWidth, texHeight, ffp.string().c_str());
            delete[] bitmap;
            bitmap = new uint8_t[texWidth * texHeight * 2];
            memset(bitmap, 0, texWidth * texHeight * 2);

            imgidx++;
            packer.Init(texWidth, texHeight, size);
            rect = packer.Insert(ginfo.width, ginfo.height);
        }
        ginfo.x = rect.x;
        ginfo.y = rect.y;

        uint8_t *buffer = new uint8_t[rect.width * 2];
        for (int y = 0; y < gslot->bitmap.rows; y++)
        {
            for (int x = 0; x < rect.width; x++)
            {
                buffer[x * 2] = 0xff;
                buffer[x * 2 + 1] = gslot->bitmap.buffer[y * rect.width + x];
            }
            int py = rect.y + y;
            memcpy_s(bitmap + (py * texHeight * 2 + rect.x * 2), rect.width * 2, buffer, rect.width * 2);
        }
        delete[] buffer;

        if (gidx % 1000 == 0)
        {
            ss.str("");
            ss << "Rendered " << gidx << " Glyphs" << endl;
            WriteDebugConsole(ss.str().c_str());
        }

        fontstream.write((const char*)&ginfo, sizeof(GlyphInfo));
        all++;
        code = FT_Get_Next_Char(face, code, &gidx);
    }

    ss.str("");
    ss << "FontOutput" << imgidx << ".png";
    auto ffp = Setting::GetRootDirectory() / SU_DATA_DIR / SU_CACHE_DIR / ss.str();
    SaveFontTexture(bitmap, texWidth, texHeight, ffp.string().c_str());
    delete[] bitmap;
    imgidx++;

    FT_Done_Face(face);
    FT_Done_FreeType(freetype);

    //ここからが本番なのだよ
    FontDataHeader header;
    header.GlyphCount = all;
    header.ImageCount = imgidx;
    header.Size = size;
    for (int i = 0; i < imgidx; i++)
    {
        std::ifstream fif;
        ss.str("");
        ss << "FontOutput" << i << ".png";
        ffp = Setting::GetRootDirectory() / SU_DATA_DIR / SU_CACHE_DIR / ss.str();
        fif.open(ffp.string(), ios::binary | ios::in);
        int fsize = fif.seekg(0, ios::end).tellg();

        uint8_t *file = new uint8_t[fsize];
        fif.seekg(ios::beg);
        fif.read((char*)file, fsize);
        fif.close();

        fontstream.write((const char*)&fsize, sizeof(int));
        fontstream.write((const char*)file, fsize);
        delete[] file;
    }
    fontstream.seekp(0, ios::beg);
    fontstream.write((const char*)&header, sizeof(header));

    fontstream.close();
}

void Font::SaveFontTexture(uint8_t *bitmap, int width, int height, std::string name)
{
    FILE *file;
    fopen_s(&file, name.c_str(), "wb");
    if (file == nullptr) return;
    auto png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    auto info = png_create_info_struct(png);
    png_init_io(png, file);
    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY_ALPHA, NULL, PNG_COMPRESSION_TYPE_DEFAULT, NULL);
    auto rows = new png_byte*[height];
    for (int i = 0; i < height; i++) rows[i] = bitmap + width * i * 2;
    png_set_rows(png, info, rows);
    png_write_png(png, info, PNG_TRANSFORM_IDENTITY, nullptr);
    png_destroy_write_struct(&png, &info);
    if (file) fclose(file);
    delete[] rows;
}

void RectPacker::Init(int w, int h, int rowh)
{
    width = w;
    height = h;
    row = 0;
    cursorX = 0;
    cursorY = 0;
}

RectPacker::Rect RectPacker::Insert(int w, int h)
{
    if (cursorX + w >= width)
    {
        cursorX = 0;
        cursorY += row;
    }
    if (cursorY + h > height) return Rect{ 0 };
    if (w > width) return Rect{ 0 };
    Rect r;
    r.x = cursorX;
    r.y = cursorY;
    r.width = w;
    r.height = h;
    cursorX += w;
    row = max(h, row);
    return r;
}

RenderTarget::RenderTarget(int w, int h)
{
    handle = MakeScreen(w, h, TRUE);
}

RenderTarget::~RenderTarget()
{
    DeleteGraph(handle);
}

std::shared_ptr<RenderTarget> RenderTarget::Create(int w, int h)
{
    return shared_ptr<RenderTarget>(new RenderTarget(w, h));
}


