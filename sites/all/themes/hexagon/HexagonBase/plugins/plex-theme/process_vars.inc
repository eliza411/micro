<?php

/**
 * When any of the following files are detected for a specific hook, they will
 * be included and processed:
 *
 * - [ HOOK ].vars.inc
 * - [ HOOK ].[ ACTIVE-THEME ].vars.inc
 * - [ HOOK ].[ func | tpl ].vars.inc
 * - [ HOOK ].[ ACTIVE-THEME ].[ func | tpl ].vars.inc
 * - [ HOOK--PATTERN ].vars.inc
 * - [ HOOK--PATTERN ].[ ACTIVE-THEME ].vars.inc
 * - [ HOOK--PATTERN ].[ func | tpl ].vars.inc
 * - [ HOOK--PATTERN ].[ ACTIVE-THEME ].[ func | tpl ].vars.inc
 */
function plex_theme_variable_include_process(&$variables, $hook) {
  // Use the advanced static pattern, since this is called very often.
  static $static_fast;
  if (!isset($static_fast['registry'])) {
    $static_fast['registry'] = &plex_theme_registry_cache();
  }
  $registry = &$static_fast['registry'];

  $active_hook = &$variables['active_theme_hook'];
  if (!$active_hook) {
    $active_hook = $hook;
  }

  $variable_includes = &$static_fast['variable_includes']["$hook:$active_hook"];

  // Gather and statically cache all .vars.inc files for the current $hook
  // and suggested hook set in $active_hook.
  if (!isset($variable_includes)) {
    $variable_includes = array();
    if (!empty($registry[$hook]['vars includes'])) {
      $variable_includes = $registry[$hook]['vars includes'];
    }
    if ($hook !== $active_hook && !empty($registry[$active_hook]['vars includes'])) {
      $variable_includes = array_merge($variable_includes, $registry[$active_hook]['vars includes']);
    }
  }

  if ($variable_includes) {
    plex_theme_variable_include($variable_includes, $variables);
  }
}

/**
 * Extracts and includes vars.inc files.
 */
function plex_theme_variable_include($variable_includes, &$variables) {
  // Allow both common spellings.
  $vars = &$variables;
  // Extract the variables to a local namespace. This is not expensive.
  extract($variables, EXTR_REFS + EXTR_SKIP);
  // Include files.
  foreach ($variable_includes as $variable_include) {
    include DRUPAL_ROOT . '/' . $variable_include;
  }
}
