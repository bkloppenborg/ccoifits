/*
 * COITarget.h
 *
 *  Created on: Nov 5, 2012
 *      Author: bkloppen
 */

#ifndef COITARGET_H_
#define COITARGET_H_

namespace ccoifits
{

class COITarget
{
protected:
	int target_id;
	string target;
	double RA;
	double DEC;
	double equinox;
	double RA_err;
	double DEC_err;
	double RV;
	string rv_type;
	string rv_def;
	double pm_ra;
	double pm_dec;
	double pm_ra_err;
	double pm_dec_err;
	double parallax;
	double parallax_err;
	string spec_type;

public:
	COITarget();
	virtual ~COITarget();
};

} /* namespace ccoifits */
#endif /* COITARGET_H_ */
