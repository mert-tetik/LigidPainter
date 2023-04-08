/* 
!SINGLE HEADER OPENAL LIBRARY & AUDIO FILE IMPORTER
!MIT License
!OpenAL binaries are needed
?You can compile the source code by cloning their github repo
?OpenAL Version : [2023-02-06] makemhr-0de7ea4
?https://github.com/kcat/openal-soft
!Define LibALmaxAudios as the max audio size that will be generated (Default value is 2000)
!Define LIBAL_OPENAL_IMPLEMENTATION if you want to use OpenAL (you don't have to unless you just need file importer)
!This header contains OpenAL include files

!Examples :
    !--OpenGL Examples:
        **This example plays 2 audios at the same time as the app starts
        ?--------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 2 
            //2 Since I used only 2 audio objects here. Defining that as 1000 or higher will be much better
            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL " << LibALerrorMsg << std::endl;
	            
                unsigned int sound1;//These are objects
	            unsigned int sound2;

	            LibAL_genAudio(sound1);//Before using them you have to declare that they are audio objects
	            LibAL_genAudio(sound2);
                !Don't try changing the values of the objects

	            LibAL_modifyAudioViaPath("./sound1.wav","wav",sound1);
	            LibAL_modifyAudioViaPath("./sound2.wav","wav",sound2);

	            LibAL_playAudioObject(sound2);
	            LibAL_playAudioObject(sound1);

                while (!glfwWindowShouldClose(window)){
                    
                    Render triangles...
                
                }    

                LibAL_end();

                return 0;
            }
        ?--------------------------

        **This example shows the use of multiple conditional sounds
        ?---------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 2 
            //2 Since I used only 2 audio objects here. Defining that as 1000 or higher will be much better
            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL\n";
	            
                unsigned int sound1;//These are objects
	            unsigned int sound2;

	            LibAL_genAudio(sound1);//Before using them you have to declare that they are audio objects
	            LibAL_genAudio(sound2);
                !Don't try changing the values of the objects

	            LibAL_modifyAudioViaPath("./sound1.wav","wav",sound1);
	            LibAL_modifyAudioViaPath("./sound2.wav","wav",sound2);

	            LibAL_playAudioObject(sound2);
	            LibAL_playAudioObject(sound1);

                while (!glfwWindowShouldClose(window)){
                    
                    if(playerDidSmtGood){
                        if(LibAL_isPlaying(sound2))
                            LibAL_stopPlaying(sound2);//You don't have to stop the previous sound since you can play multiple sounds but personally I woldn't want these 2 sound to collide
		                LibAL_stopPlaying(sound1); //Prevent consecutive calls
                        LibAL_playAudioObject(sound1);
                    }
                
                    if(playerDidSmtBad){
                        if(LibAL_isPlaying(sound1))
                            LibAL_stopPlaying(sound1);//You don't have to stop the previous sound since you can play multiple sounds but personally I woldn't want these 2 sound to collide
		                LibAL_stopPlaying(sound2);  //Prevent consecutive calls
                        LibAL_playAudioObject(sound2);
                    }

                    if(silenceNeeded)
                        LibAL_silence(); //You might want to silence all the objects manually

                    Draw triangles...
                
                }    

                LibAL_end();
                
                return 0;
            }
        
        ?----------------------------------------
        
        **This example shows the use of audio object arrays
        ?----------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 100 
            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                std::vector<std::string> characterVoiceFilePaths = {
                    "./taunt1.wav",
                    "./taunt2.wav",
                    "./taunt3.wav",
                    "./scream1.wav",
                    "./scream2.wav",
                    "./scream3.wav",
                    "./laugh1.wav",
                    "./laugh2.wav",
                    "./frightened1.wav",
                    "./frightened2.wav",
                    "./voiceLine1.wav",
                    "./voiceLine2.wav",
                    "./voiceLine3.wav",
                    "./voiceLine4.wav",
                    "./voiceLine5.wav",
                }
                std::vector<unsigned int> characterVoiceFileObjects;

                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL\n";


                for (size_t i = 0; i < characterVoiceFilePaths.size(); i++)
                {
                    unsigned int voice;
	                LibAL_genAudio(voice);
    	            LibAL_modifyAudioViaPath(characterVoiceFilePaths[i],"wav",voice);
                    characterVoiceFileObjects.push_back(voice);

                }            
    
                //*Render all the voice lines one by one

                int index = 0;
                LibAL_playAudioObject(characterVoiceFileObjects[0]);

                while (!glfwWindowShouldClose(window)){
                    
                    if(!LibAL_isPlaying(characterVoiceFileObjects[index])){
                        index++;
                        LibAL_playAudioObject(characterVoiceFileObjects[index]);
                    }

                    Render triangles...
                }    

                LibAL_end();

                return 0;
            }
        ?----------------------------------------

        **This example shows how you can modify objects without LibAL's file importer 
        ?----------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 
            //1 Since I used only 1 audio objects here. Defining that as 1000 or higher will be much better
            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL" << std::endl;
	            
                unsigned int sound;
                LibAL_genAudio(sound);

	            LibAL_genAudio(sound);
                std::string path = "./voice.wav";
                char* bufferData;
                std::uint8_t channels;
                std::int32_t& sampleRate;
                std::uint8_t bitsPerSample;
                ALsizei size;
                
	            if(!LibAL_readWAVFile(path,bufferData,channels,sampleRate,bitsPerSample,size)){ //You can use your own importer
                    std::cout << "ERROR : Reading wav file";
                }

                LibAL_modifyAudioViaData(channels,bitsPerSample,bufferData,size,sampleRate,sound);

	            LibAL_playAudioObject(sound);

                while (!glfwWindowShouldClose(window)){
                    
                    Render triangles...
                }    

                LibAL_end();

                return 0;
            }
        
        ?----------------------------------------

        **This example shows how you can manipulate audios via LibAL 
        ?----------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 1 

            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL\n";
	            
                unsigned int sound;

	            LibAL_genAudio(sound);

	            LibAL_modifyAudioViaPath("./sound.wav","wav",sound);

                LibAL_invertSoundData(sound); //Invert the sound | dnuos eht trevnI

                //*LibAL_modifySourceViaData(float pitch,float gain,float position[3],float velocity[3],unsigned int ID)
                LibAL_modifySourceViaData(pitchval,gainval,{0,0,0},{0,0,0},sound);
                //*Default pitch & gain is 1
                ///? or
                LibAL_modifyBufferViaData({1,0,0},{100,100,100});//Audio will be come from left side of the headset in slow mo

	            LibAL_playAudioObject(sound);

                while (!glfwWindowShouldClose(window)){
                    Render triangles...
                }    

                LibAL_end();

                return 0;
            }
        ?----------------------------------------
        
        **This example shows how you can pause and start playing a sound 
        ?----------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 1 

            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL\n";
	            
                unsigned int sound;

	            LibAL_genAudio(sound);

	            LibAL_modifyAudioViaPath("./sound.wav","wav",sound);

	            LibAL_playAudioObject(sound);

                while (!glfwWindowShouldClose(window)){
                    if(KEY_A == CLICKED) //Pause & play the sound object once the key a is clicked 
                    {
                        if(!LibAL_isPaused(sound))
                            LibAL_pausePlaying(sound);
                        else{
                            LibAL_startPlaying(sound);
                        }
                    }  
                }    

                LibAL_end();

                return 0;
            }
        
        ?----------------------------------------
        
        **This example is for those who only wants to use the importer and not the OpenAL 😡😡
        ?----------------------------------------
            //#define LIBAL_OPENAL_IMPLEMENTATION //Just don't define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 1 

            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...

                std::string path = "./voice.wav";
                char* bufferData;
                std::uint8_t channels;
                std::int32_t& sampleRate;
                std::uint8_t bitsPerSample;
                size_t size;
                
	            if(!LibAL_readWAVFile(path,bufferData,channels,sampleRate,bitsPerSample,size)){ //You can use your own importer
                    std::cout << "ERROR : Reading wav file";
                }

                aSoundRendererThatIsOtherThanOpenAL.playSoundblabla(path,bufferData,channels,sampleRate,bitsPerSample,size); //😡

                while (!glfwWindowShouldClose(window)){
                    
                }    

                LibAL_end();

                return 0;
            }
        
        ?----------------------------------------
    !--Console Example: 

        **This example plays the audio than returns from main function
    
        ?----------------------------------------
            #define LIBAL_OPENAL_IMPLEMENTATION
            #define LibALmaxAudios 1 

            #include "./LibAL.h"
            int main(){
                
                initialize opengl or smt...
                
                
                if(!LibAL_start())
		            std::cout << "ERROR : Initializing LibAL\n";
	            
                unsigned int sound;

	            LibAL_genAudio(sound);

	            LibAL_modifyAudioViaPath("./sound.wav","wav",sound);

	            LibAL_playAudioObjectLoop(sound);

                LibAL_end();

                return 0;
            }
        ?----------------------------------------
        
    TODO Reading mp3 file
    TODO Align parameters
*/


    std::int32_t LibALConvertToInt(char* buffer, std::size_t len);
    int LibALplayAudioDataLoop(char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,size_t& size,unsigned int ID);
    int LibALplayAudioData(char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,size_t& size,unsigned int ID);
    int LibAL_start();
    void LibAL_end();
    int LibAL_playAudioObjectLoop(unsigned int ID);
    int LibAL_playAudioObject(unsigned int ID);
    void LibAL_stopPlaying(unsigned int ID);
    void LibAL_pausePlaying(unsigned int ID);
    void LibAL_startPlaying(unsigned int ID);
    void LibAL_silence();
    void LibAL_invertSoundData(unsigned int ID);
    bool LibAL_isPlaying(unsigned int ID);
    bool LibAL_isPaused(unsigned int ID);
    bool LibAL_isStopped(unsigned int ID);
    int LibAL_readWAVFile(const std::string& path,char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,size_t& size);
    int LibAL_genAudio(unsigned int & ID);
    int LibAL_modifyAudioViaData(std::uint8_t channels,std::uint8_t bitsPerSample, char* bufferData,size_t dataSize,int32_t sampleRate,unsigned int ID);
    int LibAL_modifySourceViaData(float pitch,float gain,float position[3],float velocity[3],unsigned int ID);
    int LibAL_modifyBufferViaData(float position[3],float velocity[3]);
    int LibAL_modifyAudioViaPath(std::string path,std::string format,unsigned int ID);
    int LibAL_deleteAudio(unsigned int &ID);
    void printOpenALErrors();
       

