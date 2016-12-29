#include "ScriptResource.h"
#include "Interfaces.h"
#include "ExecutionManager.h"
#include "Misc.h"

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

// SImage ----------------------

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

// SRenderTarget -----------------------------

SRenderTarget::SRenderTarget(int w, int h) : SImage(0)
{
    Handle = MakeScreen(w, h, TRUE);
    Width = w;
    Height = h;
}

SRenderTarget * SRenderTarget::CreateBlankTarget(int w, int h)
{
    auto result = new SRenderTarget(w, h);
    result->AddRef();
    return result;
}

// SFont --------------------------------------

SFont::SFont()
{
    for (int i = 0; i < 0x10000; i++) Chars.push_back(nullptr);
}

SFont::~SFont()
{
    for (auto &i : Chars) if (i) delete i;
    for (auto &i : Images) i->Release();
}

tuple<double, double, int> SFont::RenderRaw(SRenderTarget * rt, const std::wstring & str)
{
    double cx = 0, cy = 0;
    double mx = 0, my = 0;
    int line = 1;
    if (rt)
    {
        BEGIN_DRAW_TRANSACTION(rt->GetHandle());
        ClearDrawScreen();
        SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        SetDrawBright(255, 255, 255);
    }
    for (auto &c : str)
    {
        if (c == L'\n')
        {
            line++;
            cx = 0;
            cy += Size;
            mx = max(mx, cx);
            my = line * Size;
            continue;
        }
        auto gi = Chars[c];
        if (!gi) continue;
        if (rt)DrawRectGraph(
            cx + gi->bearX, cy + gi->bearY,
            gi->x, gi->y,
            gi->width, gi->height,
            Images[gi->texture]->GetHandle(),
            TRUE, FALSE);
        cx += gi->wholeAdvance;
    }
    if (rt)
    {
        FINISH_DRAW_TRANSACTION;
    }
    mx = max(mx, cx);
    my = line * Size;
    return make_tuple(mx, my, line);
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
    result->AddRef();
    return result;
}

// SEffect --------------------------------

SEffect::SEffect(EffectData *rawdata)
{
    data = rawdata;
}

SEffect::~SEffect()
{
}

// SSound -----------------------------------
SSound::SSound(SoundManager *mng, SoundSample *smp)
{
	manager = mng;
	sample = smp;
}

SSound::~SSound()
{
	StopAll();
	if (sample) manager->ReleaseSound(sample);
	manager->ReleaseSound(sample);
	sample = nullptr;
}

void SSound::Play()
{
	manager->Play(sample);
}

void SSound::StopAll()
{
	manager->Stop(sample);
}

SSound * SSound::CreateSound(SoundManager *smanager)
{
    return new SSound(smanager, nullptr);
}

SSound * SSound::CreateSoundFromFile(SoundManager *smanager, const std::string & file, int simul)
{
    auto hs = smanager->LoadSampleFromFile(file.c_str(), simul);
    return new SSound(smanager, hs);
}


void RegisterScriptResource(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterface()->GetEngine();

    engine->RegisterObjectType(SU_IF_IMAGE, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_FACTORY, SU_IF_IMAGE "@ f()", asFUNCTION(SImage::CreateBlankImage), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_ADDREF, "void f()", asMETHOD(SImage, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_RELEASE, "void f()", asMETHOD(SImage, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_IMAGE, "int get_Width()", asMETHOD(SImage, get_Width), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_IMAGE, "int get_Height()", asMETHOD(SImage, get_Height), asCALL_THISCALL);
    //engine->RegisterObjectMethod(SU_IF_IMAGE, SU_IF_IMAGE "& opAssign(" SU_IF_IMAGE "&)", asFUNCTION(asAssign<SImage>), asCALL_CDECL_OBJFIRST);

    engine->RegisterObjectType(SU_IF_FONT, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_FACTORY, SU_IF_FONT "@ f()", asFUNCTION(SFont::CreateBlankFont), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_ADDREF, "void f()", asMETHOD(SFont, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_FONT, asBEHAVE_RELEASE, "void f()", asMETHOD(SFont, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_FONT, "int get_Size()", asMETHOD(SFont, get_Size), asCALL_THISCALL);

    engine->RegisterObjectType(SU_IF_EFXDATA, 0, asOBJ_REF);
    //engine->RegisterObjectBehaviour(SU_IF_EFXDATA, asBEHAVE_FACTORY, SU_IF_EFXDATA "@ f()", asFUNCTION(SFont::CreateBlankFont), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_EFXDATA, asBEHAVE_ADDREF, "void f()", asMETHOD(SEffect, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_EFXDATA, asBEHAVE_RELEASE, "void f()", asMETHOD(SEffect, Release), asCALL_THISCALL);

	engine->RegisterObjectType(SU_IF_SOUND, 0, asOBJ_REF);
	engine->RegisterObjectBehaviour(SU_IF_SOUND, asBEHAVE_ADDREF, "void f()", asMETHOD(SSound, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour(SU_IF_SOUND, asBEHAVE_RELEASE, "void f()", asMETHOD(SSound, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_SOUND, "void Play()", asMETHOD(SSound, Play), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_SOUND, "void StopAll()", asMETHOD(SSound, StopAll), asCALL_THISCALL);
}