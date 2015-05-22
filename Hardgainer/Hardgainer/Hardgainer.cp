/*
 *  Hardgainer.cp
 *  Hardgainer
 *
 *  Created by Farhan Mannan on 10/05/2015.
 *  Copyright (c) 2015 Farhan Mannan. All rights reserved.
 *
 */

#include "Hardgainer.h"
#include "HardgainerPriv.h"

CFStringRef HardgainerUUID(void)
{
	CHardgainer* theObj = new CHardgainer;
	return theObj->UUID();
}

CFStringRef CHardgainer::UUID()
{
	return CFSTR("0001020304050607");
}

AUDIOCOMPONENT_ENTRY(AUBaseFactory, Hardgainer)

Hardgainer::Hardgainer (AudioUnit component) : AUEffectBase (component) {
    CreateElements ();
    Globals () -> UseIndexedParameters (kNumberOfParameters);
    SetParameter (
                  kParameter_Gain,
                  kDefaultValue_Gain
                  );
    SetAFactoryPresetAsCurrent (kPresets [kPreset_Default]);
    
    
#if AU_DEBUG_DISPATCHER
    mDebugDispatcher = new AUDebugDispatcher (this);
#endif
}

#pragma mark ____HardgainerEffectKernel

Hardgainer::HardgainerKernel::HardgainerKernel (AUEffectBase *inAudioUnit ) :

AUKernelBase (inAudioUnit), mSamplesProcessed (0), mCurrentScale (0) // 1
{
//    for (int i = 0; i < kWaveArraySize; ++i) {                           // 2
//        double radians = i * 2.0 * pi / kWaveArraySize;
//        mSine [i] = (sin (radians) + 1.0) * 0.5;
//    }
//    
//    for (int i = 0; i < kWaveArraySize; ++i) {                           // 3
//        double radians = i * 2.0 * pi / kWaveArraySize;
//        radians = radians + 0.32;
//        mSquare [i] =
//        (
//         sin (radians) +
//         0.3 * sin (3 * radians) +
//         0.15 * sin (5 * radians) +
//         0.075 * sin (7 * radians) +
//         0.0375 * sin (9 * radians) +
//         0.01875 * sin (11 * radians) +
//         0.009375 * sin (13 * radians) +
//         0.8
//         ) * 0.63;
//    }
    mSampleFrequency = GetSampleRate ();                                 // 4
}

#pragma mark ____Parameters

ComponentResult Hardgainer::GetParameterInfo (
                                               AudioUnitScope			inScope,
                                               AudioUnitParameterID	inParameterID,
                                               AudioUnitParameterInfo	&outParameterInfo
                                               ) {
    ComponentResult result = noErr;
    
    outParameterInfo.flags =
    kAudioUnitParameterFlag_IsWritable | kAudioUnitParameterFlag_IsReadable;
    
    if (inScope == kAudioUnitScope_Global) {
        switch (inParameterID) {
                
            case kParameter_Gain:

                AUBase::FillInParameterName (
                                             outParameterInfo,
                                             kParamName_Gain,
                                             false
                                             );
                outParameterInfo.unit			= kAudioUnitParameterUnit_Decibels;
                outParameterInfo.minValue		= kMinimumValue_Gain;
                outParameterInfo.maxValue		= kMaximumValue_Gain;
                outParameterInfo.defaultValue	= kDefaultValue_Gain;
                break;
            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
        }
    } else {
        result = kAudioUnitErr_InvalidParameter;
    }
    return result;
}

ComponentResult Hardgainer::GetParameterValueStrings (
                                                       AudioUnitScope			inScope,
                                                       AudioUnitParameterID	inParameterID,
                                                       CFArrayRef				*outStrings
                                                       ) {
    if (inScope == kAudioUnitScope_Global) {
        
        // When this method gets called by the AUBase::DispatchGetPropertyInfo method, which
        // provides a null value for the outStrings parameter, just return without error.
        if (outStrings == NULL) return noErr;
        
        // Creates a new immutable array containing the menu item names, and places the array
        // in the outStrings output parameter.
        *outStrings = NULL ;
        return noErr;
    }
    return kAudioUnitErr_InvalidParameter;
}

#pragma mark ____Factory Presets
ComponentResult Hardgainer::GetPresets (                     
                                         CFArrayRef *outData
                                         ) const {
    
    if (outData == NULL) return noErr;                       
    
    CFMutableArrayRef presetsArray = CFArrayCreateMutable (
                                                           NULL,
                                                           kNumberPresets,
                                                           NULL
                                                           );
    
    for (int i = 0; i < kNumberPresets; ++i) {                
        CFArrayAppendValue (
                            presetsArray,
                            &kPresets [i]
                            );
    }
    
    *outData = (CFArrayRef) presetsArray;
    return noErr;
}