#ifdef LIBAL_OPENAL_IMPLEMENTATION


#ifndef LIBAL
#define LIBAL





//-------------------------------------------------------------------------------------
//---------------------------------------- al.h ----------------------------------------
//-------------------------------------------------------------------------------------











#include <stdio.h>

#ifdef AL_AL_H
#error LibAL already provides OpenAL include files. Remove the al.h include.
#endif //AL_AL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AL_API
 #if defined(AL_LIBTYPE_STATIC)
  #define AL_API
 #elif defined(_WIN32)
  #define AL_API __declspec(dllimport)
 #else
  #define AL_API extern
 #endif
#endif

#ifdef _WIN32
 #define AL_APIENTRY __cdecl
#else
 #define AL_APIENTRY
#endif


/* Deprecated macros. */
#define OPENAL
#define ALAPI                                    AL_API
#define ALAPIENTRY                               AL_APIENTRY
#define AL_INVALID                               (-1)
#define AL_ILLEGAL_ENUM                          AL_INVALID_ENUM
#define AL_ILLEGAL_COMMAND                       AL_INVALID_OPERATION

/* Supported AL versions. */
#define AL_VERSION_1_0
#define AL_VERSION_1_1

/** 8-bit boolean */
typedef char ALboolean;

/** character */
typedef char ALchar;

/** signed 8-bit integer */
typedef signed char ALbyte;

/** unsigned 8-bit integer */
typedef unsigned char ALubyte;

/** signed 16-bit integer */
typedef short ALshort;

/** unsigned 16-bit integer */
typedef unsigned short ALushort;

/** signed 32-bit integer */
typedef int ALint;

/** unsigned 32-bit integer */
typedef unsigned int ALuint;

/** non-negative 32-bit integer size */
typedef int ALsizei;

/** 32-bit enumeration value */
typedef int ALenum;

/** 32-bit IEEE-754 floating-point */
typedef float ALfloat;

/** 64-bit IEEE-754 floating-point */
typedef double ALdouble;

/** void type (opaque pointers only) */
typedef void ALvoid;


/* Enumeration values begin at column 50. Do not use tabs. */

/** No distance model or no buffer */
#define AL_NONE                                  0

/** Boolean False. */
#define AL_FALSE                                 0

/** Boolean True. */
#define AL_TRUE                                  1


/**
 * Relative source.
 * Type:    ALboolean
 * Range:   [AL_FALSE, AL_TRUE]
 * Default: AL_FALSE
 *
 * Specifies if the source uses relative coordinates.
 */
#define AL_SOURCE_RELATIVE                       0x202


/**
 * Inner cone angle, in degrees.
 * Type:    ALint, ALfloat
 * Range:   [0 - 360]
 * Default: 360
 *
 * The angle covered by the inner cone, the area within which the source will
 * not be attenuated by direction.
 */
#define AL_CONE_INNER_ANGLE                      0x1001

/**
 * Outer cone angle, in degrees.
 * Range:   [0 - 360]
 * Default: 360
 *
 * The angle covered by the outer cone, the area outside of which the source
 * will be fully attenuated by direction.
 */
#define AL_CONE_OUTER_ANGLE                      0x1002

/**
 * Source pitch.
 * Type:    ALfloat
 * Range:   [0.5 - 2.0]
 * Default: 1.0
 *
 * A multiplier for the sample rate of the source's buffer.
 */
#define AL_PITCH                                 0x1003

/**
 * Source or listener position.
 * Type:    ALfloat[3], ALint[3]
 * Default: {0, 0, 0}
 *
 * The source or listener location in three dimensional space.
 *
 * OpenAL uses a right handed coordinate system, like OpenGL, where with a
 * default view, X points right (thumb), Y points up (index finger), and Z
 * points towards the viewer/camera (middle finger).
 *
 * To change from or to a left handed coordinate system, negate the Z
 * component.
 */
#define AL_POSITION                              0x1004

/**
 * Source direction.
 * Type:    ALfloat[3], ALint[3]
 * Default: {0, 0, 0}
 *
 * Specifies the current direction in local space. A zero-length vector
 * specifies an omni-directional source (cone is ignored).
 *
 * To change from or to a left handed coordinate system, negate the Z
 * component.
 */
#define AL_DIRECTION                             0x1005

/**
 * Source or listener velocity.
 * Type:    ALfloat[3], ALint[3]
 * Default: {0, 0, 0}
 *
 * Specifies the current velocity, relative to the position.
 *
 * To change from or to a left handed coordinate system, negate the Z
 * component.
 */
#define AL_VELOCITY                              0x1006

/**
 * Source looping.
 * Type:    ALboolean
 * Range:   [AL_FALSE, AL_TRUE]
 * Default: AL_FALSE
 *
 * Specifies whether source playback loops.
 */
#define AL_LOOPING                               0x1007

/**
 * Source buffer.
 * Type:    ALuint
 * Range:   any valid Buffer ID
 * Default: AL_NONE
 *
 * Specifies the buffer to provide sound samples for a source.
 */
#define AL_BUFFER                                0x1009

