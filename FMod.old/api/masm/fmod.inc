;=====================================================================
; Translation of fmod.h to MASM 6.14 assembly language.
; Translation done by Ricardo Santos
; dxantos@yahoo.com
; Updated to 3.4 by Brett Paterson 9/09/2001
;=====================================================================

ifndef __FMOD_INC__
__FMOD_INC__ = 0

;=====================================================================
;Equates
;=====================================================================

; Version
FMOD_VERSION equ 3.4f

;---------------------------------------------------------------------
; FMOD_ERRORS
;
; Description: On failure of commands in FMOD, use FSOUND_GetError to
;              attain what happened.
;
; See Also: FSOUND_GetError
;---------------------------------------------------------------------
FMOD_ERR_NONE               equ 0   ; No errors
FMOD_ERR_BUSY               equ 1   ; Cannot call this command after FSOUND_Init.  Call FSOUND_Close first.
FMOD_ERR_UNINITIALIZED      equ 2   ; This command failed because FSOUND_Init was not called
FMOD_ERR_INIT               equ 3   ; Error initializing output device.
FMOD_ERR_ALLOCATED          equ 4   ; Error initializing output device, but more specifically, the output device is already in use and cannot be reused.
FMOD_ERR_PLAY               equ 5   ; Playing the sound failed.
FMOD_ERR_OUTPUT_FORMAT      equ 6   ; Soundcard does not support the features needed for this soundsystem (16bit stereo output)
FMOD_ERR_COOPERATIVELEVEL   equ 7   ; Error setting cooperative level for hardware.
FMOD_ERR_CREATEBUFFER       equ 8   ; Error creating hardware sound buffer.
FMOD_ERR_FILE_NOTFOUND      equ 9   ; File not found
FMOD_ERR_FILE_FORMAT        equ 10  ; Unknown file format
FMOD_ERR_FILE_BAD           equ 11  ; Error loading file
FMOD_ERR_MEMORY             equ 12  ; Not enough memory
FMOD_ERR_VERSION            equ 13  ; The version number of this file format is not supported
FMOD_ERR_INVALID_PARAM      equ 14  ; An invalid parameter was passed to this function
FMOD_ERR_NO_EAX             equ 15  ; Tried to use an EAX command on a non EAX enabled channel or output.
FMOD_ERR_NO_EAX2            equ 16  ; Tried to use an advanced EAX2 command on a non EAX2 enabled channel or output.
FMOD_ERR_CHANNEL_ALLOC      equ 17  ; Failed to allocate a new channel
FMOD_ERR_RECORD             equ 18  ; Recording is not supported on this machine
FMOD_ERR_MEDIAPLAYER		equ 19	; Required Mediaplayer codec is not installed


;---------------------------------------------------------------------
; FSOUND_OUTPUTTYPES
;
; Description:  These output types are used with FSOUND_SetOutput, to
;               choose which output driver to use.  FSOUND_OUTPUT_A3D
;               will cause FSOUND_Init to FAIL if you have not got a
;               vortex based A3D card.  The suggestion for this is to
;               immediately try and reinitialize FMOD with
;               FSOUND_OUTPUT_DSOUND, and if this fails, try
;               initializing FMOD with  FSOUND_OUTPUT_WAVEOUT.
;
;               FSOUND_OUTPUT_DSOUND will not support hardware 3d
;               acceleration if the sound card driver does not support
;               DirectX 6 Voice Manager Extensions.
;
;
; See Also:     FSOUND_SetOutput
;               FSOUND_GetOutput
;---------------------------------------------------------------------
FSOUND_OUTPUT_NOSOUND   equ 0 ; NoSound driver, all calls to this succeed but do nothing.
FSOUND_OUTPUT_WINMM     equ 1 ; Windows Multimedia driver.
FSOUND_OUTPUT_DSOUND    equ 2 ; DirectSound driver.  You need this to get EAX or EAX2 support.
FSOUND_OUTPUT_A3D       equ 3 ; A3D driver.  You need this to get geometry and EAX reverb support.

FSOUND_OUTPUT_OSS		equ 4 ; Linux/Unix OSS (Open Sound System) driver, i.e. the kernel sound drivers.
FSOUND_OUTPUT_ESD		equ 5 ; Linux/Unix ESD (Enlightment Sound Daemon) driver.
FSOUND_OUTPUT_ALSA		equ 6 ; Linux Alsa driver.


