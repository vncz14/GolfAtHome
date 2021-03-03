#include "RPGlfConfig.h"
#include <lib/RP/RPUtlRandom.h>
#include <lib/RP/RPGlfDefine.h>
#include <cstdio>

/// <summary>
/// Generates a random sequence of numbers 
/// between 0-max (exclusive), with no duplicates.
/// </summary>
/// <param name="max">Upper limit of sequence</param>
/// <param name="pArray">Pointer to array to fill</param>
void RPGlfConfig::makeRandomSequence(s32 max, s32* pArray)
{
	for (s32 i = 0; i < max; i++)
	{
		pArray[i] = -max;
	}

	s32 slotsFilled = 0;
	while (true)
	{
		s32 random = (s32)(RPUtlRandom::getF32() * (max - slotsFilled));

		if (max > 0)
		{
			for (s32 i = 0, j = 0; i < max; i++)
			{
				if ((pArray[j] < 0) && (--random < 0))
				{
					pArray[j] += max;
					pArray[slotsFilled++] += j;

					if (slotsFilled >= max)
					{
						return;
					}
					
					break;
				}
				else
				{
					j++;
				}
			}
		}
	}
}

/// <summary>
/// Generate a pool of random speeds and directions, and choose 9 of each
/// to represent the "wind set" for the current game.
/// </summary>
/// <param name="diff">Selected difficulty</param>
void RPGlfConfig::chooseWindSet(const DifficultyInfo& diff)
{
	// Simulate all random number generations before wind is generated
	RPUtlRandom::advance(CALC_BEFORE_WIND);

	RPGlfConfig* instance = getInstance();

	// Generate random sequences
	// (speed/dir, values this function will hand-pick from)
	s32 randomDirs[randomDirArraySize];
	makeRandomSequence(randomDirArraySize, randomDirs);
	s32 randomSpeeds[randomSpeedArraySize];
	makeRandomSequence(randomSpeedArraySize, randomSpeeds);

	u32 gameLength = diff.endHole - diff.startHole;

	// Holes that have non-zero wind speed
	u32 numNonzeroWinds = 0;
	u32 r20 = 0;
	u32 r19 = 0;
	u32 r27_i = 0;

	// Loop through each hole
	for (u32 i = 0; i < RPGlfDefine::HOLE_SIZE; i++)
	{
		// Will the [i]'th hole NOT be played
		// with the current difficulty?
		if ((i < diff.startHole) || (i > diff.endHole))
		{
			// If it won't, use dummy wind (32 S)
			instance->mWinds[i].mDirection = RPGlfDefine::MAX_WIND_DIV;
			instance->mWinds[i].mSpeed = RPGlfDefine::MAX_WIND_SPD;
		}
		else
		{
			s32 nextSpd = 0;
			do
			{
				// Evaluate speeds in random sequence
				nextSpd = randomSpeeds[r20++];
			} while ((nextSpd < diff.minWind) && (nextSpd > diff.maxWind));

			// It is valid, so let's use it
			instance->mWinds[i].mSpeed = nextSpd;

			if (numNonzeroWinds < 8)
			{
				s32 nextDir = randomDirs[r27_i];
				instance->mWinds[i].mDirection = nextDir;
				// TO-DO: Verify this continue jumps to for loop top
				if (nextSpd <= 0) continue;
				numNonzeroWinds++;
				r27_i++;
			}
			// If there are eight holes already with non-zero wind,
			// this hole MUST have zero wind.
			else
			{
				// If the next wind speed was going to be zero, we take it
				if (nextSpd == 0)
				{
					instance->mWinds[i].mDirection = RPGlfDefine::SOUTH;
					// TO-DO: Verify this jumps to the top of the for loop.
					continue;
				}
				// Otherwise, we pick a random hole and override its speed with zero.
				else
				{
					u32 rndNum = (u32)(RPUtlRandom::getF32() * gameLength) + diff.startHole;
					instance->mWinds[rndNum].mSpeed = 0;
					// This can't be right.
					instance->mWinds[i].mDirection = instance->mWinds[rndNum].mDirection;
				}
			}
		}
	}
}

/// <summary>
/// Gets a pointer to the RPGlfConfig member array for winds.
/// </summary>
/// <returns>Wind array pointer</returns>
Wind* RPGlfConfig::getWinds()
{
	return getInstance()->mWinds;
}

/// <summary>
/// Get static instance of RPGlfConfig class.
/// </summary>
/// <returns>RPGlfConfig instance</returns>
RPGlfConfig* RPGlfConfig::getInstance()
{
	INSTANCE_GUARD(RPGlfConfig);
	return mInstance;
}

RPGlfConfig* RPGlfConfig::mInstance = NULL;
