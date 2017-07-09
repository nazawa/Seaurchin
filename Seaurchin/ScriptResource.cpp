#include "ScriptResource.h"
#include "Interfaces.h"
#include "ExecutionManager.h"
#include "Misc.h"

using namespace std;

SResource::SResource()
{}

SResource::~SResource()
{}

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
	if (Handle) DeleteGraph(Handle);
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

SImage * SImage::CreateLoadedImageFromFile(const string &file, bool async)
{
    if (async) SetUseASyncLoadFlag(TRUE);
    auto result = new SImage(LoadGraph(ConvertUTF8ToShiftJis(file).c_str()));
    if (async) SetUseASyncLoadFlag(FALSE);
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
	Width = w;
	Height = h;
	if (w * h) Handle = MakeScreen(w, h, TRUE);
}

SRenderTarget * SRenderTarget::CreateBlankTarget(int w, int h)
{
	auto result = new SRenderTarget(w, h);
	result->AddRef();
	return result;
}

// SNinePatchImage ----------------------------
SNinePatchImage::SNinePatchImage(int ih) : SImage(ih)
{}

SNinePatchImage::~SNinePatchImage()
{
	DeleteGraph(Handle);
	Handle = 0;
	LeftSideWidth = TopSideHeight = BodyWidth = BodyHeight = 0;
}

void SNinePatchImage::SetArea(int leftw, int toph, int bodyw, int bodyh)
{
	LeftSideWidth = leftw;
	TopSideHeight = toph;
	BodyWidth = bodyw;
	BodyHeight = bodyh;
}

// SAnimatedImage --------------------------------

SAnimatedImage::SAnimatedImage(int w, int h, int count, double time) : SImage(0)
{
    CellWidth = Width = w;
    CellHeight = Height = h;
    FrameCount = count;
    SecondsPerFrame = time;
}

SAnimatedImage::~SAnimatedImage()
{
    for (auto &img : Images) DeleteGraph(img);
}