;---------------------------------------------------------------------
; FSOUND_MIXERTYPE
;
; Description:  These mixer types are used with FSOUND_SetMixer, to
;               choose which mixer to use, or to act upon for other
;               reasons using FSOUND_GetMixer.
;
; See Also:     FSOUND_SetMixer
;               FSOUND_GetMixer
;---------------------------------------------------------------------
FSOUND_MIXER_AUTODETECT                 equ 0   ; Enables autodetection of the fastest mixer based on your cpu.
FSOUND_MIXER_BLENDMODE                  equ 1   ; Enables the standard non mmx, blendmode mixer.
FSOUND_MIXER_MMXP5                      equ 2   ; Enables the mmx, pentium optimized blendmode mixer.
FSOUND_MIXER_MMXP6                      equ 3   ; Enables the mmx, ppro/p2/p3 optimized mixer.

FSOUND_MIXER_QUALITY_AUTODETECT         equ 4   ; Enables autodetection of the fastest quality mixer based on your cpu.
FSOUND_MIXER_QUALITY_FPU                equ 5   ; Enables the interpolating/volume ramping FPU mixer. 
FSOUND_MIXER_QUALITY_MMXP5              equ 6   ; Enables the interpolating/volume ramping p5 MMX mixer. 
FSOUND_MIXER_QUALITY_MMXP6              equ 7   ; Enables the interpolating/volume ramping ppro/p2/p3+ MMX mixer. 



;---------------------------------------------------------------------
; FMUSIC_TYPES
;
; Description:  These definitions describe the type of song being played.
;
; See Also:     FMUSIC_GetType
;---------------------------------------------------------------------
FMUSIC_TYPE_NONE    equ 0       ; No song being played
FMUSIC_TYPE_MOD     equ 1       ; Protracker / Fasttracker
FMUSIC_TYPE_S3M     equ 2       ; ScreamTracker 3
FMUSIC_TYPE_XM      equ 3       ; FastTracker 2
FMUSIC_TYPE_IT      equ 4       ; Impulse Tracker.
FMUSIC_TYPE_MIDI	equ 5		; MIDI file

;---------------------------------------------------------------------
; FSOUND_DSP_PRIORITIES
;
; Description:  These default priorities are
;
; See Also:     FSOUND_DSP_Create
;               FSOUND_DSP_SetPriority
;---------------------------------------------------------------------
FSOUND_DSP_DEFAULTPRIORITY_CLEARUNIT        equ 0       ; DSP CLEAR unit - done first
FSOUND_DSP_DEFAULTPRIORITY_SFXUNIT          equ 100     ; DSP SFX unit - done second
FSOUND_DSP_DEFAULTPRIORITY_MUSICUNIT        equ 200     ; DSP MUSIC unit - done third
FSOUND_DSP_DEFAULTPRIORITY_USER             equ 300     ; User priority, use this as reference
FSOUND_DSP_DEFAULTPRIORITY_FFTUNIT          equ 900     ; This reads data for FSOUND_DSP_GetSpectrum, so it comes after user units
FSOUND_DSP_DEFAULTPRIORITY_CLIPANDCOPYUNIT  equ 1000    ; DSP CLIP AND COPY unit - last

;---------------------------------------------------------------------
; FSOUND_CAPS
;
; Description:  Driver description bitfields.  Use FSOUND_Driver_GetCaps
;               to determine if a driver enumerated has the settings you
;               are after.  The enumerated driver depends on the output
;               mode, see FSOUND_OUTPUTTYPES
;
; See Also:     FSOUND_GetDriverCaps
;               FSOUND_OUTPUTTYPES
;---------------------------------------------------------------------
FSOUND_CAPS_HARDWARE                equ 01h ; This driver supports hardware accelerated 3d sound.
FSOUND_CAPS_EAX                     equ 02h ; This driver supports EAX reverb
FSOUND_CAPS_GEOMETRY_OCCLUSIONS     equ 04h ; This driver supports (A3D) geometry occlusions
FSOUND_CAPS_GEOMETRY_REFLECTIONS    equ 08h ; This driver supports (A3D) geometry reflections
FSOUND_CAPS_EAX2                    equ 10h ; This driver supports EAX2/A3D3 reverb

