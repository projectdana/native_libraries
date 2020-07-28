//Written by Barry Porter, 2020

#include "dana_lib_defs.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nli_util.h"
#include "vmi_util.h"

#define DR_FLAC_IMPLEMENTATION
#include "extras/dr_flac.h"  /* Enables FLAC decoding. */
#define DR_MP3_IMPLEMENTATION
#include "extras/dr_mp3.h"   /* Enables MP3 decoding. */
#define DR_WAV_IMPLEMENTATION
#include "extras/dr_wav.h"   /* Enables WAV decoding. */

#include "extras/stb_vorbis.c" /* Enables OGG decoding. */

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#define D_FORMAT_F32 1
#define D_FORMAT_S16 2
#define D_FORMAT_S24 3
#define D_FORMAT_S32 4
#define D_FORMAT_U8 5

/*
	const byte FORMAT_F32 = 1
	const byte FORMAT_S16 = 2
	const byte FORMAT_S24 = 3
	const byte FORMAT_S32 = 4
	const byte FORMAT_U8  = 5
*/

#ifdef WINDOWS
#define startCriticalSection(X) EnterCriticalSection(X)
#endif
#ifdef LINUX
#define startCriticalSection(X) pthread_mutex_lock(X)
#endif

#ifdef WINDOWS
#define stopCriticalSection(X) LeaveCriticalSection(X)
#endif
#ifdef LINUX
#define stopCriticalSection(X) pthread_mutex_unlock(X)
#endif

#ifdef LINUX
pthread_mutexattr_t mAttr;
#endif

#define E_DEVICE_DESTROY  1
#define E_DEVICE_STOP  2

static GlobalTypeLink *trackInfoGT = NULL;

typedef struct _device_item {
	//the device data
	ma_device device;
	
	#ifdef WINDOWS
	CRITICAL_SECTION dlock;
	#endif
	#ifdef LINUX
	pthread_mutex_t dlock;
	#endif
	
	#ifdef WINDOWS
	HANDLE eventThread;
	#endif
	#ifdef LINUX
	pthread_t eventThread;
	#endif
	
	LiveObject *audioObject;
	
	Semaphore eventSem;
	int eventID;
	
	//TOOD: add a list of tracks-to-clean-up (in the event loop)
	struct _track_item *cleanupTracks;
	
	//list of decoders currently playing on this device
	struct _track_item *tracks;
	
	struct _device_item *next;
	} DeviceInstance;

typedef struct _decoder_item {
	ma_decoder decoder;
	ma_decoder_config decoderConfig;
	unsigned char type;
	unsigned char *inputData;
	size_t inputLength;
	} DecoderInstance;

typedef struct _track_item {
	DecoderInstance *source;
	ma_decoder decoder;
	float volume; //between 0.0 and 1.0 (1.0 is the highest)
	ma_uint32 cursor;
	bool newSeekPos;
	DeviceInstance *playingOn;
	bool playing;
	bool inCleanup;
	bool sendStopEvent;
	bool loop;
	LiveObject *objectRef;
	DanaType *orType;
	struct _track_item *next;
	struct _track_item *prev;
	} TrackInstance;

static DeviceInstance *devices;

static CoreAPI *api;