SAnimatedImage * SAnimatedImage::CreateLoadedImageFromFile(const std::string & file,int xc, int yc, int w, int h, int count, double time)
{
    auto result = new SAnimatedImage(w, h, count, time);
    result->Images.resize(count);
    LoadDivGraph(ConvertUTF8ToShiftJis(file).c_str(), count, xc, yc, w, h, result->Images.data());
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
	if (rt) {
		BEGIN_DRAW_TRANSACTION(rt->GetHandle());
		ClearDrawScreen();
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		SetDrawBright(255, 255, 255);
	}
	for (auto &c : str) {
		if (c == L'\n') {
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
	if (rt) {
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

	for (int i = 0; i < header.GlyphCount; i++) {
		GlyphInfo *info = new GlyphInfo();
		font.read((char*)info, sizeof(GlyphInfo));
        if (result->Chars[info->letter]) continue; //TODO:wchar_t”ÍˆÍŠO‚Ì•¶Žš‚Ì‘Î‰ž
		result->Chars[info->letter] = info;
	}
	int size;
	for (int i = 0; i < header.ImageCount; i++) {
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
{}

// SSoundMixer ------------------------------

SSoundMixer::SSoundMixer(SoundMixerStream * mixer)
{
    this->mixer = mixer;
}

SSoundMixer::~SSoundMixer()
{
    delete mixer;
}

void SSoundMixer::Update()
{
    mixer->Update();
}

void SSoundMixer::Play(SSound * sound)
{
    mixer->Play(sound->sample);
}

void SSoundMixer::Stop(SSound * sound)
{
    mixer->Stop(sound->sample);
}

SSoundMixer * SSoundMixer::CreateMixer(SoundManager * manager)
{
    auto result = new SSoundMixer(manager->CreateMixerStream());
    result->AddRef();
    return result;
}


// SSound -----------------------------------
SSound::SSound(SoundSample *smp)
{
	sample = smp;
}

SSound::~SSound()
{
    delete sample;
}

void SSound::SetLoop(bool looping)
{
    sample->SetLoop(looping);
}

SSound * SSound::CreateSound(SoundManager *smanager)
{
    auto result =  new SSound(nullptr);
    result->AddRef();
    return result;
}

SSound * SSound::CreateSoundFromFile(SoundManager *smanager, const std::string & file, int simul)
{
	auto hs = SoundSample::CreateFromFile(ConvertUTF8ToShiftJis(file).c_str(), simul);
    auto result = new SSound(hs);
    result->AddRef();
    return result;
}

// SSettingItem --------------------------------------------

SSettingItem::SSettingItem(shared_ptr<SettingItem> s) : setting(s)
{
    
}

SSettingItem::~SSettingItem()
{
    setting->SaveValue();
}

void SSettingItem::Save()
{
    setting->SaveValue();
}

void SSettingItem::MoveNext()
{
    setting->MoveNext();
}

void SSettingItem::MovePrevious()
{
    setting->MovePrevious();
}

std::string SSettingItem::GetItemText()
{
    return setting->GetItemString();
}


void RegisterScriptResource(ExecutionManager *exm)
{
	auto engine = exm->GetScriptInterfaceUnsafe()->GetEngine();

	engine->RegisterObjectType(SU_IF_IMAGE, 0, asOBJ_REF);
	engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_FACTORY, SU_IF_IMAGE "@ f()", asFUNCTION(SImage::CreateBlankImage), asCALL_CDECL);
    engine->RegisterObjectBehaviour(SU_IF_IMAGE, asBEHAVE_FACTORY, SU_IF_IMAGE "@ f(const string &in, bool = false)", asFUNCTION(SImage::CreateLoadedImageFromFile), asCALL_CDECL);
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
    engine->RegisterObjectMethod(SU_IF_SOUND, "void SetLoop(bool)", asMETHOD(SSound, SetLoop), asCALL_THISCALL);

    engine->RegisterObjectType(SU_IF_SOUNDMIXER, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SOUNDMIXER, asBEHAVE_ADDREF, "void f()", asMETHOD(SSoundMixer, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SOUNDMIXER, asBEHAVE_RELEASE, "void f()", asMETHOD(SSoundMixer, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SOUNDMIXER, "void Play(" SU_IF_SOUND "@)", asMETHOD(SSoundMixer, Play), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SOUNDMIXER, "void Stop(" SU_IF_SOUND "@)", asMETHOD(SSoundMixer, Stop), asCALL_THISCALL);

	engine->RegisterObjectType(SU_IF_9IMAGE, 0, asOBJ_REF);
	//engine->RegisterObjectBehaviour(SU_IF_9IMAGE, asBEHAVE_FACTORY, SU_IF_IMAGE "@ f()", asFUNCTION(SNinePatchImage::CreateBlankImage), asCALL_CDECL);
	engine->RegisterObjectBehaviour(SU_IF_9IMAGE, asBEHAVE_ADDREF, "void f()", asMETHOD(SNinePatchImage, AddRef), asCALL_THISCALL);
	engine->RegisterObjectBehaviour(SU_IF_9IMAGE, asBEHAVE_RELEASE, "void f()", asMETHOD(SNinePatchImage, Release), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_9IMAGE, "int get_Width()", asMETHOD(SNinePatchImage, get_Width), asCALL_THISCALL);
	engine->RegisterObjectMethod(SU_IF_9IMAGE, "int get_Height()", asMETHOD(SNinePatchImage, get_Height), asCALL_THISCALL);

    engine->RegisterObjectType(SU_IF_ANIMEIMAGE, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_ANIMEIMAGE, asBEHAVE_ADDREF, "void f()", asMETHOD(SAnimatedImage, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_ANIMEIMAGE, asBEHAVE_RELEASE, "void f()", asMETHOD(SAnimatedImage, Release), asCALL_THISCALL);

    engine->RegisterObjectType(SU_IF_SETTING_ITEM, 0, asOBJ_REF);
    engine->RegisterObjectBehaviour(SU_IF_SETTING_ITEM, asBEHAVE_ADDREF, "void f()", asMETHOD(SSettingItem, AddRef), asCALL_THISCALL);
    engine->RegisterObjectBehaviour(SU_IF_SETTING_ITEM, asBEHAVE_RELEASE, "void f()", asMETHOD(SSettingItem, Release), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SETTING_ITEM, "void Save()", asMETHOD(SSettingItem, Save), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SETTING_ITEM, "void MoveNext()", asMETHOD(SSettingItem, MoveNext), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SETTING_ITEM, "void MovePrevious()", asMETHOD(SSettingItem, MovePrevious), asCALL_THISCALL);
    engine->RegisterObjectMethod(SU_IF_SETTING_ITEM, "string GetItemText()", asMETHOD(SSettingItem, GetItemText), asCALL_THISCALL);
}