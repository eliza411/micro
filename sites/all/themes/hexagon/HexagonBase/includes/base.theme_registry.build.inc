<?php

/**
 * @file
 * Handles integration of PHP templates with the Drupal theme system.
 */

/**
 * Implements hook_theme().
 *
 * 'theme' is the engine.
 */
function theme_theme($existing, $type, $theme, $path) {

  // hook_theme() for plug-ins hosted in $theme being processed.
  // theme_plugin vs. base_theme_plugin follows similar $type conventions.
  $plugin_type = $theme === HEX_ACTIVE_THEME ? 'theme_plugin' : 'base_theme_plugin';
  foreach (hex()->listType('plugin', $theme) as $plugin) {
    if (hex_function_exists($plugin . '_theme')) {
      _theme_process_registry($existing, $plugin, $plugin_type, $plugin, hex()->path($plugin));
    }
  }

  // There can only be one implementation for a given theme hook function.
  // The latter in the list of $prefixes (@see drupal_find_theme_function())
  // has the final say on what is overridden.
  $registry  = drupal_find_theme_functions($existing, hex()->listType('plugin', $theme) + array($theme));
  // Follow the same order for overrides as noted above.
  $registry += hex_find_templates($existing,'.tpl.php', $theme);
  foreach (array_reverse(hex()->listType('plugin', $theme)) as $plugin) {
    $registry += hex_find_templates($existing, '.tpl.php', $plugin);
  }

  return $registry;
}

/**
 * Discovers overridden templates.
 *
 * This serves the same purpose as drupal_find_theme_templates() in Drupal
 * core but it knows the difference between themes and the plug-ins they may
 * host. This is an important distinction when a running theme hosts disabled
 * plug-ins. Disabled plug-ins hosting templates will be blocked from being
 * discovered.
 *
 * @see drupal_find_theme_templates()
 * 
 * @param $existing
 *   The existing set of theme hooks to test against.
 * @param $extension
 *   The extension that these templates will have.
 * @param $scope
 *  Can be 'theme', 'plugin' or 'all'. It can also be a specific plugin or
 *  theme key, whichever "owns" the file.
 */
function hex_find_templates($existing, $extension, $scope) {
  $templates = array();
  $patterns  = array();

  $files = new HexFileScanner();
  $files->setOption('key', 'filename');
  $files->scan('/' . preg_quote($extension) . '$/', $scope);
  foreach ($files as $file) {
    $name = substr($file->filename, 0, strpos($file->filename, '.'));
    $hook = strtr($name, '-', '_');
    if (isset($existing[$hook])) {
      $templates[$hook] = array(
        'template' => $name,
        'path'     => $file->dir,
      );
    }
    $patterns[$name] = $hook;
  }

  foreach ($existing as $hook => $info) {
    $pattern = isset($info['pattern']) ? $info['pattern'] : ($hook . '__');
    if (isset($info['base hook'])) {
      // If base hook is already set, skip.
      continue;
    }
    if ($hook_pattern_matches = preg_grep('/^' . $pattern . '/', $patterns)) {
      $arg_name = isset($info['variables']) ? 'variables' : 'render element';
      foreach ($hook_pattern_matches as $name => $hook_match) {
        $templates[$hook_match] = array(
          'template'  => $name,
          'path'      => $files[$name . $extension]->dir,
          $arg_name   => $info[$arg_name],
          'base hook' => $hook,
        );
      }
    }
  }

  return $templates;
}
