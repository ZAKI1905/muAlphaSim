#include "RevolvedG4Field.hh"

// -----------------------------------------------------------------------------
/**
 * @brief Constructor that loads the .pos file using the RevolvedFieldFromPOS
 * class.
 * @param filename Path to the Gmsh .pos file
 */
RevolvedG4Field::RevolvedG4Field(const std::string& filename)
    : fieldMap(filename) {}
// -----------------------------------------------------------------------------
/**
 * @brief Implements the Geant4 field interface using the wrapped field map.
 * @param point The 4D position [x, y, z, t] where the field is evaluated
 * @param field Output array of 6 values: [Ex, Ey, Ez, Bx, By, Bz]
 */
void RevolvedG4Field::GetFieldValue(const double point[4],
                                    double* field) const {
  G4ThreeVector pos(point[0], point[1], point[2]);
  G4ThreeVector E = fieldMap.GetField(pos);

  field[0] = E.x();  // Ex
  field[1] = E.y();  // Ey
  field[2] = E.z();  // Ez
  field[3] = 0.0;    // Bx
  field[4] = 0.0;    // By
  field[5] = 0.0;    // Bz
}
// -----------------------------------------------------------------------------
