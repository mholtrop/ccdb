/*
 * Variation.cpp
 *
 *  Created on: Sep 28, 2010
 *      Author: romanov
 */

#include "CCDB/Model/Variation.h"

namespace ccdb {

	Variation::Variation()
	{
		mId=0;			//! database table uniq id;
		mCreatedTime=0;	//! Creation Time
		mUpdateTime=0;		//! Update Time
		mParentDbId = 0;
	}
}
