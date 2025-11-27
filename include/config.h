#ifndef CONFIG_H
#define CONFIG_H

#include <random>
#include <cstddef>

/**
 * @brief Number of bike-sharing sites (excluding the depot).
 */
const size_t NBSITES   = 8;

/**
 * @brief Identifier of the depot site.
 *
 * The depot is considered as an extra site after the regular sites.
 */
const size_t DEPOT_ID  = NBSITES;

/**
 * @brief Total number of sites including the depot.
 */
const size_t NB_SITES_TOTAL = NBSITES + 1;

/**
 * @brief Number of docking points (slots) per site.
 */
const size_t BORNES = 6;

/**
 * @brief Total number of bikes in the whole system.
 */
const size_t NB_BIKES = 35;

/**
 * @brief Number of people (users) simulated in the system.
 */
const size_t NBPEOPLE = 10;

/**
 * @brief Maximum capacity of the van (number of bikes it can carry).
 */
const size_t VAN_CAPACITY = 4;

/**
 * @brief Thread-local random number generator used for the simulation.
 */
static thread_local std::mt19937_64 c_rng(std::random_device{}());

/**
 * @brief Returns a random site index different from a given one.
 *
 * @param maxSite Number of valid sites (exclusive upper bound).
 * @param exclude Site index that must not be chosen.
 * @return Random site index in [0, maxSite) and != @p exclude.
 */
inline unsigned int randomSiteExcept(unsigned int maxSite, unsigned int exclude)
{
    std::uniform_int_distribution<unsigned int> dist(0, maxSite - 1);
    unsigned int s;
    do {
        s = dist(c_rng);
    } while (s == exclude);
    return s;
}

/**
 * @brief Returns a random travel time in milliseconds.
 *
 * The value is uniformly drawn between 500 ms and 2000 ms.
 *
 * @return Random travel time in milliseconds.
 */
inline unsigned int randomTravelTimeMs()
{
    std::uniform_int_distribution<unsigned int> dist(500, 2000);
    return dist(c_rng);
}

#endif // CONFIG_H
