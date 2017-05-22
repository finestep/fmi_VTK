#ifndef PARAMVISUALIZERSURF_H
#define PARAMVISUALIZERSURF_H

#include <memory>
#include <vtkSmartPointer.h>

#include "ParamVisualizerBase.h"

class vtkPolyDataMapper;

class vtkContourFilter;
class vtkColorTransferFunction;
class vtkStripper;
class vtkActor;
class vtkPolyData;
class ContourLabeler;

class ParamVisualizerSurf : public ParamVisualizerBase {
protected:

	vtkContourFilter *contour;

	vtkStripper* stripper;

	ContourLabeler &labeler;

	vtkPolyDataMapper *polyMap;

	vtkActor *polyAct;

	//false = color
	bool mode;

	void ModeIsoLine();
	void ModeColorContour();


public:
	ParamVisualizerSurf(const std::string &file, nbsMetadata &m, int param,
		vtkSmartPointer<vtkColorTransferFunction> contourColors, ContourLabeler &labeler, double range[2], int numContours);
	~ParamVisualizerSurf();

	virtual void UpdateTimeStep(double t) override;



	virtual inline void ToggleMode() {

		if (mode) {

			ModeColorContour();
		}
		else {

			ModeIsoLine();
		}

		mode = !mode;
	}
};

#endif /*PARAMVISUALIZERSURF_H*/