/**
 * Source or listener gain.
 * Type:  ALfloat
 * Range: [0.0 - ]
 *
 * For sources, an initial linear gain value (before attenuation is applied).
 * For the listener, an output linear gain adjustment.
 *
 * A value of 1.0 means unattenuated. Each division by 2 equals an attenuation
 * of about -6dB. Each multiplication by 2 equals an amplification of about
 * +6dB.
 */
#define AL_GAIN                                  0x100A

/**
 * Minimum source gain.
 * Type:  ALfloat
 * Range: [0.0 - 1.0]
 *
 * The minimum gain allowed for a source, after distance and cone attenuation
 * are applied (if applicable).
 */
#define AL_MIN_GAIN                              0x100D

/**
 * Maximum source gain.
 * Type:  ALfloat
 * Range: [0.0 - 1.0]
 *
 * The maximum gain allowed for a source, after distance and cone attenuation
 * are applied (if applicable).
 */
#define AL_MAX_GAIN                              0x100E

/**
 * Listener orientation.
 * Type:    ALfloat[6]
 * Default: {0.0, 0.0, -1.0, 0.0, 1.0, 0.0}
 *
 * Effectively two three dimensional vectors. The first vector is the front (or
 * "at") and the second is the top (or "up"). Both vectors are relative to the
 * listener position.
 *
 * To change from or to a left handed coordinate system, negate the Z
 * component of both vectors.
 */
#define AL_ORIENTATION                           0x100F

/**
 * Source state (query only).
 * Type:  ALenum
 * Range: [AL_INITIAL, AL_PLAYING, AL_PAUSED, AL_STOPPED]
 */
#define AL_SOURCE_STATE                          0x1010

/* Source state values. */
#define AL_INITIAL                               0x1011
#define AL_PLAYING                               0x1012
#define AL_PAUSED                                0x1013
#define AL_STOPPED                               0x1014

/**
 * Source Buffer Queue size (query only).
 * Type: ALint
 *
 * The number of buffers queued using alSourceQueueBuffers, minus the buffers
 * removed with alSourceUnqueueBuffers.
 */
#define AL_BUFFERS_QUEUED                        0x1015

/**
 * Source Buffer Queue processed count (query only).
 * Type: ALint
 *
 * The number of queued buffers that have been fully processed, and can be
 * removed with alSourceUnqueueBuffers.
 *
 * Looping sources will never fully process buffers because they will be set to
 * play again for when the source loops.
 */
#define AL_BUFFERS_PROCESSED                     0x1016

/**
 * Source reference distance.
 * Type:    ALfloat
 * Range:   [0.0 - ]
 * Default: 1.0
 *
 * The distance in units that no distance attenuation occurs.
 *
 * At 0.0, no distance attenuation occurs with non-linear attenuation models.
 */
#define AL_REFERENCE_DISTANCE                    0x1020

/**
 * Source rolloff factor.
 * Type:    ALfloat
 * Range:   [0.0 - ]
 * Default: 1.0
 *
 * Multiplier to exaggerate or diminish distance attenuation.
 *
 * At 0.0, no distance attenuation ever occurs.
 */
#define AL_ROLLOFF_FACTOR                        0x1021

/**
 * Outer cone gain.
 * Type:    ALfloat
 * Range:   [0.0 - 1.0]
 * Default: 0.0
 *
 * The gain attenuation applied when the listener is outside of the source's
 * outer cone angle.
 */
#define AL_CONE_OUTER_GAIN                       0x1022

/**
 * Source maximum distance.
 * Type:    ALfloat
 * Range:   [0.0 - ]
 * Default: FLT_MAX
 *
 * The distance above which the source is not attenuated any further with a
 * clamped distance model, or where attenuation reaches 0.0 gain for linear
 * distance models with a default rolloff factor.
 */
#define AL_MAX_DISTANCE                          0x1023

/** Source buffer offset, in seconds */
#define AL_SEC_OFFSET                            0x1024
/** Source buffer offset, in sample frames */
#define AL_SAMPLE_OFFSET                         0x1025
/** Source buffer offset, in bytes */
#define AL_BYTE_OFFSET                           0x1026

/**
 * Source type (query only).
 * Type:  ALenum
 * Range: [AL_STATIC, AL_STREAMING, AL_UNDETERMINED]
 *
 * A Source is Static if a Buffer has been attached using AL_BUFFER.
 *
 * A Source is Streaming if one or more Buffers have been attached using
 * alSourceQueueBuffers.
 *
 * A Source is Undetermined when it has the NULL buffer attached using
 * AL_BUFFER.
 */
#define AL_SOURCE_TYPE                           0x1027

/* Source type values. */
#define AL_STATIC                                0x1028
#define AL_STREAMING                             0x1029
#define AL_UNDETERMINED                          0x1030

/** Unsigned 8-bit mono buffer format. */
#define AL_FORMAT_MONO8                          0x1100
/** Signed 16-bit mono buffer format. */
#define AL_FORMAT_MONO16                         0x1101
/** Unsigned 8-bit stereo buffer format. */
#define AL_FORMAT_STEREO8                        0x1102
/** Signed 16-bit stereo buffer format. */
#define AL_FORMAT_STEREO16                       0x1103

/** Buffer frequency/sample rate (query only). */
#define AL_FREQUENCY                             0x2001
/** Buffer bits per sample (query only). */
#define AL_BITS                                  0x2002
/** Buffer channel count (query only). */
#define AL_CHANNELS                              0x2003
/** Buffer data size in bytes (query only). */
#define AL_SIZE                                  0x2004

/* Buffer state. Not for public use. */
#define AL_UNUSED                                0x2010
#define AL_PENDING                               0x2011
#define AL_PROCESSED                             0x2012


/** No error. */
#define AL_NO_ERROR                              0

/** Invalid name (ID) passed to an AL call. */
#define AL_INVALID_NAME                          0xA001

/** Invalid enumeration passed to AL call. */
#define AL_INVALID_ENUM                          0xA002

/** Invalid value passed to AL call. */
#define AL_INVALID_VALUE                         0xA003

/** Illegal AL call. */
#define AL_INVALID_OPERATION                     0xA004

/** Not enough memory to execute the AL call. */
#define AL_OUT_OF_MEMORY                         0xA005


/** Context string: Vendor name. */
#define AL_VENDOR                                0xB001
/** Context string: Version. */
#define AL_VERSION                               0xB002
/** Context string: Renderer name. */
#define AL_RENDERER                              0xB003
/** Context string: Space-separated extension list. */
#define AL_EXTENSIONS                            0xB004

/**
 * Doppler scale.
 * Type:    ALfloat
 * Range:   [0.0 - ]
 * Default: 1.0
 *
 * Scale for source and listener velocities.
 */
#define AL_DOPPLER_FACTOR                        0xC000

/**
 * Doppler velocity (deprecated).
 *
 * A multiplier applied to the Speed of Sound.
 */
#define AL_DOPPLER_VELOCITY                      0xC001

/**
 * Speed of Sound, in units per second.
 * Type:    ALfloat
 * Range:   [0.0001 - ]
 * Default: 343.3
 *
 * The speed at which sound waves are assumed to travel, when calculating the
 * doppler effect from source and listener velocities.
 */
#define AL_SPEED_OF_SOUND                        0xC003

/**
 * Distance attenuation model.
 * Type:    ALenum
 * Range:   [AL_NONE, AL_INVERSE_DISTANCE, AL_INVERSE_DISTANCE_CLAMPED,
 *           AL_LINEAR_DISTANCE, AL_LINEAR_DISTANCE_CLAMPED,
 *           AL_EXPONENT_DISTANCE, AL_EXPONENT_DISTANCE_CLAMPED]
 * Default: AL_INVERSE_DISTANCE_CLAMPED
 *
 * The model by which sources attenuate with distance.
 *
 * None     - No distance attenuation.
 * Inverse  - Doubling the distance halves the source gain.
 * Linear   - Linear gain scaling between the reference and max distances.
 * Exponent - Exponential gain dropoff.
 *
 * Clamped variations work like the non-clamped counterparts, except the
 * distance calculated is clamped between the reference and max distances.
 */
