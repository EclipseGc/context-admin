<?php
//$Id: noderef_create_node.inc,v 1.2 2010/09/12 06:41:49 eclipsegc Exp $

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Create Media with auto node reference'),
  'description' => t('Creates media with an automatic reference back to its parent.'),
  'required context' => new ctools_context_required(t('Node'), 'node'),
);


function context_admin_noderef_create_media_content_form(&$form, &$form_state, $cache = NULL) {
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
  $entities = entity_get_info('media');
  $types = field_info_instances('media');
  if ($types) {
    foreach ($types as $type => $field_instances) {
      foreach ($field_instances as $field_name => $field) {
        if ($field['widget']['module'] == 'node_reference') {
          $fields[$type][$field_name] = $field['label'];
          $options[$type] = $entities['bundles'][$type]['label'];
        }
      }
    }
  }
  $form['context_admin'] = array(
    '#type' => 'fieldset',
    '#title' => t('Media Creation/Reference Options'),
    '#tree' => TRUE,
  );
  $form['context_admin']['context_admin_options_items'] = array(
    '#type' => 'radios',
    '#title' => t('Select the media type you would like to create'),
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
    '#title' => t('Forward the user back to the node they were on before they created this media.'),
    '#default_value' => $forward,
  );

  $form['context_admin_custom_redirect'] = array(
    '#type' => 'textfield',
    '#title' => t('Custom redirect path'),
    '#default_value' => $custom_redirect,
    '#description' => t('Define a custom path to redirect to after the node creation. This path will be translated with the node tokens of the parent node. Note: This overrides auto forwarding back to the original node.'),
  );

  return $form;
}

function context_admin_noderef_create_media_content_form_submit(&$form, &$form_state) {
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

function context_admin_noderef_create_media_render_page($handler, $base_contexts, $args, $test = TRUE) {
  $type = $handler->conf['context_admin_options_items'];
  module_load_include('inc', 'media', 'media.browser');
/** @todo: able to call for specific type??? ***/
  if (user_access('administer media')) {
    return media_browser();
  }
  return '';
}

function context_admin_noderef_create_media_form_alter(&$form, &$form_state, $form_id) {
  $page = page_manager_get_current_page();
  switch($form_id) {
    case 'media_add_upload':
/** @todo cannot target specific media type, so we're assuming we are active if we get called to alter from context_admin.module **/
      //if ($form['#node']->type == $page['handler']->conf['context_admin_options_items']) {
      if ($noderef_fieldname = $page['handler']->conf['context_admin_field']) {
        $context = $page['contexts'][$page['handler']->conf['submitted_context']];
        $noderef_element = &$form[$noderef_fieldname]['und'][0]['nid'];
        $noderef_element['#type'] = 'textfield';
        $noderef_element['#default_value'] = $context->data->nid;
        $noderef_element['#access'] = FALSE;

        // store fieldname
        $form['context_admin_fieldname'] = array(
          '#type' => 'value',
          '#value' => $noderef_fieldname,
        );

        // store if want auto forward to node
        $form['context_admin_autoforward'] = array(
          '#type' => 'value',
          '#value' => $page['handler']->conf['context_admin_autoforward'],
        );

        // store custom_redirect
        if ($page['handler']->conf['context_admin_custom_redirect']) {
          $form['context_admin_custom_redirect'] = array(
            '#type' => 'value',
            '#value' => $page['handler']->conf['context_admin_custom_redirect'],
          );
        }

        $form['#submit'][] = 'context_admin_noderef_create_media_add_upload_submit';
        unset($noderef_element);
      }
      break;

    default:
      // get node ref nids from session data thats populated on add upload submit
      if (isset($_SESSION['context_admin']['noderef_create_media']) && isset($_SESSION['context_admin']['noderef_create_media'][$form_id])) {
        $changed = FALSE;
        foreach ($_SESSION['context_admin']['noderef_create_media'][$form_id] as $k => $v) {
          if (!isset($form[$k])) {
            continue;
          }

          $changed = TRUE;
          $noderef_element = &$form[$k]['und'][0]['nid'];
          $noderef_element['#default_value'] = $v;
          /** @todo:  when restricting access, the field value does not get saved ... **/
          //$form[$k]['und'][0]['nid']['#access'] = FALSE;

          // hide field with styles and no title
          unset($noderef_element['#title']);
          if (!isset($noderef_element['#attributes']['style'])) {
            $noderef_element['#attributes']['style'] = array('display:none;');
          }
          else {
           $noderef_element['#attributes']['style'][] = 'display:none;';
          }

          unset($noderef_element);
        }

        if ($changed) {
          drupal_add_css(drupal_get_path('module', 'context_admin') . '/plugins/context_admin/noderef_create_media.css');
          $form['#validate'][] = 'context_admin_noderef_create_media_form_validate';
        }
      }
      break;
  }
}

function context_admin_noderef_create_media_add_upload_submit($form, &$form_state) {
  if ($noderef_fieldname = $form_state['values']['context_admin_fieldname']) {
    if (isset($form[$noderef_fieldname]['und'][0]['nid'])) {
      // store node ref nid for use in edit form
      $_SESSION['context_admin']['noderef_create_media']['media_edit'][$noderef_fieldname] = $form_state['values']['nid'];

      // set redirect and destination after redirect
      $destination = array();
      if ($form_state['values']['context_admin_custom_redirect']) {
        $node = node_load($form_state['values']['nid']);
        $destination['destination'] = token_replace($form_state['values']['context_admin_custom_redirect'], array('node' => $node));
      }
      elseif ($form_state['values']['context_admin_autoforward']) {
        $destination['destination'] = 'node/' . $form_state['values']['nid'];
      }
      elseif (isset($_GET['destination'])) {
        $destination = drupal_get_destination();
      }
      unset($_GET['destination']);

      $form_state['redirect'] = array('media/' . $form_state['values']['upload']->fid . '/edit', array('query' => $destination));
    }
  }
}

function context_admin_noderef_create_media_form_validate($form, &$form_state) {
  unset($_SESSION['context_admin']['noderef_create_media'][$form['form_id']['#value']]);
}
