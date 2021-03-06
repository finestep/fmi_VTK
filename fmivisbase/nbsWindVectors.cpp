#include "nbsWindVectors.h"

#include <future>

#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>

#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>

#include <NFmiQueryData.h>
#include <NFmiFastQueryInfo.h>

#include "newBaseSourcer.h"
#include "nbsImpl.h"

#include "nbsMetadata.h"

namespace fmiVis {

	nbsWindVectors::nbsWindVectors(const std::string &file, nbsMetadata *meta, int res, int subSample) :
		newBaseSourcer(file, meta, 30000, res, subSample, 3)
	{

		ReadHeights(1);
	}

	nbsWindVectors::~nbsWindVectors() {}


	int nbsWindVectors::RequestData(vtkInformation* vtkNotUsed(request),
		vtkInformationVector** vtkNotUsed(inputVector),
		vtkInformationVector* outputVector) {


		int sizeX = meta->sizeX / subSample;
		int sizeY = meta->sizeY / subSample;
		int sizeZ = zRes / subSample;

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

			ResetImage();

			dataInfo.ResetTime();

			float minVal = kMaxFloat, maxVal = kMinFloat;


			auto t0 = std::chrono::system_clock::now();




			LoopParam(kFmiWindSpeedMS, timeI,
				[=](int x, int y, int z, float val) {

				if (meta->hasHeight) {
					z = getHeight(x, y, z);
				}

				if (z < sizeZ) {



					float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));


					if (val != kFloatMissing) pixel[2] = val;
				}

			});

			LoopParam(kFmiWindDirection, timeI,
				[&](int x, int y, int z, float val) {


				if (meta->hasHeight) {
					z = getHeight(x, y, z);
				}

				//our coordinates are sideways
				val += 90;



				if (z < sizeZ && val != kFloatMissing) {

					NFmiAngle angle(dataInfo.Area()->TrueNorthAzimuth(dataInfo.LatLon()));
					val += angle.Value();

					if (val > 360.0f) val -= 360.0f;
					if (val < 0.0f) val += 360.0f;


					float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));

					//VTK uses radians so might as well convert it here
					const float degToRad = 3.14159265358979f / 180.0f;

					float speed = pixel[2];
					if (speed == kFloatMissing) speed = 1;

					if (pixel[0] == kFloatMissing) pixel[0] = 0;
					if (pixel[1] == kFloatMissing) pixel[1] = 0;

					pixel[0] += speed * std::cos(val*degToRad);
					pixel[1] += speed * std::sin(val*degToRad);
					//TODO vertical windspeed
					pixel[2] = 0;

				}
			});

			/*

			for (int ix = 0; ix < sizeX; ++ix)
				for (int iy = 0; iy < sizeY; ++iy)
					for (int iz = 0; iz < sizeZ; ++iz) {

						float * pixel = static_cast<float*>(im->GetScalarPointer(ix, iy, iz));
						if (pixel[2] != kFloatMissing)
							pixel[2] = 0;
					}


			LoopParam(kFmiVerticalVelocityMMS,timeI,
				[&](int x, int y, int z) {

				if (meta->hasHeight) {
					z = getHeight(x, y, z);
				}

				if (z < sizeZ) {


					float val = dataInfo.FloatValue();


					float* pixel = static_cast<float*>(im->GetScalarPointer(x, y, z));

					if(val != kFloatMissing && pixel[2] != kFloatMissing)
						pixel[2] += val;
				}



			});
			*/

			auto t1 = std::chrono::system_clock::now();

			// 		for (int i = 0; i <sizeZ; ++i) {
			// 			float *x = static_cast<float*>(im->GetScalarPointer(4, 4, i));
			// 			cout << "z: " << i << " : " << x[0] << ',' << x[1] << ',' << x[2] << endl;
			// 
			// 		}
			//		cout << "interpolating..." << endl;

			unsigned int usedThreadCount = boost::thread::hardware_concurrency();
			auto threads = std::list<std::future<void>>();

			int blockWidth = 3;

			for (int i = 0; i < blockWidth*blockWidth; ++i) {
				int blockX = i % blockWidth;
				int blockY = i / blockWidth;

				int startX = blockX * (sizeX / blockWidth);
				int endX;

				if (blockX < blockWidth - 1) {

					endX = startX + sizeX / blockWidth;
				}
				else {
					endX = sizeX;
				}

				int startY = blockY * (sizeY / blockWidth);
				int endY;

				if (blockY < blockWidth - 1) {
					endY = startY + sizeY / blockWidth;
				}
				else {
					endY = sizeY;
				}

				if (threads.size() >= usedThreadCount) {
					auto iter = threads.begin();
					while (iter != threads.end())
					{

						if (iter->valid() && iter->wait_for(std::chrono::milliseconds(2)) == std::future_status::ready) {
							iter = threads.erase(iter);
						}
						else ++iter;
					}
				}
				threads.push_back(std::async(std::launch::async, [=]
				{

					InterpolateImage(startX, startY, endX, endY);

				}));
			}
			// 		for (int i = 0; i < sizeZ; ++i) {
			// 			float *x = static_cast<float*>(im->GetScalarPointer(6, 6, i));
			// 			cout << "z: " << i << " : " << x[0] <<','<< x[1]<<','<< x[2] << endl;
			// 		}

			auto t2 = std::chrono::system_clock::now();

			auto readTime = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
			auto interpTime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
			cout << "time to read: " << readTime << " time to interp: " << interpTime << endl;

			prevTime = timeI;
		}
		else cout << "Reused time " << prevTime << std::endl;
		im->GetPointData()->SetActiveVectors("ImageScalars");

		//siirret��n imagedata ulostuloon
		ds->DeepCopy(im);

		//kerrotaan mit� dataa l�ytyi
		ds->GetInformation()->Set(vtkDataObject::DATA_TIME_STEP(), dataInfo.Time().EpochTime());
		outInfo->Set(vtkDataObject::DATA_EXTENT(), reqExtent ? reqExtent : im->GetExtent(), 6);

		Modified();

		//ptime utcTime = ptime(from_time_t(dataInfo.Time().EpochTime()));

		//cout << "Returned time " << dataInfo.Time().EpochTime() << ", UTC: " << to_iso_extended_string( utcTime.date() ) <<", "<< utcTime.time_of_day() << endl;

		//ds->ReleaseData();

		return 1;
	}


}