<?php

/**
 * Initialize the plugin.
 */
function plex_theme_initialize() {

  // When cache is cleared, include the file responsible for modifying the registry.
  if (hex_theme_was_rebuilt()) {
    // Anything containing a theme function or variable process function must
    // be included early in order for it to be discovered. The type of files
    // included will depend on specific settings:
    // - 'plex_theme.load_on_demand:theme_inc' *.theme.inc
    // - 'plex_theme.load_on_demand:func_inc'  *.func.inc
    foreach (plex_theme_get_includes() as $file) {
      switch ($file->type) {
        case 'theme':
        case 'func' :
          include_once DRUPAL_ROOT . '/' . $file->uri;
      }
    }
  }

  if (hex_settings('plex_theme.core_fix:suggestions_handles_base', HEX_FIRST_PARENT)) {
    include_once DRUPAL_ROOT . '/' . hex()->path('plex_theme') . '/process_base.inc';
  }
  if (hex_settings('plex_theme.core_fix:base_handles_suggestions', HEX_FIRST_PARENT)) {
    include_once DRUPAL_ROOT . '/' . hex()->path('plex_theme') . '/process_suggestions.inc';
  }
  if (hex_settings('plex_theme.load_on_demand:vars_inc', HEX_FIRST_PARENT)) {
    include_once DRUPAL_ROOT . '/' . hex()->path('plex_theme') . '/process_vars.inc';
  }
}

/**
 * Prepares the hook by including required files and building a list of
 * variable processors.
 *
 * This information should be cached from the calling function.
 *
 * @param $registry
 *  The theme registry.
 * @param $hook
 *  The theme hook to prepare.
 * @param $process_key
 *  The key to source the variable processors from.
 */
function plex_theme_hook_prepare(&$registry, $hook, $process_key) {

  if (!empty($registry[$hook]['includes'])) {
    foreach ($registry[$hook]['includes'] as $include) {
      include_once DRUPAL_ROOT . '/' . $include;
    }
    unset($registry[$hook]['includes']);
  }

  $processors = array();
  if (!empty($registry[$hook][$process_key])) {
    foreach ($registry[$hook][$process_key] as $processor) {
      if (hex_function_exists($processor)) {
        $processors[] = $processor;
      }
    }
  }

  return $processors;
}

/**
 * Holds a referenced cache of the theme registry.
 * 
 * This may not be a valid concern but anything pointing to this function will
 * reference the same data. Since the registry needs to be available from
 * multiple functions, calling and caching its own local copy could
 * potentially require a lot more memory. If that's not the case, this should
 * not do any harm.
 */
function &plex_theme_registry_cache() {
  static $registry;
  if (!isset($registry)) {
    $registry = theme_get_registry();
  }
  return $registry;
}