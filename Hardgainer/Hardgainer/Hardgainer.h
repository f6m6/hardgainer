/*
 *  Hardgainer.h
 *  Hardgainer
 *
 *  Created by Farhan Mannan on 10/05/2015.
 *  Copyright (c) 2015 Farhan Mannan. All rights reserved.
 *
 */

#include "AUEffectBase.h"
#include "HardgainerVersion.h"

extern "C" {
#include <CoreFoundation/CoreFoundation.h>
    
    
#pragma GCC visibility push(default)
    
    /* External interface to the Hardgainer, C-based */
    
    CFStringRef HardgainerUUID(void);
    
#pragma GCC visibility pop
}

#pragma mark ____Hardgainer Parameter Constants

static CFStringRef kParamName_Gain		= CFSTR ("Gain"); // dB
static const float kDefaultValue_Gain	= 0.0;
static const float kMinimumValue_Gain	= -2.0;
static const float kMaximumValue_Gain	= 2.0;


enum {
    kParameter_Gain	= 0,
    kNumberOfParameters = 1
};

#pragma mark ____Hardgainer
class Hardgainer: public AUEffectBase {
    
public:
    Hardgainer (AudioUnit component);
    
#if AU_DEBUG_DISPATCHER
    virtual ~Hardgainer () {delete mDebugDispatcher;}
#endif
    
    virtual AUKernelBase *NewKernel () {return new HardgainerKernel(this);}
    
    virtual	ComponentResult GetParameterValueStrings (
                                                      AudioUnitScope			inScope,
                                                      AudioUnitParameterID	inParameterID,
                                                      CFArrayRef				*outStrings
                                                      );
    
    virtual	ComponentResult GetParameterInfo (
                                              AudioUnitScope			inScope,
                                              AudioUnitParameterID	inParameterID,
                                              AudioUnitParameterInfo	&outParameterInfo
                                              );
    
    virtual ComponentResult GetPropertyInfo (
                                             AudioUnitPropertyID		inID,
                                             AudioUnitScope			inScope,
                                             AudioUnitElement		inElement,
                                             UInt32					&outDataSize,
                                             Boolean					&outWritable
                                             );
    
    virtual ComponentResult GetProperty (
                                         AudioUnitPropertyID		inID,
                                         AudioUnitScope			inScope,
                                         AudioUnitElement		inElement,
                                         void					*outData
                                         );
    
    virtual	bool SupportsTail () {return true;}
    virtual ComponentResult	Version () {return kHardgainerVersion;}

    
    virtual ComponentResult GetPresets (
                                        CFArrayRef        *outData
                                        ) const;
    
    virtual OSStatus NewFactoryPresetSet (
                                          const AUPreset    &inNewFactoryPreset
                                          );
protected:
    class HardgainerKernel : public AUKernelBase {
    public:
        HardgainerKernel (AUEffectBase *inAudioUnit);
        
        virtual void Process (
                              const Float32    *inSourceP,
                              Float32          *inDestP,
                              UInt32           inFramesToProcess,
                              UInt32           inNumChannels,
                              bool             &ioSilence
                              );
        virtual void Reset();
        
    private:
        enum     {kWaveArraySize = 2000};
//        float    mSine [kWaveArraySize];             
//        float    mSquare [kWaveArraySize];           
        float    *waveArrayPointer;
        Float32  mSampleFrequency;                   
        long     mSamplesProcessed;                  
        enum     {sampleLimit = (int) 10E6};         
        float    mCurrentScale;                      
        float    mNextScale;                         
        
    };
};

#pragma mark ____Hardgainer Factory Preset Constants
static const float kParameter_Preset_Gain_Small = 1.0;

enum {
    kPreset_Small   = 0,
    kNumberPresets  = 1
};

static AUPreset kPresets [kNumberPresets] = {                
    {kPreset_Small, CFSTR ("Small boost")}
};

static const int kPreset_Default = kPreset_Small;

