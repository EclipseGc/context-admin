<?php

/**
 * Plugins are described by creating a $plugin array which will be used
 * by the system that includes this file.
 */

$plugin = array(
  'title' => t('Vocabulary Options'),
  'description' => t('Provides a number of different vocabulary related functions to allow for contextualized vocabulary maintenance.'),
  'context' => 'global',
);

function context_admin_taxonomy_list_menu_content_form(&$form, &$form_state, $cache = NULL) {
  if (!is_null($form_state['handler_id'])) {
    $default = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary'];
    $default_option = $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary_options'] ? $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary_options'] : $cache->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary_options'];
  }
  else {
    $default = $form_state['page']->new_handler->conf['context_admin_vocabulary'];
    $default_option = $form_state['page']->new_handler->conf['context_admin_vocabulary_options'];
  }
  $types = taxonomy_get_vocabularies();
  foreach ($types as $type) {
    $options[$type->vid] = $type->name;
  }
  $form['context_admin_vocabulary'] = array(
    '#type' => 'radios',
    '#title' => t('Vocabulary'),
    '#description' => t('Select the vocabulary list you would like.'),
    '#required' => TRUE,
    '#options' => $options,
    '#default_value' => $default,
  );
  $form['context_admin_vocabulary_options'] = array(
    '#type' => 'radios',
    '#title' => t('Vocabulary Option'),
    '#required' => TRUE,
    '#options' => array(
      'edit' => 'Edit Vocabulary',
      'list' => 'List Terms',
      'add' => 'Add Terms',
    ),
    '#default_value' => $default_option,
  );
  return $form;
}

function context_admin_taxonomy_list_menu_content_form_submit(&$form, &$form_state) {
  $cache = context_admin_get_page_cache($form_state['page']->subtask_id);
  if ($form_state['handler_id']) {
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary'] = $form_state['values']['context_admin_vocabulary'];
    $form_state['page']->handlers[$form_state['handler_id']]->conf['context_admin_vocabulary_options'] = $form_state['values']['context_admin_vocabulary_options'];
  }
  else {
    $form_state['page']->new_handler->conf['context_admin_vocabulary'] = $form_state['values']['context_admin_vocabulary'];
    $form_state['page']->new_handler->conf['context_admin_vocabulary_options'] = $form_state['values']['context_admin_vocabulary_options'];
  }
  context_admin_set_page_cache($form_state['page']->subtask_id, $form_state['page']);
  return $form_state;
}

function context_admin_taxonomy_list_menu_render_page($handler, $base_contexts, $args, $test = TRUE) {
  module_load_include('inc', 'taxonomy', 'taxonomy.admin');
  if (is_numeric($handler->conf['context_admin_vocabulary'])) {
    $vocabulary = taxonomy_vocabulary_load($handler->conf['context_admin_vocabulary']);
  }
  switch($handler->conf['context_admin_vocabulary_options']) {
    case 'edit':
      $form = drupal_get_form('context_admin_taxonomy_list_menu_taxonomy_form_vocabulary', (array)$vocabulary);
      return $form;
    case 'list':
      return drupal_get_form('taxonomy_overview_terms', $vocabulary);
    case 'add':
      return taxonomy_add_term_page($vocabulary);
  }
}

function context_admin_taxonomy_list_menu_taxonomy_form_vocabulary($form_state, $vocabulary) {
  $form = taxonomy_form_vocabulary($form_state, $vocabulary);
  return $form;
}

function context_admin_taxonomy_list_menu_taxonomy_form_vocabulary_submit($form, &$form_state) {
  taxonomy_form_vocabulary_submit($form, $form_state);
  $form_state['redirect'] = $_GET['q'];
}

function context_admin_taxonomy_list_menu_form_alter(&$form, $form_state, $form_id) {
  if ($form_id == 'context_admin_taxonomy_list_menu_taxonomy_form_vocabulary') {
    unset($form['delete']);
  }
}