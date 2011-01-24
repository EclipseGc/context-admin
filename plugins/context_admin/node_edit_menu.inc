<?php
//$Id: node_edit_menu.inc,v 1.1 2010/09/12 06:55:20 eclipsegc Exp $

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Edit Node'),
  'description' => t('Places a node edit form in a specified location.  Requires a node context to be passed to it.'),
  'required context' => new ctools_context_required(t('Node'), 'node'),
);


function context_admin_node_edit_menu_content_form(&$form, &$form_state, $cache = NULL) {
  return $form;
}

function context_admin_node_edit_menu_content_form_submit(&$form, &$form_state) {
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_node_edit_menu_render_page($handler, $contexts, $args, $test = TRUE) {
  $node = $contexts[$handler->conf['submitted_context']]->data;
  module_load_include('inc', 'node', 'node.pages');
  return drupal_get_form($node->type .'_node_form', $node);
}