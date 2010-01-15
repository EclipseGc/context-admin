<?php

function context_admin_term_options_menu_content_form(&$form, &$form_state, $cache = NULL) {
  $default_options = $form_state['page']->context_admin_term_options ? $form_state['page']->context_admin_term_options : $cache->handlers[$cache->task_id .'_'. $cache->subtask_id .'_menu_context']->conf['context_admin_term_options'];
  $form['help'] = array(
    '#type' => 'markup',
    '#value' => t('Taxonomy edit tabs can be limited via the access controls/selection criteria.  If they are not limited, they will apply to ALL terms within your site.'),
  );
  $form['context_admin_term_options'] = array(
    '#type' => 'radios',
    '#title' => t('Term Options'),
    '#required' => TRUE,
    '#options' => array(
      'term' => t('Edit Term'),
      'vocabulary' => t('Edit Vocabulary'),
    ),
    '#default_value' => $default_options,
  );
  return $form;
}

function context_admin_term_options_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  $form_state['page']->handlers[$form_state['page']->task_id .'_'. $form_state['page']->subtask_id .'_menu_context']->conf['context_admin_term_options'] = $form_state['values']['context_admin_term_options'];
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_term_options_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
  module_load_include('inc', 'taxonomy', 'taxonomy.admin');
  switch($handler->conf['context_admin_term_options']) {
    case 'term':
      return context_admin_term_options_menu_admin_term_edit($args[0]);
    case 'vocabulary':
      module_load_include('inc', 'context_admin', 'includes/taxonomy_list_menu');
      if ($term = (array)taxonomy_get_term($args[0])) {
        $form = drupal_get_form('context_admin_taxonomy_list_menu_taxonomy_form_vocabulary', (array)taxonomy_vocabulary_load($term['vid']));
        return $form;
      }
      return drupal_not_found();
  }
}

function context_admin_term_options_menu_admin_term_edit($tid) {
  if (!is_numeric($tid)) {
    $tid = db_result(db_query("SELECT tid FROM {term_data} WHERE name = '%s'", $tid));
  }
  if ($term = (array)taxonomy_get_term($tid)) {
    return drupal_get_form('context_admin_term_options_menu_taxonomy_form_term', taxonomy_vocabulary_load($term['vid']), $term);
  }
  return drupal_not_found();
}

function context_admin_term_options_menu_taxonomy_form_term(&$form_state, $vocabulary, $edit = array()) {
  $form = taxonomy_form_term(&$form_state, $vocabulary, $edit);
  return $form;
}

function context_admin_term_options_menu_taxonomy_form_term_submit($form, &$form_state) {
  taxonomy_form_term_submit($form, &$form_state);
  $form_state['redirect'] = $_GET['q'];
}