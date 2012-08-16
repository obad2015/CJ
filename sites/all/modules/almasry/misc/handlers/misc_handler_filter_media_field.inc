<?php

/*
 * @file
 * Handles filter media field type
 */
class misc_handler_filter_media_field extends views_handler_filter {
  var $types = array();

  function init(&$view, $options) {
    parent::init($view, $options);
    $this->types = array(
      'any' => t(' - Any - '),
      'audio' => t('Audio'),
      'video' => t('Video'),
      'image' => t('Image'),
    );
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['value']['filter_media'] = array('default' => 'any');
    return $options;
  }

  function value_form(&$form, &$form_state) {
    $form['value'] = array();
    $form['value']['filter_media'] = $this->base_form_item();
  }

  // i made this field external from value_form function in case we needed to expose it later
  function base_form_item() {
    return array(
      '#type' => 'radios',
      '#title' => t('Media Type'),
      '#description' => t('Output will be limited to the media type you select.'),
      '#options' => $this->types,
      '#default_value' => !empty($this->value['filter_media']) ? $this->value['filter_media'] : 'any',
    );
  }

  function exposed_form(&$form, &$form_state) {
    if (empty($this->options['exposed'])) {
      return;
    }
    if (!empty($this->options['expose']['identifier'])) {
      $value = $this->options['expose']['identifier'];

      $form[$value]['filter_media'] = $this->base_form_item();
      unset($form[$value]['filter_media']['#title'], $form[$value]['filter_media']['#description']);
    }
    
    return $form;
  }

  function admin_summary() {
    return 'is ' . $this->types[$this->value['filter_media']];
  }

  function query() {

    $val = is_array($this->value) ? $this->value['filter_media'] : $this->value;
    
    if (empty($val) || $val == 'any') {
      return;
    }
    else {
      // add join to file_managed table
      $table = $this->ensure_my_table();
      $join = new views_join();
      $join->construct('file_managed', 'field_data_field_media', 'field_media_fid', 'fid');
      $this->query->ensure_table('file_managed', $this->relationship, $join);

       $this->query->add_where($this->options['group'], 'file_managed.type',  $val, 'LIKE');
    }
  }
}
