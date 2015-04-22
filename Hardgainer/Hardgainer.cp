/*
 *  Hardgainer.cp
 *  Hardgainer
 *
 *  Created by Farhan Mannan on 23/04/2015.
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
