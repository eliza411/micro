<?php

/**
 * This file will be included when the plug-in is active. Set your custom
 * functions here.
 */

/**
 * Implements hook_preprocess_html()
 */
function example_plugin_preprocess_html(&$variables) {
  if (hex_settings('example_plugin.toggle_color_me')) {
    $variables['classes_array'][] = 'color-me';
  }
  if (hex_settings('example_plugin.toggle_flip_me')) {
    $variables['classes_array'][] = 'flip-me';
  }
}