#define AL_DISTANCE_MODEL                        0xD000

/* Distance model values. */
#define AL_INVERSE_DISTANCE                      0xD001
#define AL_INVERSE_DISTANCE_CLAMPED              0xD002
#define AL_LINEAR_DISTANCE                       0xD003
#define AL_LINEAR_DISTANCE_CLAMPED               0xD004
#define AL_EXPONENT_DISTANCE                     0xD005
#define AL_EXPONENT_DISTANCE_CLAMPED             0xD006

#ifndef AL_NO_PROTOTYPES
/* Renderer State management. */
AL_API void AL_APIENTRY alEnable(ALenum capability);
AL_API void AL_APIENTRY alDisable(ALenum capability);
AL_API ALboolean AL_APIENTRY alIsEnabled(ALenum capability);

/* Context state setting. */
AL_API void AL_APIENTRY alDopplerFactor(ALfloat value);
AL_API void AL_APIENTRY alDopplerVelocity(ALfloat value);
AL_API void AL_APIENTRY alSpeedOfSound(ALfloat value);
AL_API void AL_APIENTRY alDistanceModel(ALenum distanceModel);

/* Context state retrieval. */
AL_API const ALchar* AL_APIENTRY alGetString(ALenum param);
AL_API void AL_APIENTRY alGetBooleanv(ALenum param, ALboolean *values);
AL_API void AL_APIENTRY alGetIntegerv(ALenum param, ALint *values);
AL_API void AL_APIENTRY alGetFloatv(ALenum param, ALfloat *values);
AL_API void AL_APIENTRY alGetDoublev(ALenum param, ALdouble *values);
AL_API ALboolean AL_APIENTRY alGetBoolean(ALenum param);
AL_API ALint AL_APIENTRY alGetInteger(ALenum param);
AL_API ALfloat AL_APIENTRY alGetFloat(ALenum param);
AL_API ALdouble AL_APIENTRY alGetDouble(ALenum param);

/**
 * Obtain the first error generated in the AL context since the last call to
 * this function.
 */
AL_API ALenum AL_APIENTRY alGetError(void);

/** Query for the presence of an extension on the AL context. */
AL_API ALboolean AL_APIENTRY alIsExtensionPresent(const ALchar *extname);
/**
 * Retrieve the address of a function. The returned function may be context-
 * specific.
 */
AL_API void* AL_APIENTRY alGetProcAddress(const ALchar *fname);
/**
 * Retrieve the value of an enum. The returned value may be context-specific.
 */
AL_API ALenum AL_APIENTRY alGetEnumValue(const ALchar *ename);


/* Set listener parameters. */
AL_API void AL_APIENTRY alListenerf(ALenum param, ALfloat value);
AL_API void AL_APIENTRY alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
AL_API void AL_APIENTRY alListenerfv(ALenum param, const ALfloat *values);
AL_API void AL_APIENTRY alListeneri(ALenum param, ALint value);
AL_API void AL_APIENTRY alListener3i(ALenum param, ALint value1, ALint value2, ALint value3);
AL_API void AL_APIENTRY alListeneriv(ALenum param, const ALint *values);

/* Get listener parameters. */
AL_API void AL_APIENTRY alGetListenerf(ALenum param, ALfloat *value);
AL_API void AL_APIENTRY alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
AL_API void AL_APIENTRY alGetListenerfv(ALenum param, ALfloat *values);
AL_API void AL_APIENTRY alGetListeneri(ALenum param, ALint *value);
AL_API void AL_APIENTRY alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3);
AL_API void AL_APIENTRY alGetListeneriv(ALenum param, ALint *values);


/** Create source objects. */
AL_API void AL_APIENTRY alGenSources(ALsizei n, ALuint *sources);
/** Delete source objects. */
AL_API void AL_APIENTRY alDeleteSources(ALsizei n, const ALuint *sources);
/** Verify an ID is for a valid source. */
AL_API ALboolean AL_APIENTRY alIsSource(ALuint source);

/* Set source parameters. */
AL_API void AL_APIENTRY alSourcef(ALuint source, ALenum param, ALfloat value);
AL_API void AL_APIENTRY alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
AL_API void AL_APIENTRY alSourcefv(ALuint source, ALenum param, const ALfloat *values);
AL_API void AL_APIENTRY alSourcei(ALuint source, ALenum param, ALint value);
AL_API void AL_APIENTRY alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3);
AL_API void AL_APIENTRY alSourceiv(ALuint source, ALenum param, const ALint *values);

/* Get source parameters. */
AL_API void AL_APIENTRY alGetSourcef(ALuint source, ALenum param, ALfloat *value);
AL_API void AL_APIENTRY alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
AL_API void AL_APIENTRY alGetSourcefv(ALuint source, ALenum param, ALfloat *values);
AL_API void AL_APIENTRY alGetSourcei(ALuint source,  ALenum param, ALint *value);
AL_API void AL_APIENTRY alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3);
AL_API void AL_APIENTRY alGetSourceiv(ALuint source,  ALenum param, ALint *values);


/** Play, restart, or resume a source, setting its state to AL_PLAYING. */
AL_API void AL_APIENTRY alSourcePlay(ALuint source);
/** Stop a source, setting its state to AL_STOPPED if playing or paused. */
AL_API void AL_APIENTRY alSourceStop(ALuint source);
/** Rewind a source, setting its state to AL_INITIAL. */
AL_API void AL_APIENTRY alSourceRewind(ALuint source);
/** Pause a source, setting its state to AL_PAUSED if playing. */
AL_API void AL_APIENTRY alSourcePause(ALuint source);

/** Play, restart, or resume a list of sources atomically. */
AL_API void AL_APIENTRY alSourcePlayv(ALsizei n, const ALuint *sources);
/** Stop a list of sources atomically. */
AL_API void AL_APIENTRY alSourceStopv(ALsizei n, const ALuint *sources);
/** Rewind a list of sources atomically. */
AL_API void AL_APIENTRY alSourceRewindv(ALsizei n, const ALuint *sources);
/** Pause a list of sources atomically. */
AL_API void AL_APIENTRY alSourcePausev(ALsizei n, const ALuint *sources);

/** Queue buffers onto a source */
AL_API void AL_APIENTRY alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers);
/** Unqueue processed buffers from a source */
AL_API void AL_APIENTRY alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers);


/** Create buffer objects */
AL_API void AL_APIENTRY alGenBuffers(ALsizei n, ALuint *buffers);
/** Delete buffer objects */
AL_API void AL_APIENTRY alDeleteBuffers(ALsizei n, const ALuint *buffers);
/** Verify an ID is a valid buffer (including the NULL buffer) */
AL_API ALboolean AL_APIENTRY alIsBuffer(ALuint buffer);

/**
 * Copies data into the buffer, interpreting it using the specified format and
 * samplerate.
 */
AL_API void AL_APIENTRY alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei samplerate);