;---------------------------------------------------------------------
; FSOUND_MODES
;
; Description:  Sample description bitfields, OR them together for
;               loading and describing samples.
;---------------------------------------------------------------------
FSOUND_LOOP_OFF     equ 000001h	; For non looping samples.
FSOUND_LOOP_NORMAL  equ 000002h	; For forward looping samples.
FSOUND_LOOP_BIDI    equ 000004h	; For bidirectional looping samples.  (no effect if in hardware).
FSOUND_8BITS        equ 000008h	; For 8 bit samples.
FSOUND_16BITS       equ 000010h	; For 16 bit samples.
FSOUND_MONO         equ 000020h	; For mono samples.
FSOUND_STEREO       equ 000040h	; For stereo samples.
FSOUND_UNSIGNED     equ 000080h	; For source data containing unsigned samples.
FSOUND_SIGNED       equ 000100h	; For source data containing signed data.
FSOUND_DELTA        equ 000200h	; For source data stored as delta values.
FSOUND_IT214        equ 000400h	; For source data stored using IT214 compression.
FSOUND_IT215        equ 000800h	; For source data stored using IT215 compression.
FSOUND_HW3D         equ 001000h	; Attempts to make samples use 3d hardware acceleration. (if the card supports it)
FSOUND_2D           equ 002000h	; Ignores any 3d processing.  overrides FSOUND_HW3D.  Located in software.
FSOUND_STREAMABLE   equ 004000h	; For realtime streamable samples. If you dont supply this sound may come out corrupted.
FSOUND_LOADMEMORY	equ 008000h	; 'name' will be interpreted as a pointer to data for streaming and samples.
FSOUND_LOADRAW		equ 010000h	; For  will ignore file format and treat as raw pcm.
FSOUND_MPEGACCURATE	equ 020000h	; For FSOUND_Stream_OpenFile - for accurate FSOUND_Stream_GetLengthMs/FSOUND_Stream_SetTime.  WARNING, see FSOUNDStream_OpenFile for inital opening time performance issues.
FSOUND_FORCEMONO    equ 040000h ; For forcing stereo streams and samples to be mono - needed with FSOUND_HW3D - incurs speed hit
FSOUND_HW2D         equ 080000h ; 2d hardware sounds.  allows hardware specific effects
FSOUND_ENABLEFX     equ 100000h ; Allows DX8 FX to be played back on a sound.  Requires DirectX 8 - Note these sounds cant be played more than once, or have a changing frequency

;---------------------------------------------------------------------
; FSOUND_CDPLAYMODES
;
; Description:  Playback method for a CD Audio track, using
;               FSOUND_CD_Play
;
; See Also:     FSOUND_CD_Play
;---------------------------------------------------------------------
FSOUND_CD_PLAYCONTINUOUS    equ 0   ; Starts from the current track and plays to end of CD.
FSOUND_CD_PLAYONCE          equ 1   ; Plays the specified track then stops.
FSOUND_CD_PLAYLOOPED        equ 2   ; Plays the specified track looped, forever until stopped manually.
FSOUND_CD_PLAYRANDOM        equ 3   ; Plays tracks in random order

;---------------------------------------------------------------------
; FSOUND_SAMPLEMODE
;
; Description:  Misccellaneous values for FMOD functions.
;
; See Also:     FSOUND_PlaySound
;               FSOUND_PlaySoundEx
;				FSOUND_Sample_Alloc
;               FSOUND_Sample_Load
;               FSOUND_SetPan
;               
;---------------------------------------------------------------------
FSOUND_FREE          equ -1     ; value to play on any free channel, or to allocate a sample in a free sample slot.
FSOUND_UNMANAGED     equ -2     ; value to allocate a sample that is NOT managed by FSOUND or placed in a sample slot.
FSOUND_ALL			 equ -3	    ; for a channel index , this flag will affect ALL channels available!  Not supported by every function.
FSOUND_STEREOPAN     equ -1     ; value for FSOUND_SetPan so that stereo sounds are not played at half volume.  See FSOUND_SetPan for more on this.
FSOUND_SYSTEMCHANNEL equ -1000  ; special channel ID for channel based functions that want to alter the global FSOUND software mixing output channel

