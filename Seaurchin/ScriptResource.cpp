#include "ScriptResource.h"
#include "Interfaces.h"

using namespace std;

SResource::SResource()
{
}

SResource::~SResource()
{
}

void SResource::AddRef()
{
    Reference++;
}

void SResource::Release()
{
    if (--Reference == 0) delete this;
}

void SImage::ObtainSize()
{
    GetGraphSize(Handle, &Width, &Height);
}

SImage::SImage(int ih)
{
    Handle = ih;
}

SImage::~SImage()
{
    DeleteGraph(Handle);
    WriteDebugConsole(":‚ ‚Ÿ‚ñ?Å‹ß‚¾‚ç‚µ‚Ë‚¥‚È?\n");
    Handle = 0;
}

int SImage::get_Width()
{
    if (!Width) ObtainSize();
    return Width;
}

int SImage::get_Height()
{
    if (!Height) ObtainSize();
    return Height;
}

SImage * SImage::CreateBlankImage()
{
    auto result = new SImage(0);
    result->AddRef();
    return result;
}

SImage * SImage::CreateLoadedImageFromFile(const string & file)
{
    auto result = new SImage(LoadGraph(ConvertUTF8ToShiftJis(file).c_str()));
    result->AddRef();
    return result;
}

SImage * SImage::CreateLoadedImageFromMemory(void * buffer, size_t size)
{
    auto result = new SImage(CreateGraphFromMem(buffer, size));
    result->AddRef();
    return result;
}

SRenderTarget::SRenderTarget(int w, int h) : SImage(0)
{
    Handle = MakeScreen(w, h, TRUE);
    Width = w;
    Height = h;
}

SRenderTarget::~SRenderTarget()
{
    DeleteGraph(Handle);
}

SFont::SFont()
{
    for (int i = 0; i < 0x1000; i++) Chars.push_back(nullptr);
}

SFont::~SFont()
{
    for (auto &i : Chars) if (i) delete i;
    for (auto &i : Images) i->Release();
}

SFont * SFont::CreateBlankFont()
{
    auto result = new SFont();
    result->AddRef();
    return result;
}

SFont * SFont::CreateLoadedFontFromFile(const string & file)
{
    auto result = new SFont();
    ifstream font(ConvertUTF8ToShiftJis(file), ios::in | ios::binary);

    FontDataHeader header;
    font.read((char*)&header, sizeof(FontDataHeader));
    result->Size = header.Size;

    for (int i = 0; i < header.GlyphCount; i++)
    {
        GlyphInfo *info = new GlyphInfo();
        font.read((char*)info, sizeof(GlyphInfo));
        result->Chars[info->letter] = info;
    }
    int size;
    for (int i = 0; i < header.ImageCount; i++)
    {
        font.read((char*)&size, sizeof(int));
        uint8_t *pngdata = new uint8_t[size];
        font.read((char*)pngdata, size);
        result->Images.push_back(SImage::CreateLoadedImageFromMemory(pngdata, size));
        delete[] pngdata;
    }
    return result;
}

void RegisterScriptResource(asIScriptEngine * engine)
{
    engine->RegisterObjectType(SU_IF_IMAGE, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_FACTORY, SU_IF_IMAGE "@ f()", asFUNCTION(SImage::CreateBlankImage), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_ADDREF, "void f()", asMETHOD(SImage, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_RELEASE, "void f()", asMETHOD(SImage, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_IMAGE, "int get_Width()", asMETHOD(SImage, get_Width), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_IMAGE, "int get_Height()", asMETHOD(SImage, get_Height), asCALL_THISCALL);
    //engine->RegisterObjectMethod(SU_IF_IMAGE, SU_IF_IMAGE "@ opAssign(" SU_IF_IMAGE "@)", asFUNCTION(asAssign<SImage>), asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectType(SU_IF_FONT, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_FACTORY, SU_IF_FONT "@ f()", asFUNCTION(SFont::CreateBlankFont), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_ADDREF, "void f()", asMETHOD(SFont, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_RELEASE, "void f()", asMETHOD(SFont, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_FONT, "int get_Size()", asMETHOD(SFont, get_Size), asCALL_THISCALL);
}
