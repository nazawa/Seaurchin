#pragma once

class SoundManager;

enum SoundType {
	Sample,
	Stream,
};

class Sound {
    friend class SoundManager;
public:
    SoundType Type;

    virtual DWORD GetSoundHandle() = 0;
    virtual void StopSound() = 0;
    virtual void SetVolume(float vol) = 0;
};

class SoundSample : public Sound {
    friend class SoundManager;

protected:
    HSAMPLE hSample;

public:
    SoundSample(HSAMPLE sample);
    ~SoundSample();

    DWORD GetSoundHandle() override;
    void StopSound() override;
    void SetVolume(float vol) override;

    static SoundSample *CreateFromFile(const std::string &fileNameA, int maxChannels = 16);
    void SetLoop(bool looping);
};

class SoundStream : public Sound {
    friend class SoundManager;

protected:
    HSTREAM hStream;

public:
    SoundStream(HSTREAM stream);
    ~SoundStream();

    DWORD GetSoundHandle() override;
    void StopSound() override;
    void SetVolume(float vol) override;

    static SoundStream *CreateFromFile(const std::string &fileNameA);
    double GetPlayingPosition();
};

class SoundMixerStream {
protected:
    HSTREAM hMixerStream;
    std::unordered_set<HCHANNEL> PlayingSounds;

public:
    SoundMixerStream(int ch, int freq);
    ~SoundMixerStream();

    void Update();
    void SetVolume(float vol);
    void Play(Sound *sound);
    void Stop(Sound *sound);
};

class SoundManager final {
private:

public:
	SoundManager();
	~SoundManager();

    SoundMixerStream *CreateMixerStream();
    void PlayGlobal(Sound *sound);
    void StopGlobal(Sound *sound);
};
