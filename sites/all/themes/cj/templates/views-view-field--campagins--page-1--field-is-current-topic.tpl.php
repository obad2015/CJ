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
$output = '';
$links = array();
$token = drupal_get_token();


$links[] = l(t('Add Contribution'), 'node/add/contribution/' . $row->tid);
$status = '';

if (isset($row->field_field_status[0]['raw']['value'])) {
  switch ($row->field_field_status[0]['raw']['value']) {
  case 0:
    $status = t('Activate [Suspended]');
    break;

  case 1:
    $status = t('Suspend');
    break;

  case 2:
    $status = t('Activate [Suggested]');
    break;
  }
}
else {
  watchdog('misc', format_string('Campaign !term doesn\'t have status, this should never happen', array('!term' => $row->tid)));  
}

$links[] = l($status, 'masry/campain-status/' . $row->tid . '/' . $token);

if (!$row->field_field_is_current_topic[0]['raw']['value']) {
  $links[] = l('Make Main', 'masry/campain-main/' . $row->tid . '/' . $token);
}

$links[] = l(t('Edit'), 'taxonomy/term/' . $row->tid . '/edit');

$output .= implode(' - ', $links);
$output = '<h2>' . $output . '</h2>';

print $output;
?>
