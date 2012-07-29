<?php
/**
 * @file
 * pdx_micro_categories.features.menu_custom.inc
 */

/**
 * Implements hook_menu_default_menu_custom().
 */
function pdx_micro_categories_menu_default_menu_custom() {
  $menus = array();

  // Exported menu: menu-event-categories.
  $menus['menu-event-categories'] = array(
    'menu_name' => 'menu-event-categories',
    'title' => 'Event Categories',
    'description' => '',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Event Categories');


  return $menus;
}