;---------------------------------------------------------------------
; FSOUND_REVERB_ENVIRONMENTS
;
; Description:  These are environment types defined for use with the
;               FSOUND_Reverb API.
;
; See Also:     FSOUND_Reverb_SetEnvironment
;               FSOUND_Reverb_SetEnvironmentAdvanced
;---------------------------------------------------------------------
FSOUND_ENVIRONMENT_GENERIC          equ 0   ;
FSOUND_ENVIRONMENT_PADDEDCELL       equ 1   ;
FSOUND_ENVIRONMENT_ROOM             equ 2   ;
FSOUND_ENVIRONMENT_BATHROOM         equ 3   ;
FSOUND_ENVIRONMENT_LIVINGROOM       equ 4   ;
FSOUND_ENVIRONMENT_STONEROOM        equ 5   ;
FSOUND_ENVIRONMENT_AUDITORIUM       equ 6   ;
FSOUND_ENVIRONMENT_CONCERTHALL      equ 7   ;
FSOUND_ENVIRONMENT_CAVE             equ 8   ;
FSOUND_ENVIRONMENT_ARENA            equ 9   ;
FSOUND_ENVIRONMENT_HANGAR           equ 10  ;
FSOUND_ENVIRONMENT_CARPETEDHALLWAY  equ 11  ;
FSOUND_ENVIRONMENT_HALLWAY          equ 12  ;
FSOUND_ENVIRONMENT_STONECORRIDOR    equ 13  ;
FSOUND_ENVIRONMENT_ALLEY            equ 14  ;
FSOUND_ENVIRONMENT_FOREST           equ 15  ;
FSOUND_ENVIRONMENT_CITY             equ 16  ;
FSOUND_ENVIRONMENT_MOUNTAINS        equ 17  ;
FSOUND_ENVIRONMENT_QUARRY           equ 18  ;
FSOUND_ENVIRONMENT_PLAIN            equ 19  ;
FSOUND_ENVIRONMENT_PARKINGLOT       equ 20  ;
FSOUND_ENVIRONMENT_SEWERPIPE        equ 21  ;
FSOUND_ENVIRONMENT_UNDERWATER       equ 22  ;
FSOUND_ENVIRONMENT_DRUGGED          equ 23  ;
FSOUND_ENVIRONMENT_DIZZY            equ 24  ;
FSOUND_ENVIRONMENT_PSYCHOTIC        equ 25  ;
FSOUND_ENVIRONMENT_COUNT            equ 26  ;

;---------------------------------------------------------------------
; FSOUND_REVERBMIX_USEDISTANCE
;
; Description:  Used with FSOUND_Reverb_SetMix, this setting allows
;               reverb to attenuate based on distance from the listener.
;               Instead of hard coding a value with
;               FSOUND_Reverb_SetMix, this value can be used instead,
;               for a more natural reverb dropoff.
;
; See Also:     FSOUND_Reverb_SetMix
;---------------------------------------------------------------------
FSOUND_REVERBMIX_USEDISTANCE    equ -1.0f   ; used with FSOUND_Reverb_SetMix to scale reverb by distance

;---------------------------------------------------------------------
; FSOUND_REVERB_IGNOREPARAM
;
; Description:  Used with FSOUND_Reverb_SetEnvironment and
;               FSOUND_Reverb_SetEnvironmentAdvanced, this can be placed
;               in the place of a specific parameter for the reverb
;               setting.  It allows you to not set any parameters except
;               the ones you are interested in .. and example would be
;               this.
;
;               invoke FSOUND_Reverb_SetEnvironment, \
;                   FSOUND_REVERB_IGNOREPARAM, \
;                   FSOUND_REVERB_IGNOREPARAM, \
;                   FSOUND_REVERB_IGNOREPARAM, \
;                   _0_0
;
;               This means env, vol and decay are left alone, but 'damp'
;               is set to 0.
;
; See Also:     FSOUND_Reverb_SetEnvironment
;               FSOUND_Reverb_SetEnvironmentAdvanced
;---------------------------------------------------------------------
FSOUND_REVERB_IGNOREPARAM equ -9999999  ; used with FSOUND_Reverb_SetEnvironmentAdvanced to ignore certain parameters by choice.

;---------------------------------------------------------------------
; FSOUND_REVERB_PRESETS
;
; Description:  A set of predefined environment PARAMETERS, created by
;               Creative Labs These can be placed directly into the
;               FSOUND_Reverb_SetEnvironment call
;
; See Also:     FSOUND_Reverb_SetEnvironment
;---------------------------------------------------------------------

;---------------------------------------------------------------------
;Note:  Structure added to help use in assembly language by
;		dxantos@yahoo.com
;       The original defines in fmod.h are converted to constants.
;---------------------------------------------------------------------
FSOUND_REVERB_PRESET struct
    env     dword ?
    vol     real4 ?
    decay   real4 ?
    damp    real4 ?
FSOUND_REVERB_PRESET ends

;This function was added to aid.
FSOUND_Reverb_SetEnvironmentPreset proto :dword


externdef FSOUND_PRESET_GENERIC         :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_PADDEDCELL      :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_ROOM            :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_BATHROOM        :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_LIVINGROOM      :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_STONEROOM       :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_AUDITORIUM      :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_CONCERTHALL     :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_CAVE            :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_ARENA           :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_HANGAR          :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_CARPETEDHALLWAY :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_HALLWAY         :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_STONECORRIDOR   :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_ALLEY           :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_FOREST          :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_CITY            :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_MOUNTAINS       :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_QUARRY          :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_PLAIN           :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_PARKINGLOT      :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_SEWERPIPE       :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_UNDERWATER      :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_DRUGGED         :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_DIZZY           :FSOUND_REVERB_PRESET
externdef FSOUND_PRESET_PSYCHOTIC       :FSOUND_REVERB_PRESET

