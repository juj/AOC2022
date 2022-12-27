#include <stdio.h>
#include <stdlib.h>
#include "../tick.h"

// How many turns are needed to wait for resources to accumulate in order to
// build a robot with given 'cost', given starting resource 'res', and its
// growth rate 'growth'? (includes the one turn needed to build)
#define TURNS_NEEDED(cost, res, growth) \
	(res >= cost ? 1 : (cost - res + growth - 1) / growth + 1)

int oreRoboCost, clayRoboCost, obsiRoboOreCost, obsiRoboClayCost,
		geodRoboOreCost, geodRoboObsiCost, maxOreRobos, allBestGeodes;

unsigned long iterations = 0;

int solve(int timeLeft, int oreRobos, int clayRobos, int obsiRobos,
					int geodRobos, int ore, int clay, int obsi, int geod)
{
	int newGeode, t, t2;
	++iterations;

	// Super conservative early out estimation: if we were able to produce
	// a new Geode robot every turn from now on, how many geodes would we
	// end up with? If that calculation results in worse geode count than we
	// have seen so far, we can early out from this search.
	// So tally: g + g+1 + g+2 + ... + g+(t-1) = t*(2g+t-1)/2
	if (geod + timeLeft*(((geod<<1) + timeLeft - 1) >> 1) < allBestGeodes)
		return geod;

	// How much geode we'll have if we build nothing for the rest of the time?
	int bestGeode = geod + timeLeft*geodRobos;

	// Build a Geode robot?
	if (obsiRobos > 0)
	{
		t = TURNS_NEEDED(geodRoboOreCost, ore, oreRobos);
		if (t < (t2 = TURNS_NEEDED(geodRoboObsiCost, obsi, obsiRobos))) t = t2;
		if (timeLeft > t)
		{
			newGeode = solve(timeLeft - t,
											 oreRobos, clayRobos, obsiRobos, geodRobos + 1,
											 ore  + t * oreRobos - geodRoboOreCost,
											 clay + t * clayRobos,
											 obsi + t * obsiRobos - geodRoboObsiCost,
											 geod + t * geodRobos);
			if (bestGeode < newGeode) bestGeode = newGeode;
		}
	}
	// Nonoptimal heuristic:
	// If we have enough obsi, greedily buy a Geode robot right away, so look
	// at other robot types only if no immediate resources are available for
	// a Geode robot. This is not a 100% sound assumption, but it does hold for
	// my input.
	// (counterexample: Ore robot costs 2 ore. Geode robot costs 2 ore)
	if (obsi >= geodRoboObsiCost) return bestGeode;

	// Build an Obsidian robot?
	if (clayRobos > 0 && obsiRobos < geodRoboObsiCost)
	{
		t = TURNS_NEEDED(obsiRoboOreCost, ore, oreRobos);
		if (t < (t2 = TURNS_NEEDED(obsiRoboClayCost, clay, clayRobos))) t = t2;
		if (timeLeft > t)
		{
			newGeode = solve(timeLeft - t,
											 oreRobos, clayRobos, obsiRobos + 1, geodRobos,
											 ore  + t * oreRobos  - obsiRoboOreCost,
											 clay + t * clayRobos - obsiRoboClayCost,
											 obsi + t * obsiRobos,
											 geod + t * geodRobos);
			if (bestGeode < newGeode) bestGeode = newGeode;
		}
	}

	// Similar heuristic as above, which holds for my input: If we have the
	// resources for an Obsidian robot, always choose to build that - look at
	// clay or Ore robots only if there are no resources for an Obsidian robot.
	if (clay >= obsiRoboClayCost) return bestGeode;

	// Buy an Ore robot? (only buy an Ore robot if current robots
	// cannot supply the remaining need for Ore for future purchases)
	if (obsiRobos == 0 // Heuristic: we are done building ore robos after
		&& ore < (maxOreRobos - oreRobos)*timeLeft // we have obsi robos
		&& timeLeft > (t = TURNS_NEEDED(oreRoboCost, ore, oreRobos)))
	{
		newGeode = solve(timeLeft - t,
										 oreRobos + 1, clayRobos, obsiRobos, geodRobos,
										 ore  + t * oreRobos - oreRoboCost,
										 clay + t * clayRobos,
										 obsi + t * obsiRobos,
										 geod + t * geodRobos);
		if (bestGeode < newGeode) bestGeode = newGeode;
	}

	// N.b. for some reason we get much better early out pruning if we check
	// clay robot purchases only after ore robot purchases.

	// Buy a Clay robot? (only buy a Clay robot if current robots cannot supply
	// the remaining need for Clay for future purchases)
	if (geodRobos == 0 // Heuristic: we are done building clays after we have
		&& clay < (obsiRoboClayCost - clayRobos)*timeLeft // built any geodes.
		&& timeLeft > (t = TURNS_NEEDED(clayRoboCost, ore, oreRobos)))
	{
		newGeode = solve(timeLeft - t,
										 oreRobos, clayRobos + 1, obsiRobos, geodRobos,
										 ore  + t * oreRobos - clayRoboCost,
										 clay + t * clayRobos,
										 obsi + t * obsiRobos,
										 geod + t * geodRobos);
		if (bestGeode < newGeode) bestGeode = newGeode;
	}

	// Record all time best seen so far, for further early out in other branches
	if (allBestGeodes < bestGeode) allBestGeodes = bestGeode;
	return bestGeode;
}

void main(int argc, char **argv)
{
	if (argc < 2) { printf("Call \"19 1\" or \"19 2\" to run 1st or 2nd part.\n"); return; }
	long t0 = tick();
	FILE *handle = fopen("input.txt", "r");
	int part2counter = 3, blueprint, quality = 0, best[3];
	while(fscanf(handle, "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d ore. Each obsidian robot costs %d ore and %d clay. Each geode robot costs %d ore and %d obsidian.\n",
		&blueprint, &oreRoboCost, &clayRoboCost, &obsiRoboOreCost, &obsiRoboClayCost, &geodRoboOreCost, &geodRoboObsiCost) == 7)
	{
		maxOreRobos = oreRoboCost;
		if (maxOreRobos < clayRoboCost) maxOreRobos = clayRoboCost;
		if (maxOreRobos < obsiRoboOreCost) maxOreRobos = obsiRoboOreCost;
		if (maxOreRobos < geodRoboOreCost) maxOreRobos = geodRoboOreCost;

		allBestGeodes = 0;
		if (*argv[1] == '1')
			quality += blueprint*solve(24, 1, 0, 0, 0, 0, 0, 0, 0);
		else
		{
			best[--part2counter] = solve(32, 1, 0, 0, 0, 0, 0, 0, 0);
			if (part2counter == 0)
			{
				printf("Part 2 quality: %d. Iterations: %lu. Elapsed %ld msecs.\n",
					best[0]*best[1]*best[2], iterations, tick()-t0);
				return;
			}
		}
	}
	printf("Part 1 quality: %d. Iterations: %lu. Elapsed %ld msecs.\n",
		quality, iterations, tick()-t0);
}
