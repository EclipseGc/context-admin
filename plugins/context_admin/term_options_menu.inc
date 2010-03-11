<?php
//$Id$

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Taxonomy Term Options'),
  'description' => t('Provides a number of different term related functions to allow for contextualized taxonomy term maintenance.'),
  'context' => 'term',
);

function context_admin_term_options_menu_content_form(&$form, &$form_state, $cache = NULL) {
  if (!is_null($form_state['handler_id'])) {
    $default_options = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_term_options'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_term_options'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_term_options'];
  }
  else {
    $default_options = $form_state['page']->new_handler->conf['context_admin_term_options'];
  }
  $form['help'] = array(
    '#type' => 'markup',
    '#value' => t('Taxonomy edit tabs can be limited via the access controls/selection criteria.  If they are not limited, they will apply to ALL terms within your site.'),
  );
  $form['context_admin_term_options'] = array(
    '#type' => 'radios',
    '#title' => t('Term Options'),
    '#required' => TRUE,
    '#options' => array(
      'add' => t('Add Term'),
      'term' => t('Edit Term'),
      'vocabulary' => t('Edit Vocabulary'),
    ),
    '#default_value' => $default_options,
  );
  return $form;
}

function context_admin_term_options_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if (!is_null($form_state['handler_id'])) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_term_options'] = $form_state['values']['context_admin_term_options'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_term_options'] = $form_state['values']['context_admin_term_options'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_term_options_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
  module_load_include('inc', 'taxonomy', 'taxonomy.admin');
  switch($handler->conf['context_admin_term_options']) {
    case 'add':
      return context_admin_term_options_menu_admin_term_edit($args[0], TRUE);
    case 'term':
      return context_admin_term_options_menu_admin_term_edit($args[0]);
    case 'vocabulary':
      $dir = module_invoke('context_admin', 'ctools_plugin_directory', 'context_admin', 'administrations');
      ctools_include('taxonomy_list_menu', 'context_admin', $dir);
      if ($term = (array)taxonomy_get_term($args[0])) {
        $form = drupal_get_form('context_admin_taxonomy_list_menu_taxonomy_form_vocabulary', (array)taxonomy_vocabulary_load($term['vid']));
        return $form;
      }
      return drupal_not_found();
  }
}

function context_admin_term_options_menu_admin_term_edit($tid, $add = NULL) {
  if (!is_numeric($tid)) {
    $tid = db_result(db_query("SELECT tid FROM {term_data} WHERE name = '%s'", $tid));
  }
  if ($term = (array)taxonomy_get_term($tid)) {
    if (!is_null($add)) {
      return drupal_get_form('context_admin_term_options_menu_taxonomy_form_term', taxonomy_vocabulary_load($term['vid']), array(), TRUE);
    }
    else {
      return drupal_get_form('context_admin_term_options_menu_taxonomy_form_term', taxonomy_vocabulary_load($term['vid']), $term);
    }
  }
  return drupal_not_found();
}

function context_admin_term_options_menu_taxonomy_form_term(&$form_state, $vocabulary, $edit = array(), $add = NULL) {
  $form = taxonomy_form_term($form_state, $vocabulary, $edit);
  foreach (module_implements('form_alter') as $module) {
    $function = $module .'_form_alter';
    $function($form, $form_state, 'taxonomy_form_term');
  }
  if ($add) {
    $form['#add'] = TRUE;
  }
  return $form;
}

function context_admin_term_options_menu_taxonomy_form_term_submit($form, &$form_state) {
  taxonomy_form_term_submit($form, $form_state);
  $form_state['redirect'] = $_GET['q'];
}

function context_admin_term_options_menu_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'context_admin_term_options_menu_taxonomy_form_term') {
    if ($form['#add'] && arg(2)) {
      $form['advanced']['parent']['#default_value'] = array(arg(2));
    }
  }
}