OSStatus Hardgainer::NewFactoryPresetSet (                      
                                           const AUPreset &inNewFactoryPreset
                                           ) {
    SInt32 chosenPreset = inNewFactoryPreset.presetNumber;          
    
    if (                                                            
        chosenPreset == kPreset_Small
        ) {
        for (int i = 0; i < kNumberPresets; ++i) {                  
            if (chosenPreset == kPresets[i].presetNumber) {
                switch (chosenPreset) {                             
                        
                    case kPreset_Small:                             

                        SetParameter (                              
                                      kParameter_Gain,
                                      kParameter_Preset_Gain_Small
                                      );
            

                        break;
                }
                SetAFactoryPresetAsCurrent (                        
                                            kPresets [i]
                                            );
                return noErr;                                       
            }
        }
    }
    return kAudioUnitErr_InvalidProperty;
}



void Hardgainer::HardgainerKernel::Process (                        // 1
                                              const Float32   *inSourceP,                                       // 2
                                              Float32         *inDestP,                                         // 3
                                              UInt32          inSamplesToProcess,                               // 4
                                              UInt32          inNumChannels,                                    // 5
                                              bool            &ioSilence                                        // 6
) {
    if (!ioSilence) {                                                 // 7
        
        const Float32 *sourceP = inSourceP;                           // 8
        
        Float32  *destP = inDestP,                                    // 9
        inputSample,                                         // 10
        outputSample,                                        // 11
//        tremoloFrequency,                                    // 12
//        tremoloDepth,                                        // 13
        samplesPerTremoloCycle,                              // 14
        rawTremoloGain,                                      // 15
        tremoloGain;                                         // 16
        
//        int      tremoloWaveform;                                     // 17
        int hardgainerGain;
        
//        tremoloFrequency = GetParameter (kParameter_Frequency);       // 18
//        tremoloDepth     = GetParameter (kParameter_Depth);           // 19
//        tremoloWaveform  =
//        (int) GetParameter (kParameter_Waveform);                 // 20

        hardgainerGain = GetParameter(kParameter_Gain);
        
//        if (tremoloWaveform != kSineWave_Tremolo_Waveform             // 21
//            && tremoloWaveform != kSquareWave_Tremolo_Waveform)
//            tremoloWaveform = kSquareWave_Tremolo_Waveform;
//        
//        if (tremoloWaveform == kSineWave_Tremolo_Waveform)  {         // 22
//            waveArrayPointer = &mSine [0];
//        } else {
//            waveArrayPointer = &mSquare [0];
//        }
//        
//        if (tremoloFrequency < kMinimumValue_Tremolo_Freq)            // 23
//            tremoloFrequency = kMinimumValue_Tremolo_Freq;
//        if (tremoloFrequency > kMaximumValue_Tremolo_Freq)
//            tremoloFrequency = kMaximumValue_Tremolo_Freq;
//        
//        if (tremoloDepth     < kMinimumValue_Tremolo_Depth)           // 24
//            tremoloDepth     = kMinimumValue_Tremolo_Depth;
//        if (tremoloDepth     > kMaximumValue_Tremolo_Depth)
//            tremoloDepth     = kMaximumValue_Tremolo_Depth;
        
//        samplesPerTremoloCycle = mSampleFrequency / tremoloFrequency; // 25
        mNextScale = kWaveArraySize ; // ... divided by samplesPerTremoloCycle;         // 26
        
        // the sample processing loop ////////////////
        for (int i = inSamplesToProcess; i > 0; --i) {                // 27
            
            int index =                                               // 28
            static_cast<long>(mSamplesProcessed * mCurrentScale) %
            kWaveArraySize;
            
            if ((mNextScale != mCurrentScale) && (index == 0)) {      // 29
                mCurrentScale = mNextScale;
                mSamplesProcessed = 0;
            }
            
            if ((mSamplesProcessed >= sampleLimit) && (index == 0))   // 30
                mSamplesProcessed = 0;
            
            rawTremoloGain = waveArrayPointer [index];                // 31
            
//            tremoloGain       = (rawTremoloGain * tremoloDepth -      // 32
//                                 tremoloDepth + 100.0) * 0.01;
            inputSample       = *sourceP;                             // 33
            outputSample      = (inputSample + hardgainerGain);          // 34
            *destP            = outputSample;                         // 35
            sourceP           += 1;                                   // 36
            destP             += 1;                                   // 37
            mSamplesProcessed += 1;                                   // 38
        }
    }
    
}

void Hardgainer::HardgainerKernel::Reset() {
    mCurrentScale        = 0;                    // 1
    mSamplesProcessed    = 0;                    // 2
}

#pragma mark ____Properties

// this stuff below was presumably in the template but not in the tutorial


ComponentResult Hardgainer::GetPropertyInfo (
                                              // This audio unit doesn't define any custom properties, so it uses this generic code for
                                              // this method.
                                              AudioUnitPropertyID	inID,
                                              AudioUnitScope		inScope,
                                              AudioUnitElement	inElement,
                                              UInt32				&outDataSize,
                                              Boolean				&outWritable
                                              ) {
    return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

ComponentResult Hardgainer::GetProperty (
                                          // This audio unit doesn't define any custom properties, so it uses this generic code for
                                          // this method.
                                          AudioUnitPropertyID inID,
                                          AudioUnitScope 		inScope,
                                          AudioUnitElement 	inElement,
                                          void				*outData
                                          ) {
    return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}