/* Set buffer parameters. */
AL_API void AL_APIENTRY alBufferf(ALuint buffer, ALenum param, ALfloat value);
AL_API void AL_APIENTRY alBuffer3f(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
AL_API void AL_APIENTRY alBufferfv(ALuint buffer, ALenum param, const ALfloat *values);
AL_API void AL_APIENTRY alBufferi(ALuint buffer, ALenum param, ALint value);
AL_API void AL_APIENTRY alBuffer3i(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3);
AL_API void AL_APIENTRY alBufferiv(ALuint buffer, ALenum param, const ALint *values);

/* Get buffer parameters. */
AL_API void AL_APIENTRY alGetBufferf(ALuint buffer, ALenum param, ALfloat *value);
AL_API void AL_APIENTRY alGetBuffer3f(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
AL_API void AL_APIENTRY alGetBufferfv(ALuint buffer, ALenum param, ALfloat *values);
AL_API void AL_APIENTRY alGetBufferi(ALuint buffer, ALenum param, ALint *value);
AL_API void AL_APIENTRY alGetBuffer3i(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3);
AL_API void AL_APIENTRY alGetBufferiv(ALuint buffer, ALenum param, ALint *values);
#endif /* AL_NO_PROTOTYPES */

/* Pointer-to-function types, useful for storing dynamically loaded AL entry
 * points.
 */
typedef void          (AL_APIENTRY *LPALENABLE)(ALenum capability);
typedef void          (AL_APIENTRY *LPALDISABLE)(ALenum capability);
typedef ALboolean     (AL_APIENTRY *LPALISENABLED)(ALenum capability);
typedef const ALchar* (AL_APIENTRY *LPALGETSTRING)(ALenum param);
typedef void          (AL_APIENTRY *LPALGETBOOLEANV)(ALenum param, ALboolean *values);
typedef void          (AL_APIENTRY *LPALGETINTEGERV)(ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALGETFLOATV)(ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETDOUBLEV)(ALenum param, ALdouble *values);
typedef ALboolean     (AL_APIENTRY *LPALGETBOOLEAN)(ALenum param);
typedef ALint         (AL_APIENTRY *LPALGETINTEGER)(ALenum param);
typedef ALfloat       (AL_APIENTRY *LPALGETFLOAT)(ALenum param);
typedef ALdouble      (AL_APIENTRY *LPALGETDOUBLE)(ALenum param);
typedef ALenum        (AL_APIENTRY *LPALGETERROR)(void);
typedef ALboolean     (AL_APIENTRY *LPALISEXTENSIONPRESENT)(const ALchar *extname);
typedef void*         (AL_APIENTRY *LPALGETPROCADDRESS)(const ALchar *fname);
typedef ALenum        (AL_APIENTRY *LPALGETENUMVALUE)(const ALchar *ename);
typedef void          (AL_APIENTRY *LPALLISTENERF)(ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALLISTENER3F)(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALLISTENERFV)(ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALLISTENERI)(ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALLISTENER3I)(ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALLISTENERIV)(ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETLISTENERF)(ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETLISTENER3F)(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETLISTENERFV)(ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETLISTENERI)(ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETLISTENER3I)(ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETLISTENERIV)(ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALGENSOURCES)(ALsizei n, ALuint *sources);
typedef void          (AL_APIENTRY *LPALDELETESOURCES)(ALsizei n, const ALuint *sources);
typedef ALboolean     (AL_APIENTRY *LPALISSOURCE)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEF)(ALuint source, ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALSOURCE3F)(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALSOURCEFV)(ALuint source, ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALSOURCEI)(ALuint source, ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALSOURCE3I)(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALSOURCEIV)(ALuint source, ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETSOURCEF)(ALuint source, ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETSOURCE3F)(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETSOURCEFV)(ALuint source, ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETSOURCEI)(ALuint source, ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETSOURCE3I)(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETSOURCEIV)(ALuint source, ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALSOURCEPLAYV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCESTOPV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEREWINDV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEPAUSEV)(ALsizei n, const ALuint *sources);
typedef void          (AL_APIENTRY *LPALSOURCEPLAY)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCESTOP)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEREWIND)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEPAUSE)(ALuint source);
typedef void          (AL_APIENTRY *LPALSOURCEQUEUEBUFFERS)(ALuint source, ALsizei nb, const ALuint *buffers);
typedef void          (AL_APIENTRY *LPALSOURCEUNQUEUEBUFFERS)(ALuint source, ALsizei nb, ALuint *buffers);
typedef void          (AL_APIENTRY *LPALGENBUFFERS)(ALsizei n, ALuint *buffers);
typedef void          (AL_APIENTRY *LPALDELETEBUFFERS)(ALsizei n, const ALuint *buffers);
typedef ALboolean     (AL_APIENTRY *LPALISBUFFER)(ALuint buffer);
typedef void          (AL_APIENTRY *LPALBUFFERDATA)(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei samplerate);
typedef void          (AL_APIENTRY *LPALBUFFERF)(ALuint buffer, ALenum param, ALfloat value);
typedef void          (AL_APIENTRY *LPALBUFFER3F)(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3);
typedef void          (AL_APIENTRY *LPALBUFFERFV)(ALuint buffer, ALenum param, const ALfloat *values);
typedef void          (AL_APIENTRY *LPALBUFFERI)(ALuint buffer, ALenum param, ALint value);
typedef void          (AL_APIENTRY *LPALBUFFER3I)(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3);
typedef void          (AL_APIENTRY *LPALBUFFERIV)(ALuint buffer, ALenum param, const ALint *values);
typedef void          (AL_APIENTRY *LPALGETBUFFERF)(ALuint buffer, ALenum param, ALfloat *value);
typedef void          (AL_APIENTRY *LPALGETBUFFER3F)(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
typedef void          (AL_APIENTRY *LPALGETBUFFERFV)(ALuint buffer, ALenum param, ALfloat *values);
typedef void          (AL_APIENTRY *LPALGETBUFFERI)(ALuint buffer, ALenum param, ALint *value);
typedef void          (AL_APIENTRY *LPALGETBUFFER3I)(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3);
typedef void          (AL_APIENTRY *LPALGETBUFFERIV)(ALuint buffer, ALenum param, ALint *values);
typedef void          (AL_APIENTRY *LPALDOPPLERFACTOR)(ALfloat value);
typedef void          (AL_APIENTRY *LPALDOPPLERVELOCITY)(ALfloat value);
typedef void          (AL_APIENTRY *LPALSPEEDOFSOUND)(ALfloat value);
typedef void          (AL_APIENTRY *LPALDISTANCEMODEL)(ALenum distanceModel);

#ifdef __cplusplus
}  /* extern "C" */
#endif


//-------------------------------------------------------------------------------------
//---------------------------------------- al. h----------------------------------------
//-------------------------------------------------------------------------------------
















//-------------------------------------------------------------------------------------
//------------------------------------ alc.h ---------------------------------------
//-------------------------------------------------------------------------------------
















#ifdef AL_ALC_H
#error LibAL already provides OpenAL include files. Remove the alc.h include.
#endif //AL_ALC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ALC_API
 #if defined(AL_LIBTYPE_STATIC)
  #define ALC_API
 #elif defined(_WIN32)
  #define ALC_API __declspec(dllimport)
 #else
  #define ALC_API extern
 #endif
#endif

#ifdef _WIN32
 #define ALC_APIENTRY __cdecl
#else
 #define ALC_APIENTRY
#endif


/* Deprecated macros. */
#define ALCAPI                                   ALC_API
#define ALCAPIENTRY                              ALC_APIENTRY
#define ALC_INVALID                              0

/** Supported ALC version? */
#define ALC_VERSION_0_1                          1

/** Opaque device handle */
typedef struct ALCdevice ALCdevice;
/** Opaque context handle */
typedef struct ALCcontext ALCcontext;

/** 8-bit boolean */
typedef char ALCboolean;

/** character */
typedef char ALCchar;

/** signed 8-bit integer */
typedef signed char ALCbyte;

/** unsigned 8-bit integer */
typedef unsigned char ALCubyte;

/** signed 16-bit integer */
typedef short ALCshort;

/** unsigned 16-bit integer */
typedef unsigned short ALCushort;

/** signed 32-bit integer */
typedef int ALCint;

/** unsigned 32-bit integer */
typedef unsigned int ALCuint;

/** non-negative 32-bit integer size */
typedef int ALCsizei;

/** 32-bit enumeration value */
typedef int ALCenum;

/** 32-bit IEEE-754 floating-point */
typedef float ALCfloat;

/** 64-bit IEEE-754 floating-point */
typedef double ALCdouble;

/** void type (for opaque pointers only) */
typedef void ALCvoid;


