/*
Copyright (c) 2010-2014, Mathieu Labbe - IntRoLab - Universite de Sherbrooke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Universite de Sherbrooke nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <rtabmap/utilite/ULogger.h>
#include <rtabmap/utilite/UStl.h>
#include <rtabmap/utilite/UMath.h>
#include <rtabmap/utilite/UConversion.h>
#include <rtabmap/core/Optimizer.h>
#include <rtabmap/core/Graph.h>
#include <set>
#include <queue>

#include <rtabmap/core/OptimizerTORO.h>
#include <rtabmap/core/OptimizerG2O.h>
#include <rtabmap/core/OptimizerGTSAM.h>
#include <rtabmap/core/OptimizerCVSBA.h>

namespace rtabmap {

bool Optimizer::isAvailable(Optimizer::Type type)
{
	if(type == Optimizer::kTypeG2O)
	{
		return OptimizerG2O::available();
	}
	else if(type == Optimizer::kTypeGTSAM)
	{
		return OptimizerGTSAM::available();
	}
	else if(type == Optimizer::kTypeCVSBA)
	{
		return OptimizerCVSBA::available();
	}
	else if(type == Optimizer::kTypeTORO)
	{
		return true;
	}
	return false;
}

Optimizer * Optimizer::create(const ParametersMap & parameters)
{
	int optimizerTypeInt = Parameters::defaultOptimizerStrategy();
	Parameters::parse(parameters, Parameters::kOptimizerStrategy(), optimizerTypeInt);
	Optimizer::Type type = (Optimizer::Type)optimizerTypeInt;

	if(!OptimizerG2O::available() && type == Optimizer::kTypeG2O)
	{
		UWARN("g2o optimizer not available. TORO will be used instead.");
		type = Optimizer::kTypeTORO;
	}
	if(!OptimizerGTSAM::available() && type == Optimizer::kTypeGTSAM)
	{
		UWARN("GTSAM optimizer not available. TORO will be used instead.");
		type = Optimizer::kTypeTORO;
	}
	if(!OptimizerCVSBA::available() && type == Optimizer::kTypeCVSBA)
	{
		UWARN("CVSBA optimizer not available. TORO will be used instead.");
		type = Optimizer::kTypeTORO;
	}
	Optimizer * optimizer = 0;
	switch(type)
	{
	case Optimizer::kTypeGTSAM:
		optimizer = new OptimizerGTSAM(parameters);
		break;
	case Optimizer::kTypeG2O:
		optimizer = new OptimizerG2O(parameters);
		break;
	case Optimizer::kTypeCVSBA:
		optimizer = new OptimizerCVSBA(parameters);
		break;
	case Optimizer::kTypeTORO:
	default:
		optimizer = new OptimizerTORO(parameters);
		break;

	}
	return optimizer;
}

Optimizer * Optimizer::create(Optimizer::Type & type, const ParametersMap & parameters)
{
	if(!OptimizerG2O::available() && type == Optimizer::kTypeG2O)
	{
		UWARN("g2o optimizer not available. TORO will be used instead.");
		type = Optimizer::kTypeTORO;
	}
	if(!OptimizerGTSAM::available() && type == Optimizer::kTypeGTSAM)
	{
		UWARN("GTSAM optimizer not available. TORO will be used instead.");
		type = Optimizer::kTypeTORO;
	}
	Optimizer * optimizer = 0;
	switch(type)
	{
	case Optimizer::kTypeGTSAM:
		optimizer = new OptimizerGTSAM(parameters);
		break;
	case Optimizer::kTypeG2O:
		optimizer = new OptimizerG2O(parameters);
		break;
	case Optimizer::kTypeTORO:
	default:
		optimizer = new OptimizerTORO(parameters);
		type = Optimizer::kTypeTORO;
		break;

	}
	return optimizer;
}

Optimizer::Optimizer(int iterations, bool slam2d, bool covarianceIgnored, double epsilon, bool robust) :
		iterations_(iterations),
		slam2d_(slam2d),
		covarianceIgnored_(covarianceIgnored),
		epsilon_(epsilon),
		robust_(robust)
{
}

Optimizer::Optimizer(const ParametersMap & parameters) :
		iterations_(Parameters::defaultOptimizerIterations()),
		slam2d_(Parameters::defaultOptimizerSlam2D()),
		covarianceIgnored_(Parameters::defaultOptimizerVarianceIgnored()),
		epsilon_(Parameters::defaultOptimizerEpsilon()),
		robust_(Parameters::defaultOptimizerRobust())
{
	parseParameters(parameters);
}

void Optimizer::parseParameters(const ParametersMap & parameters)
{
	Parameters::parse(parameters, Parameters::kOptimizerIterations(), iterations_);
	Parameters::parse(parameters, Parameters::kOptimizerVarianceIgnored(), covarianceIgnored_);
	Parameters::parse(parameters, Parameters::kOptimizerSlam2D(), slam2d_);
	Parameters::parse(parameters, Parameters::kOptimizerEpsilon(), epsilon_);
	Parameters::parse(parameters, Parameters::kOptimizerRobust(), robust_);
}

std::map<int, Transform> Optimizer::optimize(
		int rootId,
		const std::map<int, Transform> & poses,
		const std::multimap<int, Link> & constraints,
		std::list<std::map<int, Transform> > * intermediateGraphes,
		double * finalError,
		int * iterationsDone)
{
	UERROR("Optimizer %d doesn't implement optimize() method.", (int)this->type());
	return std::map<int, Transform>();
}

std::map<int, Transform> Optimizer::optimizeBA(
		int rootId,
		const std::map<int, Transform> & poses,
		const std::multimap<int, Link> & links,
		const std::map<int, Signature> & signatures)
{
	UERROR("Optimizer %d doesn't implement optimizeBA() method.", (int)this->type());
	return std::map<int, Transform>();
}

void Optimizer::getConnectedGraph(
		int fromId,
		const std::map<int, Transform> & posesIn,
		const std::multimap<int, Link> & linksIn,
		std::map<int, Transform> & posesOut,
		std::multimap<int, Link> & linksOut,
		int depth)
{
	UASSERT(depth >= 0);
	UASSERT(fromId>0);
	UASSERT(uContains(posesIn, fromId));

	posesOut.clear();
	linksOut.clear();

	std::set<int> ids;
	std::set<int> curentDepth;
	std::set<int> nextDepth;
	nextDepth.insert(fromId);
	int d = 0;
	std::multimap<int, int> biLinks;
	for(std::multimap<int, Link>::const_iterator iter=linksIn.begin(); iter!=linksIn.end(); ++iter)
	{
		UASSERT_MSG(graph::findLink(biLinks, iter->second.from(), iter->second.to()) == biLinks.end(),
				uFormat("Input links should be unique between two poses (%d->%d).",
						iter->second.from(), iter->second.to()).c_str());
		biLinks.insert(std::make_pair(iter->second.from(), iter->second.to()));
		biLinks.insert(std::make_pair(iter->second.to(), iter->second.from()));
	}

	while((depth == 0 || d < depth) && nextDepth.size())
	{
		curentDepth = nextDepth;
		nextDepth.clear();

		for(std::set<int>::iterator jter = curentDepth.begin(); jter!=curentDepth.end(); ++jter)
		{
			if(ids.find(*jter) == ids.end())
			{
				ids.insert(*jter);
				posesOut.insert(*posesIn.find(*jter));

				for(std::multimap<int, int>::const_iterator iter=biLinks.find(*jter); iter!=biLinks.end() && iter->first==*jter; ++iter)
				{
					int nextId = iter->second;
					if(ids.find(nextId) == ids.end() && uContains(posesIn, nextId))
					{
						nextDepth.insert(nextId);

						std::multimap<int, Link>::const_iterator kter = graph::findLink(linksIn, *jter, nextId);
						if(depth == 0 || d < depth-1)
						{
							linksOut.insert(*kter);
						}
						else if(curentDepth.find(nextId) != curentDepth.end() ||
								ids.find(nextId) != ids.end())
						{
							linksOut.insert(*kter);
						}
					}
				}
			}
		}
		++d;
	}
}

} /* namespace rtabmap */
