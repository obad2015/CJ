<?php
/**
 * @file views-view-fields.tpl.php
 * Default simple view template to all the fields as a row.
 *
 * - $view: The view in use.
 * - $fields: an array of $field objects. Each one contains:
 *   - $field->content: The output of the field.
 *   - $field->raw: The raw data for the field, if it exists. This is NOT output safe.
 *   - $field->class: The safe class id to use.
 *   - $field->handler: The Views field handler object controlling this field. Do not use
 *     var_export to dump this object, as it can't handle the recursion.
 *   - $field->inline: Whether or not the field should be inline.
 *   - $field->inline_html: either div or span based on the above flag.
 *   - $field->wrapper_prefix: A complete wrapper containing the inline_html to use.
 *   - $field->wrapper_suffix: The closing tag for the wrapper.
 *   - $field->separator: an optional separator that may appear before a field.
 *   - $field->label: The wrap label text to use.
 *   - $field->label_html: The full HTML of the label to use including
 *     configured element type.
 * - $row: The raw result object from the query, with all data it fetched.
 *
 * @ingroup views_templates
 */
?>
<div class="mediaBlock">
	<?php
	foreach ($fields as $id => $field){
	?>
	<div class="mb5">
	<?php
		if($id=='field_media'){
			$fields_ids_car = $row->field_field_media_1;
			echo '<ul class="mediaList">';
			foreach($fields_ids_car as $k=>$v){
				$href = $v['rendered']['#markup'];
				$filename = $v['raw']['filename'];
				$uri = $row->field_field_media[$k]['rendered']['file']['#path'];
				$thumSrc = image_style_url('youtube_120_90',$uri);
				if($v['raw']['type']=='video'){
					//http://youtube.com/watch?v=jiJV2ApYS-I
					$url = parse_url($href);
					$query = array();
					parse_str($url['query'],$query);
					$href = 'http://www.youtube.com/embed/'.$query['v'].'?autoplay=1&width=700&height=394&iframe=true';
				}
				echo '<li><a class="colorbox-load" target="_blank" href="'.$href.'"><img src="'.$thumSrc.'" alt="'.$filename.'" /></a></li>';
			}
			echo '</ul>';
		}else{
			if (!empty($field->separator)){print $field->separator;}
			print $field->wrapper_prefix;
			print $field->label_html;
			print $field->content;
			print $field->wrapper_suffix;
		}
	?>
	</div>
	<?php
	}
	?>
</div>