<?php

/**
 * @file
 * This file is empty by default because the base theme chain (Alpha & Omega) provides
 * all the basic functionality. However, in case you wish to customize the output that Drupal
 * generates through Alpha & Omega this file is a good place to do so.
 * 
 * Alpha comes with a neat solution for keeping this file as clean as possible while the code
 * for your subtheme grows. Please read the README.txt in the /preprocess and /process subfolders
 * for more information on this topic.
 */

function cj_breadcrumb($variables) {
  $breadcrumb = $variables['breadcrumb'];
  if (!empty($breadcrumb)) {
    array_shift($breadcrumb);
  }
  if (arg(0) == 'node' && is_numeric(arg(1)) ) {
    $node = node_load(arg(1));
    if ($node->type == 'contribution') {
      $topic = field_get_items('node', $node, 'field_topic');
      $term =  taxonomy_term_load($topic[0]['tid']);
      if (is_object($term)) {
        $breadcrumb[] = l($term->name, 'taxonomy/term/' . $term->tid);
        $breadcrumb[] = l($node->title, 'node/' . $node->nid);
        return '<div class="breadcrumb">' . implode(' » ', $breadcrumb) . '</div>';
      }
    }
  }
}

