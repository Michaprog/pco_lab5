#ifndef BIKE_H
#define BIKE_H

#include <cstdlib>

/**
 * @brief Represents a bike with a given type.
 *
 * A bike is characterized only by its type. The type is encoded as an index
 * in the range [0, nbBikeTypes).
 */
class Bike
{
public:
    /**
     * @brief Type of this bike.
     *
     * Must be one of @ref VTT, @ref Road or @ref Gravel.
     */
    size_t bikeType;

    /**
     * @brief Total number of supported bike types.
     *
     * Used to size arrays that group bikes per type.
     */
    static const size_t nbBikeTypes = 3;

    /**
     * @brief Index for mountain bikes (VTT).
     */
    const size_t VTT = 0;

    /**
     * @brief Index for road bikes.
     */
    const size_t Road = 1;

    /**
     * @brief Index for gravel bikes.
     */
    const size_t Gravel = 2;
};

#endif // BIKE_H