/* Enumeration values begin at column 50. Do not use tabs. */

/** Boolean False. */
#define ALC_FALSE                                0

/** Boolean True. */
#define ALC_TRUE                                 1

/** Context attribute: <int> Hz. */
#define ALC_FREQUENCY                            0x1007

/** Context attribute: <int> Hz. */
#define ALC_REFRESH                              0x1008

/** Context attribute: AL_TRUE or AL_FALSE synchronous context? */
#define ALC_SYNC                                 0x1009

/** Context attribute: <int> requested Mono (3D) Sources. */
#define ALC_MONO_SOURCES                         0x1010

/** Context attribute: <int> requested Stereo Sources. */
#define ALC_STEREO_SOURCES                       0x1011

/** No error. */
#define ALC_NO_ERROR                             0

/** Invalid device handle. */
#define ALC_INVALID_DEVICE                       0xA001

/** Invalid context handle. */
#define ALC_INVALID_CONTEXT                      0xA002

/** Invalid enumeration passed to an ALC call. */
#define ALC_INVALID_ENUM                         0xA003

/** Invalid value passed to an ALC call. */
#define ALC_INVALID_VALUE                        0xA004

/** Out of memory. */
#define ALC_OUT_OF_MEMORY                        0xA005


/** Runtime ALC major version. */
#define ALC_MAJOR_VERSION                        0x1000
/** Runtime ALC minor version. */
#define ALC_MINOR_VERSION                        0x1001

/** Context attribute list size. */
#define ALC_ATTRIBUTES_SIZE                      0x1002
/** Context attribute list properties. */
#define ALC_ALL_ATTRIBUTES                       0x1003

/** String for the default device specifier. */
#define ALC_DEFAULT_DEVICE_SPECIFIER             0x1004
/**
 * Device specifier string.
 *
 * If device handle is NULL, it is instead a null-character separated list of
 * strings of known device specifiers (list ends with an empty string).
 */
#define ALC_DEVICE_SPECIFIER                     0x1005
/** String for space-separated list of ALC extensions. */
#define ALC_EXTENSIONS                           0x1006


/** Capture extension */
#define ALC_EXT_CAPTURE 1
/**
 * Capture device specifier string.
 *
 * If device handle is NULL, it is instead a null-character separated list of
 * strings of known capture device specifiers (list ends with an empty string).
 */
#define ALC_CAPTURE_DEVICE_SPECIFIER             0x310
/** String for the default capture device specifier. */
#define ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER     0x311
/** Number of sample frames available for capture. */
#define ALC_CAPTURE_SAMPLES                      0x312


/** Enumerate All extension */
#define ALC_ENUMERATE_ALL_EXT 1
/** String for the default extended device specifier. */
#define ALC_DEFAULT_ALL_DEVICES_SPECIFIER        0x1012
/**
 * Device's extended specifier string.
 *
 * If device handle is NULL, it is instead a null-character separated list of
 * strings of known extended device specifiers (list ends with an empty string).
 */
#define ALC_ALL_DEVICES_SPECIFIER                0x1013


#ifndef ALC_NO_PROTOTYPES
/* Context management. */

/** Create and attach a context to the given device. */
ALC_API ALCcontext* ALC_APIENTRY alcCreateContext(ALCdevice *device, const ALCint *attrlist);
/**
 * Makes the given context the active process-wide context. Passing NULL clears
 * the active context.
 */
ALC_API ALCboolean  ALC_APIENTRY alcMakeContextCurrent(ALCcontext *context);
/** Resumes processing updates for the given context. */
ALC_API void        ALC_APIENTRY alcProcessContext(ALCcontext *context);
/** Suspends updates for the given context. */
ALC_API void        ALC_APIENTRY alcSuspendContext(ALCcontext *context);
/** Remove a context from its device and destroys it. */
ALC_API void        ALC_APIENTRY alcDestroyContext(ALCcontext *context);
/** Returns the currently active context. */
ALC_API ALCcontext* ALC_APIENTRY alcGetCurrentContext(void);
/** Returns the device that a particular context is attached to. */
ALC_API ALCdevice*  ALC_APIENTRY alcGetContextsDevice(ALCcontext *context);

/* Device management. */

/** Opens the named playback device. */
ALC_API ALCdevice* ALC_APIENTRY alcOpenDevice(const ALCchar *devicename);
/** Closes the given playback device. */
ALC_API ALCboolean ALC_APIENTRY alcCloseDevice(ALCdevice *device);

/* Error support. */

/** Obtain the most recent Device error. */
ALC_API ALCenum ALC_APIENTRY alcGetError(ALCdevice *device);

/* Extension support. */

/**
 * Query for the presence of an extension on the device. Pass a NULL device to
 * query a device-inspecific extension.
 */
ALC_API ALCboolean ALC_APIENTRY alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname);
/**
 * Retrieve the address of a function. Given a non-NULL device, the returned
 * function may be device-specific.
 */
ALC_API ALCvoid*   ALC_APIENTRY alcGetProcAddress(ALCdevice *device, const ALCchar *funcname);
/**
 * Retrieve the value of an enum. Given a non-NULL device, the returned value
 * may be device-specific.
 */
ALC_API ALCenum    ALC_APIENTRY alcGetEnumValue(ALCdevice *device, const ALCchar *enumname);

/* Query functions. */

/** Returns information about the device, and error strings. */
ALC_API const ALCchar* ALC_APIENTRY alcGetString(ALCdevice *device, ALCenum param);
/** Returns information about the device and the version of OpenAL. */
ALC_API void           ALC_APIENTRY alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);

/* Capture functions. */

/**
 * Opens the named capture device with the given frequency, format, and buffer
 * size.
 */
ALC_API ALCdevice* ALC_APIENTRY alcCaptureOpenDevice(const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
/** Closes the given capture device. */
ALC_API ALCboolean ALC_APIENTRY alcCaptureCloseDevice(ALCdevice *device);
/** Starts capturing samples into the device buffer. */
ALC_API void       ALC_APIENTRY alcCaptureStart(ALCdevice *device);
/** Stops capturing samples. Samples in the device buffer remain available. */
ALC_API void       ALC_APIENTRY alcCaptureStop(ALCdevice *device);
/** Reads samples from the device buffer. */
ALC_API void       ALC_APIENTRY alcCaptureSamples(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);
#endif /* ALC_NO_PROTOTYPES */

/* Pointer-to-function types, useful for storing dynamically loaded ALC entry
 * points.
 */
typedef ALCcontext*    (ALC_APIENTRY *LPALCCREATECONTEXT)(ALCdevice *device, const ALCint *attrlist);
typedef ALCboolean     (ALC_APIENTRY *LPALCMAKECONTEXTCURRENT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCPROCESSCONTEXT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCSUSPENDCONTEXT)(ALCcontext *context);
typedef void           (ALC_APIENTRY *LPALCDESTROYCONTEXT)(ALCcontext *context);
typedef ALCcontext*    (ALC_APIENTRY *LPALCGETCURRENTCONTEXT)(void);
typedef ALCdevice*     (ALC_APIENTRY *LPALCGETCONTEXTSDEVICE)(ALCcontext *context);
typedef ALCdevice*     (ALC_APIENTRY *LPALCOPENDEVICE)(const ALCchar *devicename);
typedef ALCboolean     (ALC_APIENTRY *LPALCCLOSEDEVICE)(ALCdevice *device);
typedef ALCenum        (ALC_APIENTRY *LPALCGETERROR)(ALCdevice *device);
typedef ALCboolean     (ALC_APIENTRY *LPALCISEXTENSIONPRESENT)(ALCdevice *device, const ALCchar *extname);
typedef ALCvoid*       (ALC_APIENTRY *LPALCGETPROCADDRESS)(ALCdevice *device, const ALCchar *funcname);
typedef ALCenum        (ALC_APIENTRY *LPALCGETENUMVALUE)(ALCdevice *device, const ALCchar *enumname);
typedef const ALCchar* (ALC_APIENTRY *LPALCGETSTRING)(ALCdevice *device, ALCenum param);
typedef void           (ALC_APIENTRY *LPALCGETINTEGERV)(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);
typedef ALCdevice*     (ALC_APIENTRY *LPALCCAPTUREOPENDEVICE)(const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
typedef ALCboolean     (ALC_APIENTRY *LPALCCAPTURECLOSEDEVICE)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESTART)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESTOP)(ALCdevice *device);
typedef void           (ALC_APIENTRY *LPALCCAPTURESAMPLES)(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);

