<?php
 /**
  * This template is used to print a single field in a view. It is not
  * actually used in default Views, as this is registered as a theme
  * function which has better performance. For single overrides, the
  * template is perfectly okay.
  *
  * Variables available:
  * - $view: The view object
  * - $field: The field handler object that can process the input
  * - $row: The raw SQL result that can be used
  * - $output: The processed output that will normally be used.
  *
  * When fetching output from the $row, this construct should be used:
  * $data = $row->{$field->field_alias}
  *
  * The above will guarantee that you'll always get the correct data,
  * regardless of any changes in the aliasing that might happen if
  * the view is modified.
  */
?>
<?php 
  $nid = $row->nid;
  $node = node_load($nid);
  $options = array('attributes' => array('class' => array('use-ajax')));
  $token = drupal_get_token();

  // node workflow choices
  $choices = workflow_field_choices($node);
  $current_state = workflow_get_workflow_states_by_sid($row->workflow_node_sid);
  if (is_object($current_state)) {
    $state_name = $current_state->state;
  }
  else {
    $state_name = '';
  }
  $output = array();
  
  // if user can accept
  if (isset($choices[3])) {
    $output[] = l(t('Accept'), 'masry/nojs/contribution-workflow-state/3/' . $row->nid . '/' . $token, $options);
  }
  
  // if user can reject
  if (isset($choices[4])) {
    $output[] = l(t('Reject'), 'masry/nojs/contribution-workflow-state/4/' . $row->nid . '/' . $token, $options);
  }
  
  // if user can delete
  if (isset($choices[5])) {
    $output[] = l(t('Delete'), 'masry/nojs/contribution-workflow-state/5/' . $row->nid. '/' . $token, $options);
  }
  
  print "[$state_name] " . implode(' - ', $output) . '';
					
?>

