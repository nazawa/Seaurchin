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
};

class SoundSample : public Sound {
    friend class SoundManager;

protected:
    HSAMPLE hSample;

    SoundSample(HSAMPLE sample);
    ~SoundSample();

public:
    DWORD GetSoundHandle() override;
    void StopSound() override;

    SoundSample *CreateFromFile(const std::string &fileNameA, int maxChannels = 16);
    void SetLoop(bool looping);
};

class SoundStream : public Sound {
    friend class SoundManager;

protected:
    HSTREAM hStream;

    SoundStream(HSTREAM stream);
    ~SoundStream();

public:
    DWORD GetSoundHandle() override;
    void StopSound() override;

    SoundStream *CreateFromFile(const std::string &fileNameA);
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

//こいつのポインタをOBJFIRSTで渡すのギルティーじゃない？
class SoundManager final {
private:

public:
	SoundManager();
	~SoundManager();
};
