#include "SoundManager.h"
#include "Debug.h"

SoundManager::SoundManager()
{
	//‚æ‚ë‚µ‚­‚È‚¢
	if (!BASS_Init(-1, 44100, 0, GetMainWindowHandle(), NULL)) abort();
	WriteDebugConsole(TEXT("BASS_Init\n"));
	hPluginMix =  BASS_PluginLoad("bassmix.dll", 0);
	hPluginEffect = BASS_PluginLoad("bass_fx.dll", 0);
	if (!(hPluginMix * hPluginEffect)) abort();
}

SoundManager::~SoundManager()
{
	BASS_PluginFree(hPluginMix);
	BASS_PluginFree(hPluginEffect);
	BASS_Free();
}

void SoundManager::SetMasterVolume(float value)
{
}

void SoundManager::Update()
{
}

SoundSample * SoundManager::LoadSampleFromFile(const char * fileName, int max)
{
	return nullptr;
}

SoundStream * SoundManager::LoadStreamFromFile(const char * fileName)
{
	return nullptr;
}

SoundSample::SoundSample()
{
	Type = SoundType::Sample;
}

SoundStream::SoundStream()
{
	Type = SoundType::Stream;
}
