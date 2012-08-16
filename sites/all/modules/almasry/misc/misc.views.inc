<?php


/**
 * Implements hook_views_data().
 */
function misc_views_data() {
  $data = array();

  // adding our new filter
  $data['node']['filter_media'] = array(
    'title' => t('Contribution Type'),
    'group' => t('Almasry'),
    'filter' => array (
      'handler' => 'misc_handler_filter_media_field',
    ),
    'help' => t('Filter contribution media field (Video, Audio, Images)'),
  );

  return $data;
}

/**
 * Implments hook_views_handlers
 * provide pathes for includes files contains the handler definitions
 */
function misc_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'misc') . '/handlers',
    ),
    'handlers' => array(
      // Filters
      'misc_handler_filter_media_field' => array(
        'parent' => 'views_handler_filter',
      )
    )
  );
}


