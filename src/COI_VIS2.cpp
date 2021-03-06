/*
 * OI_VIS2.cpp
 *
 *  Created on: Nov 2, 2012
 *      Author: bkloppen
 */

#include "COI_VIS2.h"
#include "COIV2Row.h"
#include "oi_file.hpp"

#include <stdexcept>
using namespace std;

namespace ccoifits
{

COI_VIS2::COI_VIS2(ExtHDU & table, COIFile * parent)
:	COI_DATA_TABLE(table)
{
	// Verify that we have an OI_VIS2 table
	if(mTable.name() != "OI_VIS2")
		throw runtime_error("Attempted to construct an OI_VIS2 table from a non-OI_VIS2 HDU.");

	mParent = parent;
}

COI_VIS2::~COI_VIS2()
{
	// TODO Auto-generated destructor stub
}

OIDataList COI_VIS2::read()
{
	// Read in the common data:
	vector<int> target_ids;
	vector<double> times;
	vector<double> MJDs;
	vector<double> int_times;
	vector< valarray<int> > sta_indicies;
	vector< valarray<bool> > flags;
	COI_DATA_TABLE::ReadCommon(target_ids, times, MJDs, int_times, sta_indicies, flags);

	// Now read in the V2-specific columns:
	int n_rows = mTable.rows();
	vector< valarray<double> > v2_data = ReadArray<double>("VIS2DATA");
	vector< valarray<double> > v2_data_err = ReadArray<double>("VIS2ERR");

	vector<double> ucoord = ReadColumn<double>("UCOORD");
	vector<double> vcoord = ReadColumn<double>("VCOORD");

	// Now create and store V2 objects:
	vector<OIDataRowPtr> output;

	OIArrayPtr array = mParent->GetArray(this->GetArrayName());
	OIWavelengthPtr wave = mParent->GetWavelength(this->GetInstrumentName());

	for(int i = 0; i < n_rows; i++)
	{
		OITargetPtr target = mParent->GetTarget(target_ids[i]);

		OIUVPtr uv( new COIUV(ucoord[i], vcoord[i]));

        // workaround to permit a negative error to set bad data flags
		size_t flags_size = flags[i].size();
        for(size_t j = 0; j < flags_size; j++)
        {
            if(v2_data_err[i][j] < 0)
                flags[i][j] = true;
        }

		OIDataRowPtr tmp( new COIV2Row(target, array, wave, times[i], MJDs[i], int_times[i], sta_indicies[i], flags[i], uv, v2_data[i], v2_data_err[i]) );
		output.push_back(tmp);
	}

	return output;
}


} /* namespace ccoifits */