;---------------------------------------------------------------------
; FSOUND_GEOMETRY_MODES
;
; Description:  Geometry flags, used as the mode flag in
;               FSOUND_Geometry_AddPolygon
;
; See Also:     FSOUND_Geometry_AddPolygon
;---------------------------------------------------------------------
FSOUND_GEOMETRY_NORMAL              equ 0   ; Default geometry type.  Occluding polygon
FSOUND_GEOMETRY_REFLECTIVE          equ 1   ; This polygon is reflective
FSOUND_GEOMETRY_OPENING             equ 2   ; Overlays a transparency over the previous polygon.  The 'openingfactor' value supplied is copied internally.
FSOUND_GEOMETRY_OPENING_REFERENCE   equ 4   ; Overlays a transparency over the previous polygon.  The 'openingfactor' supplied is pointed to (for access when building a list)

;---------------------------------------------------------------------
; FSOUND_FX_MODES
;
; Description:  These flags are used with FSOUND_FX_Enable to enable or disable DirectX 8 FX for a channel.
;
; See Also:     
;    FSOUND_FX_Enable
;    FSOUND_FX_SetChorus
;    FSOUND_FX_SetCompressor
;    FSOUND_FX_SetDistortion
;    FSOUND_FX_SetEcho
;    FSOUND_FX_SetFlanger
;    FSOUND_FX_SetGargle
;    FSOUND_FX_SetI3DL2Reverb
;    FSOUND_FX_SetParamEQ
;    FSOUND_FX_SetWavesReverb
;---------------------------------------------------------------------

FSOUND_FX_CHORUS                    equ 0001h
FSOUND_FX_COMPRESSOR                equ 0002h
FSOUND_FX_DISTORTION                equ 0004h
FSOUND_FX_ECHO                      equ 0008h
FSOUND_FX_FLANGER                   equ 0010h
FSOUND_FX_GARGLE                    equ 0020h
FSOUND_FX_I3DL2REVERB               equ 0040h
FSOUND_FX_PARAMEQ                   equ 0080h
FSOUND_FX_WAVES_REVERB              equ 0100h


;---------------------------------------------------------------------
; FSOUND_SPEAKERMODES
;
; Description:  These are speaker types defined for use with the FSOUND_SetSpeakerMode command.
;
; See Also:     FSOUND_SetSpeakerMode
;
; Remarks:      Only works with FSOUND_OUTPUT_DSOUND output mode.
;
;---------------------------------------------------------------------

FSOUND_SPEAKERMODE_5POINT1          equ 0   ; The audio is played through a speaker arrangement of surround speakers with a subwoofer.
FSOUND_SPEAKERMODE_HEADPHONE        equ 1   ; The speakers are headphones.
FSOUND_SPEAKERMODE_MONO             equ 2   ; The speakers are monaural.
FSOUND_SPEAKERMODE_QUAD             equ 3   ; The speakers are quadraphonic.
FSOUND_SPEAKERMODE_STEREO           equ 4   ; The speakers are stereo (default value).
FSOUND_SPEAKERMODE_SURROUND         equ 5   ; The speakers are surround sound.


;---------------------------------------------------------------------
; FSOUND_INIT_FLAGS
;	
; Description:  Initialization flags.  Use them with FSOUND_Init in the flags parameter to change various behaviour.
;
; See Also:     FSOUND_Init
;---------------------------------------------------------------------
FSOUND_INIT_USEDEFAULTMIDISYNTH		equ 1	; Causes MIDI playback to force software decoding.
FSOUND_INIT_GLOBALFOCUS				equ 2	; For DirectSound output - sound is not muted when window is out of focus.
FSOUND_INIT_ENABLEOUTPUTFX          equ 4   ; For DirectSound output - Allows FSOUND_FX api to be used on global software mixer output!


;=====================================================================
; Function prototypes.
;=====================================================================

; *Pre* FSOUND_Init functions.  These can't be called after FSOUND_Init is
; called (they will fail) They set up FMOD system functionality.
FSOUND_SetOutput                PROTO :DWORD
FSOUND_SetDriver                PROTO :DWORD
FSOUND_SetMixer                 PROTO :DWORD
FSOUND_SetBufferSize            PROTO :DWORD
FSOUND_SetHWND                  PROTO :DWORD
FSOUND_SetMinHardwareChannels   PROTO :DWORD
FSOUND_SetMaxHardwareChannels   PROTO :DWORD

