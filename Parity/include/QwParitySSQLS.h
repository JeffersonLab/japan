
#ifndef QWPARITYSSQLS_HH
#define QWPARITYSSQLS_HH

#define GCC_VERSION (__GNUC__ * 10000 \
                     + __GNUC_MINOR__ * 100 \
                     + __GNUC_PATCHLEVEL__)

#if (__GNUC__ == 4) && (__GNUC_MINOR__ > 5)
#  pragma GCC diagnostic push
#endif

#if GCC_VERSION > 40200
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wvariadic-macros"
#endif

// Need to include define EXPAND_MY_SSQLS_STATICS in owner module
//
#ifndef EXPAND_QWPARITYSSQLS_STATICS
#define MYSQLPP_SSQLS_NO_STATICS
#endif

#include "mysql++.h"
#include "ssqls.h"

// Exclude from doxygen processing because of numerous warnings
/// @cond PARITY_SSQLS
namespace QwParitySSQLS {


    sql_create_6(db_schema, 1, 2 , mysqlpp::sql_int_unsigned, db_schema_id, mysqlpp::sql_char, major_release_number, mysqlpp::sql_char, minor_release_number, mysqlpp::sql_char, point_release_number, mysqlpp::sql_timestamp, time, mysqlpp::Null<mysqlpp::sql_text>, script_name) 
  
    sql_create_2(good_for, 1, 2 , mysqlpp::sql_int_unsigned, good_for_id, mysqlpp::Null<mysqlpp::sql_text>, type) 
  
    sql_create_2(run_quality, 1, 2 , mysqlpp::sql_int_unsigned, run_quality_id, mysqlpp::Null<mysqlpp::sql_text>, type) 
  
    sql_create_11(run, 1, 2 , mysqlpp::sql_int_unsigned, run_id, mysqlpp::sql_int_unsigned, run_number, mysqlpp::sql_int_unsigned, slug, mysqlpp::sql_int_unsigned, wien_slug, mysqlpp::sql_int_unsigned, injector_slug, mysqlpp::Null<mysqlpp::sql_text>, run_type, mysqlpp::Null<mysqlpp::sql_datetime>, start_time, mysqlpp::Null<mysqlpp::sql_datetime>, end_time, mysqlpp::sql_int_unsigned, n_mps, mysqlpp::sql_int_unsigned, n_qrt, mysqlpp::Null<mysqlpp::sql_text>, comment) 
  
    sql_create_10(runlet, 1, 2 , mysqlpp::sql_int_unsigned, runlet_id, mysqlpp::sql_int_unsigned, run_id, mysqlpp::sql_int_unsigned, run_number, mysqlpp::Null<mysqlpp::sql_int_unsigned>, segment_number, mysqlpp::sql_enum, full_run, mysqlpp::Null<mysqlpp::sql_datetime>, start_time, mysqlpp::Null<mysqlpp::sql_datetime>, end_time, mysqlpp::sql_int_unsigned, first_mps, mysqlpp::sql_int_unsigned, last_mps, mysqlpp::Null<mysqlpp::sql_text>, comment) 
  
    sql_create_23(analysis, 1, 2 , mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, runlet_id, mysqlpp::sql_int_unsigned, seed_id, mysqlpp::Null<mysqlpp::sql_datetime>, time, mysqlpp::Null<mysqlpp::sql_text>, bf_checksum, mysqlpp::sql_enum, beam_mode, mysqlpp::sql_int_unsigned, n_mps, mysqlpp::sql_int_unsigned, n_qrt, mysqlpp::Null<mysqlpp::sql_int_unsigned>, first_event, mysqlpp::Null<mysqlpp::sql_int_unsigned>, last_event, mysqlpp::Null<mysqlpp::sql_int>, segment, mysqlpp::Null<mysqlpp::sql_enum>, slope_calculation, mysqlpp::Null<mysqlpp::sql_enum>, slope_correction, mysqlpp::sql_text, root_version, mysqlpp::sql_text, root_file_time, mysqlpp::sql_text, root_file_host, mysqlpp::sql_text, root_file_user, mysqlpp::sql_text, analyzer_name, mysqlpp::sql_text, analyzer_argv, mysqlpp::sql_text, analyzer_svn_rev, mysqlpp::sql_text, analyzer_svn_lc_rev, mysqlpp::sql_text, analyzer_svn_url, mysqlpp::sql_text, roc_flags) 
  
    sql_create_3(parameter_files, 1, 2 , mysqlpp::sql_int_unsigned, parameter_file_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_text, filename) 
  
    sql_create_4(general_errors, 1, 2 , mysqlpp::sql_int_unsigned, general_errors_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_tinyint_unsigned, error_code_id, mysqlpp::sql_int_unsigned, n) 
  
    sql_create_9(md_slope, 1, 2 , mysqlpp::sql_int_unsigned, md_slope_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, slope_type_id, mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_int_unsigned, main_detector_id, mysqlpp::sql_tinyint_unsigned, subblock, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error) 
  
    sql_create_9(lumi_slope, 1, 2 , mysqlpp::sql_int_unsigned, lumi_slope_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, slope_type_id, mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_int_unsigned, lumi_detector_id, mysqlpp::sql_tinyint_unsigned, subblock, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error) 
  
    sql_create_8(beam, 1, 2 , mysqlpp::sql_int_unsigned, beam_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, monitor_id, mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_tinyint_unsigned, subblock, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error) 
  
    sql_create_5(beam_errors, 1, 2 , mysqlpp::sql_int_unsigned, beam_errors_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, monitor_id, mysqlpp::sql_tinyint_unsigned, error_code_id, mysqlpp::sql_int_unsigned, n) 
  
    sql_create_3(measurement_type, 1, 2 , mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_text, units, mysqlpp::sql_text, title) 
  
    sql_create_2(error_code, 1, 2 , mysqlpp::sql_tinyint_unsigned, error_code_id, mysqlpp::sql_text, quantity) 
  
    sql_create_4(slope_type, 1, 2 , mysqlpp::sql_int_unsigned, slope_type_id, mysqlpp::sql_text, slope, mysqlpp::sql_text, units, mysqlpp::sql_text, title) 
  
    sql_create_3(monitor, 1, 2 , mysqlpp::sql_int_unsigned, monitor_id, mysqlpp::sql_text, quantity, mysqlpp::sql_text, title) 
  
    sql_create_3(main_detector, 1, 2 , mysqlpp::sql_int_unsigned, main_detector_id, mysqlpp::sql_text, quantity, mysqlpp::sql_text, title) 
  
    sql_create_8(md_data, 1, 2 , mysqlpp::sql_int_unsigned, md_data_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::Null<mysqlpp::sql_int_unsigned>, main_detector_id, mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_tinyint_unsigned, subblock, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error) 
  
    sql_create_5(md_errors, 1, 2 , mysqlpp::sql_int_unsigned, md_errors_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, main_detector_id, mysqlpp::sql_tinyint_unsigned, error_code_id, mysqlpp::sql_int_unsigned, n) 
  
    sql_create_3(lumi_detector, 1, 2 , mysqlpp::sql_int_unsigned, lumi_detector_id, mysqlpp::sql_text, quantity, mysqlpp::sql_text, title) 
  
    sql_create_8(lumi_data, 1, 2 , mysqlpp::sql_int_unsigned, lumi_data_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::Null<mysqlpp::sql_int_unsigned>, lumi_detector_id, mysqlpp::sql_char, measurement_type_id, mysqlpp::sql_tinyint_unsigned, subblock, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error) 
  
    sql_create_5(lumi_errors, 1, 2 , mysqlpp::sql_int_unsigned, lumi_errors_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, lumi_detector_id, mysqlpp::sql_tinyint_unsigned, error_code_id, mysqlpp::sql_int_unsigned, n) 
  
    sql_create_11(slow_controls_settings, 1, 2 , mysqlpp::sql_int_unsigned, slow_controls_settings_id, mysqlpp::sql_int_unsigned, runlet_id, mysqlpp::Null<mysqlpp::sql_enum>, slow_helicity_plate, mysqlpp::Null<mysqlpp::sql_enum>, passive_helicity_plate, mysqlpp::Null<mysqlpp::sql_enum>, wien_reversal, mysqlpp::Null<mysqlpp::sql_enum>, precession_reversal, mysqlpp::Null<mysqlpp::sql_int_unsigned>, helicity_length, mysqlpp::Null<mysqlpp::sql_enum>, charge_feedback, mysqlpp::Null<mysqlpp::sql_enum>, position_feedback, mysqlpp::Null<mysqlpp::sql_float>, qtor_current, mysqlpp::Null<mysqlpp::sql_text>, target_position) 
  
    sql_create_4(sc_detector, 1, 2 , mysqlpp::sql_int_unsigned, sc_detector_id, mysqlpp::sql_text, name, mysqlpp::sql_text, units, mysqlpp::sql_text, comment) 
  
    sql_create_8(slow_controls_data, 1, 2 , mysqlpp::sql_int_unsigned, slow_controls_data_id, mysqlpp::sql_int_unsigned, runlet_id, mysqlpp::sql_int_unsigned, sc_detector_id, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, value, mysqlpp::sql_float, error, mysqlpp::sql_float, min_value, mysqlpp::sql_float, max_value) 
  
    sql_create_4(slow_controls_strings, 1, 2 , mysqlpp::sql_int_unsigned, slow_controls_strings_id, mysqlpp::sql_int_unsigned, runlet_id, mysqlpp::sql_int_unsigned, sc_detector_id, mysqlpp::sql_text, value) 
  
    sql_create_5(seeds, 1, 2 , mysqlpp::sql_int_unsigned, seed_id, mysqlpp::sql_int_unsigned, first_run_id, mysqlpp::sql_int_unsigned, last_run_id, mysqlpp::Null<mysqlpp::sql_text>, seed, mysqlpp::Null<mysqlpp::sql_text>, comment) 
  
    sql_create_4(bf_test, 1, 2 , mysqlpp::sql_int_unsigned, bf_test_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::Null<mysqlpp::sql_int_unsigned>, test_number, mysqlpp::Null<mysqlpp::sql_float>, test_value) 
  
    sql_create_2(modulation_type, 1, 2 , mysqlpp::sql_int_unsigned, modulation_type_id, mysqlpp::Null<mysqlpp::sql_text>, type) 
  
    sql_create_12(beam_optics, 1, 2 , mysqlpp::sql_int_unsigned, beam_optics_id, mysqlpp::sql_int_unsigned, analysis_id, mysqlpp::sql_int_unsigned, monitor_id, mysqlpp::sql_int_unsigned, modulation_type_id, mysqlpp::sql_int_unsigned, n, mysqlpp::sql_float, amplitude, mysqlpp::sql_float, phase, mysqlpp::sql_float, offset, mysqlpp::sql_float, a_error, mysqlpp::sql_float, p_error, mysqlpp::sql_float, o_error, mysqlpp::sql_float, gof_para) 
  
    }
/// @endcond

#if (__GNUC__ == 4) && (__GNUC_MINOR__ > 5)
#  pragma GCC diagnostic pop
#endif

#endif
