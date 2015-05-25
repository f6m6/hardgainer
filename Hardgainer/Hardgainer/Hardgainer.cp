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
                  kDefaultValue_Gain);
    
    SetAFactoryPresetAsCurrent (kPresets [kPreset_Default]);
    
    
#if AU_DEBUG_DISPATCHER
    mDebugDispatcher = new AUDebugDispatcher (this);
#endif
}

#pragma mark ____HardgainerEffectKernel

Hardgainer::HardgainerKernel::HardgainerKernel (AUEffectBase *inAudioUnit ) :

AUKernelBase (inAudioUnit), mSamplesProcessed (0), mCurrentScale (0) 
{ mSampleFrequency = GetSampleRate (); }

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
        
        if (outStrings == NULL) return noErr;
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
                        SetParameter (kParameter_Gain, kParameter_Preset_Gain_Small);
                        break;
                }
                SetAFactoryPresetAsCurrent (kPresets [i]);
                return noErr;
            }
        }
    }
    return kAudioUnitErr_InvalidProperty;
}



void Hardgainer::HardgainerKernel::Process (
                                            const Float32   *inSourceP,
                                            Float32         *inDestP,
                                            UInt32          inSamplesToProcess,
                                            UInt32          inNumChannels,
                                            bool            &ioSilence
                                            ) {
    if (!ioSilence) {
        
        const Float32 *sourceP = inSourceP;
        Float32 *destP = inDestP;
        int n = inSamplesToProcess;
        Float32 hardgainerGain = GetParameter(kParameter_Gain);
        Float32 factor = pow(10, hardgainerGain/20.0);
        while(n--)
        {
            float input = *sourceP++;
            float output = input * factor;
            *destP++ = output;
        }
        
    }
    
}

void Hardgainer::HardgainerKernel::Reset() {
    mCurrentScale        = 0;
    mSamplesProcessed    = 0;
}

#pragma mark ____Properties

ComponentResult Hardgainer::GetPropertyInfo (
                                             AudioUnitPropertyID	inID,
                                             AudioUnitScope		inScope,
                                             AudioUnitElement	inElement,
                                             UInt32				&outDataSize,
                                             Boolean				&outWritable
                                             ) {
    return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

ComponentResult Hardgainer::GetProperty (
                                         AudioUnitPropertyID inID,
                                         AudioUnitScope 		inScope,
                                         AudioUnitElement 	inElement,
                                         void				*outData
                                         ) {
    return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}