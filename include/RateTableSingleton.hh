/**
 * @file      RateTableSingleton.hh
 * @brief     One-liner access to a single global RateTable2D instance.
 *
 * Call `RateTable()` anywhere in the code to obtain a read-only
 * reference to the table loaded from "tunnelling_rate.tsv".
 *
 * This avoids passing the table pointer through every constructor yet keeps
 * the object alive for the duration of the program.
 *
 * @code
 *   #include "RateTableSingleton.hh"
 *   ...
 *   double w = RateTable().Interp(rho,z);
 * @endcode
 *
 * @author   Mohammadreza Zakeri
 * @date     2025-05-26
 */

#ifndef RATE_TABLE_SINGLETON_HH
#define RATE_TABLE_SINGLETON_HH

#include "RateTable2D.hh"

/**
 * @brief Accessor that lazily constructs and returns the global table.
 *
 * The first call initialises the static local "RateTable2D" object; all
 * subsequent calls return the same instance.
 *
 * @return Reference to a fully initialised "RateTable2D."
 */
inline RateTable2D& RateTable()
{
    static RateTable2D s_table("/Users/keeper/Documents/geant4-projects/muAlphaSim/tunnelling_rate.tsv");
    return s_table;
}

#endif // RATE_TABLE_SINGLETON_HH