<?php
//$Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Create Node with auto node reference'),
  'description' => t('Creates a node with an automatic reference back to its parent.'),
  'required context' => new ctools_context_required(t('Node'), 'node'),
);


function context_admin_noderef_create_node_content_form(&$form, &$form_state, $cache = NULL) {
  ctools_include('dependent');
  if (!is_null($form_state['handler_id'])) {
    $default = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_options_items'];
    $type_fields = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_content_types'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_content_types'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_content_types'];
    $forward = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_autoforward'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_autoforward'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_autoforward'];
  }
  else {
    $default = $form_state['page']->new_handler->conf['context_admin_options_items'];
    $type_fields = $form_state['page']->new_handler->conf['context_admin_content_types'];
    $forward = $form_state['page']->new_handler->conf['context_admin_autoforward'];
  }
  $types = content_types();
  foreach ($types as $type) {
    foreach ($type['fields'] as $key => $field) {
      if ($field['widget']['module'] == 'nodereference') {
        $fields[$type['type']][$field['field_name']] = $field['widget']['label'];
        $options[$type['type']] = $type['name'];
      }
    }
  }
  $form['context_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node Creation/Reference Options'),
    '#tree' => TRUE,
  );
  $form['context_admin']['context_admin_options_items'] = array(
    '#type' => 'radios',
    '#title' => t('Select the node type you would like to create'),
    '#required' => TRUE,
    '#options' => $options,
    '#default_value' => $default,
  );
  foreach ($fields as $key => $field_group) {
    $form['context_admin']['content_types'][$key] = array(
      '#type' => 'radios',
      '#title' => t('Available Reference Fields'),
      '#description' => t('Choose a reference field from the available fields'),
      '#options' => $field_group,
      '#process' => array('ctools_dependent_process', 'expand_radios'),
      '#dependency' => array('radio:context_admin[context_admin_options_items]' => array($key)),
      '#prefix' => '<div id="edit-context-admin-content-types-'. str_replace('_', '-', $key) .'-wrapper"><div>',
      '#suffix' => '</div></div>',
      '#default_value' => $type_fields[$key],
    );
  }
  $form['context_admin_autoforward'] = array(
    '#type' => 'checkbox',
    '#title' => t('Forward the user back to the node they were on before they created this node.'),
    '#default_value' => $forward,
  );
  return $form;
}

function context_admin_noderef_create_node_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if (!is_null($form_state['handler_id'])) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] = $form_state['values']['context_admin']['context_admin_options_items'];
    unset($form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_content_types']);
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_content_types'][$form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items']] = $form_state['values']['context_admin']['content_types'][$form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items']];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_field'] = $form_state['values']['context_admin']['content_types'][$form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items']];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_autoforward'] = $form_state['values']['context_admin_autoforward'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_options_items'] = $form_state['values']['context_admin']['context_admin_options_items'];
    unset($form_state['page']->new_handler->conf['context_admin_content_types']);
    $form_state['page']->new_handler->conf['context_admin_content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']] = $form_state['values']['context_admin']['content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']];
    $form_state['page']->new_handler->conf['context_admin_field'] = $form_state['values']['context_admin']['content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']];
    $form_state['page']->new_handler->conf['context_admin_autoforward'] = $form_state['values']['context_admin_autoforward'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_noderef_create_node_render_page($handler, $base_contexts, $args, $test = TRUE) {
  $type = $handler->conf['context_admin_options_items'];
  module_load_include('inc', 'node', 'node.pages');
  /**
   * For those who are wondering, we're circumventing node_add()
   * in order to get around the extra user_access check it does.
   * We don't want it, so node_add() is basically duplicated
   * here minus that small amount of code
   */
  global $user;

  $types = node_get_types();
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  // If a node type has been specified, validate its existence.
  if (isset($types[$type])) {
    // Initialize settings:
    $node = array('uid' => $user->uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $type, 'language' => '');

    drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)));
    $output = drupal_get_form($type .'_node_form', $node);
  }
  return $output;
}

function context_admin_noderef_create_node_form_alter(&$form, $form_state, $form_id) {
  $page = page_manager_get_current_page();
  switch($form_id) {
    case $page['handler']->conf['context_admin_options_items'] .'_node_form':
      if ($form['#node']->type == $page['handler']->conf['context_admin_options_items']) {
        $context = $page['contexts'][$page['handler']->conf['submitted_context']];
        $form[$page['handler']->conf['context_admin_field']][0]['#default_value']['nid'] = $context->data->nid;
        $form[$page['handler']->conf['context_admin_field']][0]['#access'] = FALSE;
      }
      break;
  }
}

function context_admin_noderef_create_node_nodeapi(&$node, $op, $a3 = NULL, $a4 = NULL) {
  switch ($op) {
    case 'insert':
      $page = page_manager_get_current_page();
      $context = $page['contexts'][$page['handler']->conf['submitted_context']];
      if ($node->type == $page['handler']->conf['context_admin_options_items'] && $page['handler']->conf['context_admin_autoforward']) {
        drupal_goto('node/'. $context->data->nid);
      }
      break;
  }
}
