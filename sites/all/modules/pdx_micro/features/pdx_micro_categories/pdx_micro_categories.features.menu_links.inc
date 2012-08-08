<?php
/**
 * @file
 * pdx_micro_categories.features.menu_links.inc
 */

/**
 * Implements hook_menu_default_menu_links().
 */
function pdx_micro_categories_menu_default_menu_links() {
  $menu_links = array();

  // Exported menu link: menu-event-categories:taxonomy/term/10
  $menu_links['menu-event-categories:taxonomy/term/10'] = array(
    'menu_name' => 'menu-event-categories',
    'link_path' => 'taxonomy/term/10',
    'router_path' => 'taxonomy/term/%',
    'link_title' => 'Alcohol-Free',
    'options' => array(
      'attributes' => array(
        'class' => array(
          0 => 'micro-alcohol-free',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-48',
  );
  // Exported menu link: menu-event-categories:taxonomy/term/14
  $menu_links['menu-event-categories:taxonomy/term/14'] = array(
    'menu_name' => 'menu-event-categories',
    'link_path' => 'taxonomy/term/14',
    'router_path' => 'taxonomy/term/%',
    'link_title' => 'Professional',
    'options' => array(
      'attributes' => array(
        'class' => array(
          0 => 'micro-professional',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-49',
  );
  // Exported menu link: menu-event-categories:taxonomy/term/2
  $menu_links['menu-event-categories:taxonomy/term/2'] = array(
    'menu_name' => 'menu-event-categories',
    'link_path' => 'taxonomy/term/2',
    'router_path' => 'taxonomy/term/%',
    'link_title' => 'Family Friendly',
    'options' => array(
      'attributes' => array(
        'class' => array(
          0 => 'micro-family-friendly',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-50',
  );
  // Exported menu link: menu-event-categories:taxonomy/term/9
  $menu_links['menu-event-categories:taxonomy/term/9'] = array(
    'menu_name' => 'menu-event-categories',
    'link_path' => 'taxonomy/term/9',
    'router_path' => 'taxonomy/term/%',
    'link_title' => 'Liquor-Centric',
    'options' => array(
      'attributes' => array(
        'class' => array(
          0 => 'micro-liquor-centric',
        ),
      ),
    ),
    'module' => 'menu',
    'hidden' => '0',
    'external' => '0',
    'has_children' => '0',
    'expanded' => '0',
    'weight' => '-47',
  );
  // Translatables
  // Included for use with string extractors like potx.
  t('Alcohol-Free');
  t('Family Friendly');
  t('Liquor-Centric');
  t('Professional');


  return $menu_links;
}
