/*
 * OITools.cpp
 *
 *  Created on: Nov 8, 2012
 *      Author: bkloppen
 */

#include "OITools.h"
#include "OIFilter.h"
#include "COIDataRow.h"
#include "COIWavelength.h"
#include "COIV2Row.h"
#include "COIT3Row.h"
#include "COICalibrator.h"

#include <random>
#include <chrono>
using namespace std;

namespace ccoifits
{

/// Creates a data set suitable for bootstrapping by randomly selecting Vis, V2, and T3 data
/// from the original set.  The total number of each of Vis, V2, and T3 is maintained.
OIDataList Bootstrap_Random(const OIDataList & data)
{




	return OIDataList();
}

/// Creates a data set suitable for bootstrapping by selecting Vis, V2, and T3 data in
/// spectrally dispersed chunks.
///
/// The total number of each Vis, V2, and T3 are maintained.
OIDataList Bootstrap_Spectral(const OIDataList & data)
{
	// Init the output and random number generator:
	OIDataList output;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator (seed);

	// First do the Visibility data.  Find all of the OI_VIS data:
	OIDataList temp = FilterByDataType(data, COIDataRow::OI_VIS);
	// Generate a random distribution of the same size as the number of OI_VIS records
	std::uniform_int_distribution<int> vis_dist(0, temp.size() - 1);
	// Find out the total number of active data points
	unsigned int n_data = CountActiveData(temp);
	while(n_data > 0)
	{
		// Include entire spectral groups by randomly including data.
		auto row = temp[ vis_dist(generator) ];
		output.push_back(row);
		n_data -= row->GetMaskedNData();
	}

	// We may have included too many points, if so, simply mask out a few
	// points on the last included set.
	if(n_data < 0)
		output[output.size() - 1]->RandomMask(abs(n_data));

	/* Repeat the same thing for V2 and T3 data */

	//V2 records
	temp = FilterByDataType(data, COIDataRow::OI_VIS2);
	std::uniform_int_distribution<int> v2_dist(0, temp.size() - 1);
	n_data = CountActiveData(temp);
	while(n_data > 0)
	{
		auto row = temp[ v2_dist(generator) ];
		output.push_back(row);
		n_data -= row->GetMaskedNData();
	}

	if(n_data < 0)
		output[output.size() - 1]->RandomMask(abs(n_data));

	// T3s records
	temp = FilterByDataType(data, COIDataRow::OI_T3);
	std::uniform_int_distribution<int> t3_dist(0, temp.size() - 1);
	n_data = CountActiveData(temp);
	while(n_data > 0)
	{
		auto row = temp[ t3_dist(generator) ];
		output.push_back(row);
		n_data -= row->GetMaskedNData();
	}

	if(n_data < 0)
		output[output.size() - 1]->RandomMask(abs(n_data));

	// Return the result.
	return output;
}

/// Counts the total number of data points that are not masked out (i.e. active data)
unsigned int CountActiveData(const OIDataList & data)
{
	int n_data = 0;
	for(auto row : data)
		n_data += row->GetMaskedNData();

	return n_data;
}


/// Performs a pseudo re-calibration on the data, returning a copy of the newly calibrated data
OIDataList Recalibrate(OIDataList data, OICalibratorPtr old_cal, OICalibratorPtr new_cal)
{
	// Notice we are passing by value, so we have a COPY of the original OIDataList.
	// We can safely modify its content without issues.
	for(OIDataRowPtr row: data)
	{
		if(row->GetType() == COIDataRow::OI_VIS2)
			Recalibrate_VIS2(row, old_cal, new_cal);
		else if(row->GetType() == COIDataRow::OI_T3)
			Recalibrate_T3(row, old_cal, new_cal);
	}

	return data;
}

/// Recalibrate a VIS2 row
void Recalibrate_VIS2(OIDataRowPtr row, OICalibratorPtr old_cal, OICalibratorPtr new_cal)
{
	// Verify we truly have an OI_VIS2 record, if not, do nothing.
	if(row->GetType() != COIDataRow::OI_VIS2)
		return;

	// We have an OI_VIS2 object, dynamic cast it as so:
	COIV2Row * v2_row = dynamic_cast<COIV2Row*>(row.get());

	// Get the UV point and wavelength information:
	OIUVPtr uv = v2_row->mUV[0];
	vector<double> wavelength = v2_row->GetRawWavelengths();
	int n_wave =wavelength.size();

	// Recalibrate using the ratio of the calibrators old/new:
	for(int i = 0; i < n_wave; i++)
		v2_row->v2_data[i] *= old_cal->GetV2(uv, wavelength[i]) / new_cal->GetV2(uv, wavelength[i]);
}

/// Recalibrate a T3 row
void Recalibrate_T3(OIDataRowPtr row, OICalibratorPtr old_cal, OICalibratorPtr new_cal)
{
	// Verify we truly have an OI_VIS2 record, if not, do nothing.
	if(row->GetType() != COIDataRow::OI_T3)
		return;

	// We have an OI_VIS2 object, dynamic cast it as so:
	COIT3Row * t3_row = dynamic_cast<COIT3Row*>(row.get());

	// Get the UV point and wavelength information:
	OIUVPtr uv_12 = t3_row->mUV[0];
	OIUVPtr uv_23 = t3_row->mUV[1];
	OIUVPtr uv_31 = t3_row->mUV[2];
	vector<double> wavelength = t3_row->GetRawWavelengths();
	int n_wave = wavelength.size();

	// Recalibrate using the ratio of the calibrators old/new:
	for(int i = 0; i < n_wave; i++)
		t3_row->t3_data[i] *= old_cal->GetT3(uv_12, uv_23, uv_31, wavelength[i]) / new_cal->GetT3(uv_12, uv_23, uv_31, wavelength[i]);
}

} /* namespace ccoifits */