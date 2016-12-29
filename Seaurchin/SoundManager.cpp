#include "SoundManager.h"
#include "Setting.h"
#include "Debug.h"

using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;

SoundSample::SoundSample(HSAMPLE sample)
{
    Type = SoundType::Sample;
    State = SoundState::Ready;
    hSample = sample;
}

SoundStream::SoundStream(HSTREAM stream)
{
    Type = SoundType::Stream;
    State = SoundState::Ready;
    hStream = stream;
}

SoundManager::SoundManager()
{
    int err = 0;
    //よろしくない
    if (!BASS_Init(-1, 44100, 0, GetMainWindowHandle(), NULL)) abort();
    WriteDebugConsole(TEXT("BASS_Init\n"));

    //TODO: 設定項目化
    MasterMixerChannel = BASS_Mixer_StreamCreate(44100, 2, 0);
    err = BASS_ErrorGetCode();
}

SoundManager::~SoundManager()
{
    BASS_Free();
}

void SoundManager::SetMasterVolume(float value)
{
}

//あんまり高頻度で呼ばないで…
void SoundManager::Update()
{
    //チャンネル自動解除
    auto cch = ConnectedChannels.begin();
    while (cch != ConnectedChannels.end()) {
        auto state = BASS_ChannelIsActive(*cch);
        if (state != BASS_ACTIVE_STOPPED) {
            cch++;
            continue;
        }
        BASS_Mixer_ChannelRemove(*cch);
        cch = ConnectedChannels.erase(cch);
    }
}

SoundSample * SoundManager::LoadSampleFromFile(const char * fileName, int max)
{
    auto handle = BASS_SampleLoad(FALSE, fileName, 0, 0, max, BASS_SAMPLE_OVER_POS);
    if (!handle) return nullptr;

    SoundSample *result = new SoundSample(handle);
    LoadedSounds.emplace(result);

    return result;
}

SoundStream * SoundManager::LoadStreamFromFile(const char * fileName)
{
    auto handle = BASS_StreamCreateFile(FALSE, fileName, 0, 0, 0);
    if (!handle) return nullptr;

    SoundStream *result = new SoundStream(handle);
    LoadedSounds.emplace(result);

    return result;
}

//deleteされるので気をつけてください
void SoundManager::ReleaseSound(SoundSample * sample)
{
    BASS_SampleFree(sample->hSample);
    LoadedSounds.erase(sample);
    delete sample;
}

//deleteされるので気をつけてください
void SoundManager::ReleaseSound(SoundStream * stream)
{
    BASS_StreamFree(stream->hStream);
    LoadedSounds.erase(stream);
    delete stream;
}

void SoundManager::Play(SoundSample * sample)
{
    HCHANNEL channel = 0;
    switch (sample->State) {
    case SoundState::Loading:
        break;
    case SoundState::Ready:
        channel = BASS_SampleGetChannel(sample->hSample, FALSE);
        BASS_ChannelPlay(channel, FALSE);
        BASS_Mixer_StreamAddChannel(MasterMixerChannel, channel, 0);
        ConnectedChannels.emplace(channel);
        break;
    case SoundState::Paused:
        //まぁ多分ならないけど
        Stop(sample);
        break;
    case SoundState::Playing:
        //確定でならない
        break;
    case SoundState::WaitToPlay:
        break;
    }
}

void SoundManager::Stop(SoundSample * sample)
{
    BASS_SampleStop(sample->hSample);
}

void SoundManager::SetVolume(SoundSample * sample, float volume)
{
    sample->PreservedVolume = clamp(volume, 0.0f, 1.0f);
    BASS_SAMPLE si = { 0 };
    BASS_SampleGetInfo(sample->hSample, &si);
    si.volume = sample->PreservedVolume;
    BASS_SampleSetInfo(sample->hSample, &si);
}

void SoundManager::Play(SoundStream * stream)
{
    switch (stream->State) {
    case SoundState::Loading:
        break;
    case SoundState::Ready:
        BASS_ChannelPlay(stream->hStream, FALSE);
        BASS_Mixer_StreamAddChannel(MasterMixerChannel, stream->hStream, 0);
        ConnectedChannels.emplace(stream->hStream);
        break;
    case SoundState::Paused:
        //まぁ多分ならないけど
        Stop(stream);
        break;
    case SoundState::Playing:
    case SoundState::WaitToPlay:
        break;
    }
}

void SoundManager::Stop(SoundStream * stream)
{
    BASS_ChannelStop(stream->hStream);
}

void SoundManager::SetVolume(SoundStream * stream, float volume)
{
    BASS_ChannelSetAttribute(stream->hStream, BASS_ATTRIB_VOL, clamp(volume, 0.0f, 1.0f));
}
