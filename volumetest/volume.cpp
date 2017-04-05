#include <math.h>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkSphere.h>
#include <vtkSampleFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkVolumeProperty.h>
#include <vtkCamera.h>
#include <vtkImageShiftScale.h>
#include <vtkImageData.h>
#include <vtkPointData.h>

#include <vtkCell.h>
#include <vtkFloatArray.h>
#include <vtkStructuredGrid.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkProbeFilter.h>
#include <vtkAlgorithmOutput.h>

#include <vtkPlanes.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkBoxWidget.h>

#include <vtkBMPReader.h>
#include <vtkPlaneSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTextureMapToPlane.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkImageShrink3D.h>

#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCubeAxesActor.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>

#include <vtkContourFilter.h>
#include <vtkPerlinNoise.h>

#include <vtkBoxWidget2.h>
#include <vtkBoxRepresentation.h>

#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>

#include <vtkPlaneWidget.h>
#include <vtkLinearSubdivisionFilter.h>

#include <vtkLight.h>

#include <list>

#include <NFmiQueryData.h>

#include "newBaseSourcerMetaData.h"
#include "newBaseSourcer.h"

static std::string *newBaseFile;
static metaData *meta;


//Callback jota kutsutaan kun laatikkoa skaalataan
class boxWidgetCallback : public vtkCommand
{
public:
	static boxWidgetCallback *New()
	{
		return new boxWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vtkBoxWidget2 *widget = reinterpret_cast<vtkBoxWidget2*>(caller);
		vtkBoxRepresentation* boxRep = reinterpret_cast<vtkBoxRepresentation*>(widget->GetRepresentation());
		
		boxRep->GetPlanes(planes);
		if (this->Mapper)
		{
			this->Mapper->SetClippingPlanes(planes);
		}
		if (this->isoSurf) {
			this->isoSurf->SetClippingPlanes(planes);
		}
	}
	void SetMapper(vtkSmartVolumeMapper* m)
	{
		this->Mapper = m;
	}
	void SetIsoSurf(vtkPolyDataMapper* i)
	{
		this->isoSurf = i;
	}
protected:
	boxWidgetCallback()
	{
		this->Mapper = nullptr;
		this->isoSurf = nullptr;
		planes = vtkPlanes::New();
	}
	vtkPlanes *planes;
	vtkSmartVolumeMapper *Mapper;
	vtkPolyDataMapper *isoSurf;
};

//callback aikasarjasliderille
class sliderWidgetCallback : public vtkCommand
{
public:
	sliderWidgetCallback() {}

	static sliderWidgetCallback *New()
	{
		return new sliderWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{
		vtkSliderWidget *sliderWidget =
			reinterpret_cast<vtkSliderWidget*>(caller);
		int value = static_cast<int>(static_cast<vtkSliderRepresentation *>(sliderWidget->GetRepresentation())->GetValue());

		for (auto i = algoList->begin(); i != algoList->end(); i++) {
			(*i)->UpdateTimeStep(value);
		}

		this->renWin->Render();
	}
	void setAlgoList(std::list<vtkAlgorithm*> *m)
	{
		this->algoList = m;
	}
	void setRenWin(vtkRenderWindow* r) {
		renWin = r;
	}

protected:
	std::list<vtkAlgorithm*> *algoList;
	vtkRenderWindow *renWin;
};

class planeWidgetCallback : public vtkCommand
{
public:
	planeWidgetCallback() {}

