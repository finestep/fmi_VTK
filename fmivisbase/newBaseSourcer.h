#ifndef NEWBASESOURCER_H
#define NEWBASESOURCER_H

#include <memory>
#include <functional>
#include <vector>

#include <vtkImageData.h>
#include <vtkImageAlgorithm.h>


class NFmiQueryData;
class NFmiFastQueryInfo;
struct nbsMetadata;

//lukee sqd-tiedoston ja muuntaa sen vtkImageDataksi
class newBaseSourcer : public vtkImageAlgorithm {
protected:

	nbsMetadata *meta;



	vtkImageData* im;
//	float* heights;
	std::vector<float> heights;

	int param;

	int prevTime;

	int zRes;
	float zHeight;

	int subSample;

	//muunnos epoch-ajasta newbasen aikaindekseiksi ja takaisin

	virtual void ResetImage(bool realloc = false);
	void AllocateHeights();

	bool LoopParam(int param, int time, std::function<void(int, int, int,float)> f );

	int getHeight(int x, int y, int z);

	void ReadHeights(int time);

	inline void freeRes() {
		if (im)
			im->Delete();
// 		if (heights)
// 			delete[] heights;
	}


public:
	//etsii aikaindeksin annetulle epoch-ajalle
	int nearestIndex(double time);

	newBaseSourcer(const std::string &file, nbsMetadata *meta, int param,int res=80, int subSample = 1);

	void Delete() override {
		freeRes();
	}


	double minT();
	double maxT();


	//kertoo VTK:lle mit� dataa on saatavilla
	int newBaseSourcer::RequestInformation(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);
	
	//hakee VTK:lle dataa
	virtual int newBaseSourcer::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector);

	void setSubSample(int s) {
		subSample = s;
		ResetImage(true);
		AllocateHeights();
		prevTime = -1;
	}


	inline nbsMetadata& getMeta() {
		return *meta;
	}
	inline int getParam() {
		return param;
	}
protected:
	virtual newBaseSourcer::~newBaseSourcer();
	newBaseSourcer(const newBaseSourcer &copy) = delete;
	void operator=(const newBaseSourcer &assign) = delete;

	struct nbsImpl;
	std::unique_ptr<nbsImpl> pimpl;
};

#endif /*NEWBASESOURCER_H*/