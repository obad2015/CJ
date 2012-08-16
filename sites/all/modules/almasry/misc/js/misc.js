jQuery(document).ready(function () {
	jQuery('#views-form-manage-contributions-page .views-field-field-media').addClass('container-inline');
	
	jQuery('td.views-field-field-media').each(function() {
		jQuery(this).find('a').each(function(index, element) {
			href = jQuery(this).attr('href');
			
			jQuery(this).attr('href', href + '/' + index);
		});
	});
});