<?php

function context_admin_node_create_menu_content_form(&$form, &$form_state, $cache = NULL) {
  $default = $form_state['page']->context_admin_options_items ? $form_state['page']->context_admin_options_items : $cache->handlers[$cache->task_id .'_'. $cache->subtask_id .'_menu_context']->conf['context_admin_options_items'];
  $types = node_get_types();
  foreach ($types as $key => $type) {
    $options[$key] = $type->name;
  }
  $form['context_admin_options_items'] = array(
    '#type' => 'radios',
    '#title' => t('Select the node type you would like to create'),
    '#required' => TRUE,
    '#options' => $options,
    '#default_value' => $default,
  );
  foreach($form_state['page']->subtask['subtask']->arguments as $context => $args) {
    if ($context == 'node') {
      $sub = $form_state['page']->context_admin_sub_page ? $form_state['page']->context_admin_sub_page : $cache->handlers[$cache->task_id .'_'. $cache->subtask_id .'_menu_context']->conf['context_admin_sub_page'];
      $form['context_admin_sub_page'] = array(
        '#type' => 'checkbox',
        '#title' => t('Create as sub node'),
        '#description' => t('Creates a node as a sub node of the currently viewed node within the menu system.'),
        '#default_value' => $sub,
      );
    }
  }
  return $form;
}

function context_admin_node_create_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  $form_state['page']->handlers[$form_state['page']->task_id .'_'. $form_state['page']->subtask_id .'_menu_context']->conf['context_admin_options_items'] = $form_state['values']['context_admin_options_items'];
  if ($form_state['values']['context_admin_sub_page']) {
    $form_state['page']->handlers[$form_state['page']->task_id .'_'. $form_state['page']->subtask_id .'_menu_context']->conf['context_admin_sub_page'] = $form_state['values']['context_admin_sub_page'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_node_create_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
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