#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include <conio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <numbers>
#define TWOPI (2 * std::numbers::pi)

#define PRINT_ERROR(a, args...) printf("ERROR %s() %s Line %d: " a, __FUNCTION__, __FILE__, __LINE__, ##args);

HWAVEOUT wave_out;
#define SAMPLING_RATE 48000
#define CHUNK_SIZE 2000
WAVEHDR header[2] = {0};
int16_t chunks[2][CHUNK_SIZE];
bool chunk_swap = false;
float frequency = 440;
float wave_position = 0;
float wave_step;

void CALLBACK WaveOutProc(HWAVEOUT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	{
		WAVEFORMATEX format = {
			.wFormatTag = WAVE_FORMAT_PCM,
			.nChannels = 1,
			.nSamplesPerSec = SAMPLING_RATE,
			.wBitsPerSample = 16,
			.cbSize = 0,
		};
		format.nBlockAlign = format.nChannels * format.wBitsPerSample / 8;
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;
		if(waveOutOpen(&wave_out, WAVE_MAPPER, &format, (DWORD_PTR)WaveOutProc, (DWORD_PTR)NULL, CALLBACK_FUNCTION) != MMSYSERR_NOERROR) {
			PRINT_ERROR("waveOutOpen failed\n");
			return -1;
		}
	}

	if(waveOutSetVolume(wave_out, 0xFFFFFFFF) != MMSYSERR_NOERROR) {
		PRINT_ERROR("waveOutGetVolume failed\n");
		return -1;
	}

	wave_step = TWOPI / ((float)SAMPLING_RATE / frequency);

	for(int i = 0; i < 2; ++i) {
		for(int j = 0; j < CHUNK_SIZE; ++j) {
			chunks[i][j] = sin(wave_position) * 32767;
			wave_position += wave_step;
		}
		header[i].lpData = (CHAR*)chunks[i];
		header[i].dwBufferLength = CHUNK_SIZE * 2;
		if(waveOutPrepareHeader(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
			PRINT_ERROR("waveOutPrepareHeader[%d] failed\n", i);
			return -1;
		}
		if(waveOutWrite(wave_out, &header[i], sizeof(header[i])) != MMSYSERR_NOERROR) {
			PRINT_ERROR("waveOutWrite[%d] failed\n", i);
			return -1;
		}
	}

	static bool quit = false;
	while(!quit) {
		switch(_getche()) {
			case 72: {
				frequency += 50;
				wave_step = TWOPI / ((float)SAMPLING_RATE / frequency);
				printf("Frequency: %f\n", frequency);
			} break;
			case 80: {
				frequency -= 50;
				wave_step = TWOPI / ((float)SAMPLING_RATE / frequency);
				printf("Frequency: %f\n", frequency);
			} break;
			case 27: {
				quit = true;
			} break;
		}
	}

	return 0;
}

void CALLBACK WaveOutProc(HWAVEOUT wave_out_handle, UINT message, DWORD_PTR instance, DWORD_PTR param1, DWORD_PTR param2) {
	switch(message) {
		case WOM_CLOSE: printf("WOM_CLOSE\n"); break;
		case WOM_OPEN:  printf("WOM_OPEN\n");  break;
		case WOM_DONE:{ printf("WOM_DONE\n");
			for(int i = 0; i < CHUNK_SIZE; ++i) {
				chunks[chunk_swap][i] = sin(wave_position) * 32767;
				wave_position += wave_step;
			}
			if(waveOutWrite(wave_out, &header[chunk_swap], sizeof(header[chunk_swap])) != MMSYSERR_NOERROR) {
				PRINT_ERROR("waveOutWrite failed\n");
			}
			chunk_swap = !chunk_swap;
		} break;
	}
}
