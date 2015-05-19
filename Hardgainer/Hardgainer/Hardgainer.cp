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
    
    
#if AU_DEBUG_DISPATCHER
    mDebugDispatcher = new AUDebugDispatcher (this);
#endif
}

#pragma mark ____HardgainerEffectKernel

Hardgainer::HardgainerKernel::HardgainerKernel (AUEffectBase *inAudioUnit ) : AUKernelBase (inAudioUnit)
{ };