#include "nbsSurface.h"

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkDelaunay2D.h>
#include <vtkFloatArray.h>

#include <future>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkDataArray.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "newBaseSourcer.h"
#include "nbsSurfaceImpl.h"

#include "nbsMetadata.h"




nbsSurface::nbsSurface(const std::string &file, nbsMetadata *meta, int param,int zHeight,bool flat) :
	pimpl(std::make_unique <nbsImpl>(file)), param(param), meta(meta), zHeight(zHeight), prevTime(-1), flat (flat)
{

	SetNumberOfInputPorts(0);
	inputPolyData = vtkPolyData::New();

	points = vtkSmartPointer<vtkPoints>::New();

	textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
	textureCoordinates->SetNumberOfComponents(3);
	textureCoordinates->SetName("TextureCoordinates");

	scalars = vtkSmartPointer<vtkFloatArray>::New();

	scalars->SetNumberOfComponents(1);
	scalars->SetName("Scalars");

	loadPoints();
}

nbsSurface::~nbsSurface() {
	inputPolyData->Delete();

}


int nbsSurface::RequestInformation(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	vtkInformation* outInfo = outputVector->GetInformationObject(0);


	double doubleRange[2];
	doubleRange[0] = meta->times.front();
	doubleRange[1] = meta->times.back();


	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_RANGE(), doubleRange, 2);
	outInfo->Set(vtkStreamingDemandDrivenPipeline::TIME_STEPS(), &meta->times[0], meta->times.size());

	outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), 0, meta->sizeX - 1, 0, meta->sizeY - 1, 0, 1);
	return 1;
}


int nbsSurface::nearestIndex(double time)
{
	for (auto iter = meta->timeIndex.begin(); iter != meta->timeIndex.end(); iter++) {
		if (iter->first == time) return iter->second;
		auto nextIter = iter;
		nextIter++;
		if (nextIter != meta->timeIndex.end() && nextIter->first > time) return iter->second;
	}
	if (meta->timeIndex.rbegin() != meta->timeIndex.rend()) return meta->timeIndex.rbegin()->second;

	return 0;
}

bool nbsSurface::loadPoints() {



	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = 1;

	NFmiFastQueryInfo &dataInfo = pimpl->dataInfo;


	points->Resize(sizeX*sizeY);

	if (flat) {
		for (int x = 0; x < sizeX; ++x)
			for (int y = 0; y < sizeY; ++y)
			{
				points->InsertNextPoint(x * 2, y * 2, 0.1); //spacing

				float tuple[3] = { float(x) / sizeX,float(y) / sizeY, 0.0 };
				textureCoordinates->InsertNextTuple(tuple);
			}
	}
	else {

		if (!dataInfo.Param(kFmiGeopHeight)) {
			cout << "kFmiGeopHeight not found!" << endl;

			if (!dataInfo.Param(kFmiGeomHeight)) {
				cout << "kFmiGeomHeight not found!!" << endl;
				return false;
			}
		}


		static std::vector<float> values;

		dataInfo.GetLevelToVec(values);

		for (int x = 0; x < sizeX; ++x)
			for (int y = 0; y < sizeY; ++y)
			{
				points->InsertNextPoint(x * 2, y * 2, values[x + y*sizeX] * 2 / zHeight * 80); //VisualizerManager zHeight ja newBaseSourcer zRes, spacing

				float tuple[3] = { float(x) / sizeX,float(y) / sizeY, 0.0 };
				textureCoordinates->InsertNextTuple(tuple);
			}
	}

	cout << "Triangulating... ";

	inputPolyData->SetPoints(points);

	static vtkSmartPointer<vtkDelaunay2D> delaunay = nullptr;
	if (!delaunay) {

		delaunay = vtkSmartPointer<vtkDelaunay2D>::New();

		// Triangulate the grid points
		delaunay->SetInputData(inputPolyData);

		//accuracy vs speed
		delaunay->SetTolerance(0.005);

		delaunay->Update();
	}

	// Add the grid points to a polydata object
	inputPolyData->ShallowCopy(delaunay->GetOutput());

	inputPolyData->GetPointData()->SetTCoords(textureCoordinates);

	return true;
}



int nbsSurface::RequestData(vtkInformation* vtkNotUsed(request),
	vtkInformationVector** vtkNotUsed(inputVector),
	vtkInformationVector* outputVector) {


	int sizeX = meta->sizeX;
	int sizeY = meta->sizeY;
	int sizeZ = 1;


	unsigned long time = 0;

	vtkInformation* outInfo = outputVector->GetInformationObject(0);

	//pyydet��nk� tietty� ajanjaksoa
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP())) {
		time = meta->timeStepToEpoch(outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()));
		cout << param << ": Request for time " << time << std::endl;
	}

	//pyydet��nk� jotain aluetta (n�ytt�isi olevan aina koko alue)
	int* reqExtent = nullptr;
	if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT())) {
		reqExtent = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());
		cout << param << ": Requested extent " << reqExtent[0] << "-" << reqExtent[1] << ", "
			<< reqExtent[2] << "-" << reqExtent[3] << ", " << reqExtent[4] << "-" << reqExtent[5] << std::endl;
	}

	vtkDataObject *ds = outInfo->Get(vtkDataSet::DATA_OBJECT());

	int timeI = nearestIndex(time);

	NFmiFastQueryInfo &dataInfo = pimpl->dataInfo;

	//onko aika-askel jo muistissa
	if (timeI != prevTime) {

		dataInfo.ResetTime();

		float minVal = kMaxFloat, maxVal = kMinFloat;

		float maxHeight = -1;


		dataInfo.TimeIndex(timeI);
		auto t0 = std::chrono::system_clock::now();


		static std::vector<float> values;

		scalars->Reset();


		if (dataInfo.Param(FmiParameterName(param))) {

			if (dataInfo.HeightParamIsRising()) dataInfo.FirstLevel();
			else dataInfo.LastLevel();

			dataInfo.GetLevelToVec(values);

			for (int x = 0; x < sizeX; ++x)
				for (int y = 0; y < sizeY; ++y)
				{

					float val = values[x + y*sizeX];

					if (val == kFloatMissing) {
						val = 0;
					}

					scalars->InsertNextTuple1(val);

				}
		}



		inputPolyData->GetPointData()->SetScalars(scalars);
		inputPolyData->GetPointData()->SetActiveAttribute("Scalars", 0);

		cout << "Done" << endl;

		prevTime = timeI;
	}
	else cout << "Reused time " << prevTime << std::endl;

	//siirret��n imagedata ulostuloon
	ds->ShallowCopy( inputPolyData );

	//kerrotaan mit� dataa l�ytyi
	ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());

	Modified();


	return 1;
}