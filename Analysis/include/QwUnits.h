#ifndef QWUNITS_H
#define QWUNITS_H

/**
 * \ingroup QwAnalysis
 *
 * \section unitsoverview Units in the QwAnalysis framework
 *
 * As more and more analyzers start contributing code to the Qweak analysis
 * code, it is important to use some conventions to avoid confusion.  One of
 * the conventions is the use of a standard system of coordinates.  The other
 * is a unified set of units.  This way we will avoid errors when angles are
 * not converted from degrees to radians, or centimeters and inches are in
 * mixed up.
 *
 * The basic units in the QwAnalysis framework are:
 * \li Length: <b>cm</b>
 * \li Time: <b>ms</b>
 * \li Energy: <b>MeV</b>
 * \li Angles: <b>rad</b>
 * \li Magnetic field: <b>T</b>
 *
 * How are these units to be used?  Any variable that is being used inside
 * subsystems and <b>especially when passing information to other subsystems</b>
 * should have values in these units.
 *
 * This does not mean that every single constant has to be defined in these
 * units, or that output has to use these units.  A set of conversion constants
 * is defined to make life easier.
 *
 * Take as an example the magnetic field map.  The values stored in the map file
 * by the external program are in kG.  To convert those values, we would write:
 * \code
 * double bx, by, bz;
 * mapfile >> bx >> by >> bz; // read values
 * bx *= Qw::kG; by *= Qw::kG; bz *= Qw::kG; // define the units (kG)
 * \endcode
 * From now on, the values in bx, by, bz are in standard Qweak units (T).  To
 * write out the values in Gauss we write:
 * \code
 * cout << bx/Qw::G << "," << by/Qw::G << "," << bz/Qw::G << " G" << endl;
 * \endcode
 * For Tesla we would write
 * \code
 * cout << bx/Qw::T << "," << by/Qw::T << "," << bz/Qw::T << " T" << endl;
 * \endcode
 *
 * If there are hard-coded values in the analyzer (which should be avoided),
 * we should use these units to define them:
 * \code
 * const double position_r3 = 398.8 * Qw::cm;
 * const double helicity_window = 1.0 * Qw::ms;
 * const double noise_frequency = 60.0 * Qw::Hz;
 * \endcode
 */
namespace Qw {

  /// Length units: base unit is mm
  //@{
  // metric
  static const double cm = 1.0;
  static const double mm = 0.1 * cm;
  static const double m = 1.0e2 * cm;
  static const double km = 1.0e3 * m;
  // imperial
  static const double in = 2.54 * cm;
  static const double mil = 0.001 * in;
  //@}

  /// Time units: base unit is ms
  //@{
  static const double ms = 1.0;
  static const double us = 1.0e-3 * ms;
  static const double ns = 1.0e-6 * ms;
  static const double sec = 1.0e3 * ms; // SI notation is s, but sec avoids ambiguity
  static const double min = 60.0 * sec;
  static const double hour = 60.0 * min;
  static const double day = 24.0 * hour;
  //@}

  /// Frequency units: base unit is kHz
  //@{
  static const double Hz = 1.0 / sec;
  static const double kHz = 1.0 / ms;
  static const double MHz = 1.0e3 * kHz;
  //@}

  //@{
  /// Energy: base unit is MeV
  static const double V = 1.0;
  static const double e = 1.0e-6;
  static const double eV = 1.0e-6;
  static const double keV = 1.0e-3;
  static const double MeV = 1.0;
  static const double GeV = 1.0e3;
  static const double MeV2 = MeV * MeV;
  static const double GeV2 = GeV * GeV;
  //@}

  //@{
  /// Angles: base unit is radian
  static const double pi = 3.14159265;
  static const double deg2rad = pi / 180.0;
  static const double rad2deg = 180.0 / pi;
  static const double rad = 1.0;
  static const double deg = deg2rad;
  //@}

  //@{
  /// Magnetic field: base unit is T
  static const double T = V * sec / (m * m);
  static const double G = 1.0e-4 * T;
  static const double kG = 1.0e3 * G;
  //@}

  //@{
  /// Physical constants
  static const double c = 299792458. * m / sec;    ///< Speed of light
  static const double Mp = 938.272013 * MeV;    ///< Mass of the proton
  //@}

} // namespace Qw

#endif // QWUNITS_H
