
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkWin32OutputWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <VisualizerFactory.h>
#include <VisualizerManager.h>
#include <fmiVisInteractor.h>
#include <vtkPNGReader.h>
#include <vtkTexture.h>
#include <nbsSurface.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkActor.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkBoxRepresentation.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <TimeAnimator.h>
#include <vtkLight.h>



#include "ParamVisualizerWindVec2D.h"

int main(size_t argc, char* argv[])
{
	cout << "Initializing VTK..." << endl;

	vtkObject::GlobalWarningDisplayOff(); 

	//lis�� aikasarjatuen?
	auto *sdp = vtkStreamingDemandDrivenPipeline::New();
	vtkAlgorithm::SetDefaultExecutivePrototype(sdp);
	sdp->Delete();

	//vtk boilerplatea
	auto renWin = vtkSmartPointer<vtkRenderWindow>::New();
	auto ren1 = vtkSmartPointer<vtkRenderer>::New();

	//taustav�ri
	ren1->SetBackground(0.4, 0.4, 0.8);

	renWin->AddRenderer(ren1);

	//renWin->SetFullScreen(true);
	renWin->SetSize(800, 800);

	vtkWin32OutputWindow::SafeDownCast(vtkOutputWindow::GetInstance())->SendToStdErrOn();

	auto iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	ren1->GetActiveCamera()->ParallelProjectionOn();

	renWin->Render();


	cout << "Initializing interface..." << endl;

	VisualizerFactory::init();

	std::string file = std::string(argc < 2 ?
		"D:/3D-dataa/201706160543_gfs_scandinavia_surface.sqd" :
		argv[1]
	);

	VisualizerManager vm{ ren1 };

	vm.GetMeta().init(file);


	auto meta = vm.GetMeta();

	auto style = vtkSmartPointer<fmiVisInteractor2D>::New();

	style->setVM(&vm);

	iren->SetInteractorStyle(style);

	//luetaan kartta levylt� ja mapataan se tasolle

	cout << "Loading map..." << endl;

	auto mapReader = vtkSmartPointer<vtkPNGReader>::New();
	mapReader->SetFileName("bottom_indexed.png");

	mapReader->Update();

	auto texture = vtkSmartPointer<vtkTexture>::New();
	texture->SetInputData(mapReader->GetOutput());
	texture->SetInterpolate(true);
	texture->Update();

	double* volumeBounds = new double[6];
	double* planeScale = new double[2];

	planeScale[0] = volumeBounds[1] - volumeBounds[0];
	planeScale[1] = volumeBounds[2] - volumeBounds[3];


	auto mapNbs = new nbsSurface(file, &meta, 1,13000,true);

	mapNbs->Update();


	auto mapMap = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapMap->SetInputData(mapNbs->GetOutput());
	mapMap->SetScalarVisibility(false);
	mapMap->Update();

	auto texturedPlane = vtkSmartPointer<vtkActor>::New();
	texturedPlane->SetMapper(mapMap);
	texturedPlane->SetTexture(texture);
	texturedPlane->SetPickable(false);
	texturedPlane->SetDragable(false);

	ren1->AddActor(texturedPlane);

	cout << "Initializing visualizers..." << endl;

	std::map<int, std::string> paramsSurf = {
		std::pair<int,std::string>(kFmiPressure,"kFmiPressure"),
		std::pair<int,std::string>(kFmiTemperature,"kFmiTemperature"),
		std::pair<int,std::string>(kFmiHumidity,"kFmiHumidity"),
		std::pair<int,std::string>(kFmiWindSpeedMS,"kFmiWindSpeedMS"),
		std::pair<int,std::string>(kFmiTotalCloudCover,"kFmiTotalCloudCover"),
		std::pair<int,std::string>(kFmiPrecipitation1h,"kFmiPrecipitation1h"),
	};
	std::map<int, visID> paramVID;

	std::vector<vtkSmartPointer<vtkTextActor> > textActs;

	auto data = NFmiQueryData(file);
	auto dataInfo = NFmiFastQueryInfo(&data);

	int *winSize = ren1->GetSize();



	auto t = vtkSmartPointer<vtkTextActor>::New();
	auto s = std::ostringstream{};

	auto textSize = 14;
	auto textVOff = winSize[1] - 50;
	auto textVSpacing = -16;


	int i = 0;
	for (i = file.length() - 1; i >= 0; --i)
		if (file[i] == '/' || file[i] == '\\') break;

	s << file.substr(i + 1);

	t->SetInput(s.str().c_str());

	t->GetTextProperty()->SetColor(0.1, 0.1, 0.1);


	t->GetTextProperty()->SetFontSize(20);

	//t->SetTextScaleModeToViewport();
	t->SetDisplayPosition(10, winSize[1] - 25);

	ren1->AddActor2D(t);


	visID vid = vm.AddVisualizer(std::make_unique<ParamVisualizerWindVec2D>(file, meta) );
	paramVID.insert(std::pair<int, visID>(ParamVisualizerWindVec2D::PARAM_WINDVEC, vid));


	t = vtkSmartPointer<vtkTextActor>::New();
	s = std::ostringstream{};

	s << int(vid + 1) << ": " << "Wind vectors";
	t->SetInput(s.str().c_str());
	textActs.push_back(t);

	t->GetTextProperty()->SetColor(0.8, 0.8, 0.8);


	t->GetTextProperty()->SetFontSize(textSize);

	//t->SetTextScaleModeToViewport();
	t->SetDisplayPosition(10, textVOff + textVSpacing * vid);

	ren1->AddActor2D(t);


	for (auto &parampair : paramsSurf) {
		if (dataInfo.Param(FmiParameterName(parampair.first))) {
			vid = vm.AddVisualizer(VisualizerFactory::makeSurfVisualizer(file, meta, vm.GetLabeler(), parampair.first,true));
			if (vid == -1)
			{
				cout << "Failed to construct visualizer for " << parampair.second << endl;
				continue;
			}
			paramVID.insert(std::pair<int, visID>(parampair.first, vid));
			vm.SetCrop(vid, false);


			t = vtkSmartPointer<vtkTextActor>::New();
			s = std::ostringstream{};
			s << int(vid + 1) << ": " << parampair.second << " ( Surface )";
			t->SetInput(s.str().c_str());
			textActs.push_back(t);
			t->GetTextProperty()->SetColor(0.2, 0.2, 0.2);
			t->GetTextProperty()->SetFontSize(textSize);


			//t->SetTextScaleModeToViewport();
			t->SetDisplayPosition(10, textVOff + textVSpacing * vid);


			ren1->AddActor2D(t);
		}
	}

	style->setVisTexts(&textActs);

	vm.EnableVis(0);


	auto sliderRep = vtkSliderRepresentation2D::New();

	sliderRep->GetPoint1Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint2Coordinate()->SetCoordinateSystemToNormalizedDisplay();
	sliderRep->GetPoint1Coordinate()->SetValue(0.1, 0.08);
	sliderRep->GetPoint2Coordinate()->SetValue(0.9, 0.08);

	sliderRep->SetShowSliderLabel(false);

	sliderRep->SetMinimumValue(0);
	sliderRep->SetMaximumValue(meta.timeSteps);
	sliderRep->SetValue(0);


	sliderRep->GetTitleProperty()->SetColor(0.2, 0.2, 0.9);
	sliderRep->GetTitleProperty()->SetFontSize(16);
	sliderRep->GetTitleProperty()->BoldOff();
	sliderRep->GetTitleProperty()->ShadowOff();

	sliderRep->SetTitleText(nbsMetadata::getTimeString(meta.minT).c_str());

	auto slider = vtkSliderWidget::New();
	slider->SetInteractor(iren);
	slider->SetRepresentation(sliderRep);
	slider->SetAnimationModeToJump();

	slider->EnabledOn();

	auto ta = TimeAnimator{ renWin,slider,&vm,&meta };

	style->setTA(&ta);

	auto overheadLight = vtkSmartPointer<vtkLight>::New();

	overheadLight->SetPosition(0, 0,4);
	overheadLight->SetExponent(1.2);
	overheadLight->SetIntensity(0.6);

	ren1->AddLight(overheadLight);

	ren1->Render();
	ren1->ResetCamera();


	iren->Initialize();
	iren->Start();


	return EXIT_SUCCESS;
}

