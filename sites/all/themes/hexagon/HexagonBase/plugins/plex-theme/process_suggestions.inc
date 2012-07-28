<?php

/**
 * @file
 * The functions in this file will be invoked by base hooks.
 */

/**
 * This is only used for base hooks and a suggestion is made by other variable
 * processors. Setting a suggestion prevents the processors connected to the
 * suggestion from running. This effectively fills the gap by allowing the
 * processors specific to the suggestion to run.
 *
 * @see plex_theme_suggestion_processors()
 */
function plex_theme_variable_preprocess_suggestion(&$variables, $hook) {
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

  // $active_hook found through hook suggestions. This is only done here.
  // Other processors will use this same key to focus on the target hook.
  $suggestions = array();
  if (!empty($variables['theme_hook_suggestions'])) {
    $suggestions = $variables['theme_hook_suggestions'];
  }
  if (!empty($variables['theme_hook_suggestion'])) {
    $suggestions[] = $variables['theme_hook_suggestion'];
  }
  if ($suggestions) {
    foreach (array_reverse($suggestions) as $suggestion) {
      if (isset($registry[$suggestion])) {
        $active_hook = $suggestion;
        break;
      }
    }
  }

  if ($active_hook !== $hook) {
    $processors = &$static_fast['processors'][$active_hook];
    if (!isset($processors)) {
      $processors = plex_theme_hook_prepare($registry, $active_hook, 'suggested preprocess functions');
    }

    foreach ($processors as $processor) {
      // We don't want a poorly behaved process function changing $active_hook.
      $hook_clone = $active_hook;
      $processor($variables, $hook_clone);
    }
  }
}

/**
 * This is only used for base hooks and a suggestion is made by other variable
 * processors. Setting a suggestion prevents the processors connected to the
 * suggestion from running. This effectively fills the gap by allowing the
 * processors specific to the suggestion to run.
 *
 * @see plex_theme_suggestion_processors()
 */
function plex_theme_variable_process_suggestion(&$variables, $hook) {
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

  if ($active_hook !== $hook) {
    $processors = &$static_fast['processors'][$active_hook];
    if (!isset($processors)) {
      $processors = plex_theme_hook_prepare($registry, $active_hook, 'suggested process functions');
    }

    foreach ($processors as $processor) {
      // We don't want a poorly behaved process function changing $active_hook.
      $hook_clone = $active_hook;
      $processor($variables, $hook_clone);
    }
  }
}
