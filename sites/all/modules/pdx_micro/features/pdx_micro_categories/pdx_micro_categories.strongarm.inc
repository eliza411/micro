<?php
/**
 * @file
 * pdx_micro_categories.strongarm.inc
 */

/**
 * Implements hook_strongarm().
 */
function pdx_micro_categories_strongarm() {
  $export = array();

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'colors_taxonomy_term_1_enabled';
  $strongarm->value = FALSE;
  $export['colors_taxonomy_term_1_enabled'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'colors_taxonomy_term_2_enabled';
  $strongarm->value = TRUE;
  $export['colors_taxonomy_term_2_enabled'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'colors_taxonomy_term_3_enabled';
  $strongarm->value = FALSE;
  $export['colors_taxonomy_term_3_enabled'] = $strongarm;

  $strongarm = new stdClass();
  $strongarm->disabled = FALSE; /* Edit this to true to make a default strongarm disabled initially */
  $strongarm->api_version = 1;
  $strongarm->name = 'colors_taxonomy_term_enabled';
  $strongarm->value = TRUE;
  $export['colors_taxonomy_term_enabled'] = $strongarm;

  return $export;
}
