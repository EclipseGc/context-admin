<?php

function context_admin_admin_section_content_form(&$form, &$form_state, $cache = NULL) {
  $form['admin_section_info'] = array(
    '#type' => 'markup',
    '#value' => t('Administrative sections are meant to be used in conjuncture with the /admin screen.  These allow you to add this item to /admin/my-path and add it to the navigation menu, and then as you add new items to this sub section of the navigation menu, this parent item will display them all properly on a sub page.'),
  );
  return $form;
}

function context_admin_admin_section_render_page($handler, $base_contexts, $args, $test = TRUE) {
  module_load_include('inc', 'system', 'system.admin');
  return system_admin_menu_block_page();
}