static void semaphore_init(Semaphore *s, unsigned int initialValue)
	{
	#ifdef WINDOWS
	s -> sem = CreateSemaphore(NULL, initialValue, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &s -> sem;
    *sem = dispatch_semaphore_create(initialValue);
	#else
	#ifdef LINUX
	sem_init(&s -> sem, initialValue, 0);
	#endif
	#endif
	}

static int semaphore_wait(Semaphore *s)
	{
	#ifdef WINDOWS
	return WaitForSingleObject(s -> sem, INFINITE);
	#endif
	#ifdef OSX
	return dispatch_semaphore_wait(s -> sem, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	return sem_wait(&s -> sem);
	#endif
	#endif
	}

static void semaphore_post(Semaphore *s)
	{
	#ifdef WINDOWS
	ReleaseSemaphore(s -> sem, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(s -> sem);
	#else
	#ifdef LINUX
	sem_post(&s -> sem);
	#endif
	#endif
	}

static void semaphore_destroy(Semaphore *s)
	{
	#ifdef WINDOWS
	CloseHandle(s -> sem);
	#endif
    #ifdef OSX
    dispatch_release(s -> sem);
    #else
	#ifdef LINUX
	sem_close(&s -> sem);
	#endif
    #endif
	}

#ifdef WINDOWS
DWORD WINAPI event_thread( LPVOID ptr ) 
#else
static void * event_thread(void *ptr)
#endif
	{
	DeviceInstance *instance = (DeviceInstance*) ptr;
	
	bool run = true;
	
	while (run)
		{
		//wait for event
		semaphore_wait(&instance -> eventSem);
		
		//decide what we're doing (just stopping the device, or shutting down the device & exiting the thread)
		
		startCriticalSection(&instance -> dlock);
		
		if (instance -> eventID == E_DEVICE_DESTROY)
			{
			run = false;
			stopCriticalSection(&instance -> dlock);
			}
			else if (instance -> eventID == E_DEVICE_STOP)
			{
			//check nothing is now queued to play, which is a possible race condition...
			if (instance -> tracks == NULL)
				{
				ma_device_stop(&instance -> device);
				stopCriticalSection(&instance -> dlock);
				}
				else
				{
				stopCriticalSection(&instance -> dlock);
				}
			}
		
		instance -> eventID = 0;
		
		//check for cleanup tracks
		
		bool moreCleaning = true;
		
		while (moreCleaning)
			{
			TrackInstance *ti = NULL;
			
			moreCleaning = false;
			
			startCriticalSection(&instance -> dlock);
			
			ti = instance -> cleanupTracks;
			
			if (ti != NULL)
				{
				instance -> cleanupTracks = instance -> cleanupTracks -> next;
				ti -> next = NULL;
				ti -> prev = NULL;
				ti -> inCleanup = false;
				}
			
			if (instance -> cleanupTracks != NULL)
				{
				instance -> cleanupTracks -> prev = NULL;
				moreCleaning = true;
				}
			
			stopCriticalSection(&instance -> dlock);
			
			if (ti != NULL)
				{
				//send event?
				if (ti -> sendStopEvent)
					{
					LiveData *nd = malloc(sizeof(LiveData));
					memset(nd, '\0', sizeof(LiveData));
					
					size_t sz = sizeof(VVarLivePTR);
					nd -> data = malloc(sz);
					memset(nd -> data, '\0', sz);
					
					nd -> gtLink = trackInfoGT;
					api -> incrementGTRefCount(nd -> gtLink);
					nd -> refi.type = nd -> gtLink -> typeLink;
					
					VVarLivePTR *ptrh = (VVarLivePTR*) nd -> data;
					
					ptrh -> content = (unsigned char*) ti -> objectRef;
					ptrh -> typeLink = ti -> orType;
					api -> decRef(NULL, ti -> objectRef);
					
					api -> pushEvent(instance -> audioObject, 0, 0, nd);
					}
				
				//decref the object
				if (ti != NULL)
					api -> decRef(NULL, ti -> objectRef);
				}
			}
		}
	
	ma_device_uninit(&instance -> device);
	free(instance);
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

static float temp_f[4096];
static ma_int32 temp_s32[4096];

ma_uint32 read_and_mix_pcm(ma_decoder* pDecoder, unsigned char format, int channels, float volume, void* pOutput, ma_uint32 frameCount)
	{
    // (basic mixing example, from miniaudio source -- only works for F32 format, could also clip samples to -1/+1)
	// -- ma_clip_samples_f32 ?
	ma_uint32 totalFramesRead = 0;
	
	if (format == D_FORMAT_F32)
		{
		ma_uint32 tempCapInFrames = ma_countof(temp_f) / channels;
		
		float* pOutputF32 = (float*)pOutput;

		while (totalFramesRead < frameCount)
			{
			ma_uint32 iSample;
			ma_uint32 framesReadThisIteration;
			ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
			ma_uint32 framesToReadThisIteration = tempCapInFrames;
			if (framesToReadThisIteration > totalFramesRemaining)
				{
				framesToReadThisIteration = totalFramesRemaining;
				}

			framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp_f, framesToReadThisIteration);
			if (framesReadThisIteration == 0)
				{
				break;
				}
			
			// apply volume delta, must be a value from 0.0 to 1.0
			size_t i = 0;
			for (i = 0; i < framesReadThisIteration; ++i)
				{
				size_t c = 0;
				for (c = 0; c < channels; ++c)
					{
					((float*)temp_f)[i*channels + c] *= volume;
					}
				}
			
			// Mix the frames together
			for (iSample = 0; iSample < framesReadThisIteration*channels; ++iSample)
				{
				pOutputF32[totalFramesRead*channels + iSample] += temp_f[iSample];
				}

			totalFramesRead += framesReadThisIteration;

			if (framesReadThisIteration < framesToReadThisIteration)
				{
				break;  /* Reached EOF. */
				}
			}
		}
		else if (format == D_FORMAT_S16)
		{
		
		}
		else if (format == D_FORMAT_S24)
		{
		
		}
		else if (format == D_FORMAT_S32)
		{
		ma_uint32 tempCapInFrames = ma_countof(temp_s32) / channels;
		
		ma_int32* pOutputS32 = (ma_int32*)pOutput;

		while (totalFramesRead < frameCount)
			{
			ma_uint32 iSample;
			ma_uint32 framesReadThisIteration;
			ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
			ma_uint32 framesToReadThisIteration = tempCapInFrames;
			if (framesToReadThisIteration > totalFramesRemaining)
				{
				framesToReadThisIteration = totalFramesRemaining;
				}

			framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp_s32, framesToReadThisIteration);
			if (framesReadThisIteration == 0)
				{
				break;
				}
			
			// apply volume delta, must be a value from 0.0 to 1.0
			size_t i = 0;
			for (i = 0; i < framesReadThisIteration; ++i)
				{
				size_t c = 0;
				for (c = 0; c < channels; ++c)
					{
					temp_s32[i*channels + c] = (ma_int32)(temp_s32[i*channels + c] * volume);
					}
				}
			
			// Mix the frames together
			for (iSample = 0; iSample < framesReadThisIteration*channels; ++iSample)
				{
				pOutputS32[totalFramesRead*channels + iSample] += temp_s32[iSample];
				}

			totalFramesRead += framesReadThisIteration;

			if (framesReadThisIteration < framesToReadThisIteration)
				{
				break;  /* Reached EOF. */
				}
			}
		}
		else if (format == D_FORMAT_U8)
		{
		
		}
    
    return totalFramesRead;
	}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
	//run through all tracks playing on this device, and mix them (DeviceInstance is passed in via pDevice->pUserData)
	DeviceInstance *instance = (DeviceInstance*) pDevice -> pUserData;
	
	if (instance -> tracks == NULL) return;
	
	startCriticalSection(&instance -> dlock);
	if (instance -> tracks != NULL)
		{
		TrackInstance *iw = instance -> tracks;
		
		while (iw != NULL)
			{
			TrackInstance *ti = iw;
			iw = iw -> next;
			
			if (ti -> newSeekPos)
				{
				ma_decoder_seek_to_pcm_frame(&ti -> decoder, ti -> cursor);
				ti -> newSeekPos = false;
				}
			
			ma_uint32 framesRead = read_and_mix_pcm(&ti -> decoder, 1, ti -> source -> decoderConfig.channels, ti -> volume, pOutput, frameCount);
			
			ti -> cursor += framesRead;
			
			if (framesRead < frameCount)
				{
				//end of this stream...(check if we're looping it; if not, remove it and if it's the last one stop the device)
				if (ti -> loop)
					{
					ti -> cursor = 0;
					ma_decoder_seek_to_pcm_frame(&ti -> decoder, ti -> cursor);
					//TODO: consume the shortfall in frames here?
					}
					else
					{
					if (ti -> prev != NULL)
						ti -> prev -> next = ti -> next;
						else
						instance -> tracks = ti -> next;
					
					if (ti -> next != NULL)
						ti -> next -> prev = ti -> prev;
					
					//move to cleanup list, where we decref the track object
					ti -> next = instance -> cleanupTracks;
					if (instance -> cleanupTracks != NULL) instance -> cleanupTracks -> prev = ti;
					instance -> cleanupTracks = ti;
					ti -> inCleanup = true;
					ti -> playing = false;
					
					semaphore_post(&instance -> eventSem); //TODO: semaphores are pretty slow for a real-time context, find something else
					}
				}
			}
		
		//if our playlist is empty, stop the device to avoid wasting cycles
		if (instance -> tracks == NULL)
			{
			instance -> eventID = E_DEVICE_STOP;
			semaphore_post(&instance -> eventSem); //TODO: semaphores are pretty slow for a real-time context, find something else
			}
		}
	stopCriticalSection(&instance -> dlock);
	}

static bool createEventThread(DeviceInstance *instance)
	{
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            event_thread,  		     // thread function name
            instance,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	
	if ((err = pthread_create(&instance -> eventThread, NULL, event_thread, instance)) != 0)
		{
		//??
		}
		else
		{
		pthread_detach(instance -> eventThread);
		}
	
	#endif
	
	return true;
	}

INSTRUCTION_DEF op_device_init(VFrame *cframe)
	{
	size_t deviceID = 0;
	copyHostInteger((unsigned char*) &deviceID, getVariableContent(cframe, 0), sizeof(size_t));
	
	//...decode format details
	unsigned char format = getVariableContent(cframe, 1)[0];
	size_t sampleRate = 0;
	copyHostInteger((unsigned char*) &sampleRate, getVariableContent(cframe, 2), sizeof(size_t));
	size_t channels = 0;
	copyHostInteger((unsigned char*) &channels, getVariableContent(cframe, 3), sizeof(size_t));
	
	ma_format formatCode = 0;
	
	if (format != D_FORMAT_F32)
		{
		api -> throwException(cframe, "unsupported audio format");
        return RETURN_OK;
		}
	
	if (format == D_FORMAT_F32)
		formatCode = ma_format_f32;
		else if (format == D_FORMAT_S16)
		formatCode = ma_format_s16;
		else if (format == D_FORMAT_S24)
		formatCode = ma_format_s24;
		else if (format == D_FORMAT_S32)
		formatCode = ma_format_s32;
		else if (format == D_FORMAT_U8)
		formatCode = ma_format_u8;
	
	// try to initialise the device
	
    ma_device_config deviceConfig;
    DeviceInstance *instance = malloc(sizeof(DeviceInstance));
	memset(instance, '\0', sizeof(DeviceInstance));
	
    deviceConfig = ma_device_config_init(ma_device_type_playback);
	//playback.pDeviceID = XXX; //pointer to an ma_device_id structure (or NULL for default device)
    deviceConfig.playback.format   = formatCode;
    deviceConfig.playback.channels = channels;
    deviceConfig.sampleRate        = sampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = instance;
	
    if (ma_device_init(NULL, &deviceConfig, &instance -> device) != MA_SUCCESS)
		{
		api -> throwException(cframe, "Failed to open playback device");
        return RETURN_OK;
		}
	
	#ifdef WINDOWS
	InitializeCriticalSection(&instance -> dlock);
	#endif
	#ifdef LINUX
	pthread_mutex_init(&instance -> dlock, &mAttr);
	#endif
	
	semaphore_init(&instance -> eventSem, 0);
	
	createEventThread(instance);
	
	instance -> audioObject = cframe -> io;
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &instance, sizeof(size_t));
	
	return RETURN_OK;
	}

