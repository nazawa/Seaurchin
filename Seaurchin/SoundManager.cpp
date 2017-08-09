#include "SoundManager.h"
#include "Setting.h"
#include "Debug.h"

using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;

// SoundSample ------------------------
SoundSample::SoundSample(HSAMPLE sample)
{
    Type = SoundType::Sample;
    hSample = sample;
}

SoundSample::~SoundSample()
{
    if (hSample) BASS_SampleFree(hSample);
    hSample = 0;
}

DWORD SoundSample::GetSoundHandle()
{
    return BASS_SampleGetChannel(hSample, FALSE);
}

void SoundSample::StopSound()
{
    BASS_SampleStop(hSample);
}

void SoundSample::SetVolume(float vol)
{
    BASS_SAMPLE si = { 0 };
    BASS_SampleGetInfo(hSample, &si);
    si.volume = vol;
    BASS_SampleSetInfo(hSample, &si);
}

SoundSample *SoundSample::CreateFromFile(const wstring &fileNameW, int maxChannels)
{
    std::ifstream file(fileNameW, ios::in);
    file.seekg(0, ios::end);
    auto ep = file.tellg();
    file.clear();
    file.seekg(0, ios::beg);
    auto size = ep - file.tellg();

    char *data = new char[size];
    file.read(data, size);
    file.close();

    auto handle = BASS_SampleLoad(TRUE, data, 0, size, maxChannels, BASS_SAMPLE_OVER_POS);
    delete[] data;

    SoundSample *result = new SoundSample(handle);
    return result;
}

void SoundSample::SetLoop(bool looping)
{
    BASS_SAMPLE info;
    BASS_SampleGetInfo(hSample, &info);
    if (looping) {
        info.flags |= BASS_SAMPLE_LOOP;
    } else {
        info.flags = info.flags & ~BASS_SAMPLE_LOOP;
    }
    BASS_SampleSetInfo(hSample, &info);
}

// SoundStream ------------------------
SoundStream::SoundStream(HSTREAM stream)
{
    Type = SoundType::Stream;
    hStream = stream;
}

SoundStream::~SoundStream()
{
    if (hStream) BASS_StreamFree(hStream);
    hStream = 0;
}

DWORD SoundStream::GetSoundHandle()
{
    return hStream;
}

void SoundStream::StopSound()
{
    BASS_ChannelStop(hStream);
}

void SoundStream::SetVolume(float vol)
{
    BASS_ChannelSetAttribute(hStream, BASS_ATTRIB_VOL, clamp(vol, 0.0f, 1.0f));
}

SoundStream *SoundStream::CreateFromFile(const wstring & fileNameW)
{
    std::ifstream file(fileNameW, ios::in);
    file.seekg(0, ios::end);
    auto ep = file.tellg();
    file.clear();
    file.seekg(0, ios::beg);
    auto size = ep - file.tellg();

    char *data = new char[size];
    file.read(data, size);
    file.close();

    auto handle = BASS_StreamCreateFile(TRUE, data, 0, size, 0);
    delete[] data;

    SoundStream *result = new SoundStream(handle);
    return result;
}

double SoundStream::GetPlayingPosition()
{
    auto pos = BASS_ChannelGetPosition(hStream, BASS_POS_BYTE);
    return BASS_ChannelBytes2Seconds(hStream, pos);
}

void SoundStream::SetPlayingPosition(double pos)
{
    auto bp = BASS_ChannelSeconds2Bytes(hStream, pos);
    BASS_ChannelSetPosition(hStream, bp, BASS_POS_BYTE);
}

// SoundMixerStream ------------------------
SoundMixerStream::SoundMixerStream(int ch, int freq)
{
    hMixerStream = BASS_Mixer_StreamCreate(freq, ch, 0);
}

SoundMixerStream::~SoundMixerStream()
{
    if (hMixerStream) {
        for (auto &ch : PlayingSounds) BASS_Mixer_ChannelRemove(ch);
        BASS_StreamFree(hMixerStream);
        hMixerStream = 0;
    }
}

void SoundMixerStream::Update()
{
    if (!hMixerStream) return;

    auto snd = PlayingSounds.begin();
    while (snd != PlayingSounds.end()) {
        auto state = BASS_ChannelIsActive(*snd);
        if (state != BASS_ACTIVE_STOPPED) {
            snd++;
            continue;
        }
        BASS_Mixer_ChannelRemove(*snd);
        snd = PlayingSounds.erase(snd);
    }
}

void SoundMixerStream::Play(Sound * sound)
{
    auto ch = sound->GetSoundHandle();
    PlayingSounds.emplace(ch);
    BASS_Mixer_StreamAddChannel(hMixerStream, ch, 0);
    BASS_ChannelPlay(ch, FALSE);
}

void SoundMixerStream::Stop(Sound * sound)
{
    sound->StopSound();
    //ƒ`ƒƒƒ“ƒlƒ‹íœ‚ÍUpdate‚É”C‚¹‚é
}

void SoundMixerStream::SetVolume(float vol)
{
    BASS_ChannelSetAttribute(hMixerStream, BASS_ATTRIB_VOL, vol);
}

// SoundManager -----------------------------
SoundManager::SoundManager()
{
    //‚æ‚ë‚µ‚­‚È‚¢
    if (!BASS_Init(-1, 44100, 0, GetMainWindowHandle(), NULL)) abort();
    WriteDebugConsole(TEXT("BASS_Init\n"));
}

SoundManager::~SoundManager()
{
    BASS_Free();
}

SoundMixerStream *SoundManager::CreateMixerStream()
{
    return new SoundMixerStream(2, 44100);
}

void SoundManager::PlayGlobal(Sound * sound)
{
    BASS_ChannelPlay(sound->GetSoundHandle(), FALSE);
}

void SoundManager::StopGlobal(Sound * sound)
{
    sound->StopSound();
}
