#include "audio.h"

#include "stb_vorbis.c"
#include <al.h>
#include <alc.h>

ALCdevice* aDevice;
ALCcontext* Context;
ALboolean g_bEAX;

ALfloat ListenerPos[3] = { 0.0,0.0,0.0 };
ALfloat ListenerVel[3] = { 0.0,0.0,0.0 };
ALfloat ListenerOri[6] = { 0.0,0.0,-1.0,0.0,1.0,0.0 };

int startAudio()
{
	alGetError();
	//alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	aDevice = alcOpenDevice(nullptr);
	if (aDevice) {
		Context = alcCreateContext(aDevice, nullptr);
		alcMakeContextCurrent(Context);
	}
	g_bEAX = alIsExtensionPresent("EAX2.0");
	audioError();
	return 0;
}

void audioError()
{
	int code = alGetError();
	switch (code)
	{
	case AL_NO_ERROR:
		break;
	case AL_OUT_OF_MEMORY:
		std::cout << "ERROR: OUT OF MEMORY \n";
		throw std::runtime_error("Audio Error");
	case AL_INVALID_NAME:
		std::cout << "ERROR: INVALID NAME \n";
		throw std::runtime_error("Audio Error");
	case AL_INVALID_ENUM:
		std::cout << "ERROR: INVALID ENUM \n";
		throw std::runtime_error("Audio Error");
	case AL_INVALID_VALUE:
		std::cout << "ERROR: INVALID VALUE \n";
		throw std::runtime_error("Audio Error");
	case AL_INVALID_OPERATION:
		std::cout << "ERROR: INVALID OPERATION \n";
		throw std::runtime_error("Audio Error");
	}
}
	
void cleanAudio()
{
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(Context);
	alcCloseDevice(aDevice);
}

AudioSource::~AudioSource()
{
	destroySource();
	unloadAudio();
}

void AudioSource::init(WAV& wav)
{
	loadAudio(wav);
	createSource();
}

void AudioSource::play()
{
	alSourcePlay(source);
	audioError();
}

void AudioSource::stop()
{
	alSourceStop(source);
	audioError();
}

void AudioSource::loadAudio(WAV& wav)
{
	alGenBuffers(1, &buffer);
	audioError();
	ALenum format;
	switch (wav.channelCount)
	{
	case 1:
		format = AL_FORMAT_MONO16;
		break;
	case 2:
		format = AL_FORMAT_STEREO16;
		break;
	default:
		throw std::runtime_error("unsupported channel count");
	}

	int size = wav.data.size() * 2;

	alBufferData(buffer, format, wav.data.data(), 
		size - (size % 4), wav.sampleRate);
	audioError();
}

void AudioSource::unloadAudio()
{
	const ALuint buffers[1] = { buffer };
	alDeleteBuffers(1, buffers);
}

void AudioSource::createSource()
{
	alGenSources(1, &source);
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
	alSourcei(source, AL_BUFFER, buffer);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_GAIN, 0.05f);
	alSourcefv(source, AL_POSITION, sourcePos);
	alSourcefv(source, AL_VELOCITY, sourceVel);
	alSourcei(source, AL_LOOPING, AL_FALSE);

	audioError();
}

void AudioSource::destroySource()
{
	const ALuint sources[1] = { source };
	alDeleteSources(1, sources);
}



WAV::~WAV()
{
	
}

void WAV::load(std::string filename)
{
	if (filename.back() == 'g')
		loadVorbis(filename);
	else
		loadWav(filename);
}

void WAV::loadWav(std::string filename)
{
	int headerSize = sizeof(WAVHeader);
	int fileLength = 0;
	WAVHeader h;

	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	fileLength = (size_t)file.tellg();
	if (fileLength > headerSize) {
		file.seekg(0);
		file.read((char*)&h, headerSize);

		this->sampleRate = h.SamplesPerSec;
		this->channelCount = h.NumOfChan;

		file.seekg(headerSize);
		this->data.resize((fileLength - headerSize) / 2);
		file.read((char*)this->data.data(), fileLength - headerSize);

		file.close();
	}
	else
	{
		file.close();
		throw std::runtime_error("file is invalid");
	}
}

void WAV::loadVorbis(std::string filename)
{
	int len, channels = 0, samplerate = 44100;
	short* decoded;
	len = stb_vorbis_decode_filename(filename.c_str(), &channels, &sampleRate, &decoded);
	if (len > 0) {

		this->data.resize(len * channels);
		this->channelCount = channels;
		this->sampleRate = samplerate;

		//std::cout << "samplerate: " << this->sampleRate << "\n";
		//std::cout << "length: " << len << "\n";
		//std::cout << "channels: " << this->channelCount << "\n";

		memcpy(this->data.data(), decoded, this->data.size() * sizeof(short));

		free(decoded);
	}
	else
	{
		std::cout << "could not load vorbis file" << std::endl;
		throw std::runtime_error("could not load vorbis file");
	}
}