#ifdef __cplusplus
} /* extern "C" */
#endif














//-------------------------------------------------------------------------------------
//------------------------------------ alc.h ---------------------------------------
//-------------------------------------------------------------------------------------
























#include <fstream>
#include <bit>

//TODO : Warning about OpenAL headers
//TODO : Fix including in multiple files 

    std::string LibALerrorMsg; //! If any error occurs the error message will be stored there

    //TODO Specialize errors 

    static inline ALenum LibALtoAlFormat(short channels, short samples)
    {
        //From https://github.com/ffainelli/openal-example/blob/master/openal-example.c#L5

    	bool stereo = (channels > 1);

    	switch (samples) {
    	case 16:
    		if (stereo)
    			return AL_FORMAT_STEREO16;
    		else
    			return AL_FORMAT_MONO16;
    	case 8:
    		if (stereo)
    			return AL_FORMAT_STEREO8;
    		else
    			return AL_FORMAT_MONO8;
    	default:
    		return -1;
    	}
    }
    static struct LibALAudio
    {
        std::uint8_t channels = 0;
        std::uint8_t bitsPerSample = 0; 
        char* bufferData = {};
        ALsizei dataSize = 0;
        int32_t sampleRate = 0;
        ALuint source;
        ALint source_state;
        ALuint buffer;
    };
    ALCdevice* LibALdevice;
    ALCcontext * LibALContext;
    #ifndef LibALmaxAudios
        #define LibALmaxAudios 2000
    #endif 
    LibALAudio LibALaudios[LibALmaxAudios];
    int LibALaudioIndex = 0;

    std::int32_t LibALConvertToInt(char* buffer, std::size_t len)
    {
        std::int32_t a = 0;
        
        // // if(std::endian::native == std::endian::little)
        std::memcpy(&a, buffer, len);
        
        // // else{
        // //     for(std::size_t i = 0; i < len; ++i)
        // //         reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
        // // }
        return a;
    }


        /// Plays the audio with given parameters in a while loop until the audio stops playing
    int LibALplayAudioDataLoop(char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,ALsizei& size,unsigned int ID){

        if(LibALaudios[ID].source_state != AL_PLAYING)
            alSourceUnqueueBuffers(LibALaudios[ID].source, 1, &LibALaudios[ID].buffer);
        
        alBufferData(LibALaudios[ID].buffer, LibALtoAlFormat(channels, bitsPerSample),
		            bufferData, 
                    size, 
                    sampleRate);

        alSourcei(LibALaudios[ID].source, AL_BUFFER, LibALaudios[ID].buffer);
	    alSourcePlay(LibALaudios[ID].source);


        //Don't return from the function until the audio stops playing
	    alGetSourcei(LibALaudios[ID].source, AL_SOURCE_STATE, &LibALaudios[ID].source_state);
	    while (LibALaudios[ID].source_state == AL_PLAYING) {
	    	alGetSourcei(LibALaudios[ID].source, AL_SOURCE_STATE, &LibALaudios[ID].source_state);
	    }

        return 1;
    }
    
    ///Plays the audio with given parameters
    int LibALplayAudioData(char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,ALsizei& size,unsigned int ID){

        if(LibALaudios[ID].source_state != AL_PLAYING)
            alSourceUnqueueBuffers(LibALaudios[ID].source, 1, &LibALaudios[ID].buffer);
        
        alBufferData(LibALaudios[ID].buffer, LibALtoAlFormat(channels, bitsPerSample),
		            bufferData, 
                    size, 
                    sampleRate);

        
        alSourcei(LibALaudios[ID].source, AL_BUFFER, LibALaudios[ID].buffer);
	    alSourcePlay(LibALaudios[ID].source);


        return 1;
    }


    //--------------------------------------------------------------------------------------



    //!START & END 
    
    int LibAL_start(){ //* 0 : An error occured , 1 : LibAL started succesfully
        //Get device, create the buffer

        //! Get the default device
        const ALCchar* defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
	    LibALdevice = alcOpenDevice(defaultDeviceName);
	    if (!LibALdevice) {
	    	LibALerrorMsg = "Unable to open default device";
	    	return 0;
	    }
        

        //! Create the AL Context
        LibALContext = alcCreateContext(LibALdevice, NULL);
	    if (!alcMakeContextCurrent(LibALContext)) {
	    	LibALerrorMsg = "Failed to make default context";
	    	return 0;
	    }


        //! set orientation

        //Where the sound will be come from
	    alListener3f(AL_POSITION, 0.0f, 0.0f, 1.0f); 
        //-- 1.f,0.f,0.f = Left side of the headset 

        //Velocity
        alListener3f(AL_VELOCITY, 0, 0, 0);
	    //-- Seting three of the params as 100.f gives a nice slow mo
        
        //Is constant
        ALfloat listenerOri[6] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };
        alListenerfv(AL_ORIENTATION, listenerOri);
        //-- Don't change the values


        return 1;
    }
    void LibAL_end(){
        //Delete sources, buffers
        //Destroy the context
        //Clear the arrays

        ALCdevice* device = alcGetContextsDevice(LibALContext);
	    alcCloseDevice(device);
	    
        alcMakeContextCurrent(NULL);
	    alcDestroyContext(LibALContext);
        for (size_t i = 0; i < LibALmaxAudios; i++)
        {
            alDeleteSources(1,&LibALaudios[i].buffer);
            alDeleteBuffers(1,&LibALaudios[i].buffer);
            LibALaudios[i].bitsPerSample = NULL;
            LibALaudios[i].bufferData = nullptr;
            LibALaudios[i].channels = NULL;
            LibALaudios[i].dataSize = NULL;
            LibALaudios[i].sampleRate = NULL;
            LibALaudios[i].source_state = NULL;
        }
        LibALaudioIndex = 0;
    }
    


    //! PLAY


    // Plays the audio with given object id's elements in a while loop until the audio stops playing
    int LibAL_playAudioObjectLoop(unsigned int ID){
        if(!LibALplayAudioDataLoop(LibALaudios[ID].bufferData, LibALaudios[ID].channels,
		            LibALaudios[ID].sampleRate, 
                    LibALaudios[ID].bitsPerSample, 
                    LibALaudios[ID].dataSize,ID)){
                        return 0;
                    }

        return 1;
    }
    
    // Plays the audio with given object id's elements
    int LibAL_playAudioObject(unsigned int ID){

        if(!LibALplayAudioData(LibALaudios[ID].bufferData, LibALaudios[ID].channels,
		            LibALaudios[ID].sampleRate, 
                    LibALaudios[ID].bitsPerSample, 
                    LibALaudios[ID].dataSize,ID)){
                        return 0;
                    }
        
        return 1;
    }


    //! Manipulations & More


    //Stops playing the source
    void LibAL_stopPlaying(unsigned int ID){
        alSourceStop(LibALaudios[ID].source);
    }
    void LibAL_pausePlaying(unsigned int ID){
        alSourcePause(LibALaudios[ID].source);
    }
    void LibAL_startPlaying(unsigned int ID){
        alSourcePlay(LibALaudios[ID].source);
    }
    void LibAL_silence(){
        for (size_t i = 0; i < LibALmaxAudios; i++)
        {
            alSourceStop(LibALaudios[i].source);
        }
    }

    void LibAL_invertSoundData(unsigned int ID){
        char* res = LibALaudios[ID].bufferData;

        for (size_t i = 0; i < LibALaudios[ID].dataSize; i++)
        {
            res[i] = LibALaudios[ID].bufferData[LibALaudios[ID].dataSize-i]; 
        }
    }

    bool LibAL_isPlaying(unsigned int ID){
        return LibALaudios[ID].source_state == AL_PLAYING;
    }
    bool LibAL_isPaused(unsigned int ID){
        return LibALaudios[ID].source_state == AL_PAUSED;
    }
    bool LibAL_isStopped(unsigned int ID){
        return LibALaudios[ID].source_state == AL_STOPPED;
    }

    //! Reading files


    int LibAL_readWAVFile(const std::string& path,char* &bufferData, std::uint8_t& channels,std::int32_t& sampleRate,std::uint8_t& bitsPerSample,ALsizei& size){

        std::ifstream in(path, std::ios::binary);
        
        if(!in.is_open())
        {
            LibALerrorMsg  = "Couldn't open the file : " + path;
            return 0;
        }

        char buff[4];

        //------------------- RIFF file description header 
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        if(std::strncmp(buff, "RIFF", 4) != 0)
        {
            LibALerrorMsg  = "This is not a wav file (riff is not found) : " + path;
            return 0;
        }
        //-------------------


        //------------------- Size of the file
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------


        //------------------- WAVE file description header
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        if(std::strncmp(buff, "WAVE", 4) != 0)
        {
            LibALerrorMsg  = "This is not a wav file (wave is not found) : " + path;
            return 0;
        }
        //-------------------
        
        
        //------------------- fmt\0 description header
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------
        
        
        //------------------- Size of WAV section chunk
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------


        //------------------- WAV type format
        if(!in.read(buff, 2))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------
        
        
        //------------------- mono/stereo flag
        if(!in.read(buff, 2))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        channels = LibALConvertToInt(buff, 2);
        //-------------------
        
        
        //------------------- Sample frequency
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        sampleRate = LibALConvertToInt(buff, 2);
        //-------------------
        
        
        //------------------- byte/sec (audio data rate in byte/sec)
        if(!in.read(buff, 4))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------
        

        //------------------- Block alignment
        if(!in.read(buff, 2))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        //-------------------
        
        
        //------------------- Bits per sample
        if(!in.read(buff, 2))
        {
            LibALerrorMsg = "Error reading the binary file";
            return 0;
        }
        bitsPerSample = LibALConvertToInt(buff, 2);
        //-------------------
        
        //------------------- Data description header
        int c = 0;
        while (std::strncmp(buff, "data", 4) == 0)
        {
            if(!in.read(buff, 4))
            {
                LibALerrorMsg = "Error reading the binary file";
                return 0;
            }
            c++;
            if(c > 20){
                LibALerrorMsg  = "Error reading wav file : locating the data description header : " + path;
                return 0;
            }
        }
        //-------------------
        
        
        //------------------- Size of data
        if(!in.read(buff, 4))
        {
            return 0;
        }
        size = LibALConvertToInt(buff, 4);
        //-------------------


        //------------------- Audio data
        bufferData = new char[size];
        in.read(bufferData, size);
        //-------------------

        return 1;
    }

    //! Audio objects


    //Generate an audio object
    int LibAL_genAudio(unsigned int & ID){
        if(LibALaudioIndex == LibALmaxAudios-1){
            return 0;
        }
        //Generate the source
        ALuint source,buffer;
	    alGenSources((ALuint)1, &source);
        
	    alGenBuffers(1, &buffer);

        //Create the source
	    alSourcef(source, AL_PITCH, 1);
	    alSourcef(source, AL_GAIN, 1);
	    alSource3f(source, AL_POSITION, 0, 0, 0);
	    alSource3f(source, AL_VELOCITY, 0, 0, 0);
	    alSourcei(source, AL_LOOPING, AL_FALSE);

        ID = LibALaudioIndex;
        
        LibALAudio audio;
        audio.bitsPerSample = 0;
        audio.bufferData  = {};
        audio.channels = 0;
        audio.dataSize = 0;
        audio.sampleRate = 0;
        audio.source = source;
        audio.buffer = buffer;
        LibALaudios[LibALaudioIndex] = audio;
        LibALaudioIndex++;
    
        return 1;
    }
    
    //Modify the generated audio object via given parameters
    int LibAL_modifyAudioViaData(std::uint8_t channels,std::uint8_t bitsPerSample, char* bufferData,ALsizei dataSize,int32_t sampleRate,unsigned int ID){
        LibALaudios[ID].bitsPerSample = bitsPerSample;
        LibALaudios[ID].bufferData  = bufferData;
        LibALaudios[ID].channels = channels;
        LibALaudios[ID].dataSize = dataSize;
        LibALaudios[ID].sampleRate = sampleRate;
    
        return 1;
    }

    int LibAL_modifySourceViaData(float pitch,float gain,float position[3],float velocity[3],unsigned int ID){
       
        alSourcef(LibALaudios[ID].source, AL_PITCH, pitch);
	    alSourcef(LibALaudios[ID].source, AL_GAIN, gain);
	    alSource3f(LibALaudios[ID].source, AL_POSITION, position[0], position[1], position[2]);
	    alSource3f(LibALaudios[ID].source, AL_VELOCITY, velocity[0], velocity[1], velocity[2]);

        return 1;
    }

    int LibAL_modifyBufferViaData(float position[3],float velocity[3]){
       
        //Where the sound will be come from
	    alListener3f(AL_POSITION, position[0], position[1], position[2]); 
        //-- 1.f,0.f,0.f = Left side of the headset 

        //Velocity
        alListener3f(AL_VELOCITY, velocity[0], velocity[1], velocity[2]);
	    //-- Seting three of the params as 100.f gives a nice slow mo
        return 1;
    }

    //Modify the audio object via path
    //Reads the path and modifies the object
    //Format will be : "wav" for the ".wav" file format
    int LibAL_modifyAudioViaPath(std::string path,std::string format,unsigned int ID){
        
        char* soundData;
	    uint8_t channels;
	    int32_t sampleRate;
	    uint8_t bitsPerSample;
	    ALsizei dataSize;
        
        if(!LibAL_readWAVFile(path,soundData,channels,sampleRate,bitsPerSample,dataSize))
            return 0;
            
        LibAL_modifyAudioViaData(channels,bitsPerSample,soundData,dataSize,sampleRate,ID);
    
        return 1;
    }
    
    int LibAL_deleteAudio(unsigned int &ID){
        //TODO LibALaudioIndex 
        
        LibALAudio audio;
        LibALaudios[ID].bitsPerSample = 0;
        LibALaudios[ID].bufferData  = {};
        LibALaudios[ID].channels = 0;
        LibALaudios[ID].dataSize = 0;
        LibALaudios[ID].sampleRate = 0;
        alDeleteBuffers(1,&LibALaudios[ID].buffer);
        alDeleteSources(1,&LibALaudios[ID].source);
        ID = 0;
        return 1;
    }

    void printOpenALErrors(){
        
        ALCenum error; 

        while(error = alGetError())
        { 
            if (error != AL_NO_ERROR) 
            { 
                if(error == AL_INVALID_ENUM)
                    std::cout << "AL_INVALID_ENUM";
                if(error == AL_INVALID_NAME)
                    std::cout << "AL_INVALID_NAME";
                if(error == AL_INVALID_OPERATION)
                    std::cout << "AL_INVALID_OPERATION";
                if(error == AL_INVALID_VALUE)
                    std::cout << "AL_INVALID_VALUE";
                if(error == AL_OUT_OF_MEMORY)
                    std::cout << "AL_OUT_OF_MEMORY";
            }
        }
    }
#endif //LIBAL
#else



#endif //LIBAL_OPENAL_IMPLEMENTATION