; Main initialization / closedown function
; Note : Use FSOUND_INIT_USEDEFAULTMIDISYNTH with FSOUND_Init for software override with MIDI playback.
FSOUND_Init     PROTO :DWORD,:DWORD,:DWORD
FSOUND_Close    PROTO

; Runtime
FSOUND_SetSpeakerMode       PROTO :DWORD
FSOUND_SetSFXMasterVolume   PROTO :DWORD
FSOUND_SetPanSeperation     PROTO :DWORD
FSOUND_File_SetCallbacks    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

; System information
FSOUND_GetError					PROTO
FSOUND_GetVersion				PROTO
FSOUND_GetOutput				PROTO
FSOUND_GetDriver				PROTO
FSOUND_GetMixer					PROTO
FSOUND_GetNumDrivers			PROTO
FSOUND_GetDriverName			PROTO :DWORD
FSOUND_GetDriverCaps			PROTO :DWORD,:DWORD
FSOUND_GetOutputRate            PROTO
FSOUND_GetMaxChannels           PROTO
FSOUND_GetMaxSamples            PROTO
FSOUND_GetSFXMasterVolume       PROTO
FSOUND_GetNumHardwareChannels   PROTO
FSOUND_GetChannelsPlaying       PROTO
FSOUND_GetCPUUsage              PROTO

;---------------------------------------------------------------------
; Sample management / load functions.
;---------------------------------------------------------------------

; Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Sample_Load to load from memory.
;        Use FSOUND_LOADRAW      flag with FSOUND_Sample_Load to treat as as raw pcm data.

; Sample creation and management functions
FSOUND_Sample_Load		PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Alloc     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Free      PROTO :DWORD
FSOUND_Sample_Upload    PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_Lock      PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_Unlock    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

; Sample control functions
FSOUND_Sample_SetLoopMode       PROTO :DWORD,:DWORD
FSOUND_Sample_SetLoopPoints     PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_SetDefaults       PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_SetMinMaxDistance PROTO :DWORD,:DWORD,:DWORD

; Sample information
FSOUND_Sample_Get           PROTO :DWORD
FSOUND_Sample_GetName		PROTO :DWORD
FSOUND_Sample_GetLength     PROTO :DWORD
FSOUND_Sample_GetLoopPoints PROTO :DWORD,:DWORD,:DWORD
FSOUND_Sample_GetDefaults   PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Sample_GetMode       PROTO :DWORD

;---------------------------------------------------------------------
; Channel control functions.
;---------------------------------------------------------------------

; Playing and stopping sounds.
FSOUND_PlaySound            PROTO :DWORD,:DWORD
FSOUND_PlaySoundEx          PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_StopSound            PROTO :DWORD

; Functions to control playback of a channel.
FSOUND_SetFrequency         PROTO :DWORD,:DWORD
FSOUND_SetVolume            PROTO :DWORD,:DWORD
FSOUND_SetVolumeAbsolute    PROTO :DWORD,:DWORD
FSOUND_SetPan               PROTO :DWORD,:DWORD
FSOUND_SetSurround          PROTO :DWORD,:DWORD
FSOUND_SetMute              PROTO :DWORD,:DWORD
FSOUND_SetPriority          PROTO :DWORD,:DWORD
FSOUND_SetReserved          PROTO :DWORD,:DWORD
FSOUND_SetPaused            PROTO :DWORD,:DWORD
FSOUND_SetLoopMode			PROTO :DWORD,:DWORD
FSOUND_SetCurrentPosition   PROTO :DWORD,:DWORD

; Functions to control DX8 only effects processing.
; Note that FX enabled samples can only be played once at a time.
FSOUND_FX_Enable            PROTO :DWORD,:DWORD     ; Set bits to enable following fx
FSOUND_FX_SetChorus         PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetCompressor     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetDistortion     PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetEcho           PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetFlanger        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetGargle         PROTO :DWORD,:DWORD,:DWORD
FSOUND_FX_SetI3DL2Reverb    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetParamEQ        PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_FX_SetWavesReverb    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD

; Channel information
FSOUND_IsPlaying            PROTO :DWORD
FSOUND_GetFrequency         PROTO :DWORD
FSOUND_GetVolume            PROTO :DWORD
FSOUND_GetPan               PROTO :DWORD
FSOUND_GetSurround          PROTO :DWORD
FSOUND_GetMute              PROTO :DWORD
FSOUND_GetPriority          PROTO :DWORD
FSOUND_GetReserved          PROTO :DWORD
FSOUND_GetPaused            PROTO :DWORD
FSOUND_GetCurrentPosition   PROTO :DWORD
FSOUND_GetCurrentSample     PROTO :DWORD
FSOUND_GetCurrentVU         PROTO :DWORD

