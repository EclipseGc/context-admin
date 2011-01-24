<?php
//$Id: noderef_create_node.inc,v 1.2 2010/09/12 06:41:49 eclipsegc Exp $

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
    $custom_redirect = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_custom_redirect'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_custom_redirect'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_custom_redirect'];
  }
  else {
    $default = $form_state['page']->new_handler->conf['context_admin_options_items'];
    $type_fields = $form_state['page']->new_handler->conf['context_admin_content_types'];
    $forward = $form_state['page']->new_handler->conf['context_admin_autoforward'];
    $custom_redirect = $form_state['page']->new_handler->conf['context_admin_custom_redirect'];
  }
  $node_entities = entity_get_info('node');
  $types = field_info_instances('node');
  if ($types) {
    foreach ($types as $type => $field_instances) {
      foreach ($field_instances as $field_name => $field) {
        if ($field['widget']['module'] == 'node_reference') {
          $fields[$type][$field_name] = $field['label'];
          $options[$type] = $node_entities['bundles'][$type]['label'];
        }
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
      '#process' => array('ctools_dependent_process', 'form_process_radios'),
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

/** @todo custom redirect uses tokens now, but would be better to use %node:nid replacements similar to page manager titles **/
  $form['context_admin_custom_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom redirect path'),
    '#default_value' => $custom_redirect,
    '#description' => t('Define a custom path to redirect to after the node creation. This path will be translated with the node tokens of the parent node. Note: This overrides auto forwarding back to the original node.'),
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
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_custom_redirect'] = $form_state['values']['context_admin_custom_redirect'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_options_items'] = $form_state['values']['context_admin']['context_admin_options_items'];
    unset($form_state['page']->new_handler->conf['context_admin_content_types']);
    $form_state['page']->new_handler->conf['context_admin_content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']] = $form_state['values']['context_admin']['content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']];
    $form_state['page']->new_handler->conf['context_admin_field'] = $form_state['values']['context_admin']['content_types'][$form_state['page']->new_handler->conf['context_admin_options_items']];
    $form_state['page']->new_handler->conf['context_admin_autoforward'] = $form_state['values']['context_admin_autoforward'];
    $form_state['page']->new_handler->conf['context_admin_custom_redirect'] = $form_state['values']['context_admin_custom_redirect'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_noderef_create_node_render_page($handler, $base_contexts, $args, $test = TRUE) {
  $type = $handler->conf['context_admin_options_items'];
  module_load_include('inc', 'node', 'node.pages');
  return node_add($type);
}

function context_admin_noderef_create_node_form_alter(&$form, &$form_state, $form_id) {
  $page = page_manager_get_current_page();
  switch($form_id) {
    case $page['handler']->conf['context_admin_options_items'] . '_node_form':
      if ($form['#node']->type == $page['handler']->conf['context_admin_options_items']) {
        $context = $page['contexts'][$page['handler']->conf['submitted_context']];
        $form[$page['handler']->conf['context_admin_field']]['und'][0]['nid']['#default_value'] = $context->data->nid;
        $form[$page['handler']->conf['context_admin_field']]['und'][0]['nid']['#access'] = FALSE;
        $context = $page['contexts'][$page['handler']->conf['submitted_context']];

        if ($page['handler']->conf['context_admin_autoforward'] || $page['handler']->conf['context_admin_custom_redirect']) {
          $form['node_context'] = array(
            '#type' => 'value',
            '#value' => $context->data,
          );

          if ($page['handler']->conf['context_admin_custom_redirect']) {
            $form['context_admin_custom_redirect'] = array(
              '#type' => 'value',
              '#value' => $page['handler']->conf['context_admin_custom_redirect'],
            );
          }

          $form['actions']['submit']['#submit'][] = 'context_admin_noderef_create_node_submit';
        }
      }
      break;
  }
}

function context_admin_noderef_create_node_submit($form, &$form_state) {
  if (isset($form_state['values']['node_context'])) {
/** @todo add access check to ensure the user is allowed to view this redirect **/
    $redirect = '';
    if ($form_state['values']['context_admin_custom_redirect']) {
      $redirect = token_replace($form_state['values']['context_admin_custom_redirect'], array('node' => $form_state['values']['node_context']));
    }
    else {
      $redirect = 'node/' . $form_state['values']['node_context']->nid;
    }

    if ($redirect) {
      $form_state['redirect'] = $redirect;
    }
  }
}
