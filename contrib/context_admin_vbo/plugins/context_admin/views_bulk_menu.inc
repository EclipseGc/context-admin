<?php
//$Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Views Administration'),
  'description' => t('Provides a generic Views Bulk Operations based view that should work for most node type needs.'),
);

function context_admin_vbo_views_bulk_menu_content_form(&$form, &$form_state, $cache = NULL) {
  if (!is_null($form_state['handler_id'])) {
    $default_type = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_node_type'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_node_type'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_node_type'];
    $default_pub = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_published'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_published'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_published'];
  }
  else {
    $default_type = $form_state['page']->new_handler->conf['context_admin_vbo_views_bulk_node_type'];
    $default_pub = $form_state['page']->new_handler->conf['context_admin_vbo_views_bulk_published'];
  }
  $types = node_get_types();
  foreach ($types as $key => $type) {
    $options[$key] = $type->name;
  }
  $form['context_admin_vbo_views_bulk_node_type'] = array(
    '#type' => 'radios',
    '#title' => t('Node Type'),
    '#required' => TRUE,
    '#options' => $options,
    '#default_value' => $default_type,
  );
  $form['context_admin_vbo_views_bulk_published'] = array(
    '#type' => 'radios',
    '#title' => t('Published Options'),
    '#required' => TRUE,
    '#options' => array(
      'published' => t('Published'),
      'unpublished' => t('Unpublished'),
      'both' => t('both'),
    ),
    '#default_value' => $default_pub,
  );
  return $form;
}

function context_admin_vbo_views_bulk_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if (!is_null($form_state['handler_id'])) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_node_type'] = $form_state['values']['context_admin_vbo_views_bulk_node_type'];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vbo_views_bulk_published'] = $form_state['values']['context_admin_vbo_views_bulk_published'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_vbo_views_bulk_node_type'] = $form_state['values']['context_admin_vbo_views_bulk_node_type'];
    $form_state['page']->new_handler->conf['context_admin_vbo_views_bulk_published'] = $form_state['values']['context_admin_vbo_views_bulk_published'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_vbo_views_bulk_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
  return views_embed_view($handler->name);
}

function context_admin_vbo_views_bulk_menu_save($handler, $update) {
  views_invalidate_cache();
}

function context_admin_vbo_views_bulk_menu_delete($handler, $update) {
  views_invalidate_cache();
}
