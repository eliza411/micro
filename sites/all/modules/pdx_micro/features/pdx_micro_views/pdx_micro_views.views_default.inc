<?php
/**
 * @file
 * pdx_micro_views.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function pdx_micro_views_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'calendar';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Calendar';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Calendar';
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['pager']['options']['items_per_page'] = '10';
  $handler->display->display_options['style_plugin'] = 'fullcalendar';
  $handler->display->display_options['style_options']['fullcalendar_options']['disableDragging'] = TRUE;
  $handler->display->display_options['style_options']['fullcalendar_options']['disableResizing'] = TRUE;
  $handler->display->display_options['style_options']['fullcalendar_options']['dayClick'] = TRUE;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = '';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Date */
  $handler->display->display_options['fields']['field_date']['id'] = 'field_date';
  $handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
  $handler->display->display_options['fields']['field_date']['field'] = 'field_date';
  $handler->display->display_options['fields']['field_date']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_date']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_date']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_date']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['field_date']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_date']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_date']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_date']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_date']['settings'] = array(
    'format_type' => 'long',
    'fromto' => 'both',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  $handler->display->display_options['fields']['field_date']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_date']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_date']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_date']['delta_first_last'] = 0;
  $handler->display->display_options['fields']['field_date']['field_api_classes'] = 0;
  /* Sort criterion: Content: Post date */
  $handler->display->display_options['sorts']['created']['id'] = 'created';
  $handler->display->display_options['sorts']['created']['table'] = 'node';
  $handler->display->display_options['sorts']['created']['field'] = 'created';
  $handler->display->display_options['sorts']['created']['order'] = 'DESC';
  /* Filter criterion: Content: Published or admin */
  $handler->display->display_options['filters']['status_extra']['id'] = 'status_extra';
  $handler->display->display_options['filters']['status_extra']['table'] = 'node';
  $handler->display->display_options['filters']['status_extra']['field'] = 'status_extra';
  $handler->display->display_options['filters']['status_extra']['group'] = 1;
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'event' => 'event',
  );
  $handler->display->display_options['filters']['type']['group'] = 1;

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'calendar';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Today';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $export['calendar'] = $view;

  $view = new view;
  $view->name = 'events';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'node';
  $view->human_name = 'Events';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Today\'s Events';
  $handler->display->display_options['access']['type'] = 'role';
  $handler->display->display_options['access']['role'] = array(
    2 => '2',
  );
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '25';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'field_event_icon' => 'field_date',
    'field_date' => 'field_date',
    'title' => 'title',
    'body' => 'title',
    'field_location' => 'title',
    'capacity_used' => 'capacity_total',
    'capacity_total' => 'capacity_total',
    'registration_settings' => 'capacity_total',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'field_event_icon' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_date' => array(
      'align' => '',
      'separator' => '<br />',
      'empty_column' => 0,
    ),
    'title' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => 'views-align-left',
      'separator' => '<br />',
      'empty_column' => 0,
    ),
    'body' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'field_location' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'capacity_used' => array(
      'align' => '',
      'separator' => '/',
      'empty_column' => 0,
    ),
    'capacity_total' => array(
      'align' => '',
      'separator' => '<br />',
      'empty_column' => 0,
    ),
    'registration_settings' => array(
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* No results behavior: Global: Text area */
  $handler->display->display_options['empty']['area']['id'] = 'area';
  $handler->display->display_options['empty']['area']['table'] = 'views';
  $handler->display->display_options['empty']['area']['field'] = 'area';
  $handler->display->display_options['empty']['area']['label'] = 'No events';
  $handler->display->display_options['empty']['area']['empty'] = FALSE;
  $handler->display->display_options['empty']['area']['content'] = 'No events have been scheduled for this day.';
  $handler->display->display_options['empty']['area']['format'] = 'filtered_html';
  $handler->display->display_options['empty']['area']['tokenize'] = 1;
  /* Relationship: Registration: Node to Registration */
  $handler->display->display_options['relationships']['registration_rel']['id'] = 'registration_rel';
  $handler->display->display_options['relationships']['registration_rel']['table'] = 'node';
  $handler->display->display_options['relationships']['registration_rel']['field'] = 'registration_rel';
  $handler->display->display_options['relationships']['registration_rel']['required'] = 0;
  /* Relationship: Registration Settings: Node to Registration Settings */
  $handler->display->display_options['relationships']['registration_settings_rel']['id'] = 'registration_settings_rel';
  $handler->display->display_options['relationships']['registration_settings_rel']['table'] = 'node';
  $handler->display->display_options['relationships']['registration_settings_rel']['field'] = 'registration_settings_rel';
  $handler->display->display_options['relationships']['registration_settings_rel']['required'] = 0;
  /* Field: Content: Event Icon */
  $handler->display->display_options['fields']['field_event_icon']['id'] = 'field_event_icon';
  $handler->display->display_options['fields']['field_event_icon']['table'] = 'field_data_field_event_icon';
  $handler->display->display_options['fields']['field_event_icon']['field'] = 'field_event_icon';
  $handler->display->display_options['fields']['field_event_icon']['label'] = 'Event icon';
  $handler->display->display_options['fields']['field_event_icon']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_event_icon']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_event_icon']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_event_icon']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_event_icon']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_event_icon']['click_sort_column'] = 'fid';
  $handler->display->display_options['fields']['field_event_icon']['settings'] = array(
    'image_style' => 'thumbnail',
    'image_link' => '',
  );
  $handler->display->display_options['fields']['field_event_icon']['field_api_classes'] = 0;
  /* Field: Content: Date */
  $handler->display->display_options['fields']['field_date']['id'] = 'field_date';
  $handler->display->display_options['fields']['field_date']['table'] = 'field_data_field_date';
  $handler->display->display_options['fields']['field_date']['field'] = 'field_date';
  $handler->display->display_options['fields']['field_date']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_date']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_date']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_date']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_date']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['field_date']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_date']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_date']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_date']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_date']['settings'] = array(
    'format_type' => 'short',
    'fromto' => 'value',
    'multiple_number' => '',
    'multiple_from' => '',
    'multiple_to' => '',
  );
  $handler->display->display_options['fields']['field_date']['group_rows'] = 1;
  $handler->display->display_options['fields']['field_date']['delta_offset'] = '0';
  $handler->display->display_options['fields']['field_date']['delta_reversed'] = 0;
  $handler->display->display_options['fields']['field_date']['delta_first_last'] = 0;
  $handler->display->display_options['fields']['field_date']['field_api_classes'] = 0;
  /* Field: Content: Title */
  $handler->display->display_options['fields']['title']['id'] = 'title';
  $handler->display->display_options['fields']['title']['table'] = 'node';
  $handler->display->display_options['fields']['title']['field'] = 'title';
  $handler->display->display_options['fields']['title']['label'] = 'Event';
  $handler->display->display_options['fields']['title']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['title']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['title']['alter']['external'] = 0;
  $handler->display->display_options['fields']['title']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['title']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['title']['alter']['word_boundary'] = 0;
  $handler->display->display_options['fields']['title']['alter']['ellipsis'] = 0;
  $handler->display->display_options['fields']['title']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['title']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['title']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['title']['alter']['html'] = 0;
  $handler->display->display_options['fields']['title']['element_type'] = 'h2';
  $handler->display->display_options['fields']['title']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['title']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['title']['hide_empty'] = 0;
  $handler->display->display_options['fields']['title']['empty_zero'] = 0;
  $handler->display->display_options['fields']['title']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['title']['link_to_node'] = 1;
  /* Field: Content: Body */
  $handler->display->display_options['fields']['body']['id'] = 'body';
  $handler->display->display_options['fields']['body']['table'] = 'field_data_body';
  $handler->display->display_options['fields']['body']['field'] = 'body';
  $handler->display->display_options['fields']['body']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['body']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['body']['alter']['external'] = 0;
  $handler->display->display_options['fields']['body']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['body']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['body']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['body']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['body']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['body']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['body']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['body']['alter']['html'] = 0;
  $handler->display->display_options['fields']['body']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['body']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['body']['hide_empty'] = 0;
  $handler->display->display_options['fields']['body']['empty_zero'] = 0;
  $handler->display->display_options['fields']['body']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['body']['type'] = 'text_summary_or_trimmed';
  $handler->display->display_options['fields']['body']['settings'] = array(
    'trim_length' => '600',
  );
  $handler->display->display_options['fields']['body']['field_api_classes'] = 0;
  /* Field: Content: Location */
  $handler->display->display_options['fields']['field_location']['id'] = 'field_location';
  $handler->display->display_options['fields']['field_location']['table'] = 'field_data_field_location';
  $handler->display->display_options['fields']['field_location']['field'] = 'field_location';
  $handler->display->display_options['fields']['field_location']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['external'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['field_location']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['field_location']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['field_location']['alter']['html'] = 0;
  $handler->display->display_options['fields']['field_location']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['field_location']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['field_location']['hide_empty'] = 0;
  $handler->display->display_options['fields']['field_location']['empty_zero'] = 0;
  $handler->display->display_options['fields']['field_location']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['field_location']['settings'] = array(
    'include_map' => 1,
    'iframe_width' => '100',
    'iframe_height' => '100',
    'include_link' => 0,
    'link_text' => 'View larger map',
    'zoom_level' => '14',
    'information_bubble' => 1,
    'include_text' => 0,
  );
  $handler->display->display_options['fields']['field_location']['field_api_classes'] = 0;
  /* Field: Registration Settings: Slots Used */
  $handler->display->display_options['fields']['capacity_used']['id'] = 'capacity_used';
  $handler->display->display_options['fields']['capacity_used']['table'] = 'node';
  $handler->display->display_options['fields']['capacity_used']['field'] = 'capacity_used';
  $handler->display->display_options['fields']['capacity_used']['label'] = '# Registered';
  $handler->display->display_options['fields']['capacity_used']['exclude'] = TRUE;
  $handler->display->display_options['fields']['capacity_used']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['external'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['capacity_used']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['capacity_used']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['capacity_used']['alter']['html'] = 0;
  $handler->display->display_options['fields']['capacity_used']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['capacity_used']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['capacity_used']['hide_empty'] = 0;
  $handler->display->display_options['fields']['capacity_used']['empty_zero'] = 0;
  $handler->display->display_options['fields']['capacity_used']['hide_alter_empty'] = 1;
  /* Field: Registration Settings: Slots Total */
  $handler->display->display_options['fields']['capacity_total']['id'] = 'capacity_total';
  $handler->display->display_options['fields']['capacity_total']['table'] = 'registration_entity';
  $handler->display->display_options['fields']['capacity_total']['field'] = 'capacity_total';
  $handler->display->display_options['fields']['capacity_total']['relationship'] = 'registration_settings_rel';
  $handler->display->display_options['fields']['capacity_total']['label'] = '# Registered';
  $handler->display->display_options['fields']['capacity_total']['alter']['alter_text'] = 1;
  $handler->display->display_options['fields']['capacity_total']['alter']['text'] = '[capacity_used]/[capacity_total]';
  $handler->display->display_options['fields']['capacity_total']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['external'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['capacity_total']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['capacity_total']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['capacity_total']['alter']['html'] = 0;
  $handler->display->display_options['fields']['capacity_total']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['capacity_total']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['capacity_total']['hide_empty'] = 0;
  $handler->display->display_options['fields']['capacity_total']['empty_zero'] = 0;
  $handler->display->display_options['fields']['capacity_total']['hide_alter_empty'] = 1;
  /* Field: Registration Settings: Registration Settings Link */
  $handler->display->display_options['fields']['registration_settings']['id'] = 'registration_settings';
  $handler->display->display_options['fields']['registration_settings']['table'] = 'node';
  $handler->display->display_options['fields']['registration_settings']['field'] = 'registration_settings';
  $handler->display->display_options['fields']['registration_settings']['label'] = 'Register';
  $handler->display->display_options['fields']['registration_settings']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['registration_settings']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['registration_settings']['alter']['more_link'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['registration_settings']['alter']['html'] = 0;
  $handler->display->display_options['fields']['registration_settings']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['registration_settings']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['registration_settings']['hide_empty'] = 0;
  $handler->display->display_options['fields']['registration_settings']['empty_zero'] = 0;
  $handler->display->display_options['fields']['registration_settings']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['registration_settings']['text'] = 'Register now';
  /* Sort criterion: Content: Date -  start date (field_date) */
  $handler->display->display_options['sorts']['field_date_value']['id'] = 'field_date_value';
  $handler->display->display_options['sorts']['field_date_value']['table'] = 'field_data_field_date';
  $handler->display->display_options['sorts']['field_date_value']['field'] = 'field_date_value';
  /* Contextual filter: Content: Date -  start date (field_date) */
  $handler->display->display_options['arguments']['field_date_value']['id'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['table'] = 'field_data_field_date';
  $handler->display->display_options['arguments']['field_date_value']['field'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['summary']['format'] = 'default_summary';
  /* Filter criterion: Content: Type */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'node';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['value'] = array(
    'event' => 'event',
  );
  /* Filter criterion: Content: Published or admin */
  $handler->display->display_options['filters']['status_extra']['id'] = 'status_extra';
  $handler->display->display_options['filters']['status_extra']['table'] = 'node';
  $handler->display->display_options['filters']['status_extra']['field'] = 'status_extra';

  /* Display: Today */
  $handler = $view->new_display('page', 'Today', 'page');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Today\'s Events';
  $handler->display->display_options['enabled'] = FALSE;
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Date -  start date (field_date) */
  $handler->display->display_options['arguments']['field_date_value']['id'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['table'] = 'field_data_field_date';
  $handler->display->display_options['arguments']['field_date_value']['field'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_date_value']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_date_value']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_date_value']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_date_value']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['path'] = 'events/today';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Today';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;

  /* Display: Feed */
  $handler = $view->new_display('feed', 'Feed', 'feed');
  $handler->display->display_options['pager']['type'] = 'some';
  $handler->display->display_options['style_plugin'] = 'rss';
  $handler->display->display_options['row_plugin'] = 'node_rss';
  $handler->display->display_options['path'] = 'events.xml';
  $handler->display->display_options['displays'] = array(
    'default' => 'default',
    'page' => 'page',
  );

  /* Display: Date browser */
  $handler = $view->new_display('date_nav', 'Date browser', 'date_nav_1');

  /* Display: All Events */
  $handler = $view->new_display('page', 'All Events', 'page_1');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'All Events';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  $handler->display->display_options['path'] = 'events';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Events';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $handler->display->display_options['tab_options']['type'] = 'normal';
  $handler->display->display_options['tab_options']['title'] = 'Events';
  $handler->display->display_options['tab_options']['weight'] = '0';
  $handler->display->display_options['tab_options']['name'] = 'main-menu';

  /* Display: Specific Days */
  $handler = $view->new_display('page', 'Specific Days', 'page_2');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Today\'s Events';
  $handler->display->display_options['defaults']['arguments'] = FALSE;
  /* Contextual filter: Content: Date -  start date (field_date) */
  $handler->display->display_options['arguments']['field_date_value']['id'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['table'] = 'field_data_field_date';
  $handler->display->display_options['arguments']['field_date_value']['field'] = 'field_date_value';
  $handler->display->display_options['arguments']['field_date_value']['default_action'] = 'default';
  $handler->display->display_options['arguments']['field_date_value']['title_enable'] = 1;
  $handler->display->display_options['arguments']['field_date_value']['title'] = 'Events for %1';
  $handler->display->display_options['arguments']['field_date_value']['breadcrumb_enable'] = 1;
  $handler->display->display_options['arguments']['field_date_value']['breadcrumb'] = '%1';
  $handler->display->display_options['arguments']['field_date_value']['default_argument_type'] = 'raw';
  $handler->display->display_options['arguments']['field_date_value']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['field_date_value']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['field_date_value']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['field_date_value']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['field_date_value']['specify_validation'] = 1;
  $handler->display->display_options['arguments']['field_date_value']['year_range'] = '2012:2013';
  $handler->display->display_options['arguments']['field_date_value']['granularity'] = 'day';
  $handler->display->display_options['arguments']['field_date_value']['use_fromto'] = 'no';
  $handler->display->display_options['path'] = 'events/%';
  $handler->display->display_options['menu']['weight'] = '0';
  $handler->display->display_options['menu']['name'] = 'main-menu';
  $handler->display->display_options['menu']['context'] = 0;
  $export['events'] = $view;

  return $export;
}
