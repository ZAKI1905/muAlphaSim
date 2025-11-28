/**
 * @file      RateTable2D.hh
 * @brief     Lightweight 2-D unstructured rate table with interpolation.
 *
 * This class reads an ASCII table of values with three whitespace- or delimiter-
 * separated columns:
 *
 * |  ρ [m]  |  z [m]  |  w(ρ,z) [s⁻¹]  |
 * |--------:|-------:|----------------:|
 *
 * Unlike regular grids, the input is allowed to be sparse or masked (e.g., due to
 * physical constraints or geometry truncations such as cone removal). This class
 * provides nearest-neighbor and bilinear interpolation if surrounding points are
 * available.
 *
 * Usage Example
 * -------------
 * @code
 *   RateTable2D table("rate_data.tsv");
 *   if (table.Inside(rho, z)) {
 *       double rate = table.Interp(rho, z);
 *   }
 * @endcode
 *
 * @author   Mohammadreza Zakeri
 * @date     2025-05-27
 */

#ifndef RATE_TABLE_2D_HH
#define RATE_TABLE_2D_HH

#include <vector>
#include <string>
#include <tuple>
#include <stdexcept>

/**
 * @class  RateTable2D
 * @brief  Stores unstructured (rho, z, w) data and performs interpolation.
 */
class RateTable2D
{
  public:
    /**
     * @brief Constructor – loads data from file.
     *
     * @param filename  Path to the data file.
     * @param delim     Delimiter character (default = tab).
     *
     * @throw std::runtime_error on file errors or parsing failure.
     */
    explicit RateTable2D(const std::string& filename, char delim = '\t');

    /// @name Domain inquiry
    /// @{
    /// Returns true if (rho, z) lies inside the convex hull of the data.
    bool Inside(double rho, double z) const;
    /// @}

    /**
     * @brief Interpolates w(rho, z) using nearest-neighbor or bilinear if possible.
     *
     * @param rho  Cylindrical radius (in meters).
     * @param z    Height coordinate (in meters).
     *
     * @return     Interpolated rate w [s^-1]. Returns 0 if out-of-domain.
     */
    double Interp(double rho, double z) const;

    /// Number of data points loaded (not a grid, unstructured).
    std::size_t Size() const { return mPoints.size(); }

    // Getter for the bounding box of the data
    /**
     * @brief Gets the bounding box of the data.
     *
     * @param[out] minRho  Minimum rho value.
     * @param[out] maxRho  Maximum rho value.
     * @param[out] minZ    Minimum z value.
     * @param[out] maxZ    Maximum z value.
     */
    void GetBoundingBox(double& minRho, double& maxRho,
                    double& minZ, double& maxZ) const;
  private:
    /**
     * @struct RatePoint
     * @brief  A single (rho, z, rate) data point.
     */
    struct RatePoint {
        double rho;   //!< Cylindrical radius
        double z;     //!< Axial coordinate
        double rate;  //!< Associated rate [s^-1]
    };

    std::vector<RatePoint> mPoints;  //!< Full unstructured list of (rho, z, rate) entries

    /**
     * @brief Finds the 4 nearest neighbors for bilinear interpolation.
     *
     * @param rho  Query point rho.
     * @param z    Query point z.
     * @param[out] neighbors  Vector of 4 points (if found).
     *
     * @return True if a proper rectangle can be formed; false otherwise.
     */
    bool FindBilinearNeighbors(double rho, double z,
                               std::vector<RatePoint>& neighbors) const;

    /**
     * @brief Performs bilinear interpolation given 4 neighbor points.
     */
    double BilinearInterp(const std::vector<RatePoint>& neighbors,
                          double rho, double z) const;
};

#endif // RATE_TABLE_2D_HH
