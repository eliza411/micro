<?php

/**
 * @file
 * Provides debugging functions for theme development. Remember to disable
 * this plug-in for live sites.
 */

/**
 * Implements hook_initialize().
 */
function plex_debugger_initialize() {
  if (plex_debug_mode()) {
    watchdog('plex_debugger', '!theme is in development mode.',
             array('!theme' => hex()->info('name', hex()->theme())),
             WATCHDOG_NOTICE);
  }
  if (plex_debug_mode('no_cache')) {
    watchdog('plex_debugger',
             'Theme caching is disabled for !theme.',
             array('!theme' => hex()->info('name', hex()->theme())),
             WATCHDOG_NOTICE);

    hex_theme_rebuild();
    hex_cache_refresh();
    hex_code_category('build');
  }
  // TODO: Look at some possible caches to clear in drupal_flush_all_caches()
  // and provide an option for it.
}

/**
 * Checks for debug settings.
 *
 * @param $child_key
 *  Optional sub-settings.
 */
function plex_debug_mode($child_key = NULL) {
  $setting = 'plex_debugger.development_mode';
  if (isset($child_key)) {
    $setting .= ":$child_key";
  }
  return hex_settings($setting, hex()->theme());
}

/**
 * Implements hook_form_theme_settings().
 *
 * This function called when on the theme specific settings form.
 *
 * @param $form
 *  Nested array of form elements that comprise the form.
 * @param $form_state
 *  A keyed array containing the current state of the form.
 */
function plex_debugger_form_theme_settings(&$form, &$form_state) {
  // TODO: this needs a redo.

  hex_code_category('build');
  $settings = hex_base_generate('settings');
  $dev_key  = 'plex_debugger.development_mode';
  $no_cache = "$dev_key:no_cache";

  $form['development'] = array(
    '#title' => t('Development Settings'),
    '#type'  => 'fieldset',
  );
  $form['development'][$settings->fapiKey($dev_key)] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Development Mode'),
    '#default_value' => hex_settings($dev_key, hex()->theme()),
  );
  // Group development options and hide when parent setting is disabled.
  $form['development']['options'] = array(
    '#type' => 'container',
    '#states'        => array(
      'visible'  => array(
        ':input[name=' . $settings->fapiKey($dev_key) . ']' => array('checked' => TRUE),
      ),
      'invisible' => array(
        ':input[name=' . $settings->fapiKey($dev_key) . ']' => array('checked' => FALSE),
      ),
    ),
  );
  $form['development']['options'][$settings->fapiKey($no_cache)] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Disable Theme Caching'),
    '#default_value' => hex_settings($no_cache, hex()->theme()),
  );
}
