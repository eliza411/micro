<?php
/**
 * @file
 * pdx_micro_categories.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function pdx_micro_categories_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'pdx_micro_global';
  $context->description = '';
  $context->tag = 'global';
  $context->conditions = array(
    'sitewide' => array(
      'values' => array(
        1 => 1,
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'menu-menu-event-categories' => array(
          'module' => 'menu',
          'delta' => 'menu-event-categories',
          'region' => 'header',
          'weight' => '-10',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('global');
  $export['pdx_micro_global'] = $context;

  return $export;
}
