<?php

/**
 * Factory for building data of various types.
 *
 * @param $type
 *  Can be plugins, settings, features or registry.
 * @param $theme
 *  Optional theme key to build for. Defaults to the current theme context.
 */
function hex_base_generate($type, $theme = NULL) {
  $static = hex_static(__FUNCTION__);
  if (!isset($theme)) $theme = hex()->theme();
  $theme_data = &$static->data[$type][$theme];
  if (!isset($theme_data) && hex()->validate($theme, __FUNCTION__, 'theme')) {
    $types = array(
      'plugins'  => 'HexBuildPlugins',
      'settings' => 'HexBuildSettings',
      'features' => 'HexBuildFeatures',
      'registry' => 'HexBuildRegistry',
    );
    if (isset($types[$type])) {
      $theme_data = new $types[$type]($theme);
    }
    else {
      trigger_error(t('%arg is invalid.', array('%arg' => $type)), E_USER_WARNING);
    }
  }
  return $theme_data;
}

/**
 * Returns plug-in data for all plug-ins for the target theme.
 *
 * implements hook_hook_build()
 */
function hex_plugins_build(&$plugins, $existing, $build_target) {
  // TODO: Show status message on enabled/disabled state.
  $plugins = hex_base_generate('plugins')->getEnabledCopy();
  hex_base_generate('plugins')->displayErrors();
}

/**
 * Builds theme settings.
 *
 * implements hook_hook_build()
 */
function hex_settings_build(&$settings, $existing, $build_target) {
  // Settings depend on plug-ins so refresh before building.
  hex_cache_refresh('plugins');
  foreach (hex()->listType('theme') as $theme) {
    $build = clone hex_base_generate('settings', $theme);
    $build->add(hex_base_generate('features', $theme)->getToggleSettings());
    foreach ($build as $key => $setting) {
      $item = new stdClass();
      $item->theme = $theme;
      $item->value = $setting;
      // Check for dependencies or "parent conditionals", e.g., 'parent:child'.
      if ($parents = $build->getParents($key)) {
        $item->parents = $parents;
      }
      // Looped from base to child so child is always overwritten...
      $settings[$key][HEX_LAST_CHILD] = $item;
      // ...while base is set on first encounter.
      if (!isset($settings[$key][HEX_FIRST_PARENT])) {
        $settings[$key][HEX_FIRST_PARENT] = $item;
      }
      // Theme specific.
      $settings[$key][$theme] = $item;
    }
  }
}

/**
 * Builds the code registry.
 *
 * implements hook_hook_build()
 */
function hex_code_registry_build(&$registry, $existing, $build_target) {
  $registry = hex_base_generate('registry')->getArrayCopy();
}

/**
 * Casts strings into constants and integers/floats.
 */
function hex_auto_type($value) {
  if (is_string($value) && is_numeric($value)) {
    $value = ctype_digit($value) ? (int) $value : (float) $value;
  }
  elseif (is_string($value) && defined(strtoupper($value))) {
    $value = constant(strtoupper($value));
  }
  return $value;
}