static bool playTrack(VFrame *frame, DeviceInstance *device, TrackInstance *track, bool loop)
	{
	bool OK = true;
	
	//add to the decoder's list and it'll start playing
	
	startCriticalSection(&device -> dlock);
	
	track -> loop = loop;
	
	//check if the trackInstance is already playing, and do nothing if so...
	if (track -> playing)
		{
		//api -> throwException(cframe, "audio track is already playing");
		stopCriticalSection(&device -> dlock);
		
		return RETURN_OK;
		}
		else if (track -> inCleanup)
		{
		track -> inCleanup = false;
		
		if (track -> prev != NULL)
			{
			track -> prev -> next = track -> next;
			}
			else
			{
			device -> cleanupTracks = track -> next;
			}
		
		if (track -> next != NULL)
			{
			track -> next -> prev = track -> prev;
			}
		
		track -> next = NULL;
		track -> prev = NULL;
		}
	
	bool start = device -> tracks == NULL;
	
	track -> next = device -> tracks;
	if (device -> tracks != NULL) device -> tracks -> prev = track;
	device -> tracks = track;
	
	//if the decoder list was empty, the device would be stopped, so start it...
	if (start)
		{
		if (ma_device_start(&device -> device) != MA_SUCCESS)
			{
			api -> throwException(frame, "Failed to start playback device");
			// ?? any more information we can get here ??
			OK = false;
			}
		}
	
	stopCriticalSection(&device -> dlock);
	
	if (OK)
		{
		track -> playing = true;
		track -> playingOn = device;
		api -> incRef(frame, track -> objectRef);
		}
	
	return OK;
	}

INSTRUCTION_DEF op_device_play(VFrame *cframe)
	{
	//add this source to the given device and play it (if it's not already playing!)
	DeviceInstance *deviceInstance;
	memcpy(&deviceInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	//param 1 is the object reference, which we need to refCount ++
	LiveObject *obj = (LiveObject*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 2), sizeof(size_t));
	
	trackInstance -> objectRef = obj;
	trackInstance -> orType = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> typeLink;
	
	playTrack(cframe, deviceInstance, trackInstance, false);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_device_loop(VFrame *cframe)
	{
	//(as above, but add a loop flag)
	DeviceInstance *deviceInstance;
	memcpy(&deviceInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	//param 1 is the object reference, which we need to refCount ++
	LiveObject *obj = (LiveObject*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 2), sizeof(size_t));
	
	trackInstance -> objectRef = obj;
	
	playTrack(cframe, deviceInstance, trackInstance, true);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_device_stop(VFrame *cframe)
	{
	DeviceInstance *deviceInstance;
	memcpy(&deviceInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 1), sizeof(size_t));
	
	startCriticalSection(&deviceInstance -> dlock);
	
	if (trackInstance -> playing)
		{
		//cleanup & decref, but don't send an event
		if (trackInstance -> prev != NULL)
			trackInstance -> prev -> next = trackInstance -> next;
			else
			deviceInstance -> tracks = trackInstance -> next;
		
		if (trackInstance -> next != NULL)
			trackInstance -> next -> prev = trackInstance -> prev;
		
		trackInstance -> playing = false;
		
		if (deviceInstance -> tracks == NULL)
			{
			ma_device_stop(&deviceInstance -> device);
			}
		}
	
	stopCriticalSection(&deviceInstance -> dlock);
	
	if (trackInstance -> objectRef != NULL)
		{
		api -> decRef(NULL, trackInstance -> objectRef);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_device_stop_all(VFrame *cframe)
	{
	DeviceInstance *deviceInstance;
	memcpy(&deviceInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	TrackInstance *tracks = NULL;
	
	startCriticalSection(&deviceInstance -> dlock);
	
	tracks = deviceInstance -> tracks;
	
	deviceInstance -> tracks = NULL;
	
	ma_device_stop(&deviceInstance -> device);
	
	TrackInstance *tw = tracks;
	while (tw != NULL)
		{
		TrackInstance *td = tw;
		tw = tw -> next;
		
		td -> playing = false;
		
		if (td -> objectRef != NULL)
			{
			api -> decRef(NULL, td -> objectRef);
			}
		
		td -> next = NULL;
		td -> prev = NULL;
		}
	
	stopCriticalSection(&deviceInstance -> dlock);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_device_destroy(VFrame *cframe)
	{
	DeviceInstance *deviceInstance;
	memcpy(&deviceInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	//run stopAll first
	op_device_stop_all(cframe);
	
	//destroy the monitor thread (use a kill event)
	deviceInstance -> eventID = E_DEVICE_DESTROY;
	semaphore_post(&deviceInstance -> eventSem);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_decoder_load(VFrame *cframe)
	{
	//create a decoder instance, and prep it for playing the given in-memory audio data
	
	//variable 0 is the decode ID/type...
	
	unsigned char dtype = getVariableContent(cframe, 0)[0];
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	//...decode format details
	unsigned char format = getVariableContent(cframe, 2)[0];
	size_t sampleRate = 0;
	copyHostInteger((unsigned char*) &sampleRate, getVariableContent(cframe, 3), sizeof(size_t));
	size_t channels = 0;
	copyHostInteger((unsigned char*) &channels, getVariableContent(cframe, 4), sizeof(size_t));
	
	ma_format formatCode = 0;
	
	if (format != D_FORMAT_F32)
		{
		api -> throwException(cframe, "unsupported audio format");
        return RETURN_OK;
		}
	
	if (format == D_FORMAT_F32)
		formatCode = ma_format_f32;
		else if (format == D_FORMAT_S16)
		formatCode = ma_format_s16;
		else if (format == D_FORMAT_S24)
		formatCode = ma_format_s24;
		else if (format == D_FORMAT_S32)
		formatCode = ma_format_s32;
		else if (format == D_FORMAT_U8)
		formatCode = ma_format_u8;
	
	//try to initialise the decoder
	
	ma_result result;
	
    DecoderInstance *instance = malloc(sizeof(DecoderInstance));
	memset(instance, '\0', sizeof(DecoderInstance));
	
	instance -> decoderConfig = ma_decoder_config_init(formatCode, channels, sampleRate);
	
	if (dtype == 0)
		result = ma_decoder_init_memory_raw(array -> data, array -> length, &instance -> decoderConfig, &instance -> decoderConfig, &instance -> decoder);
		else if (dtype == 1)
		result = ma_decoder_init_memory_wav(array -> data, array -> length, &instance -> decoderConfig, &instance -> decoder);
		else if (dtype == 2)
		result = ma_decoder_init_memory_mp3(array -> data, array -> length, &instance -> decoderConfig, &instance -> decoder);
		else if (dtype == 3)
		result = ma_decoder_init_memory_flac(array -> data, array -> length, &instance -> decoderConfig, &instance -> decoder);
		else if (dtype == 4)
		result = ma_decoder_init_memory_vorbis(array -> data, array -> length, &instance -> decoderConfig, &instance -> decoder);
		else
		{
		api -> throwException(cframe, "unknown decoder type");
		free(instance);
        return RETURN_OK;
		}
	
	if (result != MA_SUCCESS)
		{
		api -> throwException(cframe, "Failed to decode media source data");
		free(instance);
        return RETURN_OK;
		}
	
	instance -> inputData = array -> data;
	instance -> inputLength = array -> length;
	instance -> type = dtype;
	
	size_t *ret = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(ret, &instance, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_decoder_get_length_frames(VFrame *cframe)
	{
	DecoderInstance *instance;
	memcpy(&instance, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t totalLength = 0;
	
	if (instance -> type == 0)
		{
		//raw
		if (instance -> decoderConfig.format == ma_format_f32)
			{
			//F32
			totalLength = instance -> inputLength / (sizeof(float) * instance -> decoderConfig.channels);
			}
		}
		else if (instance -> type != 4)
		{
		ma_uint64 length = ma_decoder_get_length_in_pcm_frames(&instance -> decoder);
	
		totalLength = length;
		}
		else
		{
		//OGG doesn't work this way, using this particular decoder implementation, so we need another solution...
		ma_result result;
		
		ma_uint64 frameCount;
		void* pAudioData;
		result = ma_decode_memory(instance -> inputData, instance -> inputLength, &instance -> decoderConfig, &frameCount, &pAudioData);
		if (result != MA_SUCCESS) {
			api -> throwException(cframe, "decode failed");
			return RETURN_OK;
		}
		
		totalLength = frameCount;
		
		ma_free(pAudioData, NULL);
		}
	
	return_int(cframe, totalLength);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_decoder_get_raw_data(VFrame *cframe)
	{
	DecoderInstance *decoderInstance;
	memcpy(&decoderInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	ma_result result;
	
	ma_uint64 frameCount;
	void* pAudioData;
	result = ma_decode_memory(decoderInstance -> inputData, decoderInstance -> inputLength, &decoderInstance -> decoderConfig, &frameCount, &pAudioData);
	if (result != MA_SUCCESS) {
		api -> throwException(cframe, "decode failed");
        return RETURN_OK;
	}
	
	//calculate how many bytes long pAudioData is, based on frameCount + format, then allocate our own array and memcpy it in
	
	size_t formatLength = 0;
	if (decoderInstance -> decoderConfig.format == ma_format_f32)
		formatLength = 4;
		else if (decoderInstance -> decoderConfig.format == ma_format_s16)
		formatLength = 2;
		else if (decoderInstance -> decoderConfig.format == ma_format_s24)
		formatLength = 3;
		else if (decoderInstance -> decoderConfig.format == ma_format_s32)
		formatLength = 4;
		else if (decoderInstance -> decoderConfig.format == ma_format_u8)
		formatLength = 1;
	
	size_t formatChannels = decoderInstance -> decoderConfig.channels;
	size_t multi = formatLength * formatChannels;
	
	size_t totalLen = multi * frameCount;
	
	unsigned char *raw_data = malloc(totalLen);
	memcpy(raw_data, pAudioData, totalLen);
	
	ma_free(pAudioData, NULL);
	
	return_byte_array_direct(cframe, api, raw_data, totalLen);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_decoder_destroy(VFrame *cframe)
	{
	DecoderInstance *decoderInstance;
	memcpy(&decoderInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	ma_decoder_uninit(&decoderInstance -> decoder);
	
	free(decoderInstance);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_load(VFrame *cframe)
	{
	DecoderInstance *decoderInstance;
	memcpy(&decoderInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
    TrackInstance *instance = malloc(sizeof(TrackInstance));
	memset(instance, '\0', sizeof(TrackInstance));
	
	instance -> source = decoderInstance;
	
	instance -> volume = 1.0; //full volume by default
	
	//clone a new decoder, from our source's settings
	
	ma_result result;
	
	if (instance -> source -> type == 0)
		result = ma_decoder_init_memory_raw(instance -> source -> inputData, instance -> source -> inputLength, &instance -> source -> decoderConfig, &instance -> source -> decoderConfig, &instance -> decoder);
		else if (instance -> source -> type == 1)
		result = ma_decoder_init_memory_wav(instance -> source -> inputData, instance -> source -> inputLength, &instance -> source -> decoderConfig, &instance -> decoder);
		else if (instance -> source -> type == 2)
		result = ma_decoder_init_memory_mp3(instance -> source -> inputData, instance -> source -> inputLength, &instance -> source -> decoderConfig, &instance -> decoder);
		else if (instance -> source -> type == 3)
		result = ma_decoder_init_memory_flac(instance -> source -> inputData, instance -> source -> inputLength, &instance -> source -> decoderConfig, &instance -> decoder);
		else if (instance -> source -> type == 4)
		result = ma_decoder_init_memory_vorbis(instance -> source -> inputData, instance -> source -> inputLength, &instance -> source -> decoderConfig, &instance -> decoder);
	
	size_t *ret = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(ret, &instance, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_set_volume(VFrame *cframe)
	{
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 1), sizeof(size_t));
	
	float newVol = ((float) xs) / 100.0f;
	
	trackInstance -> volume = newVol;
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_seek(VFrame *cframe)
	{
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	//here we just set a new seekpos, which is detected by the decode callback
	
	size_t frameIndex = 0;
	copyHostInteger((unsigned char*) &frameIndex, getVariableContent(cframe, 1), sizeof(size_t));
	
	trackInstance -> cursor = frameIndex;
	trackInstance -> newSeekPos = true;
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_get_pos(VFrame *cframe)
	{
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t pos = trackInstance -> cursor;
	
	return_int(cframe, pos);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_set_finish_event(VFrame *cframe)
	{
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	bool on = getVariableContent(cframe, 1)[0];
	
	trackInstance -> sendStopEvent = on;
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_track_destroy(VFrame *cframe)
	{
	TrackInstance *trackInstance;
	memcpy(&trackInstance, getVariableContent(cframe, 0), sizeof(size_t));
	
	ma_decoder_uninit(&trackInstance -> decoder);
	
	free(trackInstance);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	#ifdef LINUX
	pthread_mutexattr_init(&mAttr);
	
	// setup recursive mutex for mutex attribute
	#ifdef OSX
	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE);
	#else
	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	#endif
	#endif
	
	trackInfoGT = api -> resolveGlobalTypeMapping(getTypeDefinition("TrackInfo"));
	
	setInterfaceFunction("deviceInit", op_device_init);
	setInterfaceFunction("devicePlay", op_device_play);
	setInterfaceFunction("deviceLoop", op_device_loop);
	setInterfaceFunction("deviceStop", op_device_stop);
	setInterfaceFunction("deviceStopAll", op_device_stop_all);
	setInterfaceFunction("deviceDestroy", op_device_destroy);
	
	setInterfaceFunction("decoderLoad", op_decoder_load);
	setInterfaceFunction("decoderGetLengthFrames", op_decoder_get_length_frames);
	setInterfaceFunction("decoderGetRawData", op_decoder_get_raw_data);
	setInterfaceFunction("decoderDestroy", op_decoder_destroy);
	
	setInterfaceFunction("trackLoad", op_track_load);
	setInterfaceFunction("trackSetVolume", op_track_set_volume);
	setInterfaceFunction("trackSeek", op_track_seek);
	setInterfaceFunction("trackSetFinishEvent", op_track_set_finish_event);
	setInterfaceFunction("trackGetPos", op_track_get_pos);
	setInterfaceFunction("trackDestroy", op_track_destroy);
	
	return getPublicInterface();
	}

void unload()
	{
	}
