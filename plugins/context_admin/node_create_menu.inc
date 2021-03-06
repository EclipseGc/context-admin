<?php
//$Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Create Node'),
  'description' => t('Allows the creation of a particular node type.  This circumvents the typical drupal access controls on node creation.'),
  'required context' => new ctools_context_optional(t('Node'), 'node'),
);


function context_admin_node_create_menu_content_form(&$form, &$form_state, $cache = NULL) {
  ctools_include('dependent');
  $context_options = $form['context']['#options'];
  unset($context_options['empty']);
  $context_dependencies = array();
  foreach ($context_options as $c_key => $c_option) {
    $context_dependencies[] = $c_key;
  }
  if (!is_null($form_state['handler_id'])) {
    $default = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_options_items'];
  }
  else {
    $default = $form_state['page']->new_handler->conf['context_admin_options_items'];
  }
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

  if (!is_null($form_state['handler_id'])) {
    $sub = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_sub_page'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_sub_page'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_sub_page'];
  }
  else {
    $sub = $form_state['page']->new_handler->conf['context_admin_sub_page'];
  }
  $form['context_admin_sub_page'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create as sub node'),
    '#description' => t('Creates a node as a sub node of the currently viewed node within the menu system.'),
    '#default_value' => $sub,
    '#process' => array('ctools_dependent_process'),
    '#dependency' => array('edit-context' => array(implode(', ', $context_dependencies))),
  );

  return $form;
}

function context_admin_node_create_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if (!is_null($form_state['handler_id'])) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_options_items'] = $form_state['values']['context_admin_options_items'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_options_items'] = $form_state['values']['context_admin_options_items'];
  }
  if ($form_state['values']['context_admin_sub_page']) {
    if (!is_null($form_state['handler_id'])) {
      $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_sub_page'] = $form_state['values']['context_admin_sub_page'];
    }
    else {
      $form_state['page']->new_handler->conf['context_admin_sub_page'] = $form_state['values']['context_admin_sub_page'];
    }
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

function context_admin_node_create_menu_nodeapi(&$node, $op, $a3, $a4) {
  switch ($op) {
    case 'insert':
      $page = page_manager_get_current_page();
      if ($page['handler']->conf['context_admin_sub_page']) {
        $context = $page['contexts'][$page['handler']->conf['submitted_context']];
        $parent_id = $context->argument;
        $results = db_query("SELECT menu_name, mlid FROM {menu_links} WHERE link_path = 'node/%d' AND module = 'menu'", $parent_id);
        $result = db_fetch_object($results);
        if ($result) {
          $item = array(
            'link_title' => $node->title,
            'menu_name' => $result->menu_name,
            'plid' => $result->mlid,
            'link_path' => 'node/'. $node->nid,
          );
          menu_link_save($item);
        }
      }
      break;
  }
}
