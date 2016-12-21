#pragma once

enum SoundType {
	Sample,
	Stream,
};

class Sound {
public:
	SoundType Type;
};

class SoundSample : Sound {
public:
	SoundSample();
};

class SoundStream : Sound {
public:
	SoundStream();

};

class SoundManager final {
private:
	HPLUGIN hPluginMix;
	HPLUGIN hPluginEffect;

public:
	SoundManager();
	~SoundManager();

	void SetMasterVolume(float value);
	void Update();

	SoundSample *LoadSampleFromFile(const char *fileName, int max = 16);
	SoundStream *LoadStreamFromFile(const char *fileName);
};