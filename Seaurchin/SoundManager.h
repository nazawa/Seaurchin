#pragma once

class SoundManager;
class Sound;

enum SoundType {
	Sample,
	Stream,
};

enum SoundState {
	Loading,
	Ready,
	Playing,
	WaitToPlay,
	Paused
};

class Sound {
	friend class SoundManager;

public:
	SoundType Type;
	SoundState State;
};

class SoundSample : public Sound {
	friend class SoundManager;

protected:
	HSAMPLE hSample;
	float PreservedVolume;
public:

	SoundSample(HSAMPLE sample);
};

class SoundStream : public Sound {
	friend class SoundManager;

protected:
	HSTREAM hStream;

public:
	SoundStream(HSTREAM stream);
};

//こいつのポインタをOBJFIRSTで渡すのギルティーじゃない？
class SoundManager final {
private:
	HPLUGIN hPluginMix = 0;
	HPLUGIN hPluginEffect = 0;

	HSTREAM MasterMixerChannel;
	std::set<HCHANNEL> ConnectedChannels;
	std::set<Sound*> LoadedSounds;

public:
	SoundManager();
	~SoundManager();

	void SetMasterVolume(float value);
	void Update();

	SoundSample *LoadSampleFromFile(const char *fileName, int max = 16);
	SoundStream *LoadStreamFromFile(const char *fileName);
	void ReleaseSound(SoundSample *sample);
	void ReleaseSound(SoundStream *stream);

	void Play(SoundSample *sample);
	void Stop(SoundSample *sample);
	void SetVolume(SoundSample *sample, float volume);

	void Play(SoundStream *sample);
	void Stop(SoundStream *sample);
	void SetVolume(SoundStream *sample, float volume);
};
