/*
 * DualSvmModel.cpp
 *
 *  Created on: 12.07.2012
 *      Author: poschmann
 */

#include "tracking/DualSvmModel.h"

#include "VDetectorVectorMachine.h"
#include "FdImage.h"
#include "FdPatch.h"
#include "tracking/Sample.h"

namespace tracking {

DualSvmModel::DualSvmModel(shared_ptr<VDetectorVectorMachine> preStage, shared_ptr<VDetectorVectorMachine> mainStage)
		: preStage(preStage), mainStage(mainStage) {}

DualSvmModel::~DualSvmModel() {}

void DualSvmModel::evaluate(FdImage* image, std::vector<Sample>& samples) {
	preStage->initPyramids(image);
	preStage->initROI(image);
	mainStage->initPyramids(image);
	mainStage->initROI(image);
	for (std::vector<Sample>::iterator sit = samples.begin(); sit < samples.end(); ++sit) {
		Sample& sample = *sit;
		sample.setWeight(0);
		sample.setObject(false);
		FdPatch* patch = preStage->extractPatchToPyramid(image, sample.getX(), sample.getY(), sample.getSize());
		if (patch != 0 && preStage->detectOnPatch(patch)) {
			patch = mainStage->extractPatchToPyramid(image, sample.getX(), sample.getY(), sample.getSize());
			if (patch != 0) {
				if (mainStage->detectOnPatch(patch))
					sample.setObject(true);
				sample.setWeight(patch->certainty[mainStage->getIdentifier()]);
			}
		}
	}
}

} /* namespace tracking */
