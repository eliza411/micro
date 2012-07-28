<?php

/**
 * @file
 * Theme api settings for Hexagon and any of its child themes.
 */
include_once DRUPAL_ROOT . '/' . drupal_get_path('theme', 'hex') . '/hex.theme';

/**
 * Implements hook_form_system_theme_settings_alter().
 */
function hex_form_system_theme_settings_alter(&$form, &$form_state, $form_id = NULL) {
  // Work around a core double invoke bug: http://drupal.org/node/1135794
  if (isset($form_id)) return;

  // Set the theme context to theme connected to the current settings form.
  hex()->setTheme()->themeForm();
  // Clear cache for settings.
  hex_cache_refresh('settings');
  // Load code for building.
  hex_code_category('build');

  // Used to check for default feature toggles.
  $has_default_toggles = (bool) element_children($form['theme_settings']);

  // Builds toggle checkboxes.
  $features = hex_base_generate('features');
  foreach ($features as $feature) {
    if ($features->isSupported($feature)) {
      $form['theme_settings'][$feature->toggleKey] = array(
        '#type'          => 'checkbox',
        '#title'         => t($feature->title),
        '#disabled'      => $feature->disabled,
        '#default_value' => $feature->value,
        '#weight'        => $feature->weight,
      );
    }
  }
  // When core feature toggles are disabled '#access' is disabled. If custom
  // features are enabled and excludes core features, set this back to true.
  // @see system_theme_settings()
  if (!$has_default_toggles && element_children($form['theme_settings'])
  &&  $access = &$form['theme_settings']['#access'] == FALSE) {
    $access = TRUE;
  }

  // Loop through active plug-ins and themes. Themes come later since they
  // have greater overriding power.
  foreach (hex()->listType('theme') as $theme) {
    foreach (hex()->listType('plugin', $theme) + array($theme) as $component) {
      // Use hook_form_theme_settings() for form modifications by themes and
      // plug-ins. It acts like hook_form_system_theme_settings_alter() but it
      // is active for theme specific forms whether the theme is running or not
      // and the double invoke bug is prevented.
      if (hex_function_exists($form_settings = "{$component}_form_theme_settings")) {
        $form_settings($form, $form_state);
      }
      // Provides a hook_form_theme_settings_submit() post submit functions
      // for all components.
      if (hex_function_exists($form_submit = "{$component}_form_theme_settings_submit")) {
        $form['#submit'][] = $form_submit;
      }
    }
  }

  // Reset to previous theme context.
  hex()->setTheme()->revert();

}

/**
 * Implements hook_form_theme_settings_submit().
 */
function hex_form_theme_settings_submit(&$form, &$form_state) {
  // Clear the registry and hex caches for the theme connected to the form.
  hex_theme_rebuild(reset($form_state['build_info']['args']));
}
