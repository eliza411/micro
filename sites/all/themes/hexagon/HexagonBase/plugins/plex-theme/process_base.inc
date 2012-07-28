<?php

/**
 * @file
 * The functions in this file will be invoked by hook suggestions.
 */

/**
 * This is only used for pattern based hooks or "hook suggestions" since they
 * do not carry a full set of variable processor functions.
 */
function plex_theme_variable_preprocess_base(&$variables, $hook) {
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

  $base_hook = &$registry[$active_hook]['plex base'];

  if ($base_hook) {
    $processors = &$static_fast['processors'][$base_hook];
    if (!isset($processors)) {
      $processors = plex_theme_hook_prepare($registry, $base_hook, 'preprocess functions');
    }

    foreach ($processors as $processor) {
      // We don't want a poorly behaved process function changing $active_hook.
      $hook_clone = $active_hook;
      $processor($variables, $hook_clone);
    }
  }
}

/**
 * This is only used for pattern based hooks or "hook suggestions" since they
 * do not carry a full set of variable processor functions.
 */
function plex_theme_variable_process_base(&$variables, $hook) {
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

  $base_hook = &$registry[$active_hook]['plex base'];

  if ($base_hook) {
    $processors = &$static_fast['processors'][$base_hook];
    if (!isset($processors)) {
      $processors = plex_theme_hook_prepare($registry, $base_hook, 'process functions');
    }

    foreach ($processors as $processor) {
      // We don't want a poorly behaved process function changing $active_hook.
      $hook_clone = $active_hook;
      $processor($variables, $hook_clone);
    }
  }
}