	static planeWidgetCallback *New()
	{
		return new planeWidgetCallback;
	}
	void Execute(vtkObject *caller, unsigned long, void*) VTK_OVERRIDE
	{

		for (auto i = algoList->begin(); i != algoList->end(); i++) {
			(*i)->Update();
		}

	}
	void setAlgoList(std::list<vtkAlgorithm*> *m)
	{
		this->algoList = m;
	}
	void setProbe(vtkProbeFilter *b) {
		probe = b;
	}
protected:
	vtkProbeFilter* probe;
	std::list<vtkAlgorithm*> *algoList;
};



int main(int argc, char *argv[])
{

	//lis�� aikasarjatuen?
	vtkStreamingDemandDrivenPipeline *sdp = vtkStreamingDemandDrivenPipeline::New();
	vtkAlgorithm::SetDefaultExecutivePrototype(sdp);
	sdp->Delete();


	//yleisi� tietoja newBasesta
	metaData meta;


	//vtk boilerplatea
	vtkSmartPointer<vtkRenderWindow> renWin =
		vtkSmartPointer<vtkRenderWindow>::New();
	vtkSmartPointer<vtkRenderer> ren1 =
		vtkSmartPointer<vtkRenderer>::New();
	ren1->SetBackground(0.1, 0.4, 0.2);

	renWin->AddRenderer(ren1);

	//renWin->SetFullScreen(true);
	renWin->SetSize(800, 800);


	vtkSmartPointer<vtkRenderWindowInteractor> iren =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();

	iren->SetInteractorStyle(style);

	renWin->Render();

	
	std::string file = std::string("D:/3D-dataa/201703210327_hirlam_skandinavia_mallipinta.sqd");
	

	//nbs-lukija #1
	newBaseSourcer *nbs = new newBaseSourcer(file, &meta, kFmiTotalCloudCover);

	nbs->initMeta();

	nbs->Update();

	//nbs->SetReleaseDataFlag(true);


	

	//mapperit ottavat datan ja translatoivat sen piirrett�v�ksi
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartPointer<vtkSmartVolumeMapper>::New();
	volumeMapper->SetBlendModeToComposite(); // composite first
	volumeMapper->SetRequestedRenderModeToGPU();
	volumeMapper->SetMaxMemoryInBytes(4e9);
	volumeMapper->SetInputConnection(nbs->GetOutputPort());


	//propertyt m��r��v�t kuinka actor piirret��n
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->ShadeOff();

	volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

	volumeMapper->SetRequestedRenderModeToGPU();

	//volumeMapper->SetReleaseDataFlag(true);

	volumeMapper->UpdateTimeStep(nbs->minDT());

	vtkSmartPointer<vtkPiecewiseFunction> compositeOpacity =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	compositeOpacity->AddPoint(0, 0.0);
	compositeOpacity->AddPoint(2, 0.05);
	compositeOpacity->AddPoint(100, 0.3);
	compositeOpacity->AddPoint(105, 0.9);
	volumeProperty->SetScalarOpacity(compositeOpacity); // composite first.


	vtkSmartPointer<vtkColorTransferFunction> color =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	color->AddRGBPoint(0.0, 0.0, 0.2, 0.0);
	color->AddRGBPoint(50, 0.0, 1, 0.0);
	color->AddRGBPoint(100, 1.0, 0, 0.0);
	color->ClampingOff();
	color->UseAboveRangeColorOn();
	color->SetAboveRangeColor(0, 0, 1);
	volumeProperty->SetColor(color);


	//actor hallitsee itse visualisaation piirt�misen
	vtkSmartPointer<vtkVolume> volume =
		vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	ren1->AddViewProp(volume);

	//toinen nbs-lukija
	newBaseSourcer *nbsWind = new newBaseSourcer(file, &meta, kFmiWindSpeedMS);

	nbsWind->Update();

	//t�ll� voisi subsamplata dataa, muttei vaikuta toimivan
	/*
	vtkSmartPointer<vtkImageShrink3D> shrinkWind =
		vtkSmartPointer<vtkImageShrink3D>::New();

	shrinkWind->SetInputConnection(nbsWind->GetOutputPort());

	shrinkWind->SetShrinkFactors(4,4,4);
	shrinkWind->Update();
	*/
	//isopinta-filtteri
	vtkSmartPointer<vtkContourFilter> mc =
		vtkSmartPointer<vtkContourFilter>::New();

	mc->SetInputConnection(nbsWind->GetOutputPort());

	//mc->SetReleaseDataFlag(true);

	mc->ComputeNormalsOn();
	mc->ComputeScalarsOff();
	mc->SetValue(0, 35);
	mc->UpdateTimeStep(nbsWind->minDT());

	vtkSmartPointer<vtkPolyDataMapper> windMap =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	//windMap->SetInputData(mc->GetOutput());
	windMap->SetInputConnection(mc->GetOutputPort());
	
	windMap->ScalarVisibilityOff();
	
	windMap->UpdateTimeStep(nbsWind->minDT());

	vtkSmartPointer<vtkActor> windAct =
		vtkSmartPointer<vtkActor>::New();
	windAct->SetMapper(windMap);
	windAct->GetProperty()->SetColor(0, 0.8, 0.8);
	windAct->GetProperty()->SetOpacity(0.9);
	
	windAct->SetPosition(0, 0, 0);

	ren1->AddActor(windAct);

	//koordinaattiakselit
	vtkSmartPointer<vtkCubeAxesActor> cubeAxesActor =
		vtkSmartPointer<vtkCubeAxesActor>::New();
	cubeAxesActor->SetBounds(nbs->GetOutput()->GetBounds());
	cubeAxesActor->SetCamera(ren1->GetActiveCamera());

	cubeAxesActor->GetTitleTextProperty(0)->SetColor(1.0, 0.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(0)->SetColor(1.0, 0.0, 0.0);

	cubeAxesActor->SetXAxisRange(meta.p1.Y(), meta.p2.Y());
	cubeAxesActor->SetXTitle("longtitude");

	cubeAxesActor->GetTitleTextProperty(1)->SetColor(0.0, 1.0, 0.0);
	cubeAxesActor->GetLabelTextProperty(1)->SetColor(0.0, 1.0, 0.0);

	cubeAxesActor->SetYAxisRange(meta.p1.X(), meta.p2.X());
	cubeAxesActor->SetYTitle("latitude");

	cubeAxesActor->GetTitleTextProperty(2)->SetColor(0.0, 0.0, 1.0);
	cubeAxesActor->GetLabelTextProperty(2)->SetColor(0.0, 0.0, 1.0);

	cubeAxesActor->SetZAxisRange(meta.minH, meta.maxH);
	cubeAxesActor->SetZTitle("m");

	cubeAxesActor->SetDragable(false);
	cubeAxesActor->SetPickable(false);

	ren1->AddActor(cubeAxesActor);
	

	//luetaan kartta levylt� ja mapataan se tasolle

	vtkSmartPointer<vtkBMPReader> mapReader =
		vtkSmartPointer<vtkBMPReader>::New();
	mapReader->SetFileName("D:/3d-dataa/Scandinavia.SHADED_TOPO.COLORED_HEIGHT1.bmp");

	vtkSmartPointer<vtkTexture> texture =
		vtkSmartPointer<vtkTexture>::New();
	texture->SetInputConnection(mapReader->GetOutputPort());

	double* volumeBounds = new double[6];
	volumeBounds = volume->GetBounds();
	double* planeScale = new double[2];

	planeScale[0] = volumeBounds[1] - volumeBounds[0];
	planeScale[1] = volumeBounds[2] - volumeBounds[3];


	vtkSmartPointer<vtkPlaneSource> planeSource =
		vtkSmartPointer<vtkPlaneSource>::New();

	planeSource->SetOrigin(volumeBounds[0], volumeBounds[2], volumeBounds[4]);
	planeSource->SetPoint1(planeScale[0], 0, 0);
	planeSource->SetPoint2(0, -planeScale[1], 0);

	planeSource->Update();

	vtkSmartPointer<vtkTextureMapToPlane> texturePlane =
		vtkSmartPointer<vtkTextureMapToPlane>::New();
	texturePlane->SetInputConnection(planeSource->GetOutputPort());

	vtkSmartPointer<vtkPolyDataMapper> planeMapper =
		vtkSmartPointer<vtkPolyDataMapper>::New();

	planeMapper->SetInputData(planeSource->GetOutput());

	//double* planePos = new double[2];
	//planePos[0] = planeBounds[1] / 2.0;
	//planePos[1] = planeBounds[3] / 2.0;


	vtkSmartPointer<vtkActor> texturedPlane =
		vtkSmartPointer<vtkActor>::New();
	texturedPlane->SetMapper(planeMapper);
	texturedPlane->SetTexture(texture);

	texturedPlane->SetMapper(planeMapper);
	texturedPlane->SetDragable(false);
	texturedPlane->SetPickable(false);

	ren1->AddActor(texturedPlane);

	vtkSmartPointer<vtkBoxRepresentation> boxRep =
		vtkSmartPointer<vtkBoxRepresentation>::New();
	boxRep->SetPlaceFactor(1.1);
	boxRep->PlaceWidget(volume->GetBounds() );
	boxRep->InsideOutOn();
	boxRep->GetSelectedFaceProperty()->SetOpacity(0.0);

	vtkSmartPointer<vtkBoxWidget2> boxWidget =
		vtkSmartPointer<vtkBoxWidget2>::New();
	boxWidget->SetInteractor(iren);
	boxWidget->SetRepresentation(boxRep);

	boxWidget->SetTranslationEnabled(false);
	boxWidget->SetRotationEnabled(false);

	boxWidget->EnabledOn();

	boxWidgetCallback *callback = boxWidgetCallback::New();
	callback->SetMapper( volumeMapper );
	callback->SetIsoSurf(windMap);
	boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
	callback->Delete();




	vtkSmartPointer<vtkPlaneWidget> planeWidget =
		vtkSmartPointer<vtkPlaneWidget>::New();
	planeWidget->SetInteractor(iren);
	planeWidget->SetResolution(80);
	planeWidget->SetRepresentationToOutline();
	planeWidget->SetNormalToZAxis(true);
	planeWidget->SetProp3D(texturedPlane);
	planeWidget->GetHandleProperty()->SetPointSize(0.3);
	planeWidget->GetSelectedHandleProperty()->SetPointSize(0.2);

	vtkSmartPointer<vtkProbeFilter> probe =
		vtkSmartPointer<vtkProbeFilter>::New();


	newBaseSourcer *nbsTemp = new newBaseSourcer(file, &meta, kFmiTemperature);
	nbsTemp->Update();

	probe->SetInputConnection(planeWidget->GetPolyDataAlgorithm()->GetOutputPort());
	probe->SetSourceConnection(nbsTemp->GetOutputPort());

	probe->Update();

	vtkSmartPointer<vtkContourFilter> probeContours =
		vtkSmartPointer<vtkContourFilter>::New();
	probeContours->SetInputConnection(probe->GetOutputPort());
	probeContours->GenerateValues(8, -40, 40);

	probeContours->Update();

	vtkSmartPointer<vtkColorTransferFunction> contourColor =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	contourColor->AddRGBPoint(-40, 0, 0, 1);
	contourColor->AddRGBPoint(0, 0.6, 0, 0.6);
	contourColor->AddRGBPoint(40, 1, 0, 0);

	vtkSmartPointer<vtkPolyDataMapper> probeMap =
		vtkSmartPointer<vtkPolyDataMapper>::New();
	probeMap->SetInputConnection(probeContours->GetOutputPort());
	probeMap->SetScalarRange(-40, 40);
	probeMap->SetLookupTable(contourColor);

	probeMap->Update();

	vtkSmartPointer<vtkActor> probeAct =
		vtkSmartPointer<vtkActor>::New();
	probeAct->SetMapper(probeMap);



	ren1->AddActor(probeAct);




	planeWidgetCallback *planeCallback = planeWidgetCallback::New();

	std::list<vtkAlgorithm*> pwidgetList;
	//pwidgetList.push_back(nbsTemp);
	pwidgetList.push_back(probe);
	pwidgetList.push_back(probeContours);
	pwidgetList.push_back(probeMap);

	planeCallback->setAlgoList(&pwidgetList);

	planeCallback->setProbe(probe);
	planeWidget->PlaceWidget();


	planeWidget->AddObserver(vtkCommand::InteractionEvent, planeCallback);

	planeWidget->EnabledOn();


	vtkSliderRepresentation2D * sliderRep = vtkSliderRepresentation2D::New();

	sliderRep->SetTitleText("t");
	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.08);
	sliderRep->GetPoint2Coordinate()->SetValue(0.9, 0.08);



	sliderRep->SetMinimumValue(nbs->minDT());
	sliderRep->SetMaximumValue(nbs->maxDT());
	sliderRep->SetValue(nbs->minDT());



	vtkSliderWidget *slider = vtkSliderWidget::New();
	slider->SetInteractor(iren);
	slider->SetRepresentation(sliderRep);
	slider->SetAnimationModeToAnimate();
	slider->SetNumberOfAnimationSteps(10);

	sliderWidgetCallback *sliderCallback = sliderWidgetCallback::New();

	//lista filttereit� jotka callback p�ivitt��
	std::list<vtkAlgorithm*> widgetList;
	widgetList.push_back(nbs);
	widgetList.push_back(nbsWind);
	widgetList.push_back(mc);
	widgetList.push_back(nbsTemp);
	widgetList.push_back(probe);
	widgetList.push_back(probeContours);


	sliderCallback->setAlgoList(&widgetList);
	sliderCallback->setRenWin(renWin);

	slider->AddObserver(vtkCommand::InteractionEvent, sliderCallback);
	slider->EnabledOn();



	vtkSmartPointer<vtkLight> overheadLight =
		vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0, volumeBounds[5]*1.1);
	overheadLight->SetExponent(0.9);

	ren1->AddLight(overheadLight);
	
	
	volumeMapper->UpdateTimeStep( nbs->minDT() );
	windMap->UpdateTimeStep( nbsWind->minDT() );
	windMap->SetGlobalImmediateModeRendering(true);
	ren1->Render();
	ren1->ResetCamera();

	iren->Initialize();
	iren->Start();


	return EXIT_SUCCESS;
}