#include "RPGlfWindSet.h"
#include <lib/RP/RPGlfDefine.h>
#include <lib/RP/RPGlfConfig.h>
#include <cstdlib>
#include "types.h"

/// <summary>
/// Score a wind set against a target, with the score representing how close
/// it is to its target set. Used to rank seeds in order of how close they are
/// to what wind the user wants.
/// </summary>
/// <param name="target">Target wind set to score against</param>
/// <returns></returns>
Score_t RPGlfWindSet::scoreAgainstTarget(const RPGlfWindSet& target)
{
	Score_t myScore = 0;
	for (u32 i = 0; i < RPGlfDefine::HOLE_SIZE; i++)
	{
		// Wildcard counts as them matching
		if (mWinds[i].mDirection != RPGlfDefine::WindDir::WILDCARD)
		{
			myScore += scoreBase - std::abs((int)mWinds[i].mDirection - (int)target.mWinds->mDirection);
		}
		else
		{
			myScore += scoreBase;
		}
		// The speed score is incremented by 1 and then halved
		// to have about the same priority in the total score as the direction would.
		if (mWinds[i].mSpeed != RPGlfDefine::WindSpd::WILDCARD)
		{
			myScore += scoreBase - (std::abs((int)mWinds[i].mSpeed - (int)target.mWinds->mSpeed) + 1) / 2;
		}
		else
		{
			myScore += scoreBase;
		}
	}

	return myScore;
}

/// <summary>
/// Returns data in wind set in string format
/// </summary>
/// <returns>Wind set string</returns>
std::string RPGlfWindSet::toString() const
{
	std::string result;
	result += "{";

	// Convert each speed + direction to string (keep wildcard if that was part of the input)
	for (u32 i = 0; i < RPGlfDefine::HOLE_SIZE; i++)
	{
		result += (mWinds[i].mSpeed != RPGlfDefine::WindSpd::WILDCARD) ? std::to_string(mWinds[i].mSpeed) : wildCard;
		result += (mWinds[i].mDirection != RPGlfDefine::WindDir::WILDCARD) ? windDirStrings[mWinds[i].mDirection] : wildCard;

		// Comma separated list (except last item)
		if (i < RPGlfDefine::HOLE_SIZE - 1)
		{
			result += ", ";
		}
	}

	return result + "}";
}