 #include "ParamVisualizerPolyText.h"

#include <vtkCellData.h>
#include <vtkVectorText.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkActor.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkFloatArray.h>

#include <NFmiDataIdent.h>

#include "nbsSurface.h"

namespace fmiVis {
	ParamVisualizerPolyText::ParamVisualizerPolyText(const std::string &file, nbsMetadata &m, NFmiDataIdent &paramIdent, NFmiDrawParamFactory* fac) :
		ParamVisualizerBase(new nbsSurface(file, &m, paramIdent.GetParamIdent(), 13000, true), m, paramIdent, fac), //13000 is magic for wind vectors, hopefully not in conflict with newbase
		vt(vtkSmartPointer<vtkVectorText>::New()),
		tf(vtkSmartPointer<vtkTransformPolyDataFilter>::New()),
		ap(vtkSmartPointer<vtkAppendPolyData>::New())

	{
		nbs->Update();

		map = vtkSmartPointer<vtkPolyDataMapper>::New();

		map->AddInputConnection(ap->GetOutputPort());

		map->SetScalarModeToUseCellData();
		map->SetColorModeToMapScalars();



		act = vtkSmartPointer<vtkActor>::New();
		act->SetMapper(map);


		SetActiveMapper(act->GetMapper());

		SetProp(act);

	}


	void ParamVisualizerPolyText::UpdateTimeStep(double t)
	{

		int sizeX = meta.sizeX;
		int sizeY = meta.sizeY;

		UpdateNBS(t);

		vtkPolyData * input = vtkPolyData::SafeDownCast(nbs->GetOutputDataObject(0));


		auto inputSize = input->GetNumberOfPoints();

		auto inputScalars = input->GetPointData()->GetScalars();

		auto pos = vtkSmartPointer<vtkTransform>::New();

		ap->RemoveAllInputs();

		int ix = 0, iy = 0;
		int step = 3;

		while (iy < sizeY) {
			ix = 0;
			while (ix < sizeX) {

				int i = iy + sizeY * ix;

				auto s = std::ostringstream{};

				auto val = inputScalars->GetTuple1(i);

				s << val;

				vt->SetText(s.str().c_str());
				vt->Update();

				double v[3];

				input->GetPoint(i, v);

				pos->Identity();


				pos->Translate(v);
				pos->Translate(0, 0, 2);
				pos->Scale(step*0.75, step*0.75, 1);

				tf->SetTransform(pos);


				tf->SetInputData(vt->GetOutput());
				tf->Update();

				auto data = vtkSmartPointer<vtkPolyData>::New();

				auto scalar = vtkSmartPointer<vtkFloatArray>::New();

				scalar->SetNumberOfComponents(1);

				for (int i = 0; i < tf->GetOutput()->GetNumberOfCells(); i++)
					scalar->InsertNextTuple1(val / 100);

				data->ShallowCopy(tf->GetOutput());
				data->GetCellData()->SetScalars(scalar);

				ap->AddInputData(data);
				ix += step;
			}
			iy += step;
		}

		ap->Update();

		map->Update();
	}

}