;---------------------------------------------------------------------
; 3D sound functions.
;---------------------------------------------------------------------
; see also FSOUND_Sample_SetMinMaxDistance (above)
FSOUND_3D_Update                        PROTO
FSOUND_3D_SetAttributes                 PROTO :DWORD,:DWORD,:DWORD
FSOUND_3D_GetAttributes                 PROTO :DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_SetAttributes        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_GetAttributes        PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_3D_Listener_SetDopplerFactor     PROTO :DWORD
FSOUND_3D_Listener_SetDistanceFactor    PROTO :DWORD
FSOUND_3D_Listener_SetRolloffFactor     PROTO :DWORD

;---------------------------------------------------------------------
; File Streaming functions.
;---------------------------------------------------------------------

; Note : Use FSOUND_LOADMEMORY   flag with FSOUND_Stream_OpenFile to stream from memory.
;        Use FSOUND_LOADRAW      flag with FSOUND_Stream_OpenFile to treat stream as raw pcm data.
;        Use FSOUND_MPEGACCURATE flag with FSOUND_Stream_OpenFile to open mpegs in 'accurate mode' for seeking etc.

FSOUND_Stream_OpenFile			PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_Create			PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Stream_Play				PROTO :DWORD,:DWORD
FSOUND_Stream_PlayEx            PROTO :DWORD,:DWORD,:DWORD,:DWORD

FSOUND_Stream_Stop				PROTO :DWORD
FSOUND_Stream_Close				PROTO :DWORD
FSOUND_Stream_SetEndCallback	PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_SetSynchCallback	PROTO :DWORD,:DWORD,:DWORD
FSOUND_Stream_GetSample			PROTO :DWORD
FSOUND_Stream_CreateDSP			PROTO :DWORD,:DWORD,:DWORD,:DWORD

FSOUND_Stream_SetPosition   PROTO :DWORD,:DWORD
FSOUND_Stream_GetPosition   PROTO :DWORD
FSOUND_Stream_SetTime		PROTO :DWORD,:DWORD
FSOUND_Stream_GetTime       PROTO :DWORD
FSOUND_Stream_GetLength     PROTO :DWORD
FSOUND_Stream_GetLengthMs	PROTO :DWORD

;---------------------------------------------------------------------
; CD audio functions.
;---------------------------------------------------------------------

FSOUND_CD_Play				PROTO :DWORD
FSOUND_CD_SetPlayMode		PROTO :DWORD
FSOUND_CD_Stop				PROTO
FSOUND_CD_SetPaused			PROTO :DWORD
FSOUND_CD_SetVolume			PROTO :DWORD
FSOUND_CD_Eject				PROTO
							
FSOUND_CD_GetPaused			PROTO
FSOUND_CD_GetTrack			PROTO
FSOUND_CD_GetNumTracks		PROTO
FSOUND_CD_GetVolume			PROTO
FSOUND_CD_GetTrackLength	PROTO :DWORD
FSOUND_CD_GetTrackTime		PROTO

;---------------------------------------------------------------------
; DSP functions.
;---------------------------------------------------------------------

; DSP Unit control and information functions.
FSOUND_DSP_Create       PROTO :DWORD,:DWORD,:DWORD
FSOUND_DSP_Free         PROTO :DWORD
FSOUND_DSP_SetPriority  PROTO :DWORD,:DWORD
FSOUND_DSP_GetPriority  PROTO :DWORD
FSOUND_DSP_SetActive    PROTO :DWORD,:DWORD
FSOUND_DSP_GetActive    PROTO :DWORD

; Functions to get hold of FSOUND 'system DSP unit' handles.
FSOUND_DSP_GetClearUnit         PROTO
FSOUND_DSP_GetSFXUnit           PROTO
FSOUND_DSP_GetMusicUnit         PROTO
FSOUND_DSP_GetClipAndCopyUnit   PROTO
FSOUND_DSP_GetFFTUnit           PROTO

; misc DSP functions
FSOUND_DSP_MixBuffers           PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_DSP_ClearMixBuffer       PROTO
FSOUND_DSP_GetBufferLength      PROTO
FSOUND_DSP_GetBufferLengthTotal PROTO   ; Total buffer length due to FSOUND_SetBufferSize
FSOUND_DSP_GetSpectrum          PROTO   ; Array of 512 floats 

;---------------------------------------------------------------------
; Geometry functions.
;---------------------------------------------------------------------

; scene/polygon functions
FSOUND_Geometry_AddPolygon  PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Geometry_AddList     PROTO :DWORD

; polygon list functions
FSOUND_Geometry_List_Create PROTO :DWORD
FSOUND_Geometry_List_Free   PROTO :DWORD
FSOUND_Geometry_List_Begin  PROTO :DWORD
FSOUND_Geometry_List_End    PROTO :DWORD

; material functions
FSOUND_Geometry_Material_Create         PROTO
FSOUND_Geometry_Material_Free           PROTO :DWORD
FSOUND_Geometry_Material_SetAttributes  PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Geometry_Material_GetAttributes  PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Geometry_Material_Set            PROTO :DWORD

;---------------------------------------------------------------------
; Reverb functions. (eax, eax2, a3d 3.0 reverb)
;---------------------------------------------------------------------

; eax1, eax2, a3d 3.0 (use FSOUND_REVERB_PRESETS if you like), (eax2 support through emulation/parameter conversion)
FSOUND_Reverb_SetEnvironment PROTO :DWORD,:DWORD,:DWORD,:DWORD
; eax2, a3d 3.0 only, does not work on eax1
FSOUND_Reverb_SetEnvironmentAdvanced    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Reverb_SetMix                    PROTO :DWORD,:DWORD

; information functions
FSOUND_Reverb_GetEnvironment            PROTO :DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Reverb_GetEnvironmentAdvanced    PROTO :DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD,:DWORD
FSOUND_Reverb_GetlMix                   PROTO :DWORD, :DWORD

;---------------------------------------------------------------------
; Recording functions
;---------------------------------------------------------------------

; recording initialization functions
FSOUND_Record_SetDriver         PROTO :DWORD
FSOUND_Record_GetNumDrivers     PROTO
FSOUND_Record_GetDriverName     PROTO :DWORD
FSOUND_Record_GetDriver         PROTO

; recording functionality.  Only one recording session will work at a time
FSOUND_Record_StartSample       PROTO :DWORD,:DWORD
FSOUND_Record_GetPosition       PROTO
FSOUND_Record_Stop              PROTO

;---------------------------------------------------------------------
; FMUSIC API
;---------------------------------------------------------------------

; Song management / playback functions.
FMUSIC_LoadSong             PROTO :DWORD
FMUSIC_LoadSongMemory       PROTO :DWORD,:DWORD
FMUSIC_FreeSong             PROTO :DWORD
FMUSIC_PlaySong             PROTO :DWORD
FMUSIC_StopSong             PROTO :DWORD
FMUSIC_StopAllSongs         PROTO

FMUSIC_SetZxxCallback       PROTO :DWORD,:DWORD
FMUSIC_SetRowCallback       PROTO :DWORD,:DWORD,:DWORD
FMUSIC_SetOrderCallback     PROTO :DWORD,:DWORD,:DWORD
FMUSIC_SetInstCallback		PROTO :DWORD,:DWORD,:DWORD

FMUSIC_SetSample			PROTO :DWORD,:DWORD,:DWORD
FMUSIC_OptimizeChannels     PROTO :DWORD,:DWORD,:DWORD

; Runtime song functions.
FMUSIC_SetReverb			PROTO :DWORD			; MIDI only.
FMUSIC_SetOrder             PROTO :DWORD,:DWORD
FMUSIC_SetPaused            PROTO :DWORD,:DWORD
FMUSIC_SetMasterVolume      PROTO :DWORD,:DWORD
FMUSIC_SetPanSeperation     PROTO :DWORD,:DWORD

; Static song information functions.
FMUSIC_GetName                  PROTO :DWORD
FMUSIC_GetType                  PROTO :DWORD
FMUSIC_GetNumOrders             PROTO :DWORD
FMUSIC_GetNumPatterns           PROTO :DWORD
FMUSIC_GetNumInstruments        PROTO :DWORD
FMUSIC_GetNumSamples            PROTO :DWORD
FMUSIC_GetNumChannels           PROTO :DWORD
FMUSIC_GetSample                PROTO :DWORD,:DWORD
FMUSIC_GetPatternLength         PROTO :DWORD,:DWORD

; Runtime song information.
FMUSIC_IsFinished       PROTO :DWORD
FMUSIC_IsPlaying        PROTO :DWORD
FMUSIC_GetMasterVolume  PROTO :DWORD
FMUSIC_GetGlobalVolume  PROTO :DWORD
FMUSIC_GetOrder         PROTO :DWORD
FMUSIC_GetPattern       PROTO :DWORD
FMUSIC_GetSpeed         PROTO :DWORD
FMUSIC_GetBPM           PROTO :DWORD
FMUSIC_GetRow           PROTO :DWORD
FMUSIC_GetPaused        PROTO :DWORD
FMUSIC_GetTime          PROTO :DWORD

;Error string
FMOD_ErrorString proto :dword

endif ;__FMOD